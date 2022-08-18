# GD32VF103_riscv


# Periodic Tasks
The core clock speed is 108Mhz and Timer 5 is used to handle periodic tasks every millisecond in the superloop. 
```
li t1, 0x06C        // ...set prescaler to
sw t1, PSC(t0)      //    divide by 108...
li t1, 0x3E8        // ...and then counter auto reload to
sw t1, CAR(t0)      //    reload at 1000 for a 1ms counter..
```
