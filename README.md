# Management of peripherals {#sec:mgt_per}

## R1. From this system.h file, list all the peripherals of this microcontroller (excluding memories). {#r1.-from-this-system.h-file-list-all-the-peripherals-of-this-microcontroller-excluding-memories. .unnumbered}

We started by checking the `system.h` file inside our folder: this
contains the SOPC Builder system and BSP software package information.
In the following table we paste all the addresses we can use, but after
including this file in our main, we will just call each element by
constant name directly.

  Peripheral          Address
  ------------------- -----------
  ALT_IRQ_BASE        NULL
  ALT_LOG_PORT_BASE   0x0
  ALT_STDERR_BASE     0x4041128
  ALT_STDIN_BASE      0x4041128
  ALT_STDOUT_BASE     0x4041128
  HEX0_BASE           0x40410a0
  HEX1_BASE           0x4041080
  HEX2_BASE           0x4041060
  HEX3_BASE           0x4041040
  HEX4_BASE           0x4041020
  HEX5_BASE           0x4041000
  JTAG_UART_BASE      0x4041128
  KEY_BASE            0x4041110
  LEDS_BASE           0x40410c0
  SWITCHES_BASE       0x4041100
  SYSID_QSYS_BASE     0x4041120
  TIMER_BASE          0x40410e0

## C1. Test the values on the switches {#c1.-test-the-values-on-the-switches .unnumbered}

We then started by using the reading function:

        data = IORD_ALTERA_AVALON_PIO_DATA (address)

And checked the results by trying moving some switches and printing it
directly to the serial monitor (Fig.
[1](#fig:switches_results){reference-type="ref"
reference="fig:switches_results"}):

[]{#fig:switches_results .image .placeholder
original-image-src="pictures/Switch value.png"
original-image-title="fig:" width="90%"}

As we can see, the result is the decimal number corresponding to the
binary number where 1 is \"switch on\" and 0 is \"switch off\". For
example, if we select the 4th switch corresponding to sequence
`0000001000`, we get a reading of \"8\" from the base address of the
switches

    #include <stdio.h>
    #include "altera_avalon_pio_regs.h"
    #include <unistd.h>
    #include "system.h"

    int main()
    {
      printf("Hello from Nios II!\n");
      while (1){
          int data = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
          printf("Received: %d",data);
          usleep(500000); //half a second
      }
      return 0;
    }

## R2. Fill in the following display table {#r2.-fill-in-the-following-display-table .unnumbered}

We first tried to create the binary combination corresponding to the
scheme in the annex. We then noticed that it didn't light up the
segments we wanted. Because of this, we checked the actual disposition
in our board (considering the element in the first position as \"0\" in
the scheme, the second as the number \"1\", ecc..). The new reference
disposition is shown in Fig.
[2](#fig:7-segments-display){reference-type="ref"
reference="fig:7-segments-display"}

[The new reference table working in our case. In the left we can see the
old version and in the right the new one]{#fig:7-segments-display .image
.placeholder original-image-src="pictures/schema.png"
original-image-title="fig:" width="40%"}

Furthermore we noticed that to turn on a segment of the BCD display the
binary squence needed to be inverted (for example, to have an \"8\" we
had to put `0000000` which is `0x00` in hexadecimal). While working on
excercise C.4, we also added the binary configuration for the character
\"-\".

  Digit   Binary configuration   Hexa configuration
  ------- ---------------------- --------------------
  0       1000000                40
  1       1111001                79
  2       0100100                24
  3       0110000                30
  4       0011001                19
  5       0010010                12
  6       0000010                02
  7       1111000                78
  8       0000000                00
  9       0010000                10
  \-      0111111                3F

## C2. Test by sending the value of a counter from 0 to 9999 on the 4 7-segment displays. {#c2.-test-by-sending-the-value-of-a-counter-from-0-to-9999-on-the-4-7-segment-displays. .unnumbered}

    #include <stdio.h>
    #include "altera_avalon_pio_regs.h"
    #include <unistd.h>
    #include "system.h"

    int LUT[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    int LUT_HEX[6] = {HEX0_BASE,HEX1_BASE,HEX2_BASE,HEX3_BASE,HEX4_BASE,HEX5_BASE};

    void resetBCD(){
        for(int i=0; i<6; i++) {
            //function to turn off a display (for our display 1111111 -> 7F)
            IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[i],0x7F);
        }
    }

    int powOur(int base, int exp){
        //simply computes the power
        int res = 1;
        for(int i=0; i<exp;i++) res*=base;
        return res;
    }

    int main()
    {
      printf("Hello from Nios II!\n");
      resetBCD();
      int i = 0;
      for (i; i<10000; i++){    //loop from 0 to 9999
          int j = i;
          int t;

          //here a way we thought to select one digit for each iteration
          for (int k=3; k>=0; k--){
              t = j/powOur(10,k);
              IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[k],LUT[t]);
              j = j-powOur(10,k)*t; //alternative is to use %
              usleep(10000);
          }
      }
      return 0;
    }

