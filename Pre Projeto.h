/*==============================*/
Arquitetura do processador
/*==============================*/
Instruções de 8Bits e registradores de 2 bits

/*==============================*/
Instruções a serem implementadas:
/*==============================*/

MOV,POP,PUSH,NOT,AND,OR,XOR,ADD,SUB,DIV,MUL,CMP,INC,DEC,
JMP,CALL,RET,JA,JB,JE,JGE,JLE,JNE,JZ,RET

/*==============================*/
Operações que o registrador pode realizar
/*==============================*/

REGISTRADOR,
MEMORIA,
REGISTRADOR - REGISTRADOR,
REGISTRADOR - MEMORIA,
MEMORIA - REGISTRADOR.

/*==============================*/
Registradores a serem implementada:
/*==============================*/
-Uso geral
AH,AL
//CH,CL
//DH,DL
BH,BL

-Uso especifico
CS,IP
/*==============================*/
Arquivo fonte
/*==============================*/
-Será gerado um arquivo binario em forma de texto.
-O arquivo final terá o formato do arquivo como .com

/*==============================*/
Estilo de escrita
/*==============================*/
todo objecto será separado por espaço
ex: "MOV AX BX" (OK)
"MOV, AX, BX" (ÑOK)
"MOVAXBX" (ÑOK)

/*==============================*/
Comentário
/*==============================*/
o comentário será dado por ';'

/*==============================*/

/*==============================*