;linhas abaixo deverao ser validadas
variavel DW 1h
label1:
mov AX BX
label3:
mov AX [BX]
MOV [bX] aX; falta tratar esse
mov AL [BL]
mov [BL] AL
mov AX variavel
mov AX 1h
JMP label1
JMP variavel
JMP 1h
JMP AX
mov variavel 1h
ADD AX label1
MOV [600h] AX
MOV AX [600h]
MOV [600h] AL
MOV AL [600h]

INC CX
INC CH
INC variavel
label2:
;linhas abaixo nao deverao ser validadas

;tipos incompativeis
mov AX BL;2bytes 1byte
mov BL AX; 1byte x 2bytes
;registradores que nao fazem referencia de memória
mov [AL] BL;AL nao faz referencia a memoria
mov [AL] BX;AL nao faz referencia a memoria
mov [AL] 100d;AL nao faz referencia a memoria
mov [AL] label1;AL nao faz referencia a memoria
mov [AL] variavel;AL nao faz referencia a memoria
mov [AX] BX;AX nao faz referencia a memoria
mov [AX] BL;AX nao faz referencia a memoria
mov [AX] 100d;AX nao faz referencia a memoria
mov [AX] label1;AX nao faz referencia a memoria
mov [AX] variavel;AX nao faz referencia a memoria
;tipos nao permitidos
mov 1h AX;nao tem pra onde mover
mov label1 AX;nao tem pra onde mover
INC 1h
INC label1
MOV ax,12j; tipo de número desconhecido
JMP AL     ; somente operando de 16bits
ADD label1 AX
MOV [label1] AX ; até é validado pelo emu8086, mas nao tem o porque de ser validado


mov AX BX CX; muitos elementos
AX mov BX  ; ordem errada
; Teste de redeclaracao
MOV:       ; redeclaracao de simbolos
AX:        ;label com nome de simbolos
CMP DW 10h ; redeclaracao de simbolos
AX  DW 10h ; redeclaracao de simbolos
label1 DW 10h; redeclaracao de simbolos
AX DW 10h  ; redeclaracao de simbols

