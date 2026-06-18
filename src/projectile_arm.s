    .syntax unified
    .arm
    .text
    .align 2

    .global recca_projectiles_update_gas
    .type recca_projectiles_update_gas, %function
recca_projectiles_update_gas:
    cmp r1, #0
    bxle lr

.Lprojectile_update_loop:
    ldr r2, [r0, #16]
    cmp r2, #0
    beq .Lprojectile_update_next

    ldr r2, [r0]
    ldr r3, [r0, #8]
    add r2, r2, r3
    str r2, [r0]

    ldr r3, [r0, #4]
    ldr ip, [r0, #12]
    add r3, r3, ip
    str r3, [r0, #4]

    cmp r2, #0
    blt .Lprojectile_deactivate
    cmp r2, #60
    bge .Lprojectile_deactivate
    cmp r3, #0
    blt .Lprojectile_deactivate
    cmp r3, #24
    blt .Lprojectile_update_next

.Lprojectile_deactivate:
    mov r2, #0
    str r2, [r0, #16]

.Lprojectile_update_next:
    add r0, r0, #20
    subs r1, r1, #1
    bne .Lprojectile_update_loop
    bx lr
    .size recca_projectiles_update_gas, .-recca_projectiles_update_gas

    .section .note.GNU-stack,"",%progbits
