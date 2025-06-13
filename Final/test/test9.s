.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L100: 
         mov r0, #16
         bl malloc
         mov r4, r0
         add r0, r4, #12
         ldr r1, =c2$m2
         str r1, [r0]
         add r0, r4, #8
         ldr r1, =c1$m1
         str r1, [r0]
         add r0, r4, #4
         mov r1, #100
         str r1, [r0]
         ldr r0, [r4, #4]
         mov r1, #3
         mul r0, r1, r0
         mov r1, #1
         add r0, r1, r0
         bl putch
         ldr r2, [r4, #12]
         mov r0, #10
         add r1, r0, #1
         mov r0, r4
         blx r2
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: c1^m1

.balign 4
.global c1$m1
.section .text

c1$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
c1$m1$L100: 
         str r1, [r0, #0]
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: c2^m2

.balign 4
.global c2$m2
.section .text

c2$m2:
         push {r4-r10, fp, lr}
         add fp, sp, #32
c2$m2$L100: 
         mov r4, r0
         ldr r2, [r4, #8]
         mov r0, r4
         blx r2
         ldr r1, [r0, #0]
         ldr r0, [r4, #4]
         add r0, r1, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
