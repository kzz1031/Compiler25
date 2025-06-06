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
         mov r1, #1
         mov r0, #2
         add r0, r1, r0
         mov r0, #1
         mov r1, #2
         mul r1, r0, r1
         mov r1, #1
         mov r1, #0
         mov r1, #0
         add r1, r1, #1
         mov r2, #0
         mov r1, #1
         cmp r2, r1
         bne main$L102
main$L103: 
main$L104: 
main$L107: 
         mov r2, #1
         mov r1, #3
         mul r1, r0, r0
         add r1, r2, r1
         mov r2, #0
         cmp r1, r2
         bne main$L108
main$L109: 
         mov r0, #9
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         b main$L104
main$L108: 
         bl putint
         mov r0, #10
         bl putch
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
