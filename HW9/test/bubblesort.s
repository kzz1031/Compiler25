.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #8
main$L107: 
         mov r0, #32
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #7
         ldr r10, [fp, #-36]
         str r0, [r10]
         ldr r9, [fp, #-36]
         add r0, r9, #4
         mov r1, #6
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #8
         mov r1, #3
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #12
         mov r1, #0
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #16
         mov r1, #5
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #20
         mov r1, #9
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #24
         mov r1, #1
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #28
         mov r1, #2
         str r1, [r0]
         mov r0, #8
         bl malloc
         add r1, r0, #4
         ldr r2, =b1$bubbleSort
         str r2, [r1]
         mov r4, #0
         ldr r3, [r0, #4]
         ldr r9, [fp, #-36]
         mov r1, r9
         ldr r9, [fp, #-36]
         ldr r2, [r9]
         blx r3
main$L102: 
         ldr r9, [fp, #-36]
         ldr r0, [r9]
         cmp r4, r0
         blt main$L103
main$L104: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L103: 
         ldr r9, [fp, #-36]
         mov r10, r9
         str r10, [fp, #-40]
         ldr r9, [fp, #-36]
         ldr r0, [r9]
         cmp r4, r0
         bge main$L105
main$L106: 
         add r0, r4, #1
         mov r1, #4
         mul r0, r0, r1
         ldr r9, [fp, #-40]
         add r0, r9, r0
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
         sub sp, sp, #32
b1$bubbleSort$L127: 
         mov r10, #0
         str r10, [fp, #-40]
         mov r3, #1
         cmp r2, r3
         ble b1$bubbleSort$L102
b1$bubbleSort$L103: 
b1$bubbleSort$L104: 
         ldr r9, [fp, #-40]
         mov r10, r9
         str r10, [fp, #-44]
b1$bubbleSort$L107: 
         sub r3, r2, #1
         ldr r9, [fp, #-44]
         cmp r9, r3
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
         ldr r3, [r1]
         ldr r9, [fp, #-44]
         cmp r9, r3
         bge b1$bubbleSort$L110
b1$bubbleSort$L111: 
         ldr r9, [fp, #-44]
         add r3, r9, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r1, r3
         ldr r10, [r3]
         str r10, [fp, #-60]
         ldr r9, [fp, #-44]
         add r4, r9, #1
         ldr r3, [r1]
         cmp r4, r3
         bge b1$bubbleSort$L112
b1$bubbleSort$L113: 
         add r3, r4, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r1, r3
         ldr r3, [r3]
         ldr r9, [fp, #-60]
         cmp r9, r3
         bgt b1$bubbleSort$L124
b1$bubbleSort$L125: 
b1$bubbleSort$L126: 
         ldr r9, [fp, #-44]
         add r10, r9, #1
         str r10, [fp, #-48]
         ldr r9, [fp, #-48]
         mov r10, r9
         str r10, [fp, #-44]
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
         ldr r9, [fp, #-44]
         add r3, r9, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r1, r3
         ldr r10, [r3]
         str r10, [fp, #-52]
         ldr r3, [r1]
         ldr r9, [fp, #-44]
         cmp r9, r3
         bge b1$bubbleSort$L118
b1$bubbleSort$L119: 
         ldr r9, [fp, #-44]
         add r10, r9, #1
         str r10, [fp, #-56]
         mov r10, r1
         str r10, [fp, #-64]
         ldr r3, [r1]
         ldr r9, [fp, #-56]
         cmp r9, r3
         bge b1$bubbleSort$L120
b1$bubbleSort$L121: 
         ldr r9, [fp, #-44]
         add r3, r9, #1
         mov r4, #4
         mul r3, r3, r4
         add r4, r1, r3
         ldr r9, [fp, #-56]
         add r3, r9, #1
         mov r10, #4
         str r10, [fp, #-36]
         ldr r10, [fp, #-36]
         mul r3, r3, r10
         ldr r9, [fp, #-64]
         add r3, r9, r3
         ldr r3, [r3]
         str r3, [r4]
         ldr r9, [fp, #-44]
         add r4, r9, #1
         ldr r3, [r1]
         cmp r4, r3
         bge b1$bubbleSort$L122
b1$bubbleSort$L123: 
         add r3, r4, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r1, r3
         ldr r9, [fp, #-52]
         str r9, [r3]
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
         ldr r3, [r1]
         ldr r9, [fp, #-44]
         cmp r9, r3
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
