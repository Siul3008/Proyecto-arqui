    .syntax unified
    .arm
    .text
    .align 2

;************************************************************************
; Función: 
;   recca_fill_board_gas
; Descripción: 
;   Llena la matriz de puntos en blanco
; Entradas: 
;   Puntero a matriz board
;   Cantidad total de celdas
;   Caracter de relleno
; Salidas: 
;   Ninguna
;************************************************************************
    .global recca_fill_board_gas    ;Exporta nombre de funcion para que pueda ser llamada desde los .c
    .type recca_fill_board_gas, %function   ;Declara al enlazador que es una función ejecutable
recca_fill_board_gas:   
    cmp r1, #0  ;Verifica si ya no quedan celdas por rellenar
    bxle lr ;Branch y exit si r1 es menor o igual a 0

.Lfill_loop:    ;Si no es menor o igual, pasa al bucle
    strb r2, [r0], #1   ;Guarda en la direccion a la que apunta r0, el caracter guardado en r2, además, incrementa r0 una vez
    subs r1, r1, #1 ;Resta 1 al contador de celdas y actualiza el estado de las banderas de la ALU
    bne .Lfill_loop ;Si r1 no es cero, vuelve al bucle
    bx lr   ;Si en menor, sale y regresa al .c
    .size recca_fill_board_gas, .-recca_fill_board_gas  ;Calcula cuantos bytes ocupa la funcion


;************************************************************************
; Función: 
;   recca_color_for_char_gas
; Descripción: 
;   Equivalente a un switch(caracter), es decir, recibe un caracter y devuelve
;       el indice correspondiente al color de este
; Entradas: 
;   Caracter Ascii a evaluar
; Salidas: 
;   Indice correspondiente al color que debe tener el caracter ingresado
;************************************************************************
    .global recca_color_for_char_gas    ;Exporta nombre de funcion para que pueda ser llamada desde los .c
    .type recca_color_for_char_gas, %function   ;Declara al enlazador que es una función ejecutable
recca_color_for_char_gas:
    ;Comparaciones de caracteres
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

    ;Si no hay coincidencias, devuelve que es texto estandar
    mov r0, #6
    bx lr

.Lplayer:   ;Si es de jugador
    mov r0, #1
    bx lr

.Lplayer_shot:  ;Si es disparo de jugador
    mov r0, #2
    bx lr

.Lenemy:    ;Si es enemigo
    mov r0, #3
    bx lr

.Lenemy_shot:   ;Si es disparo de enemigo
    mov r0, #4
    bx lr

.Lpowerup:  ;Si es un powerup
    mov r0, #7
    bx lr

.Lstar: ;Si es una estrella (efecto de explosion)
    mov r0, #8
    bx lr
    .size recca_color_for_char_gas, .-recca_color_for_char_gas  ;Calcula cuantos bytes ocupa la funcion

;************************************************************************
; Función: 
;   recca_put_projectiles_on_board_gas
; Descripción: 
;   Encargada de colocar en la matriz de pantalla, los proyectiles 
;       activos
; Entradas: 
;   Matriz de pantalla board   
;   Arreglo de proyectiles
;   Cantidad maxima de proyectiles
;   Caracter a colocar
; Salidas: 
;   Ninguna
;************************************************************************
    .global recca_put_projectiles_on_board_gas  ;Exporta nombre de funcion para que pueda ser llamada desde los .c
    .type recca_put_projectiles_on_board_gas, %function ;Declara al enlazador que es una función ejecutable
recca_put_projectiles_on_board_gas:
    push {r4-r8, lr}    ;Salva en stack los registros del r4 al r8 y lr, esto parea no corromper datos provenientes de C
    ;Movimiento parametros a registros seguros, es decir, hace un respaldo
    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, r3  
    mov r8, #60 ;Guarda el ancho del tablero en r8

