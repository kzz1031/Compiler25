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
         mov r5, #0
         mov r0, #32
         bl malloc
         mov r4, r0
         mov r0, #7
         str r0, [r4]
         add r0, r4, #4
         mov r1, #6
         str r1, [r0]
         add r0, r4, #8
         mov r1, #3
         str r1, [r0]
         add r0, r4, #12
         mov r1, #0
         str r1, [r0]
         add r0, r4, #16
         mov r1, #5
         str r1, [r0]
         add r0, r4, #20
         mov r1, #9
         str r1, [r0]
         add r0, r4, #24
         mov r1, #1
         str r1, [r0]
         add r0, r4, #28
         mov r1, #2
         str r1, [r0]
         mov r0, #8
         bl malloc
         add r1, r0, #4
         ldr r2, =b1$bubbleSort
         str r2, [r1]
         ldr r3, [r0, #4]
         mov r1, r4
         ldr r2, [r4]
         blx r3
main$L100: 
         ldr r0, [r4]
         cmp r5, r0
         blt main$L101
main$L102: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         ldr r0, [r4]
         cmp r5, r0
         bge main$L105
main$L106: 
         add r0, r5, #1
         mov r1, #4
         mul r0, r0, r1
         add r0, r4, r0
         ldr r0, [r0]
         bl putint
         mov r0, #32
         bl putch
         add r5, r5, #1
         b main$L100
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
b1$bubbleSort$L131: 
         mov r5, r2
         mov r4, r0
         mov r2, #0
         mov r0, #1
         cmp r5, r0
         ble b1$bubbleSort$L102
b1$bubbleSort$L103: 
b1$bubbleSort$L104: 
b1$bubbleSort$L105: 
         sub r0, r5, #1
         cmp r2, r0
         blt b1$bubbleSort$L106
b1$bubbleSort$L107: 
         ldr r3, [r4, #4]
         sub r2, r5, #1
         mov r0, r4
         blx r3
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
b1$bubbleSort$L102: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
b1$bubbleSort$L106: 
         ldr r0, [r1]
         cmp r2, r0
         bge b1$bubbleSort$L114
b1$bubbleSort$L115: 
         add r0, r2, #1
         mov r3, #4
         mul r0, r0, r3
         add r0, r1, r0
         ldr r3, [r0]
         add r0, r2, #1
         ldr r6, [r1]
         cmp r0, r6
         bge b1$bubbleSort$L116
b1$bubbleSort$L117: 
         add r0, r0, #1
         mov r6, #4
         mul r0, r0, r6
         add r0, r1, r0
         ldr r0, [r0]
         cmp r3, r0
         bgt b1$bubbleSort$L120
b1$bubbleSort$L121: 
b1$bubbleSort$L122: 
         add r2, r2, #1
         b b1$bubbleSort$L105
b1$bubbleSort$L114: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L116: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L120: 
         ldr r0, [r1]
         cmp r2, r0
         bge b1$bubbleSort$L123
b1$bubbleSort$L124: 
         add r0, r2, #1
         mov r3, #4
         mul r0, r0, r3
         add r0, r1, r0
         ldr r7, [r0]
         ldr r0, [r1]
         cmp r2, r0
         bge b1$bubbleSort$L125
b1$bubbleSort$L126: 
         add r6, r2, #1
         mov r0, r1
         ldr r3, [r1]
         cmp r6, r3
         bge b1$bubbleSort$L127
b1$bubbleSort$L128: 
         add r3, r2, #1
         mov r8, #4
         mul r3, r3, r8
         add r3, r1, r3
         add r6, r6, #1
         mov r8, #4
         mul r6, r6, r8
         add r0, r0, r6
         ldr r0, [r0]
         str r0, [r3]
         add r0, r2, #1
         ldr r3, [r1]
         cmp r0, r3
         bge b1$bubbleSort$L129
b1$bubbleSort$L130: 
         add r0, r0, #1
         mov r3, #4
         mul r0, r0, r3
         add r0, r1, r0
         str r7, [r0]
         b b1$bubbleSort$L122
b1$bubbleSort$L123: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L125: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L127: 
         mov r0, #-1
         bl exit
b1$bubbleSort$L129: 
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
