#include "gd32vf103.h"
#include "drivers-1.h"
#include "dac-1.h"
#include "pwm-1.h"
#include "adc.h"





int carPos(int adcrVal){
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
void gameOver(int aPasses){
        while(1){
            if (t5expq()){
                l88row(colset()); 
                for (int i=0; i<8; i++){
                    l88mem(i,0);
                    if (i==4)
                        l88mem(i,aPasses);
                }
            }
        }
}
void increaseSpeed(int seconds){
        if(seconds<30)
            t5fast((seconds)*10);

        else if(seconds<60) 
            t5fast(350);

        else if(seconds<100) 
            t5fast(450);

        else 
            t5fast(500+seconds); 
}
int main(void){
    int ms=0,
        s=0,
        adcr=0,
        carPass=0,
        tmp=0,
        tmp2=0,
        passes=0;
   
    t5omsi();                               // Initialize timer5 1kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    ADC3powerUpInit(1);                     // Initialize ADC0, Ch3 & Ch16

    while (1) {
                                            // Manage Asynchronous events
        if (adc_flag_get(ADC0,ADC_FLAG_EOC)==SET) { // ...ADC done?
            adcr = adc_regular_data_read(ADC0);     // ......get data
            adcr=carPos(adcr);               
            adc_flag_clear(ADC0, ADC_FLAG_EOC);     // ......clear IF
        }
        if (t5expq()) {                             // Manage periodic tasks
            l88row(colset());                       // ...8*8LED and Keyboard
            ms++;                                   // ...One millisecond
            if (tmp){
                carPass=adcr+tmp;             
                l88mem(1,carPass);
                l88mem(0,0);
                tmp2=tmp; 
                if (ms==999) {
                    passes++;
                    tmp=0;
                }       
            }
            else {
                l88mem(1,adcr);
                l88mem(0,tmp2);
            }
            if (ms==1000){
                ms=0;s++;                                
                tmp=flow();
                if (tmp==adcr)
                    gameOver(passes);
                
                increaseSpeed(s);                       
            }
            adc_software_trigger_enable(ADC0, //Trigger another ADC conversion!
                                        ADC_REGULAR_CHANNEL);
        }
    }
}
