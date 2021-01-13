@ Utilisation de tous les cas de la fonction LDM** et STM**
@ p.186 + p.481

.global main
.text

main:
	mov r0, #1
	mov r1, #2
	mov r2, #3

	@ Increment After LDMDA + STMIB
	STMIB sp!, {r0, r1, r2}
	LDMDA sp!, {r3, r4, r5}
mov r0, #4
mov r1, #5
mov r2, #6
	@ Increment Before LMDIB + STMDA
	STMIA sp!, {r0, r1, r2}
	LDMDB sp!, {r3, r4, r5}
mov r0, #7
mov r1, #8
mov r2, #9
	@ Decrement After LDMDA + STMIB
	add sp, sp, #12
	STMDA sp!, {r0, r1, r2}
	LDMIB sp!, {r3, r4, r5}
mov r0, #10
mov r1, #11
mov r2, #12
	@ Decrement Before LDMDB + STMIA
	STMDB sp!, {r0, r1, r2}
	LDMIA sp!, {r3, r4, r5}

	swi 0x123456