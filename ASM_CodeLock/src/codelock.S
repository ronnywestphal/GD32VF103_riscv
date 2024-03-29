.section .text
/////////////////////////////////////////////////////////////////////////////////////
// GPIO General Purpose I/0 Subroutine package                         0.91 AC
/////////////////////////////////////////////////////////////////////////////////////
.equ GPIOA,  0x40010800     // GPIOB Base address.
.equ GPIOB,  0x40010C00     // GPIOB Base address.
.equ CTR0,   0x00           // Port control register 0 offset
.equ ISTAT,  0x08           // Port input status register
.equ OCTL,   0x0C           // Port output control register
.equ BOP,    0x10           // Port bit operate register offset
.equ BC,     0x14           // Port bit clear register offset
.equ GPIO_AM, 0x0           // Mode: Input, Analog Mode
.equ GPIO_50, 0x3           // Mode: Output, max speed 50MHz
.equ GPIO_FI, 0x4           // Conf: (Input) Floating input
.equ GPIO_UD, 0x8           // Conf: (Input) Pull-up/-down
.equ GPIO_PP, 0x0           // Conf: (Output) Push-pull
.equ GPIO_OD, 0x4           // Conf: (Output) Open-drain

// GPIOI Initialize ////////////////////////////////////////////////// 1.0 / AC ////
// a0 Targeted GPIO-module
// a1 Configuration bits (<<2)
// a2 Mode bits
// a3 Targeted pin
gpioi:  mv t0, a0           // t0 points to GPIOX...
        mv t4, a3           //    t4 marks selected pin...
        li t1,0x8           //    Use port control register 0 or 1,
        blt t4,t1, 1f       //    thus is pin > 7?
        addi t0, t0, 4      //       Yes, adjust base address,
        addi t4,t4,-8       //            and adjust selected pin.

1:      slli t4,t4,2        //    Multiply pin nb by 4 to get steps to shift!
        li t1, 0xF          //    Create inverted configuration mask...
        sll t1, t1, t4      //    ...and move it into right position.
        not t1, t1          //    ...invert it back, then...
        mv t2, a1           //    Create configuration bits...
        or t2, t2, a2       //    
        sll t2, t2, t4      //    ...and move it into right position.

        lw t3,CTR0(t0)      //    retreive port control register 0(/1)...
        and t3,t3,t1        //    ...clear out selected pin's config/mode bits...
        or t3, t3, t2       //    ...configure new config/mode bits...
        sw t3,CTR0(t0)      //    ...and write it back.li t2,PBEN          // offset to GPIOB
        ret                 //  then go home!

// GPIOBO Bit Operations ///////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 Targeted pattern
// a2 Targeted pin(s)
gpiobo: xor a2, a2, a1      // Find zeros in targeted bits...
        slli a2, a2, 16     // ...position clear requests...
        or a1, a1, a2       // ...add set requests...
        sw a1, BOP(a0)      // ...then execute...
        ret                 // ...and return to caller!

// GPIBC Bit Clear /////////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 Targeted pin(s)
gpiobc: sw a1, BC(a0)       // Execute request...
        ret                 // ...and return to caller!

// GPIOC Output Control ////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 value
gpiooc: sw a1, OCTL(a0)      // Execute request...
        ret                 // ...and return to caller!

// GPIOIS Input Status //////////////////////////////////////////////// 1.0 / AC ///
// a0 Targeted GPIO-module
// a1 read value
gpiois: lw a1, ISTAT(a0)    // Execute request...
        ret                 // ...and return to caller!

// GPIO General Purpose I/0 Subroutine package ////////////////////////// END ///////

/////////////////////////////////////////////////////////////////////////////////////
// RCU Reset and Clock Unit Subroutine package                         0.91 AC
.equ RCU,    0x40021000
.equ APB1EN, 0x1C
.equ T5EN,   0x10
.equ APB2EN, 0x18
.equ PAEN,   0x4
.equ PBEN,   0x8
// RCU1EN Enabel APB1 bus devices //////////////////////////////////// 1.0 / AC ///
rcu1en: li t0,RCU           // t0 points to RCU...
        lw t1,APB1EN(t0)    // ...retrieve current configuration...
        or t1,t1,a0         // ...and update the configuration...
        sw t1,APB1EN(t0)    // ...then save back the updated config...
        ret                 // and go home!

