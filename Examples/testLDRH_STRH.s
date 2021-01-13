@ Utilisation de tous les cas de la fonction LDRH
@ p.473 -- A5-33

.data
  A: .hword 10
  B: .hword 11
  C: .hword 12
  
.global main
.text

main:
@ Immediate offset
  ldr r0, ptr_a
  add r0, r0, #1
  ldrh r1, [r0, #-1] @ A est chargé dans r1
  
@ Register offset
  ldr r0, ptr_b
  sub r0, r0, #1
  mov r2, #1
  ldrh r1, [r0, r2] @ B est chargé dans r1

  
@ Immediate pre-index
  ldr r0, ptr_a
  sub r0, r0, #1
  ldrh r1, [r0, #1]! @ A est chargé dans r1
  @ r0 modifié contrairement à avant

@ Register pre-indexed
  ldr r0, ptr_b
  sub r0, r0, #1
  ldrh r1, [r0, r2]! @ B est chargé dans r1
  @ r0 modifié

  
@ Immediate post-indexed
  ldr r0, ptr_a
  ldrh r1, [r0], #1 @ A est chargé dans r1
  @ r0 est modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_a

@ Register post-indexed
  ldr r0, ptr_b
  ldrh r1, [r0], r2 @ B est chargé dans r1
  @ r0 modifié
  sub r0, r0, #1 @ r0 à la valeur de ptr_b

  swi 0x123456

  ptr_a: .word A
  ptr_b: .word B
  ptr_c: .word C