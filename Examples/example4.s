.global main
.text
main:
    ldmia r4!, {r6, r7}
    swi 0x123456 
fin:
