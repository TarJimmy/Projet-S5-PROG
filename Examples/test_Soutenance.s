  @ Test regroupant l'ensemble des parties du projet
.data
	A: .byte 10
	B: .byte 11
	C: .byte 12
	  
.text
.global main

main:
  @ Tests de certains types d'instruction
	mov r4, #7		 			@r4 = 7
	add r5, r4, r4	 			@r5 = 14
	adds r5, r4, lsl #2			@
	tst r4, #3					@modification des flags
	mov r0, #2					@r0 = 2
    cmp r0, #3					@Si r0 < 3
    addlt r0, r0, #1			@	r0++
    cmp r0, #3					@Si r0 > 3
    addgt r0, r0, #1			@	r0++
	
	@ Scaled register pre-indexed
  	ldr r0, ptr_a
  	sub r0, r0, #1
  	strb r4, [r0, #1]! @ r4 est chargé dans A
  	@ r0 modifié
  	
  	@ Register post-indexed
  	mov r2, #1
  	ldr r0, ptr_b
	ldrsh r1, [r0], r2
	
	@ Immediate offset
	ldr r0, ptr_c
	sub r0, r0, #1
	ldrsb r1, [r0, #1]
	
	mov r0, #7
	mov r1, #8
	mov r2, #9
	@ Decrement After LDMDA + STMIB
	add sp, sp, #12
	STMDA sp!, {r0, r1, r2}
	LDMIB sp!, {r3, r4, r5}

  @ TEST PROGRAMME
  @ Algorithme de multiplication (axb) par addition et decalage
  @ Convention d'appel : a : r0, b : r1, valeur retour : r0
  @ resultat : r2, au: confondu avec a: r0
  
  	mov r0, #2
  	mov r1, #3

	stmdb   sp!, {r1,r2}	@ sauver aussi b par precaution
	bic r2, r0, #1        	@ resultat =0
	cmp   r0,#0        		@ if (a<0)
	rsblt   r0,r0,#0   		@   au = -a
	rsblt   r1,r1,#0   		@   b = -b
	b   condtq        		@ while (au != 0)
tq: 
	tst   r0,#1        		@ if ((au&1) != 0)
    addne   r2,r2,r1      	@ resultat = resultat + b
    mov   r1,r1, LSL #1 	@ b = b *2
    mov   r0,r0, LSR #1 	@ au = au /2
condtq:
	cmp   r0,#0
	bne   tq
	mov   r0,r2      		@ return resultat
	swi 0x123456
	
	ptr_a: .word A
    ptr_b: .word B
    ptr_c: .word C
