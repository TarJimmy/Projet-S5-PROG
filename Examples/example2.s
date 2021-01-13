.global main
.text
main:
    ldr r2, =donnee
    ldmia r2, {r5 - r6}
    ldmib r2, {r7 - r8}
    ldmda r2, {r3 - r4}
    ldmdb r2, {r1,r9}
    swi 0x123456
.data
donnee:
    .word 0x11223344
    .word 0x55667788
fin: