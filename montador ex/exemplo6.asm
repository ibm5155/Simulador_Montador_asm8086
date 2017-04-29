;==================TESTE DO SIMULADOR/MONTADOR=======================
;====================================================================
; O OBJETIVO DESTE PROGRAMA É O TESTE DE TODOS OS CASOS IMPLEMENTADOS
; DE VALIDAÇÃO OU NÃO DO SIMULADOR E DO MONTADOR
;
;
;

W_Var1 DW 100d

JMP main ; função inicial

TesteFuncao:;para a declaração de uma função será utilizado labels.
	MOV AX ax ; só para não perder o conteúdo
RET

FUNC_Rec1: ; função que chama outra função
	CALL FUNC_Rec2
RET

FUNC_Rec2: ; função que chama outra função
	CALL FUNC_Rec3
RET
FUNC_Rec3: ; função que chama outra função
	CALL FUNC_Rec4
RET

FUNC_Rec4: ; função que chama outra função
	CALL FUNC_Rec5
RET

FUNC_Rec5: ; função que chama outra função
	;inverte ax com bx
	PUSH AX
	PUSH BX
	POP AX
	POP BX
RET

main:
	B_Var2 DB 10d
;TESTE 1, CASOS DE MOV:
	MOV AX 5  ;se não tem sinal então é deduzido que seja um decimal
	MOV AX 5d ;implicitamente um decimal
	MOV AX 5h ;implicitamente um hexadecimal
	MOV AX 101b;implicitamente um binario

	MOV AL 5d
	MOV AH AL
;	MOV AX AL ; modo invalido
;	MOV AL AX ; modo invalido

	MOV BX AX ; transfere o dado de AX para BX
	MOV [fim] 5d ; transfere o valor 5 para a posição da label
	MOV BX fim
	MOV [BX] 6d
	
;   teste de PUSH POP
;   inverte os dados de AX e BX usando push pop
	PUSH AX
	PUSH BX
	POP AX
	POP BX
	
;	adiciona um valor fixo em CX
	PUSH 600d
	POP CX
	
;	teste chamada de função
	CALL TesteFuncao ; chamada simples
	MOV AX TesteFuncao
	CALL AX; faz a mesma coisa que o call anterior
	
	CALL FUNC_Rec1; chamada de função que chama outras funções
	;sem teste de função recursiva pois vai entrar em loop infinito por 
	;nao conter condições de parada com if e else
	
	JMP Pula_Ret
	RET;se executar este codigo e o fim de jogo do programa
	RET
	RET	
	RET
	Pula_Ret:
	
;	TESTE DE ULA
	MOV AX 101b
	MOV BX 001b
	ADD AX BX ; resultado = 110b
	SUB AX BX ; resultado = 101b
	INC BX ; resultado = 10b  = 2
	DEC AX ; resultado = 100b = 4
	MUL BL ; AX * BL =  AX | AX = 8 = 1000b
	DIV BL ; AX / BL =  AX = 4 = 100b DX = resto = 0b
;   como BL é um operando de 8 bits, o operando da divisao e salvo em AL e o resto e 
;   salvo em AH	
	MOV AX 4d
	DIV 3d ; AX = 1 DX = 1 | é tratado o 3d como 16 bits logo o operando é salvo em AX
;            e o resto em DX
	NOT BX
	
	ADD AL B_Var2
	ADD AH B_Var4
	ADD BX W_Var1
	
;teste variaveis
; 	MOV B_Var2 W_Var1 ; invalido
	MOV CL B_Var2
	INC CX
	MOV B_Var2 CL
;	ADD B_Var2 B_Var4
;	ADD W_Var1 W_Var3
	
fim:
RET ; lixo de memória que não será executado
RET ; lixo de memória que não será executado

W_Var3 DW 15d
B_Var4 DB 12d