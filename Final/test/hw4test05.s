.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L115: 
         mov r2, #19
main$L102: 
         mov r3, #1
         mov r1, #0
         cmp r3, r1
         bne main$L103
main$L104: 
         mov r1, #0
         cmp r0, r1
         bne main$L112
main$L113: 
         mov r0, #1
main$L114: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         mov r1, #2
         mov r0, #0
         mov r3, #0
         cmp r2, r3
         bgt main$L107
main$L108: 
         add r0, r1, r0
         b main$L104
main$L107: 
         mov r0, #1
         b main$L108
main$L112: 
         mov r0, #0
         b main$L114

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
