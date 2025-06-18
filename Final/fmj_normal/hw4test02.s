.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L122: 
         mov r0, #1
         mov r0, #0
         mov r2, #2
         mov r1, #3
         mul r1, r2, r1
         mov r2, #1
         add r1, r2, r1
         mov r2, #3
         cmp r1, r2
         bgt main$L119
main$L118: 
         mov r2, #4
         mov r1, #0
         cmp r2, r1
         bne main$L117
main$L120: 
         add r0, r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L117: 
         mov r2, #5
         mov r1, #0
         cmp r2, r1
         beq main$L119
         b main$L120
main$L119: 
         mov r0, #1
         b main$L120

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
