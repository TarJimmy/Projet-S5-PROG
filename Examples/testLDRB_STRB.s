@ Utilisation de tous les cas de la fonction LDRB
.data
	A: .byte 10
	B: .byte 11
	C: .byte 12

.global main
.text

main:
@@@ LDR @@@
@ Immediate offset
  ldr r0, ptr_a
  add r0, r0, #1
  ldrb r1, [r0, #-1] @ A est chargé dans r1
  
@ Register offset
  ldr r0, ptr_b
  sub r0, r0, #1
  mov r2, #1
  ldrb r1, [r0, r2] @ B est chargé dans r1
  
@ Scaled register offset
  @ LSL
  ldr r0, ptr_c
  sub r0, r0, #2
  ldrb r1, [r0, r2, LSL #1] @ C est chargé dans r1

  
@ Immediate pre-index
  ldr r0, ptr_a
  sub r0, r0, #1
  ldrb r1, [r0, #1]! @ A est chargé dans r1
  @ r0 modifié contrairement à avant

@ Register pre-indexed
  ldr r0, ptr_b
  sub r0, r0, #1
  ldrb r1, [r0, r2]! @ B est chargé dans r1
  @ r0 modifié

@ Scaled register pre-indexed
  @ LSL
  ldr r0, ptr_c
  sub r0, r0, #2
  ldrb r1, [r0, r2, LSL #1]! @ C est chargé dans r1
  @ r0 modifié

  
@ Immediate post-indexed
  ldr r0, ptr_a
  ldrb r1, [r0], #1 @ A est chargé dans r1
  @ r0 est modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_a

@ Register post-indexed
  ldr r0, ptr_b
  ldrb r1, [r0], r2 @ B est chargé dans r1
  @ r0 modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_b

@ Scaled register post-indexed
  @ LSL
  ldr r0, ptr_c
  ldrb r1, [r0], r2, LSL #1 @ C est chargé dans r1
  @ r0 modifié
  sub r0, r0, #2 @ r0 à la valeur de ptr_c

@@@ STR @@@ 
@ Immediate offset
  mov r1, #1
  ldr r0, ptr_a
  sub r0, r0, #1
  strb r1, [r0, #1] @ r1 est chargé dans A 
  
@ Register offset
  ldr r0, ptr_b
  sub r0, r0, #1
  mov r2, #1 @ r2 contient 1 pour tout le programme
  strb r1, [r0, r2] @ r1 est chargé dans B
  
@ Scaled register offset
  @ LSL
  ldr r0, ptr_c
  sub r0, r0, #2
  strb r1, [r0, r2, LSL #1] @ r1 est chargé dans C

  
@ Immediate pre-index !
  mov r1, #3 
  ldr r0, ptr_a
  sub r0, r0, #1
  strb r1, [r0, #1]! @ r1 est chargé dans A
  @ r0 modifié contrairement à avant

@ Register pre-indexed !
  ldr r0, ptr_b
  sub r0, r0, #1
  strb r1, [r0, r2]! @ r1 est chargé dans B
  @ r0 modifié

@ Scaled register pre-indexed !
  @ LSL
  ldr r0, ptr_c
  sub r0, r0, #2
  strb r1, [r0, r2, LSL #1]! @ r1 est chargé dans C
  @ r0 modifié

  
@ Immediate post-indexed
  mov r1, #5 
  ldr r0, ptr_a
  strb r1, [r0], #1 @ r1 est chargé dans A
  @ r0 est modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_a

@ Register post-indexed !
  ldr r0, ptr_b
  strb r1, [r0], r2 @ r1 est chargé dans B
  @ r0 modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_b

@ Scaled register post-indexed
  @ LSL
  ldr r0, ptr_c
  strb r1, [r0], r2, LSL #1 @ r1 est chargé dans C
  @ r0 modifié
  sub r0, r0, #2 @ r0 à la valeur de ptr_c
  
  swi 0x123456

  ptr_a: .word A
  ptr_b: .word B
  ptr_c: .word C