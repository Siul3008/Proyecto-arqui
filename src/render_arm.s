    .syntax unified
    .arm
    .text
    .align 2

    .global recca_fill_board_gas
    .type recca_fill_board_gas, %function
recca_fill_board_gas:
    cmp r1, #0
    bxle lr

.Lfill_loop:
    strb r2, [r0], #1
    subs r1, r1, #1
    bne .Lfill_loop
    bx lr
    .size recca_fill_board_gas, .-recca_fill_board_gas

    .global recca_color_for_char_gas
    .type recca_color_for_char_gas, %function
recca_color_for_char_gas:
    cmp r0, #'|'
    beq .Lplayer_shot
    cmp r0, #'O'
    beq .Lplayer_shot
    cmp r0, #'@'
    beq .Lplayer_shot
    cmp r0, #'*'
    beq .Lplayer_shot

    cmp r0, #'F'
    beq .Lpowerup
    cmp r0, #'S'
    beq .Lpowerup
    cmp r0, #'L'
    beq .Lpowerup
    cmp r0, #'T'
    beq .Lpowerup
    cmp r0, #'H'
    beq .Lpowerup
    cmp r0, #'D'
    beq .Lpowerup

    cmp r0, #'v'
    beq .Lenemy
    cmp r0, #'d'
    beq .Lenemy
    cmp r0, #'z'
    beq .Lenemy
    cmp r0, #'f'
    beq .Lenemy
    cmp r0, #'w'
    beq .Lenemy
    cmp r0, #'x'
    beq .Lenemy
    cmp r0, #'M'
    beq .Lenemy
    cmp r0, #'B'
    beq .Lenemy
    cmp r0, #'<'
    beq .Lenemy
    cmp r0, #'>'
    beq .Lenemy
    cmp r0, #'['
    beq .Lenemy
    cmp r0, #']'
    beq .Lenemy

    cmp r0, #'o'
    beq .Lenemy_shot
    cmp r0, #'.'
    beq .Lstar
    cmp r0, #':'
    beq .Lstar
    cmp r0, #'A'
    beq .Lplayer

    mov r0, #6
    bx lr

.Lplayer:
    mov r0, #1
    bx lr

.Lplayer_shot:
    mov r0, #2
    bx lr

.Lenemy:
    mov r0, #3
    bx lr

.Lenemy_shot:
    mov r0, #4
    bx lr

.Lpowerup:
    mov r0, #7
    bx lr

.Lstar:
    mov r0, #8
    bx lr
    .size recca_color_for_char_gas, .-recca_color_for_char_gas

    .global recca_put_projectiles_on_board_gas
    .type recca_put_projectiles_on_board_gas, %function
recca_put_projectiles_on_board_gas:
    push {r4-r8, lr}
    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, r3
    mov r8, #60

.Lprojectile_loop:
    cmp r6, #0
    ble .Lprojectile_done
    ldr r0, [r5, #16]
    cmp r0, #0
    beq .Lprojectile_next
    ldr r1, [r5]
    ldr r2, [r5, #4]
    cmp r1, #0
    blt .Lprojectile_next
    cmp r1, #60
    bge .Lprojectile_next
    cmp r2, #0
    blt .Lprojectile_next
    cmp r2, #24
    bge .Lprojectile_next
    mla r0, r2, r8, r1
    strb r7, [r4, r0]

.Lprojectile_next:
    add r5, r5, #20
    subs r6, r6, #1
    b .Lprojectile_loop

.Lprojectile_done:
    pop {r4-r8, pc}
    .size recca_put_projectiles_on_board_gas, .-recca_put_projectiles_on_board_gas

    .global recca_put_effects_on_board_gas
    .type recca_put_effects_on_board_gas, %function
recca_put_effects_on_board_gas:
    push {r4-r8, lr}
    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, r3
    mov r8, #60

.Leffect_loop:
    cmp r6, #0
    ble .Leffect_done
    ldr r0, [r5, #8]
    cmp r0, #0
    beq .Leffect_next
    ldr r1, [r5]
    ldr r2, [r5, #4]
    cmp r1, #0
    blt .Leffect_next
    cmp r1, #60
    bge .Leffect_next
    cmp r2, #0
    blt .Leffect_next
    cmp r2, #24
    bge .Leffect_next
    mla r0, r2, r8, r1
    strb r7, [r4, r0]

.Leffect_next:
    add r5, r5, #16
    subs r6, r6, #1
    b .Leffect_loop

.Leffect_done:
    pop {r4-r8, pc}
    .size recca_put_effects_on_board_gas, .-recca_put_effects_on_board_gas

    .section .note.GNU-stack,"",%progbits
