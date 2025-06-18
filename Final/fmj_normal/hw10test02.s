.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L105: 
         mov r0, #0
         mov r1, #0
         cmp r0, r1
         bgt main$L102
main$L103: 
         add r0, r0, #2
main$L104: 
         mul r0, r0, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         b main$L104

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
