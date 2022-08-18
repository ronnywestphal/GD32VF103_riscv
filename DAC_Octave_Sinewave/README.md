## Sine Wave To Audio
The tones are genereted using a 4x4 keypad where each button corresponds to a frequency of a hard coded sine wave. 
The desired frequencies are achieved by manipulating how fast the superloop handles the periodic tasks by using the keypad to change the values in Counter Auto Reload. Values between 143 and 227 corresponds to frequencies between 4.4 and 7.5KHz.

The sine wave consists of ten 12bit numbers in an array. They are converted to analog signals using one of the mcu's digital to analog converters, DAC0, which is connected to the audio amplifier on the board. 
´´´C
void DAC0set(int value){
       dac_data_set(DAC0, DAC_ALIGN_12B_R, value);     // Output = Vref * DAC_DO / 0xFFF
}
´´´
The values in the array ranges from 0 to 4096 which results in an output of 0 to 3.2768 V.  

