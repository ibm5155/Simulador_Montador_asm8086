; Nome: Rodrigo Sessegolo Pimpao   Lucas Zimerman
; Jogo: Genius 5 teclas
; Data: 18/06/2016
LabelNova3:
LabelNova3:
INCLUDE genius2.asm ; exemplo de include duplo
INCLUDE genius4.asm ; exemplo de include simples

NovaVar DB 1f

BACKUP_LINHA_COLORIDA DW 590h

NOTA_INICIAL               DW 606h
QNT_NOTAS_A_SEREM_TOCADAS  DW 600h
TEMPO_INICIADO             DW 602h
TEMPO_ATUAL                DW 604h

LIMITE_NOTAS   DW 20d

SOM_PERDEU_PT1 DW 5000h
SOM_PERDEU_PT2 DW 4000h
SOM_PERDEU_PT3 DW 3000h
SOM_PERDEU_PT4 DW 2000h
SOM_PERDEU_PT5 DW 1000h

VIDEO_SERVICES_INT DW 10h
REAL_TIME_CLOCK_SERVICES_INT DW 1Ah
READ_RTC DW 00h
DOS_SERVICES_INT21 DW 21h
CHARACTER_INPUT DW 01h



COD		SEGMENT
	ASSUME CS:COD  DS:COD  ES:COD  SS:COD
	ORG 100h
	
	TAMANHO_QUADRADO DW 05h
	TAMANHO_PRINT	DW	10h
	
MAIN	PROC	NEAR
	Main_Reset:
	MOV BX LIMITE_NOTAS
	MOV [BX] 2h
	CALL ORDENA_NOTAS
	CALL LIMPA_TELA
	CALL PRINTA_GENIUS
	CALL TOCA_SDWENCIA
	CALL CARREGA_TEMPO_INICIAL
	MOV AX 0h
	MOV BX 0h
	MOV CX 0h
	MOV DX 0h
	; LOOP TESTE
	main_loop:
	; ZIZI - TODO
	PUSH CX
	CALL LE_TECLA ; funcao usada para testes  refazer para comparar valor lido com valor gerado
	CALL CARREGA_TEMPO_ATUAL
	PUSH AX
	;================TIMEOUT
