.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L130: 
         mov r0, #44
         bl malloc
         mov r5, r0
         mov r0, #10
         str r0, [r5]
         mov r0, #44
         bl malloc
         mov r4, r0
         mov r0, #10
         str r0, [r4]
         ldr r0, [r5]
         mov r1, #0
         cmp r1, r0
         bge main$L100
main$L101: 
         mov r0, #0
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         mov r1, #1
         str r1, [r0]
         ldr r0, [r4]
         mov r1, #0
         cmp r1, r0
         bge main$L102
main$L103: 
         mov r0, #0
         mov r2, #1
         mov r1, #2
         cmp r2, r1
         blt main$L106
main$L107: 
         ldr r1, [r5]
         cmp r0, r1
         bge main$L109
main$L110: 
         mov r1, #0
         add r1, r1, #1
         mov r2, #4
         mul r1, r1, r2
         add r1, r4, r1
         add r0, r0, #1
         mov r2, #4
         mul r0, r0, r2
         add r0, r5, r0
         ldr r0, [r0]
         str r0, [r1]
         ldr r0, [r5]
         mov r1, #0
         cmp r1, r0
         bge main$L113
main$L114: 
         mov r0, #0
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         ldr r0, [r0]
         mov r1, #1
         cmp r0, r1
         beq main$L117
main$L118: 
main$L119: 
         ldr r0, [r5]
         mov r1, #1
         cmp r1, r0
         bge main$L126
main$L127: 
         mov r0, #1
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         ldr r1, [r0]
         ldr r0, [r4]
         mov r2, #1
         cmp r2, r0
         bge main$L128
main$L129: 
         mov r0, #1
         add r0, r0, #1
         mov r2, #4
         mul r0, r0, r2
         add r0, r4, r0
         ldr r0, [r0]
         add r0, r1, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L100: 
         mov r0, #-1
         bl exit
main$L102: 
         mov r0, #-1
         bl exit
main$L106: 
         mov r0, #1
         b main$L107
main$L109: 
         mov r0, #-1
         bl exit
main$L113: 
         mov r0, #-1
         bl exit
main$L117: 
         ldr r0, [r4]
         mov r1, #9
         cmp r1, r0
         bge main$L120
main$L121: 
         mov r0, #9
         add r0, r0, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r4, r0
         mov r1, #1
         str r1, [r0]
         b main$L119
main$L120: 
         mov r0, #-1
         bl exit
main$L126: 
         mov r0, #-1
         bl exit
main$L128: 
         mov r0, #-1
         bl exit

.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
