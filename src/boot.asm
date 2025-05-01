; bootloader (boot sector)
[org 0x7c00]
[bits 16]

start:
    ; initializing segments
    xor ax, ax
    mov ds, ax 
    mov es, ax 
    mov ss, ax 
    mov sp, 0x7c00  ; stack pointer

    ; loading kernel to memory
    mov ah, 0x02    ; function for reading sectors
    mov al, 1       ; sectors to read
    mov ch, 0       ; cylinder 0
    mov cl, 2       ; sector 2 
    mov dh, 0       ; header 0
    mov bx, 0x7e00  ; kernel address
    int 0x13        ; bios interrupt for disk reading
    jc diskerr      ; if error jump to error
    ; jumping to kernel
    jmp 0x7e00

diskerr:
    mov si, errmsg
    call print_string
    jmp $

print_string:
    lodsb
    cmp al, 0
    je .done
    mov ah, 0x0e
    int 0x10
    jmp print_string
.done:
    ret

errmsg: db 'Disk read error!', 0  

; padding
times 510-($-$$) db 0
dw 0xaa55         
