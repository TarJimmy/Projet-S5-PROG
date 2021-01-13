@ Utilisation des instructions LDRSH, LDRSB

.data
	A: .hword 10
	B: .hword 11
	C: .byte 12
	D: .byte 13

.global main
.text

main:
    mov r2, #1
  @@@ LDRSH @@@
@ Immediate offset
	ldr r0, ptr_a
	sub r0, r0, #1
	ldrsh r1, [r0, #1]
	
@ Resgister offset
	
	ldr r0, ptr_b
	sub r0, r0, #1
	ldrsh r1, [r0, r2]


@ Immediate pre-indexed
	ldr r0, ptr_a
	sub r0, r0, #1
	ldrsh r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_b
	sub r0, r0, #1
	ldrsh r1, [r0, r2]!


@ Immediate post-indexed
	ldr r0, ptr_a
	ldrsh r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_b
	ldrsh r1, [r0], r2


  @@@ LDRSB @@@
@ Immediate offset
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsb r1, [r0, #1]
	
@ Resgister offset
	ldr r0, ptr_d
	sub r0, r0, #1
	ldrsb r1, [r0, r2]


@ Immediate pre-indexed
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsb r1, [r0, #1]!
	
@ Resgister pre-indexed
	ldr r0, ptr_d
	sub r0, r0, #1
	ldrsb r1, [r0, r2]!


@ Immediate post-indexed
	ldr r0, ptr_c
	ldrsb r1, [r0], #1

@ Register post-indexed
	ldr r0, ptr_d
	ldrsb r1, [r0], r2

	swi 0x123456
	
 ptr_a: .word A
 ptr_b: .word B
 ptr_c: .word C
 ptr_d: .word D