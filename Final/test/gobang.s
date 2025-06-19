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
Math$mod$L100: 
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
Math$wait$L106: 
         mov r0, #0
         mov r3, #100
         mov r2, #100
         mul r2, r3, r2
         mov r3, #100
         mul r2, r2, r3
         mul r1, r2, r1
Math$wait$L101: 
         cmp r0, r1
         blt Math$wait$L102
Math$wait$L103: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$wait$L102: 
         add r0, r0, #1
         b Math$wait$L101

@ Here's function: Math^dec2hex

.balign 4
.global Math$dec2hex
.section .text

Math$dec2hex:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Math$dec2hex$L112: 
         mov r0, #10
         cmp r1, r0
         blt Math$dec2hex$L109
Math$dec2hex$L110: 
         mov r0, #55
         add r0, r0, r1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$dec2hex$L109: 
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
Math$hex2dec$L137: 
         mov r0, #48
         cmp r1, r0
         bge Math$hex2dec$L121
Math$hex2dec$L123: 
Math$hex2dec$L124: 
         mov r0, #65
         cmp r1, r0
         bge Math$hex2dec$L133
Math$hex2dec$L135: 
Math$hex2dec$L136: 
         mov r0, #0
         sub r0, r0, #1
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$hex2dec$L121: 
         mov r0, #57
         cmp r1, r0
         ble Math$hex2dec$L122
         b Math$hex2dec$L123
Math$hex2dec$L122: 
         sub r0, r1, #48
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Math$hex2dec$L133: 
         mov r0, #70
         cmp r1, r0
         ble Math$hex2dec$L134
         b Math$hex2dec$L135
Math$hex2dec$L134: 
         sub r0, r1, #55
         sub sp, fp, #32
         pop {r4-r10, fp, pc}

@ Here's function: ConsoleOp^putSP

.balign 4
.global ConsoleOp$putSP
.section .text

ConsoleOp$putSP:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ConsoleOp$putSP$L138: 
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
ConsoleOp$putLF$L139: 
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
ConsoleOp$removeLine$L140: 
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
ConsoleOp$getchar$L153: 
         bl getch
ConsoleOp$getchar$L141: 
         mov r1, #32
         cmp r0, r1
         beq ConsoleOp$getchar$L142
ConsoleOp$getchar$L152: 
         mov r1, #10
         cmp r0, r1
         beq ConsoleOp$getchar$L142
ConsoleOp$getchar$L143: 
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ConsoleOp$getchar$L142: 
         bl getch
         b ConsoleOp$getchar$L141

@ Here's function: Game^init

.balign 4
.global Game$init
.section .text

Game$init:
         push {r4-r10, fp, lr}
         add fp, sp, #32
