.global main
.text
main:
    ldr r1, =donnee
    ldr r2, =donnee
    swi 0x123456
.data
donnee:
    .word 0x11223344
    .word 0x55667788
fin: