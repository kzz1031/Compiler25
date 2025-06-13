.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L119: 
         mov r3, #19
         mov r0, #0
         mov r2, #1
         mov r1, #0
         mov r4, #1
         cmp r3, r4
         bgt main$L102
main$L103: 
         add r1, r2, r1
         mov r2, #0
         cmp r1, r2
         bne main$L116
main$L107: 
         mov r1, #2
         cmp r3, r1
         bgt main$L116
main$L115: 
         mov r1, #3
         cmp r3, r1
         bgt main$L114
main$L117: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         mov r1, #1
         b main$L103
main$L114: 
         mov r1, #0
         cmp r3, r1
         bne main$L116
         b main$L117
main$L116: 
         mov r0, #1
         b main$L117

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
