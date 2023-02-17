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
