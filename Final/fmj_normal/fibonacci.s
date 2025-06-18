.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L117: 
         mov r6, #0
         mov r0, #4
         bl malloc
         mov r5, r0
         add r0, r5, #0
         ldr r1, =fib$f
         str r1, [r0]
         mov r0, #69
         bl putch
         mov r0, #110
         bl putch
         mov r0, #116
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #32
         bl putch
         mov r0, #116
         bl putch
         mov r0, #104
         bl putch
         mov r0, #101
         bl putch
         mov r0, #32
         bl putch
         mov r0, #110
         bl putch
         mov r0, #117
         bl putch
         mov r0, #109
         bl putch
         mov r0, #98
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #32
         bl putch
         mov r0, #111
         bl putch
         mov r0, #102
         bl putch
         mov r0, #32
         bl putch
         mov r0, #116
         bl putch
         mov r0, #101
         bl putch
         mov r0, #114
         bl putch
         mov r0, #109
         bl putch
         mov r0, #58
         bl putch
         bl getint
         mov r4, r0
         mov r0, #0
         cmp r4, r0
         blt main$L109
main$L108: 
         mov r0, #47
         cmp r4, r0
         bgt main$L109
main$L110: 
main$L111: 
main$L112: 
         cmp r6, r4
         blt main$L113
main$L114: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L109: 
         mov r0, #0
         sub r0, r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
main$L113: 
         ldr r2, [r5, #0]
         mov r1, r6
         mov r0, r5
         blx r2
         bl putint
         add r6, r6, #1
         mov r0, #32
         bl putch
         b main$L112

@ Here's function: fib^f

.balign 4
.global fib$f
.section .text

fib$f:
         push {r4-r10, fp, lr}
         add fp, sp, #32
fib$f$L112: 
         mov r6, r1
         mov r4, r0
         mov r0, #0
         cmp r6, r0
         beq fib$f$L109
fib$f$L108: 
         mov r0, #1
         cmp r6, r0
         beq fib$f$L109
fib$f$L110: 
         ldr r2, [r4, #0]
         mov r0, r4
         sub r1, r6, #1
         blx r2
         mov r5, r0
         ldr r2, [r4, #0]
         sub r1, r6, #2
         mov r0, r4
         blx r2
         add r0, r5, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
fib$f$L109: 
         mov r0, r6
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
