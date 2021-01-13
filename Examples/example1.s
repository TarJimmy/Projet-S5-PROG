.global main
.text
main:
    ldr r1, =0x20026
    ldrb  R2, [R0]
    swi 0x123456
.data
donnee:
    .word 0x11223344
    .word 0x55667788
fin: