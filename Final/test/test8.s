.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #4
main$L115: 
         mov r0, #4
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #0
         ldr r10, [fp, #-36]
         str r0, [r10]
         mov r0, #24
         bl malloc
         mov r8, r0
         mov r0, #5
         str r0, [r8]
         mov r0, #24
         bl malloc
         mov r7, r0
         mov r0, #5
         str r0, [r7]
         add r0, r7, #4
         mov r1, #1
         str r1, [r0]
         add r0, r7, #8
         mov r1, #2
         str r1, [r0]
         add r0, r7, #12
         mov r1, #3
         str r1, [r0]
         add r0, r7, #16
         mov r1, #4
         str r1, [r0]
         add r0, r7, #20
         mov r1, #5
         str r1, [r0]
         mov r0, #8
         bl malloc
         mov r6, r0
         mov r0, #16
         bl malloc
         mov r1, #3
         str r1, [r0]
         add r1, r0, #4
         mov r2, #1
         str r2, [r1]
         add r1, r0, #8
         mov r2, #2
         str r2, [r1]
         add r1, r0, #12
         mov r2, #3
         str r2, [r1]
         str r0, [r6, #0]
         add r0, r6, #4
         ldr r1, =D$m
         str r1, [r0]
         mov r5, #0
         ldr r9, [fp, #-36]
         mov r0, r9
         bl getarray
         mov r4, r0
main$L102: 
         cmp r5, r4
         blt main$L103
main$L104: 
         ldr r5, [r8]
         ldr r0, [r7]
         cmp r5, r0
         bne main$L107
main$L108: 
         add r0, r5, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         mov r4, r0
         str r5, [r4]
         mov r0, #4
         add r1, r5, #1
         mov r2, #4
         mul r2, r1, r2
main$L109: 
         cmp r0, r2
         blt main$L110
main$L111: 
         ldr r5, [r4]
         add r0, r5, #1
         mov r1, #4
         mul r0, r0, r1
         bl malloc
         mov r1, r0
         str r5, [r1]
         mov r0, #4
         add r2, r5, #1
         mov r3, #4
         mul r3, r2, r3
main$L112: 
         cmp r0, r3
         blt main$L113
main$L114: 
         ldr r2, [r6, #4]
         mov r0, r6
         blx r2
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         ldr r9, [fp, #-36]
         mov r0, r9
         ldr r9, [fp, #-36]
         ldr r1, [r9]
         cmp r5, r1
         bge main$L105
main$L106: 
         add r1, r5, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         ldr r0, [r0]
         bl putint
         mov r0, #32
         bl putch
         add r0, r5, #1
         mov r5, r0
         b main$L102
main$L105: 
         mov r0, #-1
         bl exit
main$L107: 
         mov r0, #-1
         bl exit
main$L110: 
         add r3, r4, r0
         add r1, r8, r0
         ldr r5, [r1]
         add r1, r7, r0
         ldr r1, [r1]
         add r1, r5, r1
         str r1, [r3]
         add r0, r0, #4
         b main$L109
main$L113: 
         add r5, r1, r0
         add r2, r4, r0
         ldr r2, [r2]
         mov r7, #0
         sub r2, r7, r2
         str r2, [r5]
         add r0, r0, #4
         b main$L112

@ Here's function: D^m

.balign 4
.global D$m
.section .text

D$m:
         push {r4-r10, fp, lr}
         add fp, sp, #32
D$m$L100: 
         ldr r0, [r0, #0]
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
