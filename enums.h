#ifndef _enums_HEADER
#define _enums_HEADER
enum Tamanho_Opcode{
	_8BITS, _16BITS
};

enum {
	NAO, SIM
};

enum Tipo_Dado{
	REGISTRADOR,MEMORIA,INSTRUCAO,NUMERO,LABEL,VARIAVEL,LIXO
};

enum Instrucao_Tipos_Operacao{
		MEMORIA_MEMORIA, //modo inválido, mas estará ai como somente referência ao nome
		MEMORIA_REGISTRADOR16,
		MEMORIA_REGISTRADOR8,
		REGISTRADOR16_MEMORIA,
		REGISTRADOR8_MEMORIA,
		REGISTRADOR16_REGISTRADOR16,
		REGISTRADOR8_REGISTRADOR8,
		REGISTRADOR16_VAZIO,
		REGISTRADOR8_VAZIO,
		MEMORIA_VAZIO,// memória/label
		VAZIO, //exemplo : RET
		REGISTRADOR16_VALOR,
		REGISTRADOR8_VALOR,
		VALOR_VAZIO,
	    VALOR_REGISTRADOR16, //modo invalido
	    VALOR_REGISTRADOR8, // modo invalido
	    VALOR_MEMORIA,      // modo invalido
	    VALOR_VALOR,        // modo invalido
		MEMORIA_VALOR
};

#define INICIO_COMENTARIO ';'
#endif