// RCU2EN Enabel APB2 bus devices //////////////////////////////////// 1.0 / AC ///
rcu2en: li t0,RCU           // t0 points to RCU...
        lw t1,APB2EN(t0)    // ...retrieve current configuration...
        or t1,t1,a0         // ...and update the configuration...
        sw t1,APB2EN(t0)    // ...then save back the updated config...
        ret                 // and go home!
        
// RCU Reset and Clock Unit Subroutine package ////////////////////////// END /////// 


// 4 nr lock 
.section .data
buf:    .byte   0x01,0x03,0x03,0x07

buf1:   .byte   0x00,0x01,0x02,0x03 

.section .text
.global main
main:   
        li a0, PBEN+PAEN    // Prepare to turn on GPIOB...
        call rcu2en         // ...do the set-up!

        li a0, GPIOB        // Prepare to init GPIOB...
        li a1, GPIO_PP      // ...as Push-Pull Output...
        li a2, GPIO_50      // ...with max 50MHz rate...
        li a3, 0            // ...for pin 0...
        li a4, 3            // ...to pin 2...
1:      call gpioi          // ...do the set-up!
        addi a3,a3,1        // ...advance to next pin...
        bne a3,a4, 1b       // ...continue, if not done!  

        li a0, GPIOA        // Prepare to init GPIOA...
        li a1, GPIO_AM      // ...in Analoge mode...
        li a2, GPIO_FI      // ...with High-Z...
        li a3, 5            // ...for pin 5...
        li a4, 9            // ...to pin 8...
1:      call gpioi          // ...do the set-up!
        addi a3,a3,1        // ...advance to next pin...
        bne a3,a4, 1b       // ...continue, if not done!   


restart:
        li t3,4                 // counts down for every attempted pattern
        li t5,4                 // counts down for every correct pattern entered
        la t0,buf1              // Leds indicate which pattern to enter (1-3)
        la t1,buf               // Lock Patterns 

start:                                       
        lw t2,0(t0)             // load which pattern (1-3) to t2
        li a0,GPIOB             
        mv a1,t2 
        li a2,0x07
        call gpiobo             

        li a0,GPIOA
1:
        call gpiois             // get user input value 
        srli a1,a1,5            // shift it down 5 steps (A8-A5 to A3-A0)
        andi a1,a1,0x08         // mask out A8 value
        li t4,0x0FFF            // Delay counter
        bnez a1,A8on            // wait for user to confirm the pattern by turning on A8
        
        j 1b                    // Repeat until switch A8 is turned on 

A8on:   
        addi t4,t4,-1           // Delay to avoid registering wrong values on A8 
        bnez t4,A8on             
        li t4,0x0FFF            // reset delay counter 
2:                                 
        call gpiois             // get user input value 
        srli a1,a1,5            // shift it down 5 steps (A8-A5 to A3-A0)
        andi a2,a1,0x08         // mask out A8 value
        andi a1,a1,0x07         // mask out pattern...
        mv t2,a1                // store pattern in t2
        beqz a2,A8off            
        
        j 2b                    // ...repeat until switch A8 is turned off 


A8off:  
        addi t4,t4,-1           // delay loop2
        bnez t4,A8off      

        lb t4,0(t1)             // t4=value of buf address (lock pattern)      

        addi t1,t1,1            // Lock pattern address += 1 (next lock pattern)
        addi t0,t0,1            // Light pattern address += 1 (1-3)
        addi t3,t3,-1           // Pattern counter -= 1

        sub t2,t2,t4            // Compare pattern in buf with input pattern
        bnez t2,2f              // branch forward to label 2 if pattern is incorrect...
        addi t5,t5,-1           // t5 -= 1 if correct pattern

2:      
        bnez t3,start           // branch to start if all patterns are not completed 

1:      
        bnez t5,2f              // branch forward to 2: if any pattern was incorrect.
        li a0,GPIOB
        li a1,0x07              // All lights on (lock is unlocked)
        li a2,0x07
        call gpiobo
        j 1b 

2:      
        li a0, GPIOB
        li a1,0x00              // All lights off (lock remains locked)
        li a2,0x07
        call gpiobo

// turn on then off A8 to start over 
3:      
        call gpiois             // get user input value 
        srli a1,a1,5            // shift it down 5 steps (A8-A5 to A3-A0)
        andi a1,a1,0x08         // mask out A8 value
        bnez a1,4f              // turn on A8
        j 3b  
4:      
        call gpiois             // get user input value 
        srli a1,a1,5            // shift it down 5 steps (A8-A5 to A3-A0)
        andi a1,a1,0x08         // mask out A8 value    
        beqz a2,restart         // turn off A8 to start over
        j 4b
        
.end


