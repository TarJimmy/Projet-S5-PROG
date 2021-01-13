.global main
.text
main:
	@Chaque instruction (sauf TST TEQ CMP CMN) avec valeur immédiate
    mov r0, #10
    add r4, r0, #3
    sub r7, r4, #3

    mvn r4, #1
    and r4, r4, #1
    orr r4, r4, #1
    mov r2, #2
    eor r2, r2, #1

    mov r4, #1
    bic r4, r4, #1
    mov r8, #10
    rsb r7, r8, #3
    sbc r7, r8, #3
    adc r7, r8, #5

	@Avec registre (aussi equivalent a shift 0)
    mov r4, #10
    mov r5, #10
    add r4, r4, r5
    mov r4, #10
    add r4, r4, r5, lsl #0

	@Les 4 shift(avec valeur immediate puis avec registre) sur 1 instruction
    mov r4, #10
    mov r5, #10
    add r4, r4, r5, lsl #2
    mov r4, #10
    add r4, r4, r5, lsr #2
    mov r4, #10
    add r4, r4, r5, asr #2
    mov r4, #10
    add r4, r4, r5, ror #2
    
    mov r2, #2
    mov r4, #10
    mov r5, #10
    add r4, r4, r5, lsl r2
    mov r4, #10
    add r4, r4, r5, lsr r2
    mov r4, #10
    add r4, r4, r5, asr r2
    mov r4, #10
    add r4, r4, r5, ror r2

	@Verfication de la maj des flags (modification de cpsr)
    mov r4, #10
    ands r5, r4, #3
    add r5, r4, #1
    sub r5, r4, #2
    adc r5, r4, #1
    
    mov r4, #2147483648
    and r5, r4, #2147483648
    add r5, r4, #2147483648  @addition avec carry
    adc r5, r4, #2147483648
    mov r4, #0
    sub r5, r4, #1 @soustraction avec empreint

    mov r4, #10
    mov r2, #1
    and r5, r4, #3
    adds r5, r4, lsl #2
    sub r5, r4, #2
    adc r5, r4, #1

	@Test : TST TEQ CMP CMN
    mov r4, #1
    tst r4, #3
    teq r4, #1
    cmp r4, #3
    cmn r4, #3
    
    @Operation avec condition
    mov     r0, #2
    cmp     r0, #3
    addlt   r0, r0, #1
    cmp     r0, #3
    addgt   r0, r0, #1
    
    swi 0x123456

