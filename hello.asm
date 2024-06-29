section .data
    hello db 'Hello, World!', 10, 0  ; 10 est le code ASCII pour '\n'

section .text
    global _start

_start:
    ; sys_write (64-bit)
    mov rax, 1          ; numéro de syscall pour sys_write
    mov rdi, 1          ; descripteur de fichier (stdout)
    mov rsi, hello      ; message à écrire
    mov rdx, 14         ; longueur du message (13 pour "Hello, World!" + 1 pour '\n')
    syscall             ; appel au noyau

    ; sys_exit (64-bit)
    mov rax, 60         ; numéro de syscall pour sys_exit
    xor rdi, rdi        ; code de retour 0
    syscall             ; appel au noyau
