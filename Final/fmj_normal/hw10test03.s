.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L110: 
         mov r0, #0
main$L100: 
         mov r1, #0
         cmp r0, r1
         bgt main$L101
main$L102: 
         mov r1, #0
         cmp r0, r1
         blt main$L107
main$L108: 
         add r0, r0, #3
main$L109: 
         mul r0, r0, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         b main$L102
main$L107: 
         add r0, r0, #2
         b main$L109

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
