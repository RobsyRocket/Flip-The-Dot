window.onload = function () {
    configLoad(function (xhrProgressEvent) {
        xhr = this;
        var config = {};
        var valid = false;

        if (xhr.status === 200) {
            var config = JSON.parse(xhr.responseText);
            // TODO implement config check
            valid = true;
        }
        if (valid) {
            initInterface(config);
        }
        else {
            configPrompt("Config could not be loaded or was invalid!");
        }
    })
}

var use_websocket = true;

function configPrompt(prefix_msg) {
    // TODO validate input to check for numbers and positive values
    var width = prompt((prefix_msg ? (prefix_msg + " ") : "") + "Please specify the matrix dimension: Width");
    var height = prompt("Please specify the matrix dimension: Height");
    var debounce = prompt("Please specify command debounce time: Milliseconds");

    // might be need to be ordered alphabetically
    var configJson = {
        debounce: debounce,
        height: height,
        width: width
    };

    configSave(configJson, function (xhrProgressEvent) {
        xhr = this;
        if ( xhr.status === 200 && JSON.stringify(configJson) == JSON.stringify(JSON.parse(xhr.responseText)) ) {
            alert("Config updated.");
            initInterface(configJson);
        }
        else {
            if ( confirm("Error: New config could not be saved or was invalid! Click OK to try again or cancel to proceed without saving.") ) {
                configPrompt();
            } else {
                initInterface(configJson);
            }
        }
    });
}

function configLoad(callback) {
    var xhr = new XMLHttpRequest();
    xhr.open('GET', 'drawer/config');
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.onload = callback;
    xhr.send();
}

function configSave(newConfigJson, callback) {
    var xhr = new XMLHttpRequest();
    xhr.open('PUT', 'drawer/config');
    xhr.setRequestHeader('Content-Type', 'application/json');
    xhr.onload = callback;
    xhr.send(JSON.stringify(newConfigJson));
}