The result of our counter (to also prove our 7-segment display LUT
working) is shown in Fig. [3](#fig:counter_result){reference-type="ref"
reference="fig:counter_result"}:

[Our counter going from 0000 to 9999]{#fig:counter_result .image
.placeholder original-image-src="pictures/res.jpeg"
original-image-title="fig:" width="50%"}

Defining our counter this way allowed us to reuse it with minimum
changes in the following exercises in order to display any number on the
7 segments display.

# Programming interrupts

## R3. Recall the advantages and disadvantages of each method. {#r3.-recall-the-advantages-and-disadvantages-of-each-method. .unnumbered}

  Access method   Pros and cons
  --------------- ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  Polling         In this case the processor reads at regular intervals. It is mainly inconvenient to opt for this solution, because it uses processor power even if not needed and it makes it less reactive, since it is synchronous. An advantage could be that we have full control of the program, that never exits and goes in a ISR and then it is more predictable and consistent in real-time applications
  Interruptions   Since it is an asynchronous event, the program is more responsive: it allows the system to immediately respond to events or data as soon as they occur. Possible cons cab be an increased overhead (as the system must continuously manage and prioritize the handling of incoming interrupts) and complexity

## R4. What is the role of the following prefixes, often used in embedded systems: volatile on the status variable of the push buttons, static on the interrupt routine? {#r4.-what-is-the-role-of-the-following-prefixes-often-used-in-embedded-systems-volatile-on-the-status-variable-of-the-push-buttons-static-on-the-interrupt-routine .unnumbered}

  Prefix     Role
  ---------- ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  Volatile   The \"volatile\" keyword is used to indicate that a variable might be changed by multiple sources, including hardware and other software, and therefore must be read from memory every time it is accessed. In the context of a status variable for push buttons, it indicates that the value of the variable can change outside the control of the software, for example, due to the button being pressed. Using the \"volatile\" keyword ensures that the latest value of the variable is always used, and that the compiler does not optimize access to it
  Static     The \"static\" keyword is used to declare a function or a variable with local scope that has permanent storage duration. In the context of an interrupt routine, declaring it as \"static\" means that the routine will not be visible outside of the source file it is defined in and will have a local scope. This can be useful in managing the scope of interrupt routines and avoiding naming conflicts with other functions or variables

## C3. Write a code that indicates the number of the button pressed on the 7-segment displays. The red LEDs should light up to indicate the switches in the up position. This switch configuration will be used to program the waiting time in the main loop (the more switches the user lifts, the longer the maximum waiting time is). {#c3.-write-a-code-that-indicates-the-number-of-the-button-pressed-on-the-7-segment-displays.-the-red-leds-should-light-up-to-indicate-the-switches-in-the-up-position.-this-switch-configuration-will-be-used-to-program-the-waiting-time-in-the-main-loop-the-more-switches-the-user-lifts-the-longer-the-maximum-waiting-time-is. .unnumbered}

    #include <stdio.h>
    #include "system.h"
    #include "altera_avalon_pio_regs.h"
    #include "alt_types.h"
    #include <unistd.h>

    volatile int edge_capture;

    int LUT[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    int LUT_HEX[6] = {HEX0_BASE,HEX1_BASE,HEX2_BASE,HEX3_BASE,HEX4_BASE,HEX5_BASE};

    int powOur(int base, int exp){
        int res = 1;
        for(int i=0; i<exp;i++) res*=base;
        return res;

    }

    void resetBCD(){
        for(int i=0; i<6;i++) {
            IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[i],0x7F);
        }
    }

    void printBCD(int num){
          int j = num;
          int t;
          for (int k=3; k>=0; k--){
              t = j/powOur(10,k);
              IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[k],LUT[t]);
              j = j-powOur(10,k)*t;
              usleep(10000); //to be commented
          }
    }

    static void handle_button_interrupts(void* context, alt_u32 id)
    {
        /* Cast context to edge_capture's type. It is important that this
        be declared volatile to avoid unwanted compiler optimization. */
        volatile int* edge_capture_ptr = (volatile int*) context;
        /* Read the edge capture register on the button PIO. Store value. */
        *edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
        /* Write to the edge capture register to reset it. */
        IOWR_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE, 0);
        /* Read the PIO to delay ISR exit. This is done to prevent a
        spurious interrupt in systems with high processor -> pio
        latency and fast interrupts. */
        IORD_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE);
    }



    static void init_button_pio()
    {
        /* Recast the edge_capture pointer to match the alt_irq_register() function
        prototype. */
        void* edge_capture_ptr = (void*) &edge_capture;
        /* Enable all 4 button interrupts. */
        IOWR_ALTERA_AVALON_PIO_IRQ_MASK (KEY_BASE, 0xf);
        /* Reset the edge capture register. */
        IOWR_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE, 0x0);
        /* Register the ISR. */
        alt_irq_register( KEY_IRQ, edge_capture_ptr, handle_button_interrupts );
    }

    int countOnes(int decimal) {
        int count = 0;
        while (decimal) {
            if (decimal & 1) {
                count++;
            }
            decimal >>= 1;
        }
        return count;
    }

    int main()
    {
      printf("Hello from Nios II!\n");
      init_button_pio();
      resetBCD();

      while (1){
          int data = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
          IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE,data);
          printf("numero su: %i\n", countOnes(data));

          if (edge_capture!=0){
              switch(edge_capture){
                  case 1:
                      printBCD(1);
                      break;
                  case 2:
                      printBCD(2);
                      break;
                  case 4:
                      printBCD(3);
                      break;
                  case 8:
                      printBCD(4);
                      break;
              }
              edge_capture=0;
          }
          usleep(100000*countOnes(data));

      }


      return 0;
    }

Our code displays the button pressed on the 7 segments display, and the
LEDs light up based on the corresponding switch's position (Fig.
[4](#fig:button_result){reference-type="ref"
reference="fig:button_result"})

[An example with button 2 pressed and some switches
up]{#fig:button_result .image .placeholder
original-image-src="pictures/button press switch.jpeg"
original-image-title="fig:" width="50%"}

# Reflex game

## C4. Now that the peripherals are programmable, create a code that meets the following specifications: {#c4.-now-that-the-peripherals-are-programmable-create-a-code-that-meets-the-following-specifications .unnumbered}

-   To start the test, the user must press button 1,

-   All red LEDs will then light up after a random time,

-   During this time the red LEDs blink regularly (even and then odd
    LEDs\...),

-   The user should press button 4 (left) as quickly as possible,

-   The system measures the user's reaction time between switching on
    and pressing the button, and displays this value: seconds and
    milliseconds on the 7 segments. Use the timer device for this
    purpose (appendix 4).

-   Switches are used to provide the maximum waiting time before
    ignition,

-   A new reflex test is started by pressing button 1,

-   By pressing button 2, the system displays the average reaction time
    since the beginning of the tests.

