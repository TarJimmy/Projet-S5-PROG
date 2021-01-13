.global main
.text
main:
    mov r0, #10
    sub r0, R0, #4
    swi 0x123456
fin: