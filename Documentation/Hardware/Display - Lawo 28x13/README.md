# Lawo 28x13

This display is one of the various displays made by the company Lawo-Luminator-Europa Mark IV Industries GmbH. Based on its size with only 28x13 dots, it was probably used to show the route number on the side or rear end of the bus.

![front view of a Lawo display with 28x13 flip-dots](Pictures/01%20-%20Lawo%2028x13%20-%2050%20Pin%20-%20Front.jpg)
![back view of a Lawo display with 28x13 flip-dots](Pictures/02%20-%20Lawo%2028x13%20-%2050%20Pin%20-%20Back.jpg)

Driving it requires additional hardware as it only has one FP2800a onboard which has its 28 outputs wired to the column pins of the flip-dot elements. The additional hardware for driving the rows, which could be another FP2800a, needs to be connected via the IDC connector. 

![picture of the IC socket and connector on a Lawo display with 28x13 flip-dots](Pictures/04%20-%20Lawo%2028x13%20-%2050%20Pin%20-%20Interface%20-%20Top%20View.jpg)

Each of the 13 rows actually needs two connections to perform the desired action. The reason or that lies within the construction of each individual dot as they contain two diodes. Driving one flip-dot requires alternating the current flow according to the desired action - set it to the colored face or reset to the black. For example using two FP2800a, the one onboard and an additional one, they need to be controlled in such a way that they alternate between being the source or drain (one always the opposite of the other) and additionally have the correct outputs selected when activated. Speaking about activation, it also must be ensured that the flip-dots receive current only for a very short time to not burn out. Controlling and powering the onboard FP2800a is done thru via some the remaining pins of the IDC connector. The exact pinout can be found in the schematic below.

![schematic of the IC socket and connector on a Lawo display with 28x13 flip-dots](Schematic/Plots/Lawo%2028x13%20Matrix%20Components.svg)

Please also consider having a look at the sub-project called [Flip-The-Dot Lawo 28x13 Controller](https://github.com/RobsyRocket/Flip-The-Dot_Lawo_28x13_Controller) which is about a PCB to ease wiring and controlling such Lawo 28x13 display.
