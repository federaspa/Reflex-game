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