Game$init$L154: 
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
Game$mainLoop$L155: 
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
Game$clear$L156: 
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
Game$run$L162: 
         mov r4, r0
         ldr r1, [r4, #36]
         mov r0, r4
         blx r1
Game$run$L157: 
         mov r5, #1
         ldr r1, [r4, #40]
         mov r0, r4
         blx r1
         mov r1, #0
         cmp r0, r1
         bne Game$run$L158
Game$run$L159: 
         ldr r1, [r4, #28]
         mov r0, r4
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
Game$run$L158: 
         b Game$run$L157

@ Here's function: ChessBoard^mainLoop

.balign 4
.global ChessBoard$mainLoop
.section .text

ChessBoard$mainLoop:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$mainLoop$L227: 
         mov r5, r0
         ldr r1, [r5, #44]
         mov r0, r5
         blx r1
         ldr r1, [r5, #52]
         mov r0, r5
         blx r1
         ldr r0, [r5, #4]
         ldr r1, [r0, #64]
         ldr r0, [r5, #4]
         blx r1
         mov r6, r0
         ldr r0, [r5, #4]
         ldr r1, [r0, #64]
         ldr r0, [r5, #4]
         blx r1
         mov r4, r0
         ldr r0, [r5, #8]
         ldr r2, [r0, #88]
         ldr r0, [r5, #8]
         mov r1, r6
         blx r2
         mov r6, r0
         ldr r0, [r5, #8]
         ldr r2, [r0, #88]
         ldr r0, [r5, #8]
         mov r1, r4
         blx r2
         mov r4, r0
         mov r2, #1
         mov r0, #0
         mov r1, #0
         cmp r6, r1
         bge ChessBoard$mainLoop$L207
ChessBoard$mainLoop$L215: 
         mov r1, #0
         cmp r0, r1
         bne ChessBoard$mainLoop$L219
ChessBoard$mainLoop$L220: 
ChessBoard$mainLoop$L221: 
         ldr r3, [r5, #60]
         mov r2, r4
         mov r1, r6
         mov r0, r5
         blx r3
         ldr r3, [r5, #24]
         mov r2, r4
         mov r1, r6
         mov r0, r5
         blx r3
         mov r1, #0
         cmp r0, r1
         bne ChessBoard$mainLoop$L224
ChessBoard$mainLoop$L225: 
ChessBoard$mainLoop$L226: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$mainLoop$L207: 
         ldr r1, [r5, #16]
         cmp r6, r1
         blt ChessBoard$mainLoop$L210
         b ChessBoard$mainLoop$L215
ChessBoard$mainLoop$L210: 
         mov r1, #0
         cmp r4, r1
         bge ChessBoard$mainLoop$L213
         b ChessBoard$mainLoop$L215
ChessBoard$mainLoop$L213: 
         ldr r1, [r5, #12]
         cmp r4, r1
         ble ChessBoard$mainLoop$L214
         b ChessBoard$mainLoop$L215
ChessBoard$mainLoop$L214: 
         mov r0, #1
         b ChessBoard$mainLoop$L215
ChessBoard$mainLoop$L219: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$mainLoop$L224: 
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
ChessBoard$init$L235: 
         mov r2, r0
         mov r0, #0
         str r1, [r2, #4]
         str r1, [r2, #8]
         add r3, r2, #20
         mov r4, #0
         str r4, [r3]
ChessBoard$init$L228: 
         ldr r4, [r2, #16]
         ldr r3, [r2, #12]
         mul r3, r4, r3
         cmp r0, r3
         blt ChessBoard$init$L229
ChessBoard$init$L230: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$init$L229: 
         ldr r3, [r1]
         cmp r0, r3
         bge ChessBoard$init$L233
ChessBoard$init$L234: 
         add r3, r0, #1
         mov r4, #4
         mul r3, r3, r4
         add r3, r1, r3
         mov r4, #46
         str r4, [r3]
         add r0, r0, #1
         b ChessBoard$init$L228
ChessBoard$init$L233: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^clear

.balign 4
.global ChessBoard$clear
.section .text

ChessBoard$clear:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$clear$L236: 
         mov r4, r0
         ldr r1, [r4, #44]
         mov r0, r4
         blx r1
         add r1, r4, #20
         ldr r0, [r4, #20]
         mov r2, #1
         sub r0, r2, r0
         str r0, [r1]
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
ChessBoard$update$L248: 
         mov r4, r0
         ldr r0, [r4, #0]
         ldr r3, [r4, #12]
         mul r3, r1, r3
         add r3, r3, r2
         ldr r5, [r0]
         cmp r3, r5
         bge ChessBoard$update$L239
ChessBoard$update$L240: 
         add r3, r3, #1
         mov r5, #4
         mul r3, r3, r5
         add r0, r0, r3
         ldr r0, [r0]
         mov r3, #46
         cmp r0, r3
         beq ChessBoard$update$L243
ChessBoard$update$L244: 
ChessBoard$update$L245: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$update$L239: 
         mov r0, #-1
         bl exit
ChessBoard$update$L243: 
         ldr r0, [r4, #0]
         ldr r3, [r4, #12]
         mul r1, r1, r3
         add r3, r1, r2
         ldr r1, [r0]
         cmp r3, r1
         bge ChessBoard$update$L246
ChessBoard$update$L247: 
         ldr r2, [r4, #32]
         add r1, r3, #1
         mov r3, #4
         mul r1, r1, r3
         add r5, r0, r1
         mov r0, r4
         blx r2
         str r0, [r5]
         add r1, r4, #20
         ldr r0, [r4, #20]
         mov r2, #1
         sub r0, r2, r0
         str r0, [r1]
         b ChessBoard$update$L245
ChessBoard$update$L246: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^checkwin

.balign 4
.global ChessBoard$checkwin
.section .text

ChessBoard$checkwin:
         push {r4-r10, fp, lr}
         add fp, sp, #32
         sub sp, sp, #40
ChessBoard$checkwin$L529: 
         mov r10, r2
         str r10, [fp, #-44]
         mov r10, r1
         str r10, [fp, #-40]
         mov r10, r0
         str r10, [fp, #-36]
         mov r5, #0
         mov r0, #36
         bl malloc
         mov r1, #8
         str r1, [r0]
         add r1, r0, #4
         mov r2, #0
         str r2, [r1]
         add r1, r0, #8
         mov r2, #1
         str r2, [r1]
         add r1, r0, #12
         mov r2, #1
         str r2, [r1]
         add r1, r0, #16
         mov r2, #0
         str r2, [r1]
         add r1, r0, #20
         mov r2, #1
         str r2, [r1]
         add r1, r0, #24
         mov r2, #1
         str r2, [r1]
         add r1, r0, #28
         mov r2, #1
         str r2, [r1]
         add r1, r0, #32
         mov r2, #-1
         str r2, [r1]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r1, [r9, #12]
         ldr r9, [fp, #-40]
         mul r1, r9, r1
         ldr r10, [fp, #-44]
         add r1, r1, r10
         ldr r3, [r2]
         cmp r1, r3
         bge ChessBoard$checkwin$L249
ChessBoard$checkwin$L250: 
         add r1, r1, #1
         mov r3, #4
         mul r1, r1, r3
         add r1, r2, r1
         ldr r10, [r1]
         str r10, [fp, #-52]
         mov r10, r5
         str r10, [fp, #-48]
ChessBoard$checkwin$L251: 
         mov r1, #4
         ldr r9, [fp, #-48]
         cmp r9, r1
         blt ChessBoard$checkwin$L252
ChessBoard$checkwin$L253: 
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$checkwin$L249: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L252: 
         mov r1, #1
         ldr r9, [fp, #-40]
         mov r2, r9
         ldr r9, [fp, #-44]
         mov r10, r9
         str r10, [fp, #-56]
         mov r3, r2
         mov r2, #2
         ldr r9, [fp, #-48]
         mul r2, r9, r2
         ldr r5, [r0]
         cmp r2, r5
         bge ChessBoard$checkwin$L258
ChessBoard$checkwin$L259: 
         add r2, r2, #1
         mov r5, #4
         mul r2, r2, r5
         add r2, r0, r2
         ldr r2, [r2]
         add r7, r3, r2
         ldr r9, [fp, #-56]
         mov r10, r9
         str r10, [fp, #-68]
         mov r2, #2
         ldr r9, [fp, #-48]
         mul r2, r9, r2
         add r2, r2, #1
         ldr r3, [r0]
         cmp r2, r3
         bge ChessBoard$checkwin$L262
ChessBoard$checkwin$L263: 
         add r2, r2, #1
         mov r3, #4
         mul r2, r2, r3
         add r2, r0, r2
         ldr r2, [r2]
         ldr r9, [fp, #-68]
         add r2, r9, r2
         mov r5, r2
ChessBoard$checkwin$L264: 
         mov r6, r7
         mov r6, r4
         mov r2, #0
         cmp r6, r2
         bge ChessBoard$checkwin$L368
ChessBoard$checkwin$L266: 
         ldr r9, [fp, #-40]
         mov r2, r9
         ldr r9, [fp, #-44]
         mov r10, r9
         str r10, [fp, #-60]
         mov r3, r2
         mov r2, #2
         ldr r9, [fp, #-48]
         mul r2, r9, r2
         ldr r5, [r0]
         cmp r2, r5
         bge ChessBoard$checkwin$L392
ChessBoard$checkwin$L393: 
         add r2, r2, #1
         mov r5, #4
         mul r2, r2, r5
         add r2, r0, r2
         ldr r2, [r2]
         sub r6, r3, r2
         ldr r9, [fp, #-60]
         mov r10, r9
         str r10, [fp, #-72]
         mov r2, #2
         ldr r9, [fp, #-48]
         mul r2, r9, r2
         add r2, r2, #1
         ldr r3, [r0]
         cmp r2, r3
         bge ChessBoard$checkwin$L396
ChessBoard$checkwin$L397: 
         add r2, r2, #1
         mov r3, #4
         mul r2, r2, r3
         add r2, r0, r2
         ldr r2, [r2]
         ldr r9, [fp, #-72]
         sub r2, r9, r2
         mov r5, r2
ChessBoard$checkwin$L398: 
         mov r7, r6
         ldr r9, [fp, #-64]
         mov r7, r9
         mov r2, #0
         cmp r7, r2
         bge ChessBoard$checkwin$L502
ChessBoard$checkwin$L400: 
         mov r2, #5
         cmp r1, r2
         bge ChessBoard$checkwin$L526
ChessBoard$checkwin$L527: 
ChessBoard$checkwin$L528: 
         ldr r9, [fp, #-48]
         add r1, r9, #1
         mov r10, r1
         str r10, [fp, #-48]
         b ChessBoard$checkwin$L251
ChessBoard$checkwin$L258: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L262: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L265: 
         mov r3, r6
         mov r2, r5
         mov r4, r3
         mov r3, #2
         ldr r9, [fp, #-48]
         mul r3, r9, r3
         ldr r5, [r0]
         cmp r3, r5
         bge ChessBoard$checkwin$L384
ChessBoard$checkwin$L385: 
         add r3, r3, #1
         mov r5, #4
         mul r3, r3, r5
         add r3, r0, r3
         ldr r3, [r3]
         add r4, r4, r3
         mov r3, #2
         ldr r9, [fp, #-48]
         mul r3, r9, r3
         add r3, r3, #1
         ldr r5, [r0]
         cmp r3, r5
         bge ChessBoard$checkwin$L388
ChessBoard$checkwin$L389: 
         add r3, r3, #1
         mov r5, #4
         mul r3, r3, r5
         add r3, r0, r3
         ldr r3, [r3]
         add r2, r2, r3
         add r1, r1, #1
         mov r5, r2
         b ChessBoard$checkwin$L264
ChessBoard$checkwin$L368: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #16]
         cmp r6, r2
         blt ChessBoard$checkwin$L371
         b ChessBoard$checkwin$L266
ChessBoard$checkwin$L371: 
         mov r2, #0
         cmp r5, r2
         bge ChessBoard$checkwin$L374
         b ChessBoard$checkwin$L266
ChessBoard$checkwin$L374: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #12]
         cmp r5, r2
         blt ChessBoard$checkwin$L381
         b ChessBoard$checkwin$L266
ChessBoard$checkwin$L377: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L378: 
         add r2, r2, #1
         mov r8, #4
         mul r2, r2, r8
         add r2, r3, r2
         ldr r2, [r2]
         ldr r10, [fp, #-52]
         cmp r2, r10
         beq ChessBoard$checkwin$L265
         b ChessBoard$checkwin$L266
ChessBoard$checkwin$L381: 
         ldr r9, [fp, #-36]
         ldr r3, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #12]
         mul r2, r6, r2
         add r2, r2, r5
         ldr r8, [r3]
         cmp r2, r8
         bge ChessBoard$checkwin$L377
         b ChessBoard$checkwin$L378
ChessBoard$checkwin$L384: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L388: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L392: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L396: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L399: 
         mov r3, r7
         mov r2, r5
         mov r5, r3
         mov r3, #2
         ldr r9, [fp, #-48]
         mul r3, r9, r3
         ldr r7, [r0]
         cmp r3, r7
         bge ChessBoard$checkwin$L518
ChessBoard$checkwin$L519: 
         add r3, r3, #1
         mov r7, #4
         mul r3, r3, r7
         add r3, r0, r3
         ldr r3, [r3]
         sub r10, r5, r3
         str r10, [fp, #-64]
         mov r3, #2
         ldr r9, [fp, #-48]
         mul r3, r9, r3
         add r3, r3, #1
         ldr r5, [r0]
         cmp r3, r5
         bge ChessBoard$checkwin$L522
ChessBoard$checkwin$L523: 
         add r3, r3, #1
         mov r5, #4
         mul r3, r3, r5
         add r3, r0, r3
         ldr r3, [r3]
         sub r2, r2, r3
         add r1, r1, #1
         mov r5, r2
         b ChessBoard$checkwin$L398
ChessBoard$checkwin$L502: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #16]
         cmp r7, r2
         blt ChessBoard$checkwin$L505
         b ChessBoard$checkwin$L400
ChessBoard$checkwin$L505: 
         mov r2, #0
         cmp r5, r2
         bge ChessBoard$checkwin$L508
         b ChessBoard$checkwin$L400
ChessBoard$checkwin$L508: 
         ldr r9, [fp, #-36]
         ldr r2, [r9, #12]
         cmp r5, r2
         blt ChessBoard$checkwin$L515
         b ChessBoard$checkwin$L400
ChessBoard$checkwin$L511: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L512: 
         add r2, r2, #1
         mov r8, #4
         mul r2, r2, r8
         add r2, r3, r2
         ldr r2, [r2]
         ldr r10, [fp, #-52]
         cmp r2, r10
         beq ChessBoard$checkwin$L399
         b ChessBoard$checkwin$L400
ChessBoard$checkwin$L515: 
         ldr r9, [fp, #-36]
         ldr r3, [r9, #0]
         ldr r9, [fp, #-36]
         ldr r2, [r9, #12]
         mul r2, r7, r2
         add r2, r2, r5
         ldr r8, [r3]
         cmp r2, r8
         bge ChessBoard$checkwin$L511
         b ChessBoard$checkwin$L512
ChessBoard$checkwin$L518: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L522: 
         mov r0, #-1
         bl exit
ChessBoard$checkwin$L526: 
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
ChessBoard$printDivider$L535: 
         mov r4, r0
         ldr r0, [r4, #4]
         ldr r1, [r0, #72]
         ldr r0, [r4, #4]
         blx r1
         ldr r0, [r4, #4]
         ldr r1, [r0, #72]
         ldr r0, [r4, #4]
         blx r1
         mov r0, #43
         bl putch
         mov r5, #0
ChessBoard$printDivider$L530: 
         ldr r0, [r4, #12]
         cmp r5, r0
         blt ChessBoard$printDivider$L531
ChessBoard$printDivider$L532: 
         mov r0, #45
         bl putch
         mov r0, #43
         bl putch
         ldr r0, [r4, #4]
         ldr r1, [r0, #68]
         ldr r0, [r4, #4]
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$printDivider$L531: 
         mov r0, #45
         bl putch
         mov r0, #45
         bl putch
         add r5, r5, #1
         b ChessBoard$printDivider$L530

@ Here's function: ChessBoard^getCurChess

.balign 4
.global ChessBoard$getCurChess
.section .text

ChessBoard$getCurChess:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$getCurChess$L541: 
         ldr r0, [r0, #20]
         mov r1, #0
         cmp r0, r1
         beq ChessBoard$getCurChess$L538
ChessBoard$getCurChess$L539: 
         mov r0, #79
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$getCurChess$L538: 
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
ChessBoard$printHint$L542: 
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
ChessBoard$printBoard$L560: 
         mov r5, r0
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         mov r4, #0
ChessBoard$printBoard$L543: 
         ldr r0, [r5, #12]
         cmp r4, r0
         blt ChessBoard$printBoard$L544
ChessBoard$printBoard$L545: 
         ldr r0, [r5, #4]
         ldr r1, [r0, #68]
         ldr r0, [r5, #4]
         blx r1
         ldr r1, [r5, #48]
         mov r0, r5
         blx r1
         mov r4, #0
ChessBoard$printBoard$L548: 
         ldr r0, [r5, #16]
         cmp r4, r0
         blt ChessBoard$printBoard$L549
ChessBoard$printBoard$L550: 
         ldr r1, [r5, #48]
         mov r0, r5
         blx r1
         mov r0, #0
         sub sp, fp, #32
         pop {r4-r10, fp, pc}
ChessBoard$printBoard$L544: 
         ldr r0, [r5, #8]
         ldr r2, [r0, #84]
         ldr r0, [r5, #8]
         mov r1, r4
         blx r2
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         add r4, r4, #1
         b ChessBoard$printBoard$L543
ChessBoard$printBoard$L549: 
         ldr r0, [r5, #8]
         ldr r2, [r0, #84]
         ldr r0, [r5, #8]
         mov r1, r4
         blx r2
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         mov r0, #124
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         mov r6, #0
ChessBoard$printBoard$L553: 
         ldr r0, [r5, #12]
         cmp r6, r0
         blt ChessBoard$printBoard$L554
ChessBoard$printBoard$L555: 
         mov r0, #124
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #68]
         ldr r0, [r5, #4]
         blx r1
         add r4, r4, #1
         b ChessBoard$printBoard$L548
ChessBoard$printBoard$L554: 
         ldr r0, [r5, #0]
         ldr r1, [r5, #12]
         mul r1, r4, r1
         add r1, r1, r6
         ldr r2, [r0]
         cmp r1, r2
         bge ChessBoard$printBoard$L558
ChessBoard$printBoard$L559: 
         add r1, r1, #1
         mov r2, #4
         mul r1, r1, r2
         add r0, r0, r1
         ldr r0, [r0]
         bl putch
         ldr r0, [r5, #4]
         ldr r1, [r0, #72]
         ldr r0, [r5, #4]
         blx r1
         add r6, r6, #1
         b ChessBoard$printBoard$L553
ChessBoard$printBoard$L558: 
         mov r0, #-1
         bl exit

@ Here's function: ChessBoard^printWin

.balign 4
.global ChessBoard$printWin
.section .text

ChessBoard$printWin:
         push {r4-r10, fp, lr}
         add fp, sp, #32
ChessBoard$printWin$L561: 
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
