; Código de teste do simulador 
;
;
JMP Inicio

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



Inicio:; 100h
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
RET