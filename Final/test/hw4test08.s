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
         mov r0, #19
         mov r2, #0
         cmp r1, r2
         bne main$L102
main$L103: 
main$L104: 
         mov r2, #0
         cmp r1, r2
         bne main$L107
main$L108: 
main$L109: 
         mov r2, #0
         cmp r1, r2
         bne main$L112
main$L113: 
main$L114: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L102: 
         b main$L104
main$L107: 
         b main$L109
main$L112: 
         mov r0, #20
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
