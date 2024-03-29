#include "gd32vf103.h"
#include "drivers.h"
#include "dac.h"


/* Changes the values in Counter Auto Reload of Timer 5 */
void switchFreq(int aLook[],int aKey){
    switch(aLook[aKey]){
        case 0:  t5freq(227.2); break;
        case 1:  t5freq(381.6); break;
        case 2:  t5freq(361.0); break;
        case 3:  t5freq(340.1); break;
        case 4:  t5freq(321.5); break;
        case 5:  t5freq(303.0); break;
        case 6:  t5freq(286.5); break;
        case 7:  t5freq(270.2); break;
        case 8:  t5freq(255.1); break;
        case 9:  t5freq(240.9); break;
        case 10: t5freq(214.5); break;
        case 11: t5freq(202.4); break;
        case 12: t5freq(191.2); break;
        case 13: t5freq(178.0); break;
        case 14: t5freq(151.7); break;
        case 15: t5freq(143.2); break;
        default: break;
    }
}

void powerUp(void){
    t5omsi();                               // Initialize timer5 1kHz
    keyinit();                              // Initialize keypad
    DAC0powerUpInit();                      // Initialize DAC0/PA4 toolbox
}

int main(void)
{
    powerUp();

    int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13},                  // translates the key values to what is displayed on the keypad
        sine[10]={0x7FF,0xAFF,0xDFF,0xFFF,0xDFF,0xAFF,0x5FF,0x2FF,0x00,0x3FF},  // the shape of the wave... (only positive values)
        key=-1,
        i=0;

    while (1) {     

        /* Manage Asynchronous Events */

        if ((key=keyscan())>=0)         // Key pressed?
        {           
            switchFreq(lookUpTbl,key);
            key=-1;
        }        

        /* Manage periodic tasks */

        if (t5expq())                   // Timer 5 controls the frequency... 
        {                               // ..."draws" one point in the wave each time timer 5 resets
            DAC0set(sine[i++]);         // Convert to analog
            if (i==10)
                i=0; 
        }


    }
}