.Lprojectile_loop:
    cmp r6, #0
    ble .Lprojectile_done   ;Si cantidad restante es menor o igual a cero, sale del bucle
    ldr r0, [r5, #16]   ;Mueve a r0 el valor del parámetro active, este se encuentra adelantado 16 bits   
    cmp r0, #0  
    beq .Lprojectile_next   ;Si el proyectil se encuetra inactivo, salta al siguiente
    ldr r1, [r5]    ;Carga coordenada x
    ldr r2, [r5, #4]    ;Carga coordenada y
    cmp r1, #0  
    blt .Lprojectile_next   ;Si se sale por la izquierda de la pantalla, salta al siguiente
    cmp r1, #60
    bge .Lprojectile_next   ;Si se sale por la derecha de la pantalla, salta al siguiente 
    cmp r2, #0
    blt .Lprojectile_next   ;Si se sale por arriba, salta al siguiente
    cmp r2, #24 
    bge .Lprojectile_next   ;Si se sale por abajo, salta al siguiente
    mla r0, r2, r8, r1  ;De lo contrario, multiplica y acumua, es decir, aplica indexacion de matrices (r0 = (r2 * 60) + r1).
    strb r7, [r4, r0]   ;Guarda el caracter destindo, en esa posicion calculada

.Lprojectile_next:  
    add r5, r5, #20 ;Avanza el puntero 20 bytes, tomando en cuenta que cada proyectil esta compuesto por 20 bytes
    subs r6, r6, #1 ;Decrementa en 1 la cantidad de proyectiles pendientes
    b .Lprojectile_loop ;Vuelve al bucle

.Lprojectile_done:
    pop {r4-r8, pc} ;Recupera los datos previamente guardados en el stack, haciendo que la funcion termine y vuelva al .c
    .size recca_put_projectiles_on_board_gas, .-recca_put_projectiles_on_board_gas  ;Calcula cuantos bytes ocupa la funcion

;************************************************************************
; Función: 
;   recca_put_projectiles_on_board_gas
; Descripción: 
;   Encargada de colocar en la matriz de pantalla, los efectos 
;       activos
; Entradas: 
;   Matriz de pantalla board   
;   Arreglo de efectos
;   Cantidad maxima de proyectiles
;   Caracter a colocar
; Salidas: 
;   Ninguna
;************************************************************************
    .global recca_put_effects_on_board_gas
    .type recca_put_effects_on_board_gas, %function
recca_put_effects_on_board_gas:
    push {r4-r8, lr}    ;Salva en stack los registros del r4 al r8 y lr, esto parea no corromper datos provenientes de C
    ;Movimiento parametros a registros seguros, es decir, hace un respaldo
    mov r4, r0
    mov r5, r1
    mov r6, r2
    mov r7, r3
    mov r8, #60

.Leffect_loop:
    cmp r6, #0
    ble .Leffect_done   ;Si cantidad restante es menor o igual a cero, sale del bucle
    ldr r0, [r5, #8]    ;Mueve a r0 el valor del parámetro active, este se encuentra adelantado 8 bits 
    cmp r0, #0
    beq .Leffect_next   ;Si el proyectil se encuetra inactivo, salta al siguiente
    ldr r1, [r5]        ;Carga coordenada x
    ldr r2, [r5, #4]    ;Carga coordenada y
    cmp r1, #0
    blt .Leffect_next   ;Si se sale por la izquierda de la pantalla, salta al siguiente
    cmp r1, #60
    bge .Leffect_next   ;Si se sale por la derecha de la pantalla, salta al siguiente
    cmp r2, #0
    blt .Leffect_next   ;Si se sale por arriba, salta al siguiente
    cmp r2, #24
    bge .Leffect_next   ;Si se sale por abajo, salta al siguiente
    mla r0, r2, r8, r1  ;De lo contrario, multiplica y acumua, es decir, aplica indexacion de matrices (r0 = (r2 * 60) + r1).
    strb r7, [r4, r0]   ;Guarda el caracter destindo, en esa posicion calculada

.Leffect_next:
    add r5, r5, #16     ;Avanza el puntero 16 bytes, tomando en cuenta que cada proyectil esta compuesto por 16 bytes
    subs r6, r6, #1     ;Decrementa en 1 la cantidad de proyectiles pendientes
    b .Leffect_loop     ;Vuelve al bucle

.Leffect_done:
    pop {r4-r8, pc} ;Recupera los datos previamente guardados en el stack, haciendo que la funcion termine y vuelva al .c
    .size recca_put_effects_on_board_gas, .-recca_put_effects_on_board_gas  ;Calcula cuantos bytes ocupa la funcion

    .section .note.GNU-stack,"",%progbits   ;Directiva de LINUX que indica que el archivo no ocupa que se genere una pila ejecutable, esto previene inyecciones de codigo externas
