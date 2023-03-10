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
    //power function
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

        //
        for (int k=3; k>=0; k--){
            t = j/powOur(10,k);
            IOWR_ALTERA_AVALON_PIO_DATA(LUT_HEX[k],LUT[t]);
            //alternative is to use %
            j = j-powOur(10,k)*t; 
            usleep(10000);
        }
    }
    return 0;
}
