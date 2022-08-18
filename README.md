# GD32VF103_riscv


## Overview
Drivers and some projects written in C and Assembly using the PlatformIO development platform in Visual Studio Code. 
<p align="center">
<img src="https://user-images.githubusercontent.com/84048902/185358460-77ec5ef7-c7b7-4ef8-a25d-3e9d5236b8e8.jpg" height="450" />
</p>

## Periodic Tasks
The core clock speed is 108Mhz and Timer 5 is used to handle periodic tasks every millisecond in the superloop. 
```
li t1, 0x06C        // ...set prescaler to
sw t1, PSC(t0)      //    divide by 108...
li t1, 0x3E8        // ...and then counter auto reload to
sw t1, CAR(t0)      //    reload at 1000 for a 1ms counter..
```
