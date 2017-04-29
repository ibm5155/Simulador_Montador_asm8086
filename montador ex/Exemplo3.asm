MOV AX 0
MOV BX 5
Inicio:
CMP AX BX
JE Fim
Add AX 1
JMP Inicio
Fim:
RET