;	MOV BX TEMPO_INICIADO
;	MOV AX [BX]
;	ADD AX 180d
;	MOV BX TEMPO_ATUAL
;	MOV CX [BX]
;	CMP CX AX
;	JA Errrouuu ;se o horario atual for maior que o horario inicial + 10 segundos  perdeu...
	;===============TIMEOUT
	POP CX
	MOV AX 2h
	MUL CL      ; multiplica o contador por dois e salva em ax
	MOV BX NOTA_INICIAL ; endereço da primeira nota
	ADD BX AX   ; pega a nota atual a ser testada
	POP AX
	MOV DX [BX] ;salva a nota desejada em DX
	CMP AL DL   ;compara a nota tocada com a nota desejada
	JNE Errrouuu
	;acertou  incrementa número de elementos
	INC CX ; vai pra proxima nota
	MOV BX QNT_NOTAS_A_SEREM_TOCADAS
	MOV AX [BX]
	CMP AX CX ; chegou ao final?
	JE Terminou
	;{
	;não terminou então só repita o jogo
		PUSH AX
		POP AX
		PUSH AX
		POP AX
		CALL CARREGA_TEMPO_INICIAL
		JMP main_loop
	;}
	OutorRangeMain:
		JMP Main_Reset
	Terminou:
	;{
		MOV BX QNT_NOTAS_A_SEREM_TOCADAS
		MOV AX [BX]
		ADD AX 1
		MOV [BX] AX
		MOV CX 0h ; zera o contador
		;toca algo indicando que conseguiu		
		MOV AX SOM_PERDEU_PT1
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		MOV AX SOM_PERDEU_PT5
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		MOV AX SOM_PERDEU_PT5
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA

		CALL LIMPA_TELA
		CALL PRINTA_GENIUS

		CMP AX LIMITE_NOTAS ; passou do limite de notas :OS
		JE RECORDE
		
		CALL TOCA_SDWENCIA
		CALL CARREGA_TEMPO_INICIAL
		MOV CX 0h
		JMP main_loop
	;}
	Errrouuu:
	;{
		MOV AX SOM_PERDEU_PT5
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA

		MOV AX SOM_PERDEU_PT5
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		MOV AX SOM_PERDEU_PT3
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		MOV AX SOM_PERDEU_PT1
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		MOV AX SOM_PERDEU_PT1
		CALL INSERE_LINHAS_COLORIDAS
		CALL TOCA_NOTA
		
		CALL LIMPA_TELA
		CALL PRINTA_GENIUS

		
		JMP Main_Reset

	; Gerar numero rand
	JMP main_loop
	
	RECORDE:
;{
	CALL LIMPA_TELA
	CMP AL 'R'
	JE OutorRangeMain
	RET    
;}

	; Jogo encerra apertando ESC na LE_TECLA
	
MAIN	ENDP

CARREGA_TEMPO_INICIAL PROC NEAR
	PUSH AX
	PUSH BX
	PUSH CX
	PUSH DX
	MOV AH 00h
	INT REAL_TIME_CLOCK_SERVICES_INT
	MOV BX TEMPO_INICIADO
;	MOV AH 0h
;	MOV AL DH
;	MOV CX 60d
;	MUL CL
;	MOV DH 0d
;	ADD AX DX
	MOV [BX] DX
;	MOV [BX] AX
	POP DX
	POP CX
	POP BX
	POP AX
	RET
CARREGA_TEMPO_INICIAL ENDP

CARREGA_TEMPO_ATUAL PROC NEAR
	PUSH AX
	PUSH BX
	PUSH CX
	PUSH DX
	MOV AH 00h
	INT REAL_TIME_CLOCK_SERVICES_INT
	MOV BX TEMPO_ATUAL
;	MOV AH 0h
;	MOV AL DH
;	MOV CL 60d
;	MUL CX
;	MOV DH 0d
;	ADD AX DX
	MOV [BX] DX
;	MOV [BX] AX
	POP DX
	POP CX
	POP BX
	POP AX
	RET
CARREGA_TEMPO_ATUAL ENDP


TOCA_SDWENCIA PROC NEAR
	;Fim_Toca_SDWencia
	MOV CX 0h
	MOV BX NOTA_INICIAL
	Prox_SDWencia:
	MOV AX [BX]
	PUSH CX
	PUSH BX
	CMP AL 0h
	JE @@atualiza_genius1S
	CMP AL 1h
	JE @@atualiza_genius2S
	CMP AL 2h
	JE @@atualiza_genius3S
	CMP AL 3h
	JE @@atualiza_genius4S
	CMP AL 4h
	JE @@atualiza_genius5S
	Prox_Nota:
	POP BX
	POP CX
	CALL Delay
	CALL Delay
	CALL Delay
	CALL Delay
	CALL Delay
	CALL Delay
	MOV AL CL
	ADD BX 2h
	ADD CX 1h
	MOV DI QNT_NOTAS_A_SEREM_TOCADAS
	MOV AX [DI]
	CMP CX AX
	JL Prox_SDWencia ;toque todas as notas até que tenha chegado ao fim da lista de notas conhecidas
	RET
	
	@@atualiza_genius1S:
	CALL atualiza_genius1
	JMP Prox_Nota
	
	@@atualiza_genius2S:
	CALL atualiza_genius2
	JMP Prox_Nota
	
	@@atualiza_genius3S:
	CALL atualiza_genius3
	JMP Prox_Nota
	
	@@atualiza_genius4S:
	CALL atualiza_genius4
	JMP Prox_Nota
	
	@@atualiza_genius5S:
	CALL atualiza_genius5
	JMP Prox_Nota
TOCA_SDWENCIA ENDP

ORDENA_NOTAS PROC NEAR
	MOV BX QNT_NOTAS_A_SEREM_TOCADAS
	MOV [BX] 01h
	MOV CX 0h
	MOV BX NOTA_INICIAL
Loop_Ordena:
	CALL INSERE_LINHAS_COLORIDAS
	CALL RAND
	MOV [BX] AX
	ADD  BX 2h
	INC CX
	CMP CX LIMITE_NOTAS
	JNE LOOP_Ordena
	RET
ENDP

RAND PROC NEAR
    ;BACKUP DOS REGISTRADORES ANTIGOS
    PUSH BX
    PUSH CX
    PUSH DX 
    ; retorno: AX
	CALL Delay ; para variar melhor as notas
    MOV Ah READ_RTC
    INT REAL_TIME_CLOCK_SERVICES_INT ; pega os tics  (ignora a outra parte do CX)
    MOV AX DX
    PUSH AX      
    MOV AH 0h
    MOV BL 2h
    DIV BL
    CMP AH 1h ; e impar
    JE IMPAR_RAND
    ;multiplica a sobra
	CALL Delay
	CALL Delay
	CALL Delay
	CALL Delay
	CALL Delay
    POP AX
    MOV BL 2h
    MUL BL  
    JMP FIM_RAND  
    IMPAR_RAND:         
    ;retorna BL
	CALL Delay
	CALL Delay
	CALL Delay
    POP AX
    MOV BL 7h
    MUL BL
    FIM_RAND: ; faz o mod do que sobrou
    MOV AH 0h
    MOV BL 5h
    DIV BL 
    MOV AL AH
    MOV AH 0h
    ;resposta em AH
    POP DX
    POP CX
    POP BX
    RET
ENDP


LIMPA_TELA	PROC	NEAR
	MOV DH 0d ; linha inicial
	MOV DL 0d ; coluna inicial

	JMP limpa_tela_for_linha
	
	limpa_tela_for_linha:
	CMP DH 1Ah ; ultima linha
	JB limpa_tela_for_coluna
	JMP fim
	
	limpa_tela_for_coluna:
	MOV BH 0h ; pagina de video 0
	MOV AH 2h ; move o cursor para a posicao DH DL
	INT 10h
	MOV AL 0h ; caracter nulo
	MOV BL 00h ; cor preta -> nao precisa de cor pois o caracter eh nulo  mas...
	MOV CX 01h ; printa caracter 1 vez
	MOV AH 09h ; escreve caracter/atributo na tela
	INT 10h
	INC DL ; incrementa o index da coluna
	CMP DL 50h ; ultimo index visivel
	JB limpa_tela_for_coluna
	MOV DL 0h ; reseta a coluna
	INC DH ; incrementa a linha
	JMP limpa_tela_for_linha
	
	fim:
	RET
LIMPA_TELA	ENDP

ESCREVE_CARACTER	PROC	NEAR
	MOV BH 0h ; pagina de video 0
	MOV AH 2h ; move o cursor para a posicao DH DL
	INT 10h
	MOV AH 09h ; escreve caracter/atributo na tela
	INT 10h
	RET
ESCREVE_CARACTER	ENDP

DESENHA_QUADRADO	PROC	NEAR
	local count:byte
	MOV count 0h
	PUSH CX
	MOV CX 09h  ; repeticoes do caracter
	MOV AL 0DBh ; caracter █
	
	for_linha:
	CMP count TAMANHO_QUADRADO
	JE fim_quadrado
	CALL ESCREVE_CARACTER
	INC DH
	INC count
	JMP for_linha
	
	fim_quadrado:
	POP CX
	RET
DESENHA_QUADRADO	ENDP

PRINTA_GENIUS	PROC	NEAR
	;;Primeiro Quadrado
	MOV DH 03d
	MOV DL 13d
	MOV BL 01h ;Azul
	CALL DESENHA_QUADRADO
	;;Segundo Quadrado
	MOV DH 03d
	MOV DL 25d
	MOV BL 02h ;Verde
	CALL DESENHA_QUADRADO
	;;Terceiro Quadrado
	MOV DH 10d
	MOV DL 19d
	MOV BL 03h ;Ciano
	CALL DESENHA_QUADRADO
	;;Quarto Quadrado
	MOV DH 17d
	MOV DL 13d
	MOV BL 04h ;Vermelho
	CALL DESENHA_QUADRADO
	;;Quinto Quadrado
	MOV DH 17d
	MOV DL 25d
	MOV BL 05h ;Magenta
	CALL DESENHA_QUADRADO
	
	RET
PRINTA_GENIUS	ENDP

LE_TECLA	PROC	NEAR
	loop_tecla:
	MOV AH 00h ; le caracter do buffer -> resultado em AL
	INT 16h
	CMP AL 31h ; Numero 1
	JE @@atualiza_genius1
	CMP AL 32h ; Numero 2
	JE @@atualiza_genius2
	CMP AL 33h ; Numero 3
	JE @@atualiza_genius3
	CMP AL 34h ; Numero 4
	JE @@atualiza_genius4
	CMP AL 35h ; Numero 5
	JE @@atualiza_genius5
	CMP AL 1Bh ; ESC
	JE fim_jogo
	JMP loop_tecla ; se tecla nao reconhecida  le novamente
	
	fim_jogo:
	INT 20h
	RET
	
	@@atualiza_genius1:
	CALL atualiza_genius1
	RET
	
	@@atualiza_genius2:
	CALL atualiza_genius2
	RET
	
	@@atualiza_genius3:
	CALL atualiza_genius3
	RET
	
	@@atualiza_genius4:
	CALL atualiza_genius4
	RET
	
	@@atualiza_genius5:
	CALL atualiza_genius5
	RET
LE_TECLA	ENDP

TOCA_NOTA	PROC	NEAR
	MOV AL  182 
	OUT 43h  AL ;prepara a nota 
	OUT 42h  AL ; manda byte menos significativo 
	MOV AL  AH 
	OUT 42h  AL ; manda byte mais significativo 
	IN AL  61h ; verifica qual o valor está na porta 61h 
	OR AL  00000011b ; "seta" os dois bits menos significativos 
	OUT 61h  AL ; atualiza o valor na porta 61h ... reproduz 
	MOV BX  8h ; determina a duração do som
	JMP DURA
	
	DURA: 
	MOV CX  0FFFFh 
	DURAC: 
	DEC CX 
	JNE DURAC 
	DEC BX 
	JNE DURA 
	IN AL  61h ; verifica qual o valor está na porta 61h 
	AND AL  11111100b ; "zera" os dois bits menos significativos 
	OUT 61h  AL ; atualiza valor na porta 61h  fim reprodução
	RET
TOCA_NOTA	ENDP


INSERE_LINHAS_COLORIDAS PROC NEAR
PUSH AX
PUSH BX
PUSH CX
PUSH DX
;DH DL (row column)
MOV DH 0d
MOV DL 0d
FOR1:
CMP DH 19h
JE ENDFOR1
FOR2: ;SETA O CURSOR
MOV AH 2h
MOV BH 0
INT 10h ;ESCREVE UM CARACTER
mov ah  09h  
mov AL 176d
mov bh  0
mov DI BACKUP_LINHA_COLORIDA
mov bl  [DI] 
mov CX 1     
int 10h       
INC DL
CMP DL 50h
JE ENDFOR2
JMP FOR2
ENDFOR2:
MOV bh  0
MOV bl  [DI]
INC bl
CMP BL 0Fh
JLE Nzera
MOV bl 0
Nzera:
MOV [DI] bl  
MOV DL 0
INC DH
JMP FOR1
ENDFOR1:
POP DX
POP CX
POP BX
POP AX
RET
INSERE_LINHAS_COLORIDAS ENDP

atualiza_genius1 PROC NEAR
	MOV DH 03d
	MOV DL 13d
	MOV BL 09h ;Azul claro
	CALL DESENHA_QUADRADO
	MOV AX  4560d ; dó
	CALL TOCA_NOTA
	; depois de piscar o quadrado e tocar a nota  printa genius original
	MOV DH 03d
	MOV DL 13d
	MOV BL 01h ;Azul
	CALL DESENHA_QUADRADO
	MOV AX 0d
	ret
atualiza_genius1 ENDP

atualiza_genius2 PROC NEAR
	MOV DH 03d
	MOV DL 25d
	MOV BL 0Ah ;Verde
	CALL DESENHA_QUADRADO
	MOV AX  4063d ; re
	CALL TOCA_NOTA
	; depois de piscar o quadrado e tocar a nota  printa genius original
	MOV DH 03d
	MOV DL 25d
	MOV BL 02h ;Verde
	CALL DESENHA_QUADRADO
	MOV AX 1d
	RET
atualiza_genius2 ENDP
	
atualiza_genius3 PROC NEAR
	MOV DH 10d
	MOV DL 19d
	MOV BL 0Bh ;Ciano
	CALL DESENHA_QUADRADO
	MOV AX  3619d ; mi
	CALL TOCA_NOTA
	; depois de piscar o quadrado e tocar a nota  printa genius original
	MOV DH 10d
	MOV DL 19d
	MOV BL 03h ;Ciano
	CALL DESENHA_QUADRADO
	MOV AX 2d
	RET
atualiza_genius3 ENDP

atualiza_genius4 PROC NEAR
	MOV DH 17d
	MOV DL 13d
	MOV BL 0Ch ;Vermelho
	CALL DESENHA_QUADRADO
	  AX  3416d ; fa
	CALL TOCA_NOTA
	; depois de piscar o quadrado e tocar a nota  printa genius original
	MOV DH 17d
	MOV DL 13d
	MOV BL 04h ;Vermelho
	CALL DESENHA_QUADRADO
	MOV AX 3d
	RET
atualiza_genius4 ENDP
	
atualiza_genius5 PROC NEAR
	MOV DH 17d
	MOV DL 25d
	MOV BL 0Dh ;Magenta
	CALL DESENHA_QUADRADO
	MOV AX  3043d ; sol
	CALL TOCA_NOTA
	; depois de piscar o quadrado e tocar a nota  printa genius original
	MOV DH 17d
	MOV DL 25d
	MOV BL 05h ;Magenta
	CALL DESENHA_QUADRADO
	MOV AX 4d
	RET
atualiza_genius5 ENDP

Delay PROC NEAR
	PUSH CX
	PUSH BX
	PUSH AX
	PUSH DX
	mov     CX  001H ;quanto menor mais rapido o delay
    delRep: 
	push    CX
	mov     CX  0D090H
    delDec: 
	dec     CX
	jnz     delDec
	pop     CX
	dec     CX
	jnz     delRep
	POP DX
	POP AX
	POP BX
	POP CX
	RET
Delay ENDP

NovaVar3 DW 0xFFh
MAINTEMPO_ATUAL                DW 604h
FimDeTudo:
COD		ENDS
END

