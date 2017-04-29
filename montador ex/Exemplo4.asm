JMP Inicio

include testecall.asm
; Código de teste do simulador 
;
;

Funcao_Troca_AX_BX:
;{
	PUSH AX
	PUSH BX
	MOV Var1 BX
	MOV BX AX
	MOV AX BX
;================
	POP BX
	POP AX
;}
RET


Testa_Multiplicacao:
;{
	PUSH AX
	PUSH BX
	PUSH DX
;testa mul entre registradores 16-8bits
	MOV AX 4d
	MOV BH 2d
	MUL BX
;* entre registradores 16-16 bits
	MOV AX 3000d
	MOV BX 100d
	MUL BX
;* entre registrador e imediato
	MOV AX 4d
	MUL 2d
;* entre registrador e memoria
	MOV AX,2d
	MUL [Inicio] ; no conteudo de inicio tem um MOV, esse mov representa a operação de Registrador 16 bits e imediato, logo esse mov possui valor 00000111 (ou 7)
	; logo é a mesma coisa que 2*7=14
	POP DX
	POP BX
	POP AX
;}
RET


Inicio:; 100h
CALL testecall
MOV AX 1h
MOV BH 10b
MOV BL 3d
MOV CX 4
ADD AX BX
INC CX
DEC CX
INC CH
DEC CL
Var1 DW 100h
SUB BX CX
MOV BX Inicio
MOV [BX]                       00010100b; estou alterando o tipo MOV para ADD na linha 100h
CALL Funcao_Troca_AX_BX
CALL Testa_Multiplicacao
MOV AX 1d
MOV BX 2d
CMP AX BX
JE EIGUAL
JNE NEIGUAL
EIGUAL:
 MOV AX 0
 JMP TesteFim
NEIGUAL:
 MOV BX 0
 TesteFim:
RET