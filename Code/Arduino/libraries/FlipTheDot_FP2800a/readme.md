# Summary
This library encapsulates the pin initialization, togglings and timings of a FP2800a by providing simple methods to control the 28 source/sink pins of FP2800a chips.

It contains ready made classes to control the following setups:
* ```FlipTheDot_FP2800a```: single standalone FP2800a, who can change between source or sink at any time
* ```FlipTheDot_FP2800a_Fixed```: two FP2800a with fixed data pins, which get enabled depending on the requested action
* ```FlipTheDot_FP2800a_Multi```: like the FlipTheDot_FP2800a but utilize multiple ICs and maps the selected output to the correct IC


# About the FP2800a:
The FP2800a has 5 pins to control which one of the 28 output pins should be used, plus 1 pin for data (source/sink mode) and 1 pin to enable the output.
The selection of the output is done in a binary definition, with low and high levels on the 5 pins. These pins are label in the datasheet as A0, A1, B0, B1 and B2 – which has nothing todo with the Arduino analog Pins A0->Ax.

Only one output can be selected at the same time – like a line decoder. The "data" pin decides if the output should act as source or sink.
By setting this pin low, the selected output will be defined as sink or if high as source to send power from VSS to the connected circuit.
The selected output does nothing until the "enable" pin is set to high. This should be done only for a short time, like 100 micoseconds for a flipdot or longer for LEDs.

## Pinout of the FP2800a (top view):
```
                    ┌────┐ ┌────┐
            COM  x──┤1   └─┘  40├──o  2D
             1D  o──┤2        39├──o  2F
             1F  o──┤3        38├──o  2C
             1C  o──┤4        37├──o  2B
             1B  o──┤5        36├──o  2A
             1A  o──┤6        35├──►  VS
            GND ·|──┤7        34├──o  2E
             1E  o──┤8    F   33├──o  2G
             1G  o──┤9    P   32├──o  3G
             0G  o──┤10   2   31├──o  3E
             0E  o──┤11   8   30├──o  3F
             0F  o──┤12   0   29├──o  3D
             0D  o──┤13   0   28├──o  3C
             0C  o──┤14   a   27├──o  3B
             0B  o──┤15       26├──o  3A
             0A  o──┤16       25├──|· GND
            VCC  ◄──┤17       24├──<  ENABLE
             A1  >──┤18       23├──<  DATA
             A0  >──┤19       22├──<  B1
             A2  >──┤20       21├──<  BO
                    └───────────┘
```

### Symbol Legend:
```
  ├──o  IO
  ├──<  Input
  ├──>  Output
  ├──►  Positiv Voltage
  ├──|· Ground
  ├──x  unused
```

## Operating Conditions:
```
  VCC = Logic supply voltage:  5V
   VS = Power supply voltage:  <=26V
    I = Power supply current:  350mA
  Temperature Range:        :  -40°C to 80°C
  Operating Frequency       :  >= 5Hz
```

## Timing Waveformes:
```
  Output Select Time:                   On/Off:
                      _____  ┆__┆___  |                 ┆ ______ ┆
      Address Lines   ▓▓▓▓▓\/┆  ┆     |                 ┆/      \┆
                      ▓▓▓▓▓/\┆  ┆     |     Enable      ┆        ┆
                      ¯¯¯¯¯  ┆¯¯┆¯¯¯  |         _______/┆        ┆\________
                      _____  ┆__┆___  |                 ┆        ┆
      Data Line       ▓▓▓▓▓\/┆  ┆     |                 ┆        ┆
                      ▓▓▓▓▓/\┆  ┆     |                 ┆  ┆ ____┆____ ┆
                      ¯¯¯¯¯  ┆¯¯┆¯¯¯  |                 ┆  ┆/    ┆    \┆
                             ┆  ┆ __  |     Any Output  ┆  ┆     ┆     ┆
      Enable                 ┆  ┆/    |         ________┆_/┆     ┆     ┆\__
                             ┆  ┆     |                 ┆  ┆     ┆     ┆
                      _______┆_/┆     |                 ┆  ┆     ┆     ┆
                             ┆  ┆     |                 ┆  ┆     ┆     ┆
      µsec max               ┆50┆     |     µsec max    ┆50┆     ┆ 150 ┆
```

# Debugging
Debugging via serial communication can be activated at compile time by adding all or one of the following define statements:
```
#define FlipTheDot_FP2800a_DEBUG_SERIAL Serial
#define FlipTheDot_FP2800aFixed_DEBUG_SERIAL Serial
#define FlipTheDot_FP2800aMulti_DEBUG_SERIAL Serial
```