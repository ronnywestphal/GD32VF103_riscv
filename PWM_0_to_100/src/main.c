#include "gd32vf103.h"
#include "drivers.h"
#include "dac.h"
#include "pwm.h"
#define COL 8
#define ROW 9

void ledPattern(int aOf, int aPat[][COL]){
    for (int i=0,j=aOf; i<COL; i++)
      l88mem(i, aPat[j][i]); 
}
void powerUp(void){
    t5omsi();                               // Initialize timer5 2kHz
    colinit();                              // Initialize column toolbox
    l88init();                              // Initialize 8*8 led toolbox
    keyinit();                              // Initialize keyboard toolbox
    T1powerUpInitPWM(0x3);                  // Timer #1, Ch #0 & 1 PWM
}
int main(void){
    powerUp();                              // Initialize timer5 2kHz

    int speed=0, state=4, key;
    int lookUpTbl[16]={1,4,7,14,2,5,8,0,3,6,9,15,10,11,12,13};
    int pattern[ROW][COL]={
                          {~0x6C,~0x6C,~0x6C,~0x7E,~0x7E,~0x0C,~0x0C,~0x0C},
                          {~0x3C,~0x7E,~0x06,~0x1E,~0x3E,~0x06,~0x7E,~0x3C},
                          {~0x3C,~0x7E,~0x06,~0x3E,~0x7C,~0x60,~0x7E,~0x7E},                        
                          {~0x08,~0x18,~0x38,~0x18,~0x18,~0x18,~0x18,~0x3C},  //Backward                        
                          {0x7C,0x66,0x66,0x7C,0x7C,0x66,0x66,0x7E},          //B
                          {0x08,0x18,0x38,0x18,0x18,0x18,0x18,0x3C},          //Forward
                          {0x3C,0x7E,0x06,0x3E,0x7C,0x60,0x7E,0x7E},
                          {0x3C,0x7E,0x06,0x1E,0x3E,0x06,0x7E,0x3C},
                          {0x6C,0x6C,0x6C,0x7E,0x7E,0x0C,0x0C,0x0C}};

    ledPattern(state, pattern); T1setPWMmotorB(speed=0);
    while (1) {
      if (t5expq()){
        l88row(colset());                    // ...8*8LED and Keyboard  
          if ((key=keyscan())>=0) {          // ...Any key pressed?                                                                                   
            switch (lookUpTbl[key]){
              case 1: if(state<8) {T1setPWMmotorB(speed+=25); state++; ledPattern(state, pattern);} break;          
              case 2: T1setPWMmotorB(speed=0); state=4; ledPattern(state, pattern); break;
              case 3: if(state>0) {T1setPWMmotorB(speed-=25); state--; ledPattern(state, pattern);} break;
              default: break;
            }                  
          }
      }
    }
}