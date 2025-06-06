.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L107: 
         mov r0, #32
         bl malloc
         mov r5, r0
         mov r0, #7
         str r0, [r5]
         add r0, r5, #4
         mov r1, #6
         str r1, [r0]
         add r0, r5, #8
         mov r1, #3
         str r1, [r0]
         add r0, r5, #12
         mov r1, #0
         str r1, [r0]
         add r0, r5, #16
         mov r1, #5
         str r1, [r0]
         add r0, r5, #20
         mov r1, #9
         str r1, [r0]
         add r0, r5, #24
         mov r1, #1
         str r1, [r0]
         add r0, r5, #28
         mov r1, #2
         str r1, [r0]
         mov r0, #8
         bl malloc
         add r1, r0, #4
         ldr r2, =b1$bubbleSort
         str r2, [r1]
         mov r4, #0
         ldr r3, [r0, #4]
         mov r1, r5
         ldr r2, [r5]
         blx r3
main$L102: 
         ldr r0, [r5]
         cmp r4, r0
         blt main$L103
main$L104: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         ldr r0, [r5]
         cmp r4, r0
         bge main$L105
main$L106: 
         add r0, r4, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r5, r0
         ldr r0, [r0]
         bl putint
         mov r0, #32
         bl putch
         add r4, r4, #1
         b main$L102
main$L105: 
         mov r0, #-1
         bl exit

@ Here's function: b1^bubbleSort

.balign 4
.global b1$bubbleSort
.section .text

b1$bubbleSort:
         push {r4-r10, fp, lr}
         add fp, sp, #32
b1$bubbleSort$L127: 
         mov r3, #0
         mov r4, #1
         cmp r2, r4
         ble b1$bubbleSort$L102
b1$bubbleSort$L103: 
b1$bubbleSort$L104: 
b1$bubbleSort$L107: 
         sub r4, r2, #1
         cmp r3, r4
         blt b1$bubbleSort$L108
b1$bubbleSort$L109: 
         ldr r3, [r0, #4]
         sub r2, r2, #1
         blx r3
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
b1$bubbleSort$L102: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
b1$bubbleSort$L108: 
         ldr r4, [r1]
         cmp r3, r4
         bge b1$bubbleSort$L110
b1$bubbleSort$L111: 
         add r4, r3, #1
         mov r5, #4
         mul r4, r4, r5
         add r4, r1, r4
         ldr r5, [r4]
         add r6, r3, #1
         ldr r4, [r1]
         cmp r6, r4
         bge b1$bubbleSort$L112
b1$bubbleSort$L113: 
         add r4, r6, #1
         mov r6, #4
         mul r4, r4, r6
         add r4, r1, r4
         ldr r4, [r4]
         cmp r5, r4
         bgt b1$bubbleSort$L124
b1$bubbleSort$L125: 
b1$bubbleSort$L126: 
         add r3, r3, #1
         b b1$bubbleSort$L107
b1$bubbleSort$L110: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L112: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L116: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L117: 
         add r4, r3, #1
         mov r5, #4
         mul r4, r4, r5
         add r4, r1, r4
         ldr r7, [r4]
         ldr r4, [r1]
         cmp r3, r4
         bge b1$bubbleSort$L118
b1$bubbleSort$L119: 
         add r6, r3, #1
         mov r5, r1
         ldr r4, [r1]
         cmp r6, r4
         bge b1$bubbleSort$L120
b1$bubbleSort$L121: 
         add r4, r3, #1
         mov r8, #4
         mul r4, r4, r8
         add r8, r1, r4
         add r4, r6, #1
         mov r6, #4
         mul r4, r4, r6
         add r4, r5, r4
         ldr r4, [r4]
         str r4, [r8]
         add r5, r3, #1
         ldr r4, [r1]
         cmp r5, r4
         bge b1$bubbleSort$L122
b1$bubbleSort$L123: 
         add r4, r5, #1
         mov r5, #4
         mul r4, r4, r5
         add r4, r1, r4
         str r7, [r4]
         b b1$bubbleSort$L126
b1$bubbleSort$L118: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L120: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L122: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L124: 
         ldr r4, [r1]
         cmp r3, r4
         bge b1$bubbleSort$L116
         b b1$bubbleSort$L117
.global malloc
.global getint
.global putint
.global putch
.global putarray
.global getch
.global getarray
.global starttime
.global stoptime
