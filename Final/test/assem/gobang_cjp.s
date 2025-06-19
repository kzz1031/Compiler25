.section .note.GNU-stack

@ Here is the RPI code

@ Here's function: _^main^_^main

.balign 4
.global main
.section .text

main:
         push {r4-r10, fp, lr}
         add fp, sp, #32
main$L100: 
         mov r0, #100
         bl malloc
         mov r4, r0
         mov r0, #904
         bl malloc
         mov r1, #225
         str r1, [r0]
         str r0, [r4, #0]
         mov r1, #15
         str r1, [r4, #12]
         mov r1, #15
         str r1, [r4, #16]
         add r0, r4, #24
         ldr r1, =ChessBoard$checkwin
         str r1, [r0]
         add r0, r4, #28
         ldr r1, =ChessBoard$clear
         str r1, [r0]
         add r0, r4, #32
         ldr r1, =ChessBoard$getCurChess
         str r1, [r0]
         add r0, r4, #36
         ldr r1, =ChessBoard$init
         str r1, [r0]
         add r0, r4, #40
         ldr r1, =ChessBoard$mainLoop
         str r1, [r0]
         add r0, r4, #44
         ldr r1, =ChessBoard$printBoard
         str r1, [r0]
         add r0, r4, #48
         ldr r1, =ChessBoard$printDivider
         str r1, [r0]
         add r0, r4, #52
         ldr r1, =ChessBoard$printHint
         str r1, [r0]
         add r0, r4, #56
         ldr r1, =ChessBoard$printWin
         str r1, [r0]
         add r0, r4, #60
         ldr r1, =ChessBoard$update
         str r1, [r0]
         add r0, r4, #80
         ldr r1, =Game$run
         str r1, [r0]
         ldr r1, [r4, #80]
         mov r0, r4
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Math^mod

.balign 4
.global Math$mod
.section .text

Math$mod:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Math$mod$L101: 
         sdiv r0, r1, r2
         mul r0, r0, r2
         sub r0, r1, r0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Math^wait

.balign 4
.global Math$wait
.section .text

Math$wait:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Math$wait$L102: 
         mov r0, #0
         mov r3, #100
         mov r2, #100
         mul r2, r3, r2
         mov r3, #100
         mul r2, r2, r3
         mul r1, r2, r1
Math$wait$L104: 
         cmp r0, r1
         blt Math$wait$L103
Math$wait$L105: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$wait$L103: 
         add r0, r0, #1
         b Math$wait$L104

@ Here's function: Math^dec2hex

.balign 4
.global Math$dec2hex
.section .text

Math$dec2hex:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Math$dec2hex$L106: 
         mov r0, #10
         cmp r1, r0
         blt Math$dec2hex$L107
Math$dec2hex$L108: 
         mov r0, #55
         add r0, r0, r1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$dec2hex$L107: 
         mov r0, #48
         add r0, r0, r1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Math^hex2dec

.balign 4
.global Math$hex2dec
.section .text

Math$hex2dec:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Math$hex2dec$L110: 
         mov r0, #48
         cmp r1, r0
         bge Math$hex2dec$L114
Math$hex2dec$L112: 
Math$hex2dec$L113: 
         mov r0, #65
         cmp r1, r0
         bge Math$hex2dec$L118
Math$hex2dec$L116: 
Math$hex2dec$L117: 
         mov r0, #0
         sub r0, r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$hex2dec$L111: 
         sub r0, r1, #48
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$hex2dec$L114: 
         mov r0, #57
         cmp r1, r0
         ble Math$hex2dec$L111
         b Math$hex2dec$L112
Math$hex2dec$L115: 
         sub r0, r1, #55
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$hex2dec$L118: 
         mov r0, #70
         cmp r1, r0
         ble Math$hex2dec$L115
         b Math$hex2dec$L116

@ Here's function: ConsoleOp^putSP

.balign 4
.global ConsoleOp$putSP
.section .text

ConsoleOp$putSP:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ConsoleOp$putSP$L119: 
         mov r0, #32
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ConsoleOp^putLF

.balign 4
.global ConsoleOp$putLF
.section .text

ConsoleOp$putLF:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ConsoleOp$putLF$L120: 
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ConsoleOp^removeLine

.balign 4
.global ConsoleOp$removeLine
.section .text

ConsoleOp$removeLine:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ConsoleOp$removeLine$L121: 
         mov r0, #13
         bl putch
         mov r0, #27
         bl putch
         mov r0, #91
         bl putch
         mov r0, #65
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ConsoleOp^getchar

.balign 4
.global ConsoleOp$getchar
.section .text

ConsoleOp$getchar:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ConsoleOp$getchar$L122: 
         bl getch
ConsoleOp$getchar$L124: 
         mov r1, #32
         cmp r0, r1
         beq ConsoleOp$getchar$L123
ConsoleOp$getchar$L126: 
         mov r1, #10
         cmp r0, r1
         beq ConsoleOp$getchar$L123
ConsoleOp$getchar$L125: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ConsoleOp$getchar$L123: 
         bl getch
         b ConsoleOp$getchar$L124

@ Here's function: Game^init

.balign 4
.global Game$init
.section .text

Game$init:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Game$init$L127: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Game^mainLoop

.balign 4
.global Game$mainLoop
.section .text

Game$mainLoop:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Game$mainLoop$L128: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Game^clear

.balign 4
.global Game$clear
.section .text

Game$clear:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Game$clear$L129: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: Game^run

.balign 4
.global Game$run
.section .text

Game$run:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Game$run$L130: 
         mov r4, r0
         ldr r1, [r4, #36]
         mov r0, r4
         blx r1
Game$run$L132: 
         ldr r1, [r4, #40]
         mov r0, r4
         blx r1
         mov r1, #0
         cmp r0, r1
         bne Game$run$L133
Game$run$L131: 
         b Game$run$L132
Game$run$L133: 
         ldr r1, [r4, #28]
         mov r0, r4
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^mainLoop

.balign 4
.global ChessBoard$mainLoop
.section .text

ChessBoard$mainLoop:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$mainLoop$L136: 
         mov r6, r0
         ldr r1, [r6, #44]
         mov r0, r6
         blx r1
         ldr r1, [r6, #52]
         mov r0, r6
         blx r1
         ldr r0, [r6, #4]
         ldr r1, [r0, #64]
         blx r1
         mov r4, r0
         ldr r0, [r6, #4]
         ldr r1, [r0, #64]
         blx r1
         mov r5, r0
         ldr r0, [r6, #8]
         ldr r2, [r0, #88]
         mov r1, r4
         blx r2
         mov r4, r0
         ldr r0, [r6, #8]
         ldr r2, [r0, #88]
         mov r1, r5
         blx r2
         mov r5, r0
         mov r0, #0
         cmp r4, r0
         bge ChessBoard$mainLoop$L140
ChessBoard$mainLoop$L137: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$mainLoop$L138: 
ChessBoard$mainLoop$L139: 
         ldr r3, [r6, #60]
         mov r2, r5
         mov r1, r4
         mov r0, r6
         blx r3
         ldr r3, [r6, #24]
         mov r2, r5
         mov r1, r4
         mov r0, r6
         blx r3
         mov r1, #0
         cmp r0, r1
         bne ChessBoard$mainLoop$L143
ChessBoard$mainLoop$L144: 
ChessBoard$mainLoop$L145: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$mainLoop$L140: 
         ldr r0, [r6, #16]
         cmp r4, r0
         blt ChessBoard$mainLoop$L141
         b ChessBoard$mainLoop$L137
ChessBoard$mainLoop$L141: 
         mov r0, #0
         cmp r5, r0
         bge ChessBoard$mainLoop$L142
         b ChessBoard$mainLoop$L137
ChessBoard$mainLoop$L142: 
         ldr r0, [r6, #12]
         cmp r5, r0
         ble ChessBoard$mainLoop$L138
         b ChessBoard$mainLoop$L137
ChessBoard$mainLoop$L143: 
         mov r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^init

.balign 4
.global ChessBoard$init
.section .text

ChessBoard$init:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$init$L148: 
         mov r5, r0
         mov r0, #100
         bl malloc
         mov r6, r0
         add r0, r6, #64
         ldr r1, =ConsoleOp$getchar
         str r1, [r0]
         add r0, r6, #68
         ldr r1, =ConsoleOp$putLF
         str r1, [r0]
         add r0, r6, #72
         ldr r1, =ConsoleOp$putSP
         str r1, [r0]
         add r0, r6, #76
         ldr r1, =ConsoleOp$removeLine
         str r1, [r0]
         mov r4, #0
         mov r0, #100
         bl malloc
         add r1, r0, #84
         ldr r2, =Math$dec2hex
         str r2, [r1]
         add r1, r0, #88
         ldr r2, =Math$hex2dec
         str r2, [r1]
         add r1, r0, #92
         ldr r2, =Math$mod
         str r2, [r1]
         add r1, r0, #96
         ldr r2, =Math$wait
         str r2, [r1]
         str r6, [r5, #4]
         str r0, [r5, #8]
         add r0, r5, #20
         mov r1, #0
         str r1, [r0]
ChessBoard$init$L150: 
         ldr r1, [r5, #16]
         ldr r0, [r5, #12]
         mul r0, r1, r0
         cmp r4, r0
         blt ChessBoard$init$L149
ChessBoard$init$L151: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$init$L149: 
         ldr r0, [r5, #0]
         ldr r1, [r0]
         cmp r4, r1
         bge ChessBoard$init$L152
ChessBoard$init$L153: 
         add r1, r4, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         mov r1, #46
         str r1, [r0]
         add r4, r4, #1
         b ChessBoard$init$L150
ChessBoard$init$L152: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^clear

.balign 4
.global ChessBoard$clear
.section .text

ChessBoard$clear:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$clear$L154: 
         mov r4, r0
         ldr r1, [r4, #44]
         mov r0, r4
         blx r1
         ldr r0, [r4, #20]
         mov r1, #1
         sub r1, r1, r0
         str r1, [r4, #20]
         ldr r1, [r4, #56]
         mov r0, r4
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^update

.balign 4
.global ChessBoard$update
.section .text

ChessBoard$update:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$update$L155: 
         mov r5, r0
         ldr r0, [r5, #0]
         ldr r3, [r5, #12]
         mul r3, r1, r3
         add r4, r3, r2
         ldr r3, [r0]
         cmp r4, r3
         bge ChessBoard$update$L159
ChessBoard$update$L160: 
         add r3, r4, #1
         mov r4, #4
         mul r3, r3, r4
         add r0, r0, r3
         ldr r0, [r0]
         mov r3, #46
         cmp r0, r3
         beq ChessBoard$update$L156
ChessBoard$update$L157: 
ChessBoard$update$L158: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$update$L156: 
         ldr r4, [r5, #0]
         ldr r0, [r5, #12]
         mul r0, r1, r0
         add r6, r0, r2
         ldr r0, [r4]
         cmp r6, r0
         bge ChessBoard$update$L161
ChessBoard$update$L162: 
         ldr r1, [r5, #32]
         mov r0, r5
         blx r1
         add r1, r6, #1
         mov r2, #4
         mul r1, r1, r2
         add r1, r4, r1
         str r0, [r1]
         ldr r0, [r5, #20]
         mov r1, #1
         sub r1, r1, r0
         str r1, [r5, #20]
         b ChessBoard$update$L158
ChessBoard$update$L159: 
         mov r0, #-1
         bl exit
ChessBoard$update$L161: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^checkwin

.balign 4
.global ChessBoard$checkwin
.section .text

ChessBoard$checkwin:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #12
ChessBoard$checkwin$L163: 
         mov r10, r2
         str r10, [fp, #-44]
         mov r10, r1
         str r10, [fp, #-40]
         mov r10, r0
         str r10, [fp, #-36]
         mov r4, #0
         mov r0, #36
         bl malloc
         mov r1, r0
         mov r0, #8
         str r0, [r1]
         add r0, r1, #4
         mov r2, #0
         str r2, [r0]
         add r0, r1, #8
         mov r2, #1
         str r2, [r0]
         add r0, r1, #12
         mov r2, #1
         str r2, [r0]
         add r0, r1, #16
         mov r2, #0
         str r2, [r0]
         add r0, r1, #20
         mov r2, #1
         str r2, [r0]
         add r0, r1, #24
         mov r2, #1
         str r2, [r0]
         add r0, r1, #28
         mov r2, #1
         str r2, [r0]
         add r0, r1, #32
         mov r2, #-1
         str r2, [r0]
         ldr r9, [fp, #-36]
         ldr r0, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #12]
         ldr r9, [fp, #-40]
         mul r2, r9, r2
         ldr r10, [fp, #-44]
         add r3, r2, r10
         ldr r2, [r0]
         cmp r3, r2
         bge ChessBoard$checkwin$L164
ChessBoard$checkwin$L165: 
         add r2, r3, #1
         mov r3, #4
         mul r2, r2, r3
         add r0, r0, r2
         ldr r0, [r0]
         mov r2, r0
         mov r3, r4
ChessBoard$checkwin$L167: 
         mov r0, #4
         cmp r3, r0
         blt ChessBoard$checkwin$L166
ChessBoard$checkwin$L168: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$checkwin$L164: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L166: 
         mov r5, #1
         ldr r9, [fp, #-40]
         mov r0, r9
         ldr r9, [fp, #-44]
         mov r7, r9
         mov r4, r0
         mov r0, #2
         mul r6, r3, r0
         ldr r0, [r1]
         cmp r6, r0
         bge ChessBoard$checkwin$L169
ChessBoard$checkwin$L170: 
         add r0, r6, #1
         mov r6, #4
         mul r0, r0, r6
         add r0, r1, r0
         ldr r0, [r0]
         add r0, r4, r0
         mov r6, r0
         mov r4, r7
         mov r0, #2
         mul r0, r3, r0
         add r7, r0, #1
         ldr r0, [r1]
         cmp r7, r0
         bge ChessBoard$checkwin$L171
ChessBoard$checkwin$L172: 
         add r0, r7, #1
         mov r7, #4
         mul r0, r0, r7
         add r0, r1, r0
         ldr r0, [r0]
         add r0, r4, r0
         mov r7, r0
         mov r4, r5
ChessBoard$checkwin$L174: 
         mov r0, #0
         cmp r6, r0
         bge ChessBoard$checkwin$L176
ChessBoard$checkwin$L175: 
         ldr r9, [fp, #-40]
         mov r0, r9
         ldr r9, [fp, #-44]
         mov r7, r9
         mov r5, r0
         mov r0, #2
         mul r6, r3, r0
         ldr r0, [r1]
         cmp r6, r0
         bge ChessBoard$checkwin$L186
ChessBoard$checkwin$L187: 
         add r0, r6, #1
         mov r6, #4
         mul r0, r0, r6
         add r0, r1, r0
         ldr r0, [r0]
         sub r0, r5, r0
         mov r6, r0
         mov r5, r7
         mov r0, #2
         mul r0, r3, r0
         add r7, r0, #1
         ldr r0, [r1]
         cmp r7, r0
         bge ChessBoard$checkwin$L188
ChessBoard$checkwin$L189: 
         add r0, r7, #1
         mov r7, #4
         mul r0, r0, r7
         add r0, r1, r0
         ldr r0, [r0]
         sub r0, r5, r0
         mov r7, r0
         mov r5, r4
ChessBoard$checkwin$L191: 
         mov r0, #0
         cmp r6, r0
         bge ChessBoard$checkwin$L193
ChessBoard$checkwin$L192: 
         mov r0, #5
         cmp r5, r0
         bge ChessBoard$checkwin$L203
ChessBoard$checkwin$L204: 
ChessBoard$checkwin$L205: 
         add r0, r3, #1
         mov r3, r0
         b ChessBoard$checkwin$L167
ChessBoard$checkwin$L169: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L171: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L173: 
         mov r0, r6
         mov r5, r0
         mov r0, #2
         mul r6, r3, r0
         ldr r0, [r1]
         cmp r6, r0
         bge ChessBoard$checkwin$L182
ChessBoard$checkwin$L183: 
         add r0, r6, #1
         mov r6, #4
         mul r0, r0, r6
         add r0, r1, r0
         ldr r0, [r0]
         add r0, r5, r0
         mov r6, r0
         mov r5, r7
         mov r0, #2
         mul r0, r3, r0
         add r7, r0, #1
         ldr r0, [r1]
         cmp r7, r0
         bge ChessBoard$checkwin$L184
ChessBoard$checkwin$L185: 
         add r0, r7, #1
         mov r7, #4
         mul r0, r0, r7
         add r0, r1, r0
         ldr r0, [r0]
         add r0, r5, r0
         mov r5, r0
         add r0, r4, #1
         mov r7, r5
         mov r4, r0
         b ChessBoard$checkwin$L174
ChessBoard$checkwin$L176: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #16]
         cmp r6, r0
         blt ChessBoard$checkwin$L177
         b ChessBoard$checkwin$L175
ChessBoard$checkwin$L177: 
         mov r0, #0
         cmp r7, r0
         bge ChessBoard$checkwin$L178
         b ChessBoard$checkwin$L175
ChessBoard$checkwin$L178: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #12]
         cmp r7, r0
         blt ChessBoard$checkwin$L181
         b ChessBoard$checkwin$L175
ChessBoard$checkwin$L179: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L180: 
         add r0, r8, #1
         mov r8, #4
         mul r0, r0, r8
         add r0, r5, r0
         ldr r0, [r0]
         cmp r0, r2
         beq ChessBoard$checkwin$L173
         b ChessBoard$checkwin$L175
ChessBoard$checkwin$L181: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r5, [r9, #12]
         mul r5, r6, r5
         add r8, r5, r7
         mov r5, r0
         ldr r0, [r0]
         cmp r8, r0
         bge ChessBoard$checkwin$L179
         b ChessBoard$checkwin$L180
ChessBoard$checkwin$L182: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L184: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L186: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L188: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L190: 
         mov r0, r6
         mov r4, r0
         mov r0, #2
         mul r6, r3, r0
         ldr r0, [r1]
         cmp r6, r0
         bge ChessBoard$checkwin$L199
ChessBoard$checkwin$L200: 
         add r0, r6, #1
         mov r6, #4
         mul r0, r0, r6
         add r0, r1, r0
         ldr r0, [r0]
         sub r0, r4, r0
         mov r6, r0
         mov r4, r7
         mov r0, #2
         mul r0, r3, r0
         add r7, r0, #1
         ldr r0, [r1]
         cmp r7, r0
         bge ChessBoard$checkwin$L201
ChessBoard$checkwin$L202: 
         add r0, r7, #1
         mov r7, #4
         mul r0, r0, r7
         add r0, r1, r0
         ldr r0, [r0]
         sub r0, r4, r0
         mov r4, r0
         add r0, r5, #1
         mov r7, r4
         mov r5, r0
         b ChessBoard$checkwin$L191
ChessBoard$checkwin$L193: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #16]
         cmp r6, r0
         blt ChessBoard$checkwin$L194
         b ChessBoard$checkwin$L192
ChessBoard$checkwin$L194: 
         mov r0, #0
         cmp r7, r0
         bge ChessBoard$checkwin$L195
         b ChessBoard$checkwin$L192
ChessBoard$checkwin$L195: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #12]
         cmp r7, r0
         blt ChessBoard$checkwin$L198
         b ChessBoard$checkwin$L192
ChessBoard$checkwin$L196: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L197: 
         add r0, r8, #1
         mov r8, #4
         mul r0, r0, r8
         add r0, r4, r0
         ldr r0, [r0]
         cmp r0, r2
         beq ChessBoard$checkwin$L190
         b ChessBoard$checkwin$L192
ChessBoard$checkwin$L198: 
         ldr r9, [fp, #-36]
         ldr r0, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r4, [r9, #12]
         mul r4, r6, r4
         add r8, r4, r7
         mov r4, r0
         ldr r0, [r0]
         cmp r8, r0
         bge ChessBoard$checkwin$L196
         b ChessBoard$checkwin$L197
ChessBoard$checkwin$L199: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L201: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L203: 
         mov r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^printDivider

.balign 4
.global ChessBoard$printDivider
.section .text

ChessBoard$printDivider:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$printDivider$L206: 
         mov r5, r0
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         blx r1
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         blx r1
         mov r0, #43
         bl putch
         mov r4, #0
ChessBoard$printDivider$L208: 
         ldr r0, [r5, #12]
         cmp r4, r0
         blt ChessBoard$printDivider$L207
ChessBoard$printDivider$L209: 
         mov r0, #45
         bl putch
         mov r0, #43
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #68]
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$printDivider$L207: 
         mov r0, #45
         bl putch
         mov r0, #45
         bl putch
         add r4, r4, #1
         b ChessBoard$printDivider$L208

@ Here's function: ChessBoard^getCurChess

.balign 4
.global ChessBoard$getCurChess
.section .text

ChessBoard$getCurChess:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$getCurChess$L210: 
         ldr r0, [r0, #20]
         mov r1, #0
         cmp r0, r1
         beq ChessBoard$getCurChess$L211
ChessBoard$getCurChess$L212: 
         mov r0, #79
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$getCurChess$L211: 
         mov r0, #88
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^printHint

.balign 4
.global ChessBoard$printHint
.section .text

ChessBoard$printHint:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$printHint$L214: 
         mov r4, r0
         mov r0, #73
         bl putch
         mov r0, #116
         bl putch
         mov r0, #39
         bl putch
         mov r0, #115
         bl putch
         mov r0, #32
         bl putch
         ldr r1, [r4, #32]
         mov r0, r4
         blx r1
         bl putch
         mov r0, #39
         bl putch
         mov r0, #115
         bl putch
         mov r0, #32
         bl putch
         mov r0, #116
         bl putch
         mov r0, #117
         bl putch
         mov r0, #114
         bl putch
         mov r0, #110
         bl putch
         mov r0, #58
         bl putch
         mov r0, #10
         bl putch
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ChessBoard^printBoard

.balign 4
.global ChessBoard$printBoard
.section .text

ChessBoard$printBoard:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$printBoard$L215: 
         mov r6, r0
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         mov r4, #0
ChessBoard$printBoard$L217: 
         ldr r0, [r6, #12]
         cmp r4, r0
         blt ChessBoard$printBoard$L216
ChessBoard$printBoard$L218: 
         ldr r0, [r6, #4]
         ldr r1, [r0, #68]
         blx r1
         ldr r1, [r6, #48]
         mov r0, r6
         blx r1
         mov r5, #0
ChessBoard$printBoard$L220: 
         ldr r0, [r6, #16]
         cmp r5, r0
         blt ChessBoard$printBoard$L219
ChessBoard$printBoard$L221: 
         ldr r1, [r6, #48]
         mov r0, r6
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$printBoard$L216: 
         ldr r0, [r6, #8]
         ldr r2, [r0, #84]
         mov r1, r4
         blx r2
         bl putch
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         add r4, r4, #1
         b ChessBoard$printBoard$L217
ChessBoard$printBoard$L219: 
         ldr r0, [r6, #8]
         ldr r2, [r0, #84]
         mov r1, r5
         blx r2
         bl putch
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         mov r0, #124
         bl putch
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         mov r4, #0
ChessBoard$printBoard$L223: 
         ldr r0, [r6, #12]
         cmp r4, r0
         blt ChessBoard$printBoard$L222
ChessBoard$printBoard$L224: 
         mov r0, #124
         bl putch
         ldr r0, [r6, #4]
         ldr r1, [r0, #68]
         blx r1
         add r5, r5, #1
         b ChessBoard$printBoard$L220
ChessBoard$printBoard$L222: 
         ldr r0, [r6, #0]
         ldr r1, [r6, #12]
         mul r1, r5, r1
         add r2, r1, r4
         ldr r1, [r0]
         cmp r2, r1
         bge ChessBoard$printBoard$L225
ChessBoard$printBoard$L226: 
         add r1, r2, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         ldr r0, [r0]
         bl putch
         ldr r0, [r6, #4]
         ldr r1, [r0, #72]
         blx r1
         add r4, r4, #1
         b ChessBoard$printBoard$L223
ChessBoard$printBoard$L225: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^printWin

.balign 4
.global ChessBoard$printWin
.section .text

ChessBoard$printWin:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$printWin$L227: 
         ldr r1, [r0, #32]
         blx r1
         bl putch
         mov r0, #32
         bl putch
         mov r0, #119
         bl putch
         mov r0, #105
         bl putch
         mov r0, #110
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
         mov r0, #103
         bl putch
         mov r0, #97
         bl putch
         mov r0, #109
         bl putch
         mov r0, #101
         bl putch
         mov r0, #46
         bl putch
         mov r0, #10
         bl putch
         mov r0, #0
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