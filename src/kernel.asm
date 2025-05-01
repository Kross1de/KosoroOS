; main kernel file
[org 0x7e00]
[bits 16]

kernel_start:
    ; setting video mode 12h (640x480, 16 colors)
    mov ax, 0x12
    int 0x10

    ; disable interrupts
    cli

    ; loading GDT
    lgdt [gdt_descriptor]

    ; switching to protected mode
    mov eax, cr0
    or eax, 1       ; setting the PE (Protected Mode Enable) bit
    mov cr0, eax

    jmp 0x08:protected_mode_start

; GDT
gdt_start:
    ; null descriptor
    dd 0x00000000
    dd 0x00000000

    ; code segment (CS): base address 0, limit 4GB, 32-bit
    dw 0xFFFF
    dw 0x0000
    db 0x00
    db 0x9A
    db 0xCF
    db 0x00

    ; data segment (DS): base address 0, limit 4GB, 32-bit
     dw 0xFFFF
     dw 0x0000
     db 0x00
     db 0x92
     db 0xCF
     db 0x00
gdt_end: 

gdt_descriptor:
    dw gdt_end - gdt_start - 1  ; GDT size
    dd gdt_start                ; GDT address

[bits 32]
protected_mode_start:
    ; setting register segments
    mov ax, 0x10    ; data segment
    mov ds, ax
    mov es, ax 
    mov fs, ax 
    mov gs, ax 
    mov ss, ax
    mov esp, 0x90000; installing stack  

    ; drawing pixel =)
    mov dx, 0x3C4   ; sequencer port
    mov ax, 0x0F02  ; map mask register (0x02), enable all planes (0x0f)
    out dx, ax 

    mov dx, 0x3CE   ; graphics controller port
    mov ax, 0x0005  ; write mode 0 (0x00), read mode 0, register 0x05
    out dx, ax
    mov ax, 0xff08  ; bit mask register (0x08), enable all bits
    out dx, ax

    mov edi, 0xA0000; vga memory base
    add edi, 19240  ; offset for (320,240)

    mov al, 0x80    ; bit 7
    mov [edi], al   ; write to set the pixel in all planes (color 0x0f)
    
    ; infinite loop
    jmp $      

times 512-($-$$) db 0   
