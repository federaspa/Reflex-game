# Management of peripherals 

## Peripherals available

Let us check the `system.h` file and list all the peripherals available to us.
The table shows each peripheral's address, but in the code each element will be called directly with its name.

|     Peripheral    	|  Address  	|
|:-----------------:	|:---------:	|
| ALT_IRQ_BASE      	|    NULL   	|
| ALT_LOG_PORT_BASE 	|    0x0    	|
| ALT_STDERR_BASE   	| 0x4041128 	|
| ALT_STDIN_BASE    	| 0x4041128 	|
| ALT_STDOUT_BASE   	| 0x4041128 	|
| HEX0_BASE         	| 0x40410a0 	|
| HEX1_BASE         	| 0x4041080 	|
| HEX2_BASE         	| 0x4041060 	|
| HEX3_BASE         	| 0x4041040 	|
| HEX4_BASE         	| 0x4041020 	|
| HEX5_BASE         	| 0x4041000 	|
| JTAG_UART_BASE    	| 0x4041128 	|
| KEY_BASE          	| 0x4041110 	|
| LEDS_BASE         	| 0x40410c0 	|
| SWITCHES_BASE     	| 0x4041100 	|
| SYSID_QSYS_BASE   	| 0x4041120 	|
| TIMER_BASE        	| 0x40410e0 	|

## Test switches values

By using the reading function:

        data = IORD_ALTERA_AVALON_PIO_DATA (address)

We can check each switch's value by changing them and printing the result directly to the serial monitor.

Our result is the decimal conversion of a binary number where 1 is "switch on" and 0 is \"switch off\". \
For example, by turning on the 4th switch, we get a reading of \"8\", corresponding to sequence `0000001000`.

![Switch_value](https://github.com/federaspa/Reflex-game/blob/main/Images/Switch%20value.png)

## Building the display table

We first tried to create the binary combination corresponding to a default scheme (on the left). We then noticed that it didn't light up the
segments we wanted. After checking the actual disposition
in our board (considering the element in the first position as \"0\" in
the scheme, the second as \"1\", ecc..), we built the new scheme (on the right).

![Schema](https://github.com/federaspa/Reflex-game/blob/main/Images/schema.png)

Furthermore we noticed that to turn on a segment of the BCD display the
binary squence needed to be inverted (for example, to have an \"8\" we
had to put `0000000` which is `0x00` in hexadecimal). In order to implement a functionality described later, we also added the binary configuration for the character
\"-\".

| Digit 	| Binary configuration 	| Hexadecimal configuration 	|
|:-----:	|:--------------------:	|:--------------------:	|
| 0     	|        1000000       	| 40                 	|
| 1     	|        1111001       	| 79                 	|
| 2     	|        0100100       	| 24                 	|
| 3     	|        0110000       	| 30                 	|
| 4     	|        0011001       	| 19                 	|
| 5     	|        0010010       	| 12                 	|
| 6     	|        0000010       	| 02                 	|
| 7     	|        1111000       	| 78                 	|
| 8     	|        0000000       	| 00                 	|
| 9     	|        0010000       	| 10                 	|
| \-    	|        0111111       	| 3F                 	|

## Testing the reading and the 7-segment display

In order to familiarize ourselves with the board's functionalities, we coded a counter from 0 to 9999 and displayed it on the 7-segment display.

We build two look up tables:
* The first one contains the hexadecimal congiguration for digit `i` at the `i`th position.
* The second one contains the addresses of the 7-segment display

The result of our code also proves our 7-segment display LUT
working.

<img src="https://github.com/federaspa/Reflex-game/blob/main/Images/res.jpeg" width=50%>

Defining our counter this way allowed us to reuse it with minimum
changes in the following exercises in order to display any number on the
7 segments display.

# Programming interrupts

## Coding basic features
We start by writing a code that:
* Indicates the number of the button pressed on the 7-segment displays.
* Lights up the red LEDs to indicate the switches in the up position. 

This switch configuration will be used to program the waiting time in the main loop (the more switches the user lifts, the longer the maximum waiting time is).

Our code displays the button pressed on the 7 segments display, and the
LEDs light up based on the corresponding switch's position .

![switch_button](https://github.com/federaspa/Reflex-game/blob/main/Images/button%20press%20switch.jpeg)

# Reflex game

## Coding the reflex game
Now that everything has been tested and the peripherals are programmable, we can create our code.\
It will incude the following features: 

-   To start the game, the user must press button 1,

-   The LEDs will then start blinking for a random amount of time

-   The user can decide the maximum waiting time by flipping up more switches (only number of switches up matters, not position),

-   When the waiting time is over, all the LEDs will light up simultaneously,

-   The user should press button 4 (left) as quickly as possible,

-   The system measures the user's reaction time and displays its his value  in the seven-segment display in the format `seconds-microseconds`,


-   A new reflex test is started by pressing button 1,

-   By pressing button 2, the system displays the average reaction time since the beginning of the game,

-   By pressing button 3, the display is reset.

Below are displayed some recordings of a playtest and an image of the displayed result:

![final](https://github.com/federaspa/Reflex-game/blob/main/Images/display%20dash.jpeg)

| Time (s,ms) 	| Trial 1 	| Trial 2 	| Trial 3 	| Trial 4 	| Trial 5 	|
|:-----------:	|:-------:	|:-------:	|:-------:	|:-------:	|:-------:	|
|    User 1   	|  0.332  	|  0.343  	|  0.311  	|  0.273  	|  0.284  	|
|    User 2   	|  0.310  	|  0.272  	|  0.273  	|  0.284  	|  0.307  	|