function initInterface(configJson) {
    var webSocket = null;
    function sendCommandViaWS(command){
        webSocket.send(command);
    }

    var cMax = configJson.width;
    var rMax = configJson.height;
    var debounceTime = configJson.debounce || 100;
    var chunksize = (2000) / (encodeURI(cMax + 'x' + rMax + 'x?' + ' ').length);

    var bind = function (nodeList, action, callback) {
        nodeList = nodeList != null && typeof nodeList[Symbol.iterator] === 'function' ? nodeList : [nodeList];
        each(nodeList, function (node) {
            node['on' + action] = callback;
        });
    };
    var each = function (nodeList, callback) {
        Array.prototype.slice.call(nodeList).map(function (node, index) {
            callback(node, index);
        });
    };


    var getState = function (node) {
        return node.getAttribute('data-active') == 1;
    };

    var setState = function (node, newState) {
        node.setAttribute('data-active', newState ? 1 : 0);
        return getState(node);
    };

    var setCell = function (node, newState) {
        var column = node.getAttribute('data-c');
        var row = node.getAttribute('data-r');

        // update only if needed
        if (newState != getState(node)) {
            sendRequest(column, row, newState);
            setState(node, newState);
        }
    };

    function getCellAtCoordinate(c, r) {
        c--;
        r--;

        if ( c >= 0 && r >= 0 ) {
            var table = document.getElementsByTagName('table')[0];
            var rows = table.getElementsByTagName('tr');
            if (rows.length > 0 && rows.length - 1 >= r) {
                var cols = rows[r].getElementsByTagName('td');
                if (cols.length > 0 && cols.length - 1 >= c) {
                    return cols[c];
                }
            }
        }
        return null;
    }

    var invertCell = function (node) {
        setCell(node, !getState(node));
    };

    var buffer = new Array();
    var sendRequest = function (column, row, state) {

        var command = column + 'x' + row + 'x' + (state ? 1 : 0);
        var targetPath = 'drawer/commands';

        if ( use_websocket ) {
            try {
                if ( !webSocket )
                {
                    webSocket = new WebSocket('ws://' + window.location.hostname + ':81/');
                    webSocket.onopen = function () {
                        try {
                            while (buffer.length > 0) {
                            // take the first (oldest) element from the buffer
                            sendCommandViaWS(buffer.shift());
                            }
                        }
                        catch (e) {
                            console.error(e);
                            // add back to where it was taken from, the beginning of the buffer
                            buffer.unshift(command);
                        }
                    };
                    webSocket.onclose = function(e) {
                        console.log('websocket close', e);
                        if (e.code != 1000 ) {  // not CLOSE_NORMAL
                            webSocket = null;
                        }
                    };
                    webSocket.onerror = function(e) {
                        console.log('websocket error', e);
                        if (e.code  == 'ECONNREFUSED' ) {
                            webSocket.close();
                            webSocket = null;
                        }
                    };
                }
                if (webSocket && webSocket.readyState !== 1) {
                    // websocket not available, add to the buffer to try it again later
                    buffer.push(command);
                }
                else {
                    sendCommandViaWS(command);
                }
            }
            catch (e) {
                console.error("websocket exception", e);
                // websocket on error, add to the buffer to try it again later
                buffer.push(command);
            }
        }
        else {
            buffer.push(command);

            // TODO think about to collect commands and make a bulk request: https://davidwalsh.name/javascript-debounce-function
            // https://blog.garstasio.com/you-dont-need-jquery/ajax/#getting

            setTimeout(function () {
                var commands = buffer.splice(0, chunksize).join(' ');
                if (commands < 1) {
                    return false;
                }
                var xhr = new XMLHttpRequest();
                xhr.open('GET', targetPath + '?commands=' + commands);
                xhr.onload = function () {
                    if (xhr.status === 200) {
                        console.log('Success:' + xhr.responseText);
                    }
                    else {
                        // TODO add implementation to the failed commands back to the buffer for later processing (like: buffer.shift(...commands);)
                        // TODO add implementation to try again sending the buffered commands without manual interaction of the user
                        console.log('Request failed.  Returned status of ' + xhr.status);
                        console.log('ResponseText:' + xhr.responseText);
                    }
                };
                xhr.send();
            }, debounceTime);
        }
    };

    var isActiveDrawOnMove = false;
    var drawCallback;

    var checkDrawHover = function (node) {
        if (isDrawOnMove() && drawCallback) {
            drawCallback(node);
        }
    };

    var isDrawOnMove = function () {
        return isActiveDrawOnMove === true;
    };

    var setDrawOnMove = function (state) {
        isActiveDrawOnMove = state === true;
    };

    var display = '<table class="display noselect">';
    for (var r = rMax; r >= 1; r--) {
        display += "<tr>";
        for (var c = 1; c <= cMax; c++) {
            display += '<td data-c="' + c + '" data-r="' + r + '" data-active="0"></td>';
        }
        display += "</tr>";
    }
    display += '</table>';

    var drawActions = [
        { label: 'Set', callback: function (node) { setCell(node, true) } },
        { label: 'Remove', callback: function (node) { setCell(node, false) } },
        { label: 'Invert', callback: invertCell },
    ];
    drawCallback = drawActions[2].callback;

    display += 'Mode: ';
    for (var dOMA_i = 0; dOMA_i < drawActions.length; dOMA_i++) {
        display += '<label><input type="radio" name="drawAction" id="drawAction_' + dOMA_i + '" ' + (drawCallback == drawActions[dOMA_i].callback ? 'checked' : '') + ' /><button id="drawAction_' + dOMA_i + '_btn">' + drawActions[dOMA_i].label + '</button></label>';
    }

    display += 'Move: ';
    display += '<button name="left">&#9664;</button>';
    display += '<button name="right">&#9654;</button>';
    display += '<button name="top">&#x25B2;</button>';
    display += '<button name="down">&#x25BC;</button>';
    display += 'Actions: ';
    display += '<button name="do_all" title="Perform draw mode action on all image parts at once.">Do all</button>';
    display += '<button name="export" title="Generate and provide list of commands for the current image to save it elsewhere.">Export</button>';
    display += '<button name="import" title="Allows pasting of previously exported list of commands to restore image.">Import</button>';
    display += '<button name="config" title="Configure image width, height and command transmission rate.">Config</button>';
    display += '<span title="Using local browser storage to save current image and load it back.">Storage: ';
    display += '<button name="save_graphic_locally" title="Using local browser storage to save current image.">Save</button>';
    display += '<button name="load_graphic_locally" title="Restore image from local browser storage.">Load</button>';


    document.body.innerHTML = display;

    for (var dOMA_i = 0; dOMA_i < drawActions.length; dOMA_i++) {
        (function (i, callback) {
            bind(document.getElementById('drawAction_' + i), 'change', function () {
                drawCallback = callback;
                this.setAttribute('checked', 'checked');
            });
        })(dOMA_i, drawActions[dOMA_i].callback);
    }

    for (var dOMA_i = 0; dOMA_i < drawActions.length; dOMA_i++) {
        (function (i) {
            bind(document.getElementById('drawAction_' + i + '_btn'), 'click', function () {
                var input = document.getElementById('drawAction_' + i);
                input.setAttribute('checked', 'checked');
                input.onchange();
            });
        })(dOMA_i);
    }

    bind(document.getElementsByName('config'), 'click', function () {
        configPrompt();
    });

    bind(document.getElementsByName('do_all'), 'click', function () {
        var cells = document.getElementsByTagName('td');
        for (var ci = 0; ci < cells.length; ci++) {
            cells[ci].onmousedown();
        }
    });


    /*
    var commands = '';
    var bitmap = '';
    var rows = document.getElementsByTagName('tr');
    for ( var ri = 0; ri < rows.length; ri++ ) {
      var row_cells = rows[ri].getElementsByTagName('td');
      for ( var ci = 0; ci < row_cells.length; ci++ ) {
        var column = row_cells[ci].getAttribute('data-c');
        var row = row_cells[ci].getAttribute('data-r');
        var state = row_cells[ci].getAttribute('data-active');
        commands += ' '+column+'x'+row+'x'+state;
      }
    }
    console.log(commands);
    */

    bind(document.getElementsByTagName('html'), 'mouseup', function () { setDrawOnMove(false); });
    // change only when triggered with event, not manual call
    bind(document.getElementsByTagName('table'), 'mousedown', function (evt) { if (evt) { setDrawOnMove(true); } });

    var moveHorizontal = function (right) {
        each(document.getElementsByTagName('tr'), function (row) {
            var cells = Array.prototype.slice.call(row.getElementsByTagName('td'), 0);
            if (right === true) {
                cells.reverse();
            }
            var backupState = getState(cells[0]);
            each(cells, function (cell, index) {
                if (cells[index + 1]) {
                    setCell(cell, getState(cells[index + 1]));
                }
            });
            setCell(cells[cells.length - 1], backupState);
        });
    };

    bind(document.getElementsByName('left'), 'click', function () { moveHorizontal(); });
    bind(document.getElementsByName('right'), 'click', function () { moveHorizontal(true); });

    var moveVertical = function (down) {
        var rows = Array.prototype.slice.call(document.getElementsByTagName('tr'), 0);
        if (down === true) {
            rows.reverse();
        }
        var row_backup = rows[0].cloneNode(true);
        var cells_to, cells_from;

        each(rows, function (row, ri) {
            if (rows[ri + 1]) {
                cells_from = rows[ri + 1].getElementsByTagName('td');
                cells_to = rows[ri].getElementsByTagName('td');
                each(cells_from, function (cell, ci) {
                    setCell(cells_to[ci], getState(cells_from[ci]));
                });
            }
        });


        cells_from = row_backup.getElementsByTagName('td');
        cells_to = rows[rows.length - 1].getElementsByTagName('td');
        each(cells_from, function (cell, ci) {
            setCell(cells_to[ci], getState(cells_from[ci]));
        });
    }

    bind(document.getElementsByName('top'), 'click', function () { moveVertical(); });
    bind(document.getElementsByName('down'), 'click', function () { moveVertical(true); });


    function tableToCommands(omitInactive, omitActive) {
        var cells = document.getElementsByTagName('table')[0].getElementsByTagName('td');
        var result = '';
        Array.prototype.slice.call(cells).map(function (cell) {
            var state = cell.getAttribute('data-active') == 1 ? 1 : 0;
            if ((state == 0 && omitInactive !== true) || (state == 1 && omitActive !== true)) {
                result += cell.getAttribute('data-c') + 'x' + cell.getAttribute('data-r') + 'x' + state + ' ';
            }
        });
        return result;
    }

    function commandsToTable(commands) {
        // TODO implement validation and error feedback
        commands = ((typeof commands == 'string') ? commands : '');
        var commandList = commands.split(/\s+/g);
        var pattern = /^(\d+)x(\d+)x(\d+)$/;

        for (var i = 0; i < commandList.length; i++) {
            var command = commandList[i];
            if (pattern.test(command)) {
                var parts = command.match(pattern);
                var cell = getCellAtCoordinate(parts[1], parts[2]);
                if (cell !== null) {
                    setCell(cell, parts[3] === '1');
                }
            }
        }
    }

    function saveCommandsLocally() {
        var table = document.getElementsByTagName('table')[0];
        var cells = table.getElementsByTagName('td');
        var rows = table.getElementsByTagName('tr');

        var data = {
            width: cells.length / rows.length,
            height: rows.length,
            states: ''
        };

        Array.prototype.slice.call(cells).map(function (cell) {
            data.states += cell.getAttribute('data-active') == 1 ? 1 : 0;
        });

        try {
            localStorage.setItem('commands', JSON.stringify(data));
            return JSON.stringify(data) === localStorage.getItem('commands');
        }
        catch (e){
            console.error(e);
            return false;
        }
    }

    function readbackCommandsLocally() {
        var data = JSON.parse(localStorage.getItem('commands'));
        if ( data && data.width && data.height && data.states ) {
            var statesSplitted = data.states.split("");
            var rowIndex = 0;
            var cellToChange;
            while ( statesSplitted && rowIndex < data.height ) {
                // take relevant portion to process out of the data stream
                var stateOfThisRow = statesSplitted.slice(0, data.width);
                // remove the taken part and keep the renaming part for the next iteration
                statesSplitted = statesSplitted.slice(data.width);

                for ( var colIndex = 0; colIndex < stateOfThisRow.length; colIndex++ ) {
                    cellToChange = getCellAtCoordinate(colIndex + 1, rowIndex + 1 );
                    if (cellToChange !== null) {
                        setCell(cellToChange, parseInt(stateOfThisRow[colIndex]));
                    }
                }
                rowIndex++;
            }
            return true;
        }
        return false;
    }

    bind(document.getElementsByName('export'), 'click', function () {
        alert(tableToCommands(true, false));
    });

    bind(document.getElementsByName('import'), 'click', function () {
        var commands = prompt("Please enter the command list:", "");
        if (commands != null) {
            commandsToTable(commands);
        }
    });

    bind(document.getElementsByName('save_graphic_locally'), 'click', function () {
        alert(saveCommandsLocally() ? "Saved successfully in your browser." : "Could not save data in your browser.");
    });
        
    bind(document.getElementsByName('load_graphic_locally'), 'click', function () {
        !readbackCommandsLocally() ? alert("Could find or load locally saved data.") : '';
    });

    Array.prototype.slice.call(document.getElementsByTagName('td')).map(function (cell) {
        cell.onmousedown = function () { drawCallback(cell); };
        cell.onmouseover = function () { checkDrawHover(cell); };
    });

    var fitScreenSize = function (event) {
        var table = document.getElementsByTagName('table');
        var tdList = table[0].getElementsByTagName('td');
        var controlsHeight = 24;
        var edgeLength = Math.min((window.innerHeight - controlsHeight) / rMax, window.innerWidth / cMax);
        for (var i = 0; i < tdList.length; i++) {
            tdList[i].style.height = edgeLength + "px";
            tdList[i].style.width = edgeLength + "px";
        }
        table[0].style.width = cMax * edgeLength + "px";
    };

    window.removeEventListener('resize', fitScreenSize);
    window.addEventListener('resize', fitScreenSize);
    fitScreenSize();
};