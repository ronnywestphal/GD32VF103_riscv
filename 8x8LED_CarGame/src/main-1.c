#include "gd32vf103.h"
#include "drivers-1.h"
#include "dac-1.h"
#include "pwm-1.h"
#include "adc.h"

/* Returns which column the players car is on the 8x8 LED Matrix */
int carPos(int adcrVal)
{
        if (adcrVal>0x0CFF)
            return 128;
        else if (adcrVal>0x0AFF)
            return 64;
        else if (adcrVal>0x08FF)
            return 32;
        else if (adcrVal>0x06FF)
            return 16;
        else if (adcrVal>0x04FF)
            return 8;
        else if (adcrVal>0x02FF)
            return 4;
        else if (adcrVal>0x00FF)
            return 2;
        else  
            return 1;
}   

/* Clears the screen and displays the number of cars the player was able to avoid */
void gameOver(int aScore)
{
        while(1){
            if (t5expq())
            {
                l88row(colset()); 
                for (int i=0; i<8; i++)
                {
                    l88mem(i,0);
                    if (i==4)
                        l88mem(i,aScore);
                }
            }
        }
}

/* Lower the Counter Auto Reload of timer 5 over time to increase the speed of the cars */
void increaseSpeed(int seconds)
{
        if(seconds<30)
            t5fast((seconds)*10);

        else if(seconds<60) 
            t5fast(350);

        else if(seconds<100) 
            t5fast(450);

        else 
            t5fast(500+seconds); 
}

int main(void)
{
    int t5exp=0,        // Used to manage some periodic tasks and starts as a 'millisecond' counter
        t5exp_1000=0,   // Starts as a 'second' counter ('t5exp' and 't5exp_1000' gets shorter over time to increase approaching car speed)
        adcr=0,         // used to read from ADC register before storing the player car position     
        tmp=0,          // temporarily stores the value of other cars in the player car row
        carAvoided=0,   // if a car was avoided: displays other car + player car on the same row
        tmp2=0,         // temporarily holds the value of tmp to display the passed car on the row behind the player car
        score=0;        // final score is in number of cars avoided
   
    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    ADC3powerUpInit(1);                     // Initialize ADC0, Ch3 & Ch16

    while (1) 
    {
        /* Manage Asynchronous events */
        
        /* Player position controlled by a potentiometer */    
        if (adc_flag_get(ADC0,ADC_FLAG_EOC)==SET)   // ADC done?
        { 
            adcr = adc_regular_data_read(ADC0);     // get data
            adcr=carPos(adcr);                      // use data to store the player car position
            adc_flag_clear(ADC0, ADC_FLAG_EOC);     // clear IF
        }
        
        /* Manage periodic tasks */

        if (t5expq()) 
        {                             
            l88row(colset());                       // 8*8LED
            t5exp++;                              
            
            if (tmp)                                // car avoided
            {                             
                carAvoided=adcr+tmp;                // save row data
                l88mem(1,carAvoided);               // display row data
                l88mem(0,0);                        // make sure row behind car is cleared (previous car should already be outside the 8x8)
                tmp2=tmp;                           // store other car position in new tmp to display it behind player car

                if (t5exp==999)                     // right before 8x8 gets shifted
                {
                    score++;                        
                    tmp=0;                          // clear position of the other car to enter the else statement...
                }       
            }

            else                                    // ...next loop around (ms==1000)...
            {
                l88mem(1,adcr);                     // ...display the player car alone (row 1)
                l88mem(0,tmp2);                     // ...display the other car alone (row 0)
            }

            if (t5exp==1000)                        // time to shift the 8x8LED 1 step
            {
                t5exp=0;
                t5exp_1000++;                                
                tmp=flow();                         // shift 8x8led by calling the subroutine flow() from drivers-1.s and store row1 data
                
                if (tmp==adcr)                      // position of other car == player car -> game over
                    gameOver(score);
                
                increaseSpeed(t5exp_1000);                       
            }

            adc_software_trigger_enable(ADC0,       //Trigger another ADC conversion!
                                        ADC_REGULAR_CHANNEL);
        }
    }
}
