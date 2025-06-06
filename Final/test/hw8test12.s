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
         mov r4, #10
main$L102: 
         mov r0, #0
         cmp r4, r0
         bgt main$L103
main$L104: 
         mov r0, r4
         bl putint
         mov r0, #10
         bl putch
         add r0, r4, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         sub r4, r4, #1
         b main$L102

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
