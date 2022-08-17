## Sine Wave To Audio
The tones are genereted using a 4x4 keyboard where each button corresponds to a frequency of a hardcoded sinewave. 
The core clock speed is 108MHz so to achieve frequencies around 4.5-7KHz, the pre scaler of timer 5 is set to 108, then the values in Counter Auto Reload, which is controlled by the keyboard, ranges between 143 and 227. 
