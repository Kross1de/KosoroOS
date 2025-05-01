; main kernel file
[org 0x7e00]
[bits 16]

kernel_start:
    ; setting video mode 12h (640x480, 16 colors)
    mov ax, 0x12
    int 0x10

    ; pallete
    mov ax, 0x1003
    mov bx, 0
    int 0x10

    ; drawing pixel =)
    mov ax, 0x0c0f
    mov cx, 320     ; x coordinate
    mov dx, 240     ; y coordinate
    int 0x10

    ; infinite loop
    jmp $

times 512-($-$$) db 0   
