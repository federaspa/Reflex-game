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