```{=html}
<!-- -->
```
    #include <stdio.h>
    #include "system.h"
    #include "altera_avalon_pio_regs.h"
    #include "altera_avalon_timer_regs.h"
    #include "alt_types.h"
    #include <unistd.h>
    #include <stdbool.h>

    //#include "sys/alt_timestamp.h"

    volatile int edge_capture;

    int LUT[10] = {0x40, 0x79, 0x24, 0x30, 0x19, 0x12, 0x02, 0x78, 0x00, 0x10};
    int LUT_HEX[6] = {HEX0_BASE,HEX1_BASE,HEX2_BASE,HEX3_BASE,HEX4_BASE,HEX5_BASE};

    //our function to compute the power
    int powOur(int base, int exp){
        int res = 1;
        for(int i=0; i<exp;i++) res*=base;
        return res;

    }

    //to switch off all the 7 segments
    void resetBCD(){
        for(int i=0; i<6;i++) {
            IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[i],0x7F);
        }
    }

    //our function to print the number split
    void printBCD(int num){
          int j = num;
          int t;

          //here we write to the fourth display
          t = j/powOur(10,3);
          IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[4],LUT[t]);
          j = j-powOur(10,3)*t;

          //here we add a dash in between
          IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[3],0x3F);

          //here we print the milliseconds
          for (int k=2; k>=0; k--){
              t = j/powOur(10,k);
              IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[k],LUT[t]);
              j = j-powOur(10,k)*t;
          }
    }

    //interrupt handling for the buttons
    static void handle_button_interrupts(void* context, alt_u32 id)
    {
        /* Cast context to edge_capture's type. It is important that this
        be declared volatile to avoid unwanted compiler optimization. */
        volatile int* edge_capture_ptr = (volatile int*) context;
        /* Read the edge capture register on the button PIO. Store value. */
        *edge_capture_ptr = IORD_ALTERA_AVALON_PIO_EDGE_CAP(KEY_BASE);
        /* Write to the edge capture register to reset it. */
        IOWR_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE, 0);
        /* Read the PIO to delay ISR exit. This is done to prevent a
        spurious interrupt in systems with high processor -> pio
        latency and fast interrupts. */
        IORD_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE);
    }


    static void init_button_pio()
    {
        /* Recast the edge_capture pointer to match the alt_irq_register() function
        prototype. */
        void* edge_capture_ptr = (void*) &edge_capture;
        /* Enable all 4 button interrupts. */
        IOWR_ALTERA_AVALON_PIO_IRQ_MASK (KEY_BASE, 0xf);
        /* Reset the edge capture register. */
        IOWR_ALTERA_AVALON_PIO_EDGE_CAP (KEY_BASE, 0x0);
        /* Register the ISR. */
        alt_irq_register( KEY_IRQ, edge_capture_ptr, handle_button_interrupts );
    }

    //function to count the number of switches up from the decimal number received
    int countOnes(int decimal) {
        int count = 0;
        while (decimal) {
            if (decimal & 1) {
                count++;
            }
            decimal >>= 1;
        }
        return count;
    }

    int main(){
      int sum = 0; //to keep track of the average
      int try = 0;
      srand(time(NULL));


      init_button_pio();
      resetBCD();
      alt_timestamp_start();

      printf("-----Starting program!-----\n");
      while (1){
          //here we turn the led corresponding to the switches up
          int data = IORD_ALTERA_AVALON_PIO_DATA(SWITCHES_BASE);
          IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE,data);

          switch(edge_capture){
              case 1: //if press one it starts the game
              {
                  int max_time = countOnes(data)+1; //we add 1 to the max_time to avoid to have 0 as timing
                  int random = rand() % max_time +1;
                  printf("%i\n", random);

                  int counter=0;
                  while (counter/(1000000/200000) < random){
                      //here we switch between the 2 configurations (341 and 682)
                      IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE, 341*powOur(2,counter%2==0) );
                      counter++;
                      usleep(200000);
                  }

                  //turns on the leds and takes the starting time
                  IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE,1023);
                  long int start_time = alt_timestamp();

                  edge_capture=0; //in case someone pressed the 4th button before

                  while(edge_capture!=8){
                      //we wait for the user to press (only 10us inside while)
                      usleep(10);
                  }

                  //We want result in milliseconds
                  long int delay = (alt_timestamp() - start_time)/(alt_timestamp_freq()/1000);
                  printf("milliseconds: %i\n", delay);
                  printBCD(delay);
                  IOWR_ALTERA_AVALON_PIO_DATA(LEDS_BASE,0);

                  //keeping track of sum and trials to compute the average
                  sum += delay;
                  try++;
                  break;
              }
              case 2: //prints the average
              {
                  printBCD(sum/try);
                  break;
              }
              case 4: //added feature to clear the display
              {
                  resetBCD();
                  break;
              }
          }
      }
      usleep(100000);
    }

Our code properly starts the game when button 1 is pressed, blinks the
LEDs for a random ammount of time proportional to the number of
flipped-up switches and then lights all the LEDs. After it registers the
button 4 being pressed by the player, it displays the time elapsed as
`seconds - milliseconds`.\
Pressing button 2 properly displays the average between trials.\
As an additional feature, pressing button 3 resets the display.

  Time (s,ms)   Trial 1   Trial 2   Trial 3   Trial 4   Trial 5
  ------------- --------- --------- --------- --------- ---------
  User 1        0.332     0.343     0.311     0.273     0.284
  User 2        0.310     0.272     0.273     0.284     0.307

[A reaction time displayed on the 7 segment display]{#fig:reflex_result
.image .placeholder original-image-src="pictures/display dash.jpeg"
original-image-title="fig:" width="70%"}

