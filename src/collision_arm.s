    .syntax unified
    .arm
    .text
    .align 2

    .global recca_positions_overlap_gas
    .type recca_positions_overlap_gas, %function
recca_positions_overlap_gas:
    cmp r0, r2
    bne .Lpositions_no_hit
    cmp r1, r3
    moveq r0, #1
    movne r0, #0
    bx lr

.Lpositions_no_hit:
    mov r0, #0
    bx lr
    .size recca_positions_overlap_gas, .-recca_positions_overlap_gas

    .global recca_ranges_overlap_gas
    .type recca_ranges_overlap_gas, %function
recca_ranges_overlap_gas:
    cmp r0, r3
    bgt .Lranges_no_hit
    cmp r2, r1
    movle r0, #1
    movgt r0, #0
    bx lr

.Lranges_no_hit:
    mov r0, #0
    bx lr
    .size recca_ranges_overlap_gas, .-recca_ranges_overlap_gas

    .global recca_projectile_hits_player_gas
    .type recca_projectile_hits_player_gas, %function
recca_projectile_hits_player_gas:
    cmp r1, r3
    bne .Lplayer_no_hit
    cmp r0, r2
    blt .Lplayer_no_hit
    add r2, r2, #2
    cmp r0, r2
    movle r0, #1
    movgt r0, #0
    bx lr

.Lplayer_no_hit:
    mov r0, #0
    bx lr
    .size recca_projectile_hits_player_gas, .-recca_projectile_hits_player_gas

    .global recca_projectile_hits_enemy_gas
    .type recca_projectile_hits_enemy_gas, %function
recca_projectile_hits_enemy_gas:
    cmp r1, r3
    bne .Lenemy_no_hit

    ldr ip, [sp]
    cmp ip, #6
    moveq ip, #2
    beq .Lenemy_half_ready
    cmp ip, #7
    moveq ip, #3
    movne ip, #0

.Lenemy_half_ready:
    sub r1, r2, ip
    cmp r0, r1
    blt .Lenemy_no_hit
    add r2, r2, ip
    cmp r0, r2
    movle r0, #1
    movgt r0, #0
    bx lr

.Lenemy_no_hit:
    mov r0, #0
    bx lr
    .size recca_projectile_hits_enemy_gas, .-recca_projectile_hits_enemy_gas

    .global recca_player_hits_enemy_gas
    .type recca_player_hits_enemy_gas, %function
recca_player_hits_enemy_gas:
    cmp r1, r3
    bne .Lplayer_enemy_no_hit

    ldr ip, [sp]
    cmp ip, #6
    moveq ip, #2
    beq .Lplayer_enemy_half_ready
    cmp ip, #7
    moveq ip, #3
    movne ip, #0

.Lplayer_enemy_half_ready:
    sub r1, r2, ip
    add r0, r0, #2
    cmp r0, r1
    blt .Lplayer_enemy_no_hit
    add r2, r2, ip
    sub r0, r0, #2
    cmp r0, r2
    movle r0, #1
    movgt r0, #0
    bx lr

.Lplayer_enemy_no_hit:
    mov r0, #0
    bx lr
    .size recca_player_hits_enemy_gas, .-recca_player_hits_enemy_gas

    .global recca_capped_timer_add_gas
    .type recca_capped_timer_add_gas, %function
recca_capped_timer_add_gas:
    sub r3, r2, r1
    cmp r0, r3
    addlt r0, r0, r1
    movge r0, r2
    bx lr
    .size recca_capped_timer_add_gas, .-recca_capped_timer_add_gas

    .section .note.GNU-stack,"",%progbits
