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
main$L114: 
         mov r0, #32
         bl malloc
         mov r10, r0
         str r10, [fp, #-36]
         mov r0, #7
         ldr r10, [fp, #-36]
         str r0, [r10]
         ldr r9, [fp, #-36]
         add r0, r9, #4
         mov r1, #1
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #8
         mov r1, #2
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #12
         mov r1, #3
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #16
         mov r1, #4
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #20
         mov r1, #5
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #24
         mov r1, #6
         str r1, [r0]
         ldr r9, [fp, #-36]
         add r0, r9, #28
         mov r1, #7
         str r1, [r0]
         mov r4, #0
         mov r0, #8
         bl malloc
         mov r6, r0
         mov r1, #2
         str r1, [r6, #0]
         add r0, r6, #4
         ldr r1, =c1$m1
         str r1, [r0]
         mov r0, #8
         bl malloc
         mov r5, r0
         mov r1, #2
         str r1, [r5, #0]
         add r0, r5, #4
         ldr r1, =c1$m1
         str r1, [r0]
         ldr r9, [fp, #-36]
         ldr r7, [r9]
         mov r8, r4
main$L100: 
         cmp r8, r7
         blt main$L101
main$L102: 
         mov r1, r7
         ldr r9, [fp, #-36]
         mov r0, r9
         bl putarray
         mov r0, r7
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L101: 
         mov r0, #2
         sdiv r0, r8, r0
         mov r1, #2
         mul r0, r0, r1
         cmp r0, r8
         beq main$L107
main$L108: 
         ldr r9, [fp, #-36]
         mov r0, r9
         ldr r9, [fp, #-36]
         ldr r1, [r9]
         cmp r8, r1
         bge main$L112
main$L113: 
         ldr r2, [r5, #4]
         add r1, r8, #1
         mov r3, #4
         mul r1, r1, r3
         add r4, r0, r1
         mov r1, r7
         mov r0, r5
         blx r2
         str r0, [r4]
main$L109: 
         add r0, r8, #1
         mov r8, r0
         b main$L100
main$L107: 
         ldr r9, [fp, #-36]
         mov r0, r9
         ldr r9, [fp, #-36]
         ldr r1, [r9]
         cmp r8, r1
         bge main$L110
main$L111: 
         ldr r2, [r6, #4]
         add r1, r8, #1
         mov r3, #4
         mul r1, r1, r3
         add r4, r0, r1
         mov r1, r8
         mov r0, r6
         blx r2
         str r0, [r4]
         b main$L109
main$L110: 
         mov r0, #-1
         bl exit
main$L112: 
         mov r0, #-1
         bl exit

@ Here's function: c1^m1

.balign 4
.global c1$m1
.section .text

c1$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
c1$m1$L100: 
         ldr r0, [r0, #0]
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: c2^m1

.balign 4
.global c2$m1
.section .text

c2$m1:
         push {r4-r10, fp, lr}
         add fp, sp, #32
c2$m1$L101: 
         ldr r0, [r0, #0]
         add r0, r0, r1
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
