#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "Texto.h"
#include "Log.h"
#include "Instrucao.h"
#include "Registrador.h"
#include "Variavel.h"
#include "Label.h"
#include "PilhaObjetos.h"
#include "enums.h"
#include "Mmain.h"
#include "Montador.h"
#include "../../MT2D/Window_core.h"
#include "../../MT2D/system_calls.h"
#include "../../MT2D/Keyboard.h"
#include "../../MT2D/building_functions/generic_menu.h"
#include "../../MT2D/building_functions/display_popup_with_result.h"
#include "../../MT2D/building_functions/display_popup.h"

#include "../../MT2D/building_functions/generic_string_reader.h"

char BUFFmsg[400];
int ParserMontadorErros = 0;
int PonteiroOpcode = 16;//indica em que posição está o próximo estágio. (utilizado para salvar o opcode das labels

TabelaLabel *TLabels=0;
TabelaInstrucao *TInstrucao=0;
TabelaRegistrador *TRegistrador=0;
TabelaVariavel *TVariavel=0;
TabelaPilhaOBJ *TOBJ = 0;//onde será salvo o ponteiro de todos os elementos a serem compilados
Texto *ArquivoFonteFinal = 0;//salva a estrutura final do código (não em codigo binario)

char *Opcodes = 0; //arquivo final codigo de maquina
int Opcodes_qnt = 0;

int Parser_Numero_Validado = 0;
/**
retorna 1 se for um número e 0 se não for.
retorna 2 se for um endereço de um número
Caso seja um número, o valor desse número será salvo em decimal na variavel "Parser_Numero_Validado"
tipos de números:
Xh : hexadecimal
Xd : decimal
Xb : binario
se não conter nenhum marcador, será conciderado um número decimal.
*/
int Parser_Numero(char *Linha) {
	unsigned char tipo = 0;
	int Saida = 0;
	bool Marcador = 0;
	int base = 10;
	int c = 0, d = 0;
	bool EndMem = false;
	int erro = 0;
	char *tmp = 0;
	char *LinhaNova = 0;
	if (Linha) {
		c = strlen(Linha) - 1;
		if (c >= 0) {
			if (Linha[0] == '[' && Linha[c] == ']') {
				EndMem = true;
				LinhaNova = (char*)malloc((c+1) * sizeof(char));
				for (d = 1; d < c; d++) {
					LinhaNova[d - 1] = Linha[d];
				}
				LinhaNova[d - 1] = '\0';
				d = 0;
			}
			else {
				LinhaNova = Linha;
			}

			//			filtra o tipo do número
			switch (LinhaNova[c]) {
			case 'h':
			case 'H':
				base = 16;
				Marcador = true;
				break;
			case 'b':
			case 'B':
				Marcador = true;
				base = 2;
				break;
			case 'd':
			case 'D':
				
				base = 10;
				Marcador = true;
				break;
			}
			tmp = (char*)malloc((c + 2) * sizeof(char));
			strcpy(tmp, LinhaNova);
			tmp[c + 1 - Marcador] = '\0';
			Parser_Numero_Validado = 0;
			Parser_Numero_Validado = (int)strtol(tmp, NULL, base);
			if (Parser_Numero_Validado == 0) { // evita cair em um caso falso
				while (d <= c - Marcador) {
					if (tmp[d] != '0') {
						d = c + 100 - 1;
						erro = true;
					}
					d++;
				}
			}
			if (erro == false) {
				if (EndMem == true) {
					Saida = 2;
				}
				else {
					Saida = 1;
				}
			}
			free(tmp);
			if (EndMem) {
				free(LinhaNova);
			}
			c = c;
		}

	}
	return Saida;
}

/**
2 se for um endereço de registrador [BX]
1 se for um registrador. BX
0 caso contrário. bla
-1 se for um endereço de registrador que não faz referência a memoria [AX]
*/
int  Parser_Registrador(char *string) {
	int Saida = false;
	int c = 0, d = 0;
	char *string_Nova = 0;
	if (string) {
		c = strlen(string);
		if (string[0] == '[' && string[c - 1] == ']') {
			string_Nova = (char*)malloc(c * sizeof(char));
			while (d < c - 2) {
				string_Nova[d] = string[d + 1];
				d++;
			}
			string_Nova[d] = '\0';
		}
		else {
			string_Nova = string;
		}
		c = 0;
		while (c < TRegistrador->Quantidade) {
			if (_stricmp(string_Nova, TRegistrador->Registradores[c].Nome) == 0) {
				if (d) {
					if (TRegistrador->Registradores[c].FazReferenciaMemoria == SIM) {
						Saida = 2;
					}
					else {
						Saida = -1;
					}
				}
				else {
					Saida = 1;
				}
				c = TRegistrador->Quantidade; //força um break
			}
			c++;
		}
		if (d) { //foi criado uma string nova?
			free(string_Nova);
		}
	}
	return Saida;
}

/**
retorna true caso a string seja uma instrução, false caso contrário
*/
bool Parser_Instrucao(char *string) {
	bool Saida = false;
	int c = 0;
	while (c < TInstrucao->QntInstrucao) {
		if (_stricmp(string, TInstrucao->Instrucoes[c].Nome) == 0) {
			Saida = true;
			c = TInstrucao->QntInstrucao; //força um break
		}
		c++;
	}
	return Saida;
}

/**
RETORNO:
2 = se for uma declaração de label.
1 =  é uma label
0 = não é uma lablel.
-1 = label com nome de registrador
-2 = label com nome de instrução
-3 = declaração de label duplicada
*/
int Parser_Label(char *string) {
	int Saida = true;
	int strlenght = strlen(string);
	if (string[strlenght - 1] == ':') {
		string[strlenght - 1] = '\0';
		Saida = -1;
	}
	int c = 0;
	if (Parser_Registrador(string) == true) {
		Saida = -1;
	}
	else if (Parser_Instrucao(string) == true) {
		Saida = -2;
	}
	else {//não é um registrador nem uma instrução, então verificar se é uma label
		while (c < TLabels->quantidade) {
			if (_stricmp(string, TLabels->label[c].Nome) == 0) {
				c = TLabels->quantidade + 10; //força um break | achou uma label
			}
			c++;
		}
		if (Saida == -1) {// devolve o ':' no final da string
						  //string[strlenght-1] = ':';
			if (c == TLabels->quantidade + 11) {//redefinição de labels
				Saida = -3;
			}
			else {
				Saida = 2;
				//				string[strlenght - 1] = ':';
			}
		}
		else if (c == TLabels->quantidade) {//Label não encontrada
			Saida = false;
		}
	}
	return Saida;
}

/**
RETORNO:
SIM : é uma variável.
NÃO : não é uma variável (se é então não está declarada)
*/
bool Parser_ChecaVariavel(char *string) {
	bool Saida = NAO;
	int i = 0;
	if (string) {
		for (i = 0; i < TVariavel->Quantidade && Saida == 0; i++) {
			if (_stricmp(string, TVariavel->Variaveis[i].Nome) == 0) {
				Saida = SIM; // achou uma variável
			}
		}
		return Saida;
	}
}

/**
Remove os espaços das bordas (borda da esquerda e direita)
*/
void Parser_Remove_Espacos(char *linha) {
	int i = 0;
	int j = 0;
	int l = 0;
	if (linha) {
		i = strlen(linha) - 1;
		if (i > 0) {
			while (linha[i] == ' ' || linha[i] == 9) {
				linha[i] = '\0';
				i--;
			}
			while (linha[j] == ' ' || linha[i] == 9) {
				j++;
			}
			//transfere a string do ponto j para o inicio de l
			if (j > 0) {
				for (; j <= i; j++, l++) {
					linha[l] = linha[j];
				}
				linha[l] = '\0';
			}//senão, não contem espacos antes da frase na linha
		}
	}
}

void Parser_Limpa_Comentarios(char *linha) {
	int fim = strlen(linha);
	int Limpa = false;

	for (int i = 0; i < fim; i++) {
		if (Limpa) {
			linha[i] = '\0';
		}
		else if (linha[i] == ';') {
			Limpa = i;
			linha[i] = '\0';

		}
	}
}

/**
RETORNO:
2 = se for uma declaração de variavel. (já adicona a variavel na tabela)
1 =  é uma variavel
0 = não é uma variavel.
-1 = variavel com nome de registrador
-2 = variavel com nome de instrução
-3 = variavel com nome de label
-4 = declaração de variavel duplicada
-5 = muitos elementos na string
-6 = variavel com nome de número
-7 = valor inicial na variavel difere de um numero
*/
int Parser_CarregaVariavel(Linha *linha, char *fonte,int linha_Arq) {
	int Saida = 0;
	int strlenght = 0;
	int Tipo = -1; //nada a declarar
	int i = 0;
	char *fragmento[3] = { 0,0,0 };
	if (linha) {
		fragmento[0] = Carrega_Proxima_Palavra(linha, &i);
		if (fragmento[0]) {
			fragmento[1] = Carrega_Proxima_Palavra(linha, &i);
			if (fragmento[1]) {
				fragmento[2] = Carrega_Proxima_Palavra(linha, &i);
				if (fragmento[2]) {//possível comparação de declaração de variavel
					if (_stricmp(fragmento[1], "DB") == 0) {
						Tipo = _8BITS;
					}
					else if (_stricmp(fragmento[1], "DW") == 0) {
						Tipo = _16BITS;
					}
					if (Tipo == _8BITS || Tipo == _16BITS) {
						if (i < linha->QntCaracteres) {
							//ainda tem coisa não processada nessa string
							Saida = -5;
						}
						else {
							//O uso de vários switches não da a melhor performance, entretanto deixa o código o mais organizado possível
							switch (Parser_Instrucao(fragmento[0])) {
							case SIM://90% de um código assembler a primeira palavra da linha é uma instrução, então primeiro cheque ela
								Saida = -2;
								break;
							case NAO:
								switch (Parser_Registrador(fragmento[0])) {
								case SIM:
									Saida = -1;
									break;
								case NAO:
									switch (Parser_Label(fragmento[0])) {
									case SIM:
										Saida = -3;
										break;
									case NAO:
										switch (Parser_Numero(fragmento[0])){
										case SIM:
										case 2:
											Saida = -6;
											break;
										case NAO:

											switch (Parser_Numero(fragmento[2])) {
											case NAO:
												Saida = -7;
												break;
											case SIM:
											case 2:
												if (Parser_ChecaVariavel(fragmento[0]) == NAO) {// LEGIT VAR
													AdicionaTabela(TVariavel, fragmento[0], fonte, linha_Arq, PonteiroOpcode, Parser_Numero_Validado, Tipo);
													//PonteiroOpcode += (Tipo + 1) * 8; não é mais salvo o opcode da variavel no inicio do código mas sim no final.
													Saida = 2;
												}
												else {
													Saida = -4;
												}
											}
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return Saida;
}

/**
 Retorna o ponteiro de uma string caso a linha descrita contenha um include.
 -O ponteiro irá "apontar" para o endereço da string a ser incluida...
 -Retorno também pode ser zero, dizendo que a linha processada não contém um include
*/
char *Parser_Include(char *string) {
	//Case sensitive...
	int c = 0;
	int d = 0;
	int fim = 0;
	char *TesteInclude = 0;
	char *Texto = 0;
	if (string) {
		fim = strlen(string);
		if (fim > 8) {
			TesteInclude = (char*)malloc(9 * sizeof(char));
			while (c < 8) {
				TesteInclude[c] = string[c];
				c++;
			}
			TesteInclude[c] = '\0';

			if (_stricmp("INCLUDE ", TesteInclude) == 0) {// include valido
				while (c < fim && string[c] == ' ') {
					c++;
				}
				if (c < fim) {
					Texto = (char*)malloc((fim - c + 1) * sizeof(char));
					while (c < fim) {
						Texto[d] = string[c];
						d++;
						c++;
					}
					Texto[d] = '\0';
				}

			}
			free(TesteInclude);
		}
	}
	return Texto;
}


void Limpa_Linha(Linha *linha) {
	Parser_Limpa_Comentarios(linha->Caracteres);
	Parser_Remove_Espacos(linha->Caracteres);
	linha->QntCaracteres = strlen(linha->Caracteres);
}

int Parser_Montador_Ordem = 1;//utilizado simplesmente para marcar cada chamada recursiva do parser do montador com uma id unica
/**
-Fonte: um objeto contendo um conjunto te caracteres.
-NivelChamada: serve exclusivamente para saber em qual recursão o Parser está
-ProcessamentoEtapaUm: Caso verdadeiro, Indica se o parser deverá realizar o pré-processamento de variáveis/labels/defines.
Caso falso, Indica ao parser que deverá realizar o processamento das instruções
*/
bool Parser_Montador(Texto *fonte , int NivelChamada,bool ProcessamentoEtapaUm) {
#pragma region COMENTÁRIO
/*ETAPAS: 
1: Verifica se o texto é vazio ou não
2: Limpa comentários e espaços em branco do código fonte
3: Carrega Labels
4: Carrega os includes (usando uma função recursividade do Parser_Montador)
5: Verifica a sintaxe dos comandos
6: Transforma em código de maquina
*/
/* Ações que o montador deve realizar:
-Substituir ender
eços simbólicos por endereços numéricos
-Podendo assumir que instruções são referentes ao Inicio (0)
-Reservar armazenamento para instruções e dados.
-Traduzir constantes em representação de maquina
-Não compilar um arquio duas vezes , ex: (a.asm e b.asm incluem c.asm , d.asm inclui a.asm e b.asm, logo c.asm só deve ser processado uma vez e não duas)
*/
#pragma endregion
#pragma region CÓDIGO
	bool existe = false;
	int  LinhaAtual = 0;
	int c = 0;
	char *d = 0;
	Texto *FonteInclude = 0;
	if (fonte) {
		existe = true;
#pragma region Parser Montador - Pré Montador
		if (ProcessamentoEtapaUm == true) {
			sprintf(BUFFmsg, "Parser %d Iniciado pre-montador.", NivelChamada);
			AdicionaLog(BUFFmsg);
			AtualizaTelaMontador();
			//ETAPA 1: filtro de label, remoção de comentários e espaços indesejados
			while (LinhaAtual < fonte->QntLinhas) {
				Limpa_Linha(&fonte->Linhas[LinhaAtual]);
				if (fonte->Linhas[LinhaAtual].Caracteres[0] == ';') { // se só tiver comentário na linha, "mate" ela
					free(fonte->Linhas[LinhaAtual].Caracteres);
					fonte->Linhas[LinhaAtual].Caracteres = (char*) malloc(2*sizeof(char));
					fonte->Linhas[LinhaAtual].Caracteres[0] = '\0';
					fonte->Linhas[LinhaAtual].QntCaracteres = 0;
				}
				if (fonte->Linhas[LinhaAtual].QntCaracteres > 0) {// não processa linhas vazias

					c = Parser_Label(fonte->Linhas[LinhaAtual].Caracteres);
					switch (c) {
					case -3:
						sprintf(BUFFmsg, "Erro no arquivo %s linha %d: redeclaracao da label %s", fonte->Fonte, LinhaAtual + 1, fonte->Linhas[LinhaAtual].Caracteres);
						AdicionaLog(BUFFmsg);
///						AtualizaTelaMontador();
						fonte->Linhas[LinhaAtual].Caracteres[fonte->Linhas[LinhaAtual].QntCaracteres - 1] = ':';
						break;
					case 2:
						AdicionaTabela(TLabels, fonte->Linhas[LinhaAtual].Caracteres, fonte->Fonte, LinhaAtual);
						sprintf(BUFFmsg, "Label %s adicionada.", fonte->Linhas[LinhaAtual].Caracteres);
						fonte->Linhas[LinhaAtual].Caracteres[fonte->Linhas[LinhaAtual].QntCaracteres - 1] = ':';
						AdicionaLog(BUFFmsg);
//						AtualizaTelaMontador();
						break;
					case 0://pode ser uma linha vazia
						//ETAPA 1.2: CARREGA INCLUDES
						d = Parser_Include(fonte->Linhas[LinhaAtual].Caracteres);
						if (d) {//encontrado o include, chame esse include em uma segunda função
							FonteInclude = CarregaTexto(d);
							if (FonteInclude == 0) {// confirmado, é uma label
								AdicionaTabela(TLabels, fonte->Linhas[LinhaAtual].Caracteres, fonte->Fonte, LinhaAtual);
								sprintf(BUFFmsg, "Parser %d: Erro, nao foi encontrado o arquivo %s", NivelChamada, d);
//								AdicionaLog(BUFFmsg);
								AtualizaTelaMontador();
							}
							else {
								sprintf(BUFFmsg, "Parser %d: Abrindo o arquivo %s", NivelChamada, d);
								AdicionaLog(BUFFmsg);
								AtualizaTelaMontador();
								Parser_Montador_Ordem++;
								Parser_Montador(FonteInclude, Parser_Montador_Ordem,true);
								Destroi_Texto(FonteInclude);
								sprintf(BUFFmsg, "Parser %d: Retomando processamento do arquivo %s", NivelChamada, fonte->Fonte);
								AdicionaLog(BUFFmsg);
								AtualizaTelaMontador();
							}
						}
						else {
							c = Parser_CarregaVariavel(&fonte->Linhas[LinhaAtual], fonte->Fonte, LinhaAtual);
							if (c != 0) {//algo relacionado a declaração de variáveis
								switch (c) {
								case 2:
									sprintf(BUFFmsg, "Parser %d: Variavel %s com dado inicial %d adicionado na linha %d - %s", NivelChamada, TVariavel->Variaveis[TVariavel->Quantidade-1].Nome, TVariavel->Variaveis[TVariavel->Quantidade - 1].Dado, TVariavel->Variaveis[TVariavel->Quantidade - 1].Linha+1, TVariavel->Variaveis[TVariavel->Quantidade - 1].ArquivoFonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -1:
									sprintf(BUFFmsg, "Parser %d Erro: Declaracao Variavel na linha %d tem nome de registrador - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -2:
									sprintf(BUFFmsg, "Parser %d Erro: Declaracao Variavel na linha %d tem nome de instrucao - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -3:
									sprintf(BUFFmsg, "Parser %d Erro: Declaracao Variavel na linha %d tem nome de label - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -4:
									sprintf(BUFFmsg, "Parser %d Erro: redeclaracao de Variavel na linha %d - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -5:
									sprintf(BUFFmsg, "Parser %d Erro: muitos elementos na linha %d para uma declaracao de variavel - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -6:
									sprintf(BUFFmsg, "Parser %d Erro: declaracao de variavel com nome de numero na linha %d - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								case -7:
									sprintf(BUFFmsg, "Parser %d Erro: valor inicial a ser atribuido a label se difere de um numero na linha %d - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									break;
								}
							}
						}
						break;
					}
				}
				LinhaAtual++;
			}
			sprintf(BUFFmsg, "Parser %d Finalizando pre-montador.", NivelChamada);
			AdicionaLog(BUFFmsg);
			AtualizaTelaMontador();
		}
#pragma endregion
#pragma region Parser_Montador - MONTADOR
		if(ProcessamentoEtapaUm == false || NivelChamada == 1){//somente a primeira chamada poderá iniciar o parser do montador (pois ele vai dar inicio a "festa"
			sprintf(BUFFmsg, "Parser %d Iniciado Montador.", NivelChamada);
			AdicionaLog(BUFFmsg);
			AtualizaTelaMontador();
			int Metapa = 0;
			char ObjetosEncontradosTipos[3] = {0,0,0};
			char *ObjetosEncontrados[3] = { 0,0,0 };
			bool ObjetosEncontrados_FazRefMemoria[3] = { 0,0,0 };// indica que o objeto encontrado está entre colchetes
			int validador = 0;
			int TipoInstrucao = 0;
			Registrador *RegistradoresEncontrados[2] = { NULL,NULL };
			Variavel *VariaveisEncontrados[2];
			LinhaAtual = 0;
			while (LinhaAtual < fonte->QntLinhas) {
#pragma region MONTADOR_PARTE1
				if (LinhaAtual + 1 == 9) {//usado para dar debug em uma linha específicado arquivo;
					LinhaAtual = LinhaAtual;
				}
				Limpa_Linha(&fonte->Linhas[LinhaAtual]);
				d = Parser_Include(fonte->Linhas[LinhaAtual].Caracteres);
				if (d) { // entra no include e compila o que tem ali...
					FonteInclude = CarregaTexto(d);
					if (FonteInclude == 0) {
						sprintf(BUFFmsg, "Parser %d: Erro, nao foi encontrado o arquivo %s", NivelChamada, d);
						AdicionaLog(BUFFmsg);
//						AtualizaTelaMontador();
						ParserMontadorErros++;
					}
					else {
						sprintf(BUFFmsg, "Parser %d: Abrindo o arquivo %s", NivelChamada, d);
						AdicionaLog(BUFFmsg);
						AtualizaTelaMontador();
						Parser_Montador_Ordem++;
						Parser_Montador(FonteInclude, Parser_Montador_Ordem, false);
						Destroi_Texto(FonteInclude);
						sprintf(BUFFmsg, "Parser %d: Retomando processamento do arquivo %s", NivelChamada, fonte->Fonte);
						AdicionaLog(BUFFmsg);
						AtualizaTelaMontador();
					}
				}
				else {
					d = (char*)1;
					ObjetosEncontradosTipos[0] = -1;
					ObjetosEncontradosTipos[1] = -1;
					ObjetosEncontradosTipos[2] = -1;
					Metapa = 0;
					c = 0;
					while (d != 0) {//processa caracter por caracter
						
						d = Carrega_Proxima_Palavra(&fonte->Linhas[LinhaAtual], &c);
						if (d) {
//=====================================TESTE INSTRUCAO
							if (Metapa == 3) {
								sprintf(BUFFmsg, "Parser %d linha %d: Muitos elementos na linha - %s", NivelChamada, LinhaAtual + 1, fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
								c = fonte->Linhas[LinhaAtual].QntCaracteres + 1; // pula o processamento.
							}
							else {
								ObjetosEncontrados_FazRefMemoria[Metapa] = false;
								validador = Parser_Instrucao(d);
								if (validador) {
									if (Metapa == 0) {
										ObjetosEncontrados[0] = d;
										ObjetosEncontradosTipos[0] = INSTRUCAO;
									}
									else {//alguma coisa seguida de instrução = invalido
										if (Parser_Instrucao(ObjetosEncontrados[0])) {//objeto anteriormente era uma instrução e a atual também é
											//instrução seguida de instrução
											sprintf(BUFFmsg, "Parser %d linha %d: Erro, instrucao %s seguido da instrucao %s no arquivo %s", NivelChamada, LinhaAtual + 1, d, ObjetosEncontrados[0], fonte->Fonte);
											AdicionaLog(BUFFmsg);
//											AtualizaTelaMontador();
											ParserMontadorErros++;
										}
									}
								}
								else {
//=====================================TESTE REGISTRADOR
									validador = Parser_Registrador(d);
									if (validador != 0) {

										if (Metapa == 0) {
											sprintf(BUFFmsg, "Parser %d linha %d: Erro, Registrador %s encontrado sem instrucao ao canto esquerdo no arquivo %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
											AdicionaLog(BUFFmsg);
											ParserMontadorErros++;
										}
										else if (validador == -1) {//é registrador, mas não faz referência a memória
											sprintf(BUFFmsg, "Parser %d linha %d: Erro, Registrador %s nao faz referencia a memoria %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
											AdicionaLog(BUFFmsg);
											ParserMontadorErros++;
										}else {
											if (validador == 2) {
												ObjetosEncontrados_FazRefMemoria[Metapa] = true;
												for (int i = 1; i < strlen(d)-1; i++) {
													d[i - 1] = d[i];
												}
												d[strlen(d) - 2] = '\0';
											}
											else {
												ObjetosEncontrados_FazRefMemoria[Metapa] = false;
											}
											ObjetosEncontradosTipos[Metapa] = REGISTRADOR;
											ObjetosEncontrados[Metapa] = d;
										}
									}
									else {
//=====================================TESTE LABEL
										validador = Parser_Label(d);
										if (validador == -3 || validador > 0) {
											if (validador == 1) {
												if (Metapa == 0) {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, label %s nao pode ser escria antes de uma instrucao : %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
													AdicionaLog(BUFFmsg);
//													AtualizaTelaMontador();
													ParserMontadorErros++;
												}
												else {
													ObjetosEncontrados[Metapa] = d;
													ObjetosEncontradosTipos[Metapa] = LABEL;
												}
											}
											else if (validador == -3) {//declaração de label
												if (Metapa == 0) {
													ObjetosEncontrados[0] = d;
													ObjetosEncontradosTipos[0] = LABEL;
													AdicionaOpcode(TLabels, d, PonteiroOpcode);
												}
												else {//declaração no local errado
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Declaracao da label %s nao pode ser feito depois de uma instrucao : %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
													AdicionaLog(BUFFmsg);
//													AtualizaTelaMontador();
													ParserMontadorErros++;
												}
												fonte->Linhas[LinhaAtual].Caracteres[fonte->Linhas[LinhaAtual].QntCaracteres - 1] = ':';//volta o simbolo de declaração que foi removido pelo parser
											}
											else if (validador == 2) {
												sprintf(BUFFmsg, "Parser %d linha %d: Erro, Declaracao da label %s nao realizada no pre-montador : %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
												AdicionaLog(BUFFmsg);
//												AtualizaTelaMontador();
												ParserMontadorErros++;
											}
										}
										else {
//=====================================TESTE NUMERO
											validador = Parser_Numero(d);
											if (validador) {
												if (Metapa == 0) {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Declaracao do numero %s realizado antes de uma instrucao: %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
													AdicionaLog(BUFFmsg);
//													AtualizaTelaMontador();
													ParserMontadorErros++;
												}
												else {
													if (validador == 2) {
														ObjetosEncontrados_FazRefMemoria[Metapa] = true;
													}
													ObjetosEncontrados[Metapa] = d;
													ObjetosEncontradosTipos[Metapa] = NUMERO;
												}
											}
											else {
//=====================================TESTE VARIAVEL
												validador = Parser_ChecaVariavel(d);
												if (validador) {
													ObjetosEncontrados[Metapa] = d;
													ObjetosEncontradosTipos[Metapa] = VARIAVEL;
												}
												else {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, elemento %s e desconhecido: %s", NivelChamada, LinhaAtual + 1, d, fonte->Fonte);
													AdicionaLog(BUFFmsg);

												}
											}
										}
									}
								}
							}
						}
						Metapa++;
					}
					//chegado aqui você terá uma linha filtrada por tipos...
#pragma endregion - aqui será feito o filtro de elemento por elemento da linham verficando coisas basicas e elemento por elemento...
#pragma region MOTNADOR_PARTE2
					//limpa objetos
					RegistradoresEncontrados[0] = 0;
//					RegistradoresEncontrados_FazRefMemoria[0] = false; // é limpado no loop anterior
					RegistradoresEncontrados[1] = 0;
//					RegistradoresEncontrados_FazRefMemoria[1] = false;
					VariaveisEncontrados[0] = 0;
					VariaveisEncontrados[1] = 0;
					if (Metapa < 5) {
						if (ObjetosEncontradosTipos[1] == REGISTRADOR) {
							for (c = 0, validador = false; c < TRegistrador->Quantidade && validador == false; c++) {
								if (_stricmp(TRegistrador->Registradores[c].Nome, ObjetosEncontrados[1]) == 0) {
									RegistradoresEncontrados[0] = &TRegistrador->Registradores[c];
									validador = true;
								}
							}
						}
						else if(ObjetosEncontradosTipos[1] == VARIAVEL){
							//VariaveisEncontrados
							for (c = 0, validador = false; c < TVariavel->Quantidade && validador == false; c++) {
								if (_stricmp(TVariavel->Variaveis[c].Nome, ObjetosEncontrados[1]) == 0) {
									VariaveisEncontrados[0] = &TVariavel->Variaveis[c];
									validador = true;
								}
							}
						}
						if (ObjetosEncontradosTipos[2] == REGISTRADOR) {
							for (c = 0, validador = false; c < TRegistrador->Quantidade && validador == false; c++) {
								if (_stricmp(TRegistrador->Registradores[c].Nome, ObjetosEncontrados[2]) == 0) {
									RegistradoresEncontrados[1] = &TRegistrador->Registradores[c];
									validador = true;
								}
							}
						}
						else if (ObjetosEncontradosTipos[2] == VARIAVEL) {
							for (c = 0, validador = false; c < TVariavel->Quantidade && validador == false; c++) {
								if (_stricmp(TVariavel->Variaveis[c].Nome, ObjetosEncontrados[2]) == 0) {
									VariaveisEncontrados[1] = &TVariavel->Variaveis[c];
									validador = true;
								}
							}
						}
						if (ObjetosEncontradosTipos[0] == INSTRUCAO) {
							if ((ObjetosEncontradosTipos[1] == MEMORIA || ObjetosEncontrados_FazRefMemoria[1] == true) && (ObjetosEncontrados_FazRefMemoria[2] == true || ObjetosEncontradosTipos[2] == MEMORIA)) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com dois end de memoria : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else if (ObjetosEncontradosTipos[1] == LABEL  && ObjetosEncontradosTipos[2] == LABEL) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com duas labels : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == false  && ObjetosEncontradosTipos[2] == NUMERO) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com dois numeros : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == false && ObjetosEncontradosTipos[2] == REGISTRADOR) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com um numero em seguida de um registrador : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == false && ObjetosEncontradosTipos[2] == LABEL) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com um numero em seguida de uma label : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontradosTipos[2] == MEMORIA) {
								sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao pode operar diretamente com um numero em seguida da MEMORIA: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
								AdicionaLog(BUFFmsg);
//								AtualizaTelaMontador();
								ParserMontadorErros++;
							}
							else {//combinação provavelmente válida
								validador = 0;
								for (c = 0; c < TInstrucao->QntInstrucao && validador == false; c++) {
									if (_stricmp(ObjetosEncontrados[0], TInstrucao->Instrucoes[c].Nome) == 0) {//uma das instruções encontradas
										TipoInstrucao = TInstrucao->Instrucoes[c].Operacao;

										if (TipoInstrucao == VAZIO) {
											if (ObjetosEncontradosTipos[1] != -1 || ObjetosEncontradosTipos[2] != -1) {
												sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s nao aceita nenhum operando : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], fonte->Fonte);
												AdicionaLog(BUFFmsg);
//												AtualizaTelaMontador();
												ParserMontadorErros++;
											}
											else {
												validador = true;
												PonteiroOpcode += 8;//instrução  8 bits

											}
										}

										else if (TipoInstrucao == MEMORIA_REGISTRADOR16) {
											if (RegistradoresEncontrados[1]) {
												if (RegistradoresEncontrados[1]->Tamanho == _16BITS) {
													if (VariaveisEncontrados[0]) {
														if (VariaveisEncontrados[0]->Tamanho == _16BITS) {// variavel registrador
															validador = true;
															AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO,NAO);
															AdicionaTabela(TOBJ, &*VariaveisEncontrados[0], VARIAVEL, SIM);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR,NAO);
															AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
															PonteiroOpcode += 32;//instrução  8 bits, variavel 16 bits, registrador 4 bits, lixo 4 bits
														}
														else {
															sprintf(BUFFmsg, "Parser %d linha %d: Erro, Variavel %s e Registrador %s tem tamanhos diferentes: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], ObjetosEncontrados[2], fonte->Fonte);
															AdicionaLog(BUFFmsg);
														}
													}
													else if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == true) {// [BX] AX
														if (RegistradoresEncontrados[0]->Tamanho == _16BITS) {
															validador = true;
															AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, SIM);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
															AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
															PonteiroOpcode += 32;//instrução  8 bits, end registrador 16 bits, registrador 4 bits, lixo 4 bits
														}
													}
													else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == true) {// [600h] AX
														validador = true;
														AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
														Parser_Numero(ObjetosEncontrados[1]);
														AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _16BITS);
														AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
														AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
														PonteiroOpcode += 32;//instrução  8 bits, endereço número 16 bits, registrador 4 bits, lixo 4 bits
													}
												}
											}

										}
										else if (TipoInstrucao == MEMORIA_REGISTRADOR8) {
											if (RegistradoresEncontrados[1]) {
												if (RegistradoresEncontrados[1]->Tamanho == _8BITS) {
													if (VariaveisEncontrados[0]) {
														if (VariaveisEncontrados[0]->Tamanho == _8BITS) {// variavel registrador
															validador = true;
															AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
															AdicionaTabela(TOBJ, &*VariaveisEncontrados[0], VARIAVEL, SIM);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
															AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
															PonteiroOpcode += 32;//instrução  8 bits, variavel 16 bits, registrador 4 bits, lixo 4 bits.
														}
														else {
															sprintf(BUFFmsg, "Parser %d linha %d: Erro, Variavel %s e Registrador %s tem tamanhos diferentes: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], ObjetosEncontrados[2], fonte->Fonte);
															AdicionaLog(BUFFmsg);
														}
													}
													else if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == true) {// [BL] AL
														if (RegistradoresEncontrados[0]->Tamanho == _8BITS) {
															validador = true;
															AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, SIM);
															AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
															AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
															PonteiroOpcode += 32;//instrução  8 bits, end registrador 16 bits, registrador 4 bits, lixo 4 bits
														}
													}
													else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == true) {// [600h] AX
														validador = true;
														AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
														Parser_Numero(ObjetosEncontrados[1]);
														AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _8BITS), NUMERO, _8BITS);
														AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
														AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
														PonteiroOpcode += 32;//instrução  8 bits, endereço número 16 bits, registrador 4 bits, lixo 4 bits
													}
												}
											}

										}
										else if (TipoInstrucao == MEMORIA_VALOR) {
											if (ObjetosEncontradosTipos[2] == NUMERO || ObjetosEncontradosTipos[2] == LABEL) {
												if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == true && RegistradoresEncontrados[0]->Tamanho == _16BITS) { // [BX] (10h ou label)
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, SIM);
													if (ObjetosEncontradosTipos[2] == NUMERO) {
														Parser_Numero(ObjetosEncontrados[2]);
														AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _16BITS);
													}
													else {
														AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[2]), LABEL, NAO);
													}
													validador = true;
													PonteiroOpcode += 40;//instrução  8 bits, end registrador 16 bits, numero ou label 16 bits
												}
												else if (VariaveisEncontrados[0]) {// variavel 10h ou label
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*VariaveisEncontrados[0], VARIAVEL, SIM);
													if (ObjetosEncontradosTipos[2] == NUMERO) {
														Parser_Numero(ObjetosEncontrados[2]);
														AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _16BITS);
													}
													else {
														AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[2]), LABEL, NAO);
													}
													validador = true;
													PonteiroOpcode += 40;//instrução  8 bits, variavel 16 bits, valor 16 bits
												}
											}
										}
										else if (TipoInstrucao == MEMORIA_VAZIO) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == true && ObjetosEncontradosTipos[2] == -1) {//[BX]
												if (RegistradoresEncontrados[0]->Tamanho == _16BITS) {
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, SIM);
													PonteiroOpcode += 24;//instrução  8 bits, end registrador 16 bits
												}else {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Registrador %s precisa ser de 16 bits: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], fonte->Fonte);
													AdicionaLog(BUFFmsg);
												}
											}
											else if (VariaveisEncontrados[0] && ObjetosEncontradosTipos[2] == -1) {// variavel
												if (VariaveisEncontrados[0]->Tamanho == _16BITS) {
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*VariaveisEncontrados[0], VARIAVEL, SIM);
													PonteiroOpcode += 24;//instrução  8 bits, variavel 16 bits
												}else {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Variavel %s precisa ser de 16 bits: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], fonte->Fonte);
													AdicionaLog(BUFFmsg);
												}
											}else if (ObjetosEncontradosTipos[1] == LABEL) {// JE LABEL
												validador = true;
												AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
												AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[1]), LABEL, NAO);
												PonteiroOpcode += 24;//instrução  8 bits, label 16 bits
											}else if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontrados_FazRefMemoria[1] == SIM) {// JE LABEL
												validador = true;
												AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
												Parser_Numero(ObjetosEncontrados[2]);
												AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _16BITS);
												PonteiroOpcode += 24;//instrução  8 bits, label 16 bits
											}
										}
										

										else if (TipoInstrucao == REGISTRADOR16_MEMORIA){
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && VariaveisEncontrados[1]) {
												if (VariaveisEncontrados[1]->Tamanho == _16BITS && RegistradoresEncontrados[0]->Tamanho == _16BITS) { // AX variavel
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, &*VariaveisEncontrados[1], VARIAVEL, SIM);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);

													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, variavel 16 bits, lixo 4 bits
												}else {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Variavel %s e Registrador %s tem tamanhos diferentes: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], ObjetosEncontrados[2], fonte->Fonte);
													AdicionaLog(BUFFmsg);
												}

											}else if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && RegistradoresEncontrados[1] && ObjetosEncontrados_FazRefMemoria[2] == true) {// AX [BX]
												if (RegistradoresEncontrados[0]->Tamanho == _16BITS && RegistradoresEncontrados[1]->Tamanho == _16BITS) {
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, SIM);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);

													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, endereço registrador 16 bits, lixo 4 bits
												}
											}
										}

										else if (TipoInstrucao == REGISTRADOR16_REGISTRADOR16) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && RegistradoresEncontrados[1] && ObjetosEncontrados_FazRefMemoria[2] == false) {
												if (RegistradoresEncontrados[0]->Tamanho == _16BITS && RegistradoresEncontrados[1]->Tamanho == _16BITS) {
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO,NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR,NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR,NAO);
													PonteiroOpcode += 16;//instrução  8 bits, registrador 4 bits, registrador 4 bits
												}
											}
										}

										else if (TipoInstrucao == REGISTRADOR16_VALOR) {
											if (RegistradoresEncontrados[0]) {
												if (RegistradoresEncontrados[0]->Tamanho == _16BITS && ObjetosEncontrados_FazRefMemoria[1] == false) {
													if (ObjetosEncontradosTipos[2] == NUMERO) {// AX 600h
														AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
														AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
														Parser_Numero(ObjetosEncontrados[2]);
														AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _16BITS);
														AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
														validador = true;
														PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, numero 16 bits, lixo 4 bits
													}
													else if(ObjetosEncontradosTipos[2] == LABEL){//AX label
														AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
														AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
														AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[2]), LABEL, NAO);
														AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
														validador = true;
														PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, label 16 bits, lixo 4 bits
													}
												}
											}
										}
										else if (TipoInstrucao == REGISTRADOR16_VAZIO) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false) {
												if (RegistradoresEncontrados[0]->Tamanho == _16BITS && ObjetosEncontradosTipos[2] == -1) {
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 16;//instrução  8 bits, registrador 4 bits, lixo 4 bits
												}
											}
										}
										else if (TipoInstrucao == REGISTRADOR8_VALOR) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && ObjetosEncontradosTipos[2] == NUMERO) {
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS /*Sem testes para números*/) { // AL número
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													Parser_Numero(ObjetosEncontrados[2]);
													AdicionaTabela(TOBJ,InteiroParaCharBinario(Parser_Numero_Validado, _8BITS),NUMERO,_8BITS);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);

													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, numero 16 bits, lixo 4 bits
												}
											}
											else if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false &&  ObjetosEncontradosTipos[2] == LABEL) {//AL label
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS /*Sem testes para números*/) {// AL Label
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[1]), LABEL, NAO);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, label 16 bits, lixo 4 bits
												}
											}

										}
										else if (TipoInstrucao == REGISTRADOR8_MEMORIA) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && VariaveisEncontrados[1]) {
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS && VariaveisEncontrados[1]->Tamanho == _8BITS) { // AL variavel
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, &*VariaveisEncontrados[1], VARIAVEL, SIM);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, variavel 16 bits (endereço), lixo 4 bits 
												}
												else {
													sprintf(BUFFmsg, "Parser %d linha %d: Erro, Variavel %s e Registrador %s tem tamanhos diferentes: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[1], ObjetosEncontrados[2], fonte->Fonte);
													AdicionaLog(BUFFmsg);
												}

											}else if (ObjetosEncontrados_FazRefMemoria[1] == false && RegistradoresEncontrados[0] && RegistradoresEncontrados[1] && ObjetosEncontrados_FazRefMemoria[2] == true) {// al [bl]
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS && RegistradoresEncontrados[1]->Tamanho == _8BITS) { // al [bl]
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, SIM);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, end registrador 16 bits (end memoria), 4 bits lixo
												}
											}
											else if (ObjetosEncontrados_FazRefMemoria[0] == false && RegistradoresEncontrados[0] && ObjetosEncontradosTipos[2] == NUMERO && ObjetosEncontrados_FazRefMemoria[2] == true) {// al [600h]
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS) { // al [bl]
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													Parser_Numero(ObjetosEncontrados[2]);
													AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _16BITS), NUMERO, _8BITS);// pois é um endereço
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 32;//instrução  8 bits, registrador 4 bits, end registrador 16 bits, lixo 4 bits
												}
											}
										}
										else if (TipoInstrucao == REGISTRADOR8_REGISTRADOR8) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false && RegistradoresEncontrados[1] && ObjetosEncontrados_FazRefMemoria[2] == false) {
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS && RegistradoresEncontrados[1]->Tamanho == _8BITS) {// al bl
													/*CASO CORRETO ENCONTRADO*/
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[1], REGISTRADOR, NAO);
													PonteiroOpcode += 16;//instrução  8 bits, registrador 4 bits, registrador 4 bits
												}
											}
										}

										else if (TipoInstrucao == REGISTRADOR8_VAZIO) {
											if (RegistradoresEncontrados[0] && ObjetosEncontrados_FazRefMemoria[1] == false) {
												if (RegistradoresEncontrados[0]->Tamanho == _8BITS && ObjetosEncontradosTipos[2] == -1) {//registrador 8 bits
													/*CASO CORRETO ENCONTRADO*/
													validador = true;
													AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
													AdicionaTabela(TOBJ, &*RegistradoresEncontrados[0], REGISTRADOR, NAO);
													AdicionaTabela(TOBJ, GeraLixo(4), LIXO, NAO);
													PonteiroOpcode += 16;// instrução  8 bits, registrador 4 bits, lixo 4 bits
												}
											}
										}
										else if (TipoInstrucao == VALOR_VAZIO) {
											if (ObjetosEncontradosTipos[1] == NUMERO && ObjetosEncontradosTipos[2] == -1) {// número
												/*CASO CORRETO ENCONTRADO*/
												validador = true;
												AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
												Parser_Numero(ObjetosEncontrados[1]);
												AdicionaTabela(TOBJ, InteiroParaCharBinario(Parser_Numero_Validado, _8BITS), NUMERO, _16BITS);
												PonteiroOpcode += 24;//instrução  8 bits, valor 16 bits
											}
											else if (ObjetosEncontradosTipos[1] == LABEL && ObjetosEncontradosTipos[2] == -1) {//label
												validador = true;
												AdicionaTabela(TOBJ, &TInstrucao->Instrucoes[c], INSTRUCAO, NAO);
												AdicionaTabela(TOBJ, ProcuraLabel(TLabels, ObjetosEncontrados[1]), LABEL, NAO);
												PonteiroOpcode += 24;//instrução  8 bits, label 16 bits
											}

										}

									}
								}
								if (validador == false) { // mensagem de erro simples para cada combinação de operando
									if (ObjetosEncontradosTipos[1] == REGISTRADOR && ObjetosEncontradosTipos[2] == REGISTRADOR) {
										sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s  nao aceita o uso de %s com  %s: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], ObjetosEncontrados[1], ObjetosEncontrados[2], fonte->Fonte);
										AdicionaLog(BUFFmsg);
//										AtualizaTelaMontador();
										ParserMontadorErros++;
									}
									else if (ObjetosEncontradosTipos[1] == REGISTRADOR && ObjetosEncontradosTipos[2] == MEMORIA) {
										sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s  nao aceita o uso de %s com a memoria: %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], ObjetosEncontrados[1], fonte->Fonte);
										AdicionaLog(BUFFmsg);
//										AtualizaTelaMontador();
										ParserMontadorErros++;
									}
									else if (ObjetosEncontradosTipos[1] == MEMORIA && ObjetosEncontradosTipos[2] == REGISTRADOR) {
										sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s  nao aceita a operacao de MEMORIA com o registrador %s : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], ObjetosEncontrados[1], fonte->Fonte);
										AdicionaLog(BUFFmsg);
//										AtualizaTelaMontador();
										ParserMontadorErros++;
									}
									else if (ObjetosEncontradosTipos[1] == MEMORIA && ObjetosEncontradosTipos[2] == REGISTRADOR) {
										sprintf(BUFFmsg, "Parser %d linha %d: Erro, Instrucao %s  nao aceita a operacao de MEMORIA com o registrador %s : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontrados[0], ObjetosEncontrados[1], fonte->Fonte);
										AdicionaLog(BUFFmsg);
//										AtualizaTelaMontador();
										ParserMontadorErros++;
									}
								}
								else {//registra o opcode

								}

								if (validador) {
									sprintf(BUFFmsg, "Parser %d linha %d: Operacao %s %s %s foi validada : %s", NivelChamada, LinhaAtual + 1, ObjetosEncontradosTipos[0] != -1 ? ObjetosEncontrados[0] : "", ObjetosEncontradosTipos[1] != -1 ? ObjetosEncontrados[1] : "", ObjetosEncontradosTipos[2] != -1 ? ObjetosEncontrados[2] : "", fonte->Fonte);
									AdicionaLog(BUFFmsg);
//									AtualizaTelaMontador();
									ParserMontadorErros++;
									AdicionaLinha(ArquivoFonteFinal, fonte->Linhas[LinhaAtual].Caracteres);
								}
							}
						}
						if (ObjetosEncontradosTipos[0] != -1 && ObjetosEncontrados[0]) {
							free(ObjetosEncontrados[0]);
						}
						if (ObjetosEncontradosTipos[1] != -1 && ObjetosEncontrados[1]) {
							free(ObjetosEncontrados[1]);
						}
						if (ObjetosEncontradosTipos[2] != -1 && ObjetosEncontrados[2]) {
							free(ObjetosEncontrados[2]);
						}
					}
#pragma endregion - aqui será feito o filtro de todos os elementos processados como um todo, vendo se a união entre eles forma um padrão permitido pela instrução : aqui é gerado código de maquina.

				}
				LinhaAtual++;
			}
			sprintf(BUFFmsg, "Parser %d Montador Finalizado.", NivelChamada);
			AdicionaLog(BUFFmsg);
			AtualizaTelaMontador();
#pragma endregion
		}
		//FIM PARSER

	}
	else {
		AdicionaLog("Erro Parser, arquivo fonte nao alocao na memoria");
		AtualizaTelaMontador();
	}
	return existe;
#pragma endregion
}

void Montador(Texto *fonte) {
	int Menu_Opcao = 0;
	int scroll_pos = 0;//usado para as tabelas
	int i = 0;
	int j = 0;
	float l = 0;
// <Ponteiros do opcode a ser gerado>
	Registrador *Rtmp = 0;
	Label *Ltmp = 0;
	Variavel *Vtmp = 0;
	Instrucao *Itmp = 0;
	char *Ntmp = 0;
// </Ponteiros do opcode a ser gerado>

//<vars salvar opcode>
	bool CodigoSalvo = false;
	char *EndSalvo = 0;
	FILE *Arquivo = 0;
//</vars salvar opcode>

	Parser_Montador_Ordem = 1;
	ParserMontadorErros = 0;
	if (Opcodes) {
		free(Opcodes);
	}
	Opcodes = 0;
	Opcodes_qnt = 0;
#pragma region INICIALIZAÇÃO
#pragma region INSTRUCAO
	AdicionaLog("Carregando Lista de Instrucoes");
	AtualizaTelaMontador();
	TInstrucao = CriaTabelaInstrucao();

	if (TInstrucao->TabelaValida == SIM) {
		AdicionaLog("Carregamento finalizado - SEM ERROS");
	}
	else {
		AdicionaLog("Carregamento finalizado - HOUVERAM ERROS");
	}
#pragma endregion CARREGAMENTO
#pragma region REGISTRADOR
	AdicionaLog("Carregando Lista de Registradores");
	AtualizaTelaMontador();

	TRegistrador = CriaTabelaRegistrador();
	if (TRegistrador->TabelaValida == SIM) {
		AdicionaLog("Carregamento finalizado - SEM ERROS");
	}
	else {
		AdicionaLog("Carregamento finalizado - HOUVERAM ERROS");
	}

#pragma endregion CARREGAMENTO
#pragma region LABELS
	AdicionaLog("Criando tabela de labels");
	AtualizaTelaMontador();
	TLabels = CriaTabelaLabel();
	if (TLabels) {
		AdicionaLog("Tabela de Labels foi criada");
	}
	else {
		AdicionaLog("Tabela de Labels nao foi criada");
	}
#pragma endregion INCIALIZAÇÃO
#pragma region VARIAVEL
	AdicionaLog("Criando tabela de Variaveis");
	AtualizaTelaMontador();
	TVariavel = CriaTabelaVariavel();
	if (TVariavel) {
		AdicionaLog("Tabela de variavel foi criada");
	}
	else {
		AdicionaLog("Tabela de variavel nao foi criada");
	}

#pragma endregion INICIALIZAÇÃO
#pragma region TABELA OBJETOS
	AdicionaLog("Criando tabela de Objetos");
	TOBJ = CriaTabelaPonteiroOBJ();
	if (TOBJ) {
		AdicionaLog("Tabela de Objetos criada");
	}else{
		AdicionaLog("Falha ao criar a tabela de Objetos");
	}
#pragma endrecion INICIALIZAÇÃO
#pragma endregion
	Texto *novo = NULL;

	if (fonte) {
		novo = CarregaTexto(fonte->Fonte);
	}
	if(novo && TInstrucao && TRegistrador && TLabels && TVariavel && TOBJ){

		ArquivoFonteFinal = (Texto*)malloc(sizeof(Texto));
		ArquivoFonteFinal->Fonte = 0;
		ArquivoFonteFinal->Linhas = 0;
		ArquivoFonteFinal->QntLinhas = 0;
		ArquivoFonteFinal->Salvo = SIM;

		PonteiroOpcode = 16;

		AdicionaLog("Iniciando Parser");
		AtualizaTelaMontador();
		Parser_Montador(novo, 1, true);
		AdicionaLog("Parser Finalizado");
		AdicionaLog("Criando tabela de opcodes para a tabela de variaveis");

		AdicionaOpcode(TVariavel, &PonteiroOpcode);
		for (i = 0; i < TVariavel->Quantidade; i++) {
			//salva o valor da variavel no final do arquivo
			AdicionaTabela(TOBJ, InteiroParaCharBinario(TVariavel->Variaveis[i].Dado, TVariavel->Variaveis[i].Tamanho), NUMERO, TVariavel->Variaveis[i].Tamanho);
		}
		AdicionaLog("tabela de opcodes das variaveis criadas");

#pragma region MENU_COMPILADOR
		do {
			scroll_pos = 0;
			AtualizaTelaMontador();
			transfer_window1_to_window2();
			Menu_Opcao = generic_menu("Menu do montador", 10, 10, 5, 0, SIM, ' ', '>',
				"Sair",
				"Ver tabela de Labels",
				"Ver tabela de Registradores",
				"Ver tabela de Instrucoes",
				"Ver tabela de Variaveis",
				"Ver Opcode",
				"Salvar Opcode",
				"Ver Arquivo Fonte Final", NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 8);
			switch (Menu_Opcao) {
			case 1:
				if (!CodigoSalvo) {
					if (print_popup_wiht_result("Tentativa de sair", "Voce deseja sair sem gerar um opcode?", "SIM", "NAO") == 2) {
						Menu_Opcao = 0;
					}
				}
				break;
			case 2: // LABELS
				do {
					transfer_window1_to_window2();
					create_window_layout(2, 2, 60, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Tabela de Labels", 2, (50 + 2) / 2, DISPLAY_WINDOW2);
					insert_string_on_display("Label", 3, 3, DISPLAY_WINDOW2);
					insert_string_on_display("Opcode", 3, 40, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos, 22,12, DISPLAY_WINDOW2);
					insert_number_on_display(TLabels->quantidade, 22,22, DISPLAY_WINDOW2);

					for (i = scroll_pos; i < TLabels->quantidade && i - scroll_pos + 5 < 22; i++) {
						insert_string_on_display(TLabels->label[i].Nome, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
						for (j = 40; j < 56;j++) {
							insert_number_on_display(TLabels->label[i].Opcode[j-40] == true ? 1 : 0, i - scroll_pos + 5, j, DISPLAY_WINDOW2); // porque dessa "gambiarra" ? pois por algum motivo o visual studio 2015 agora trata booleano true com valor de 205 e false com outro valor...
						}
					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < TLabels->quantidade) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;
			case 3: //REGISTRADORES
				do {
					transfer_window1_to_window2();
					create_window_layout(2, 2, 60, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Tabela de Registradores", 2, (40 + 2) / 2, DISPLAY_WINDOW2);
					insert_string_on_display("Registrador", 3, 3, DISPLAY_WINDOW2);
					insert_string_on_display("Opcode", 3, 17, DISPLAY_WINDOW2);
					insert_string_on_display("Tamanho", 3, 37, DISPLAY_WINDOW2);
					insert_string_on_display("Ref Memoria", 3, 47, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos, 22,10, DISPLAY_WINDOW2);
					insert_number_on_display(TRegistrador->Quantidade, 22,30, DISPLAY_WINDOW2);

					for (i = scroll_pos; i < TRegistrador->Quantidade  &&  i - scroll_pos + 5 < 22; i++) {
						insert_string_on_display(TRegistrador->Registradores[i].Nome, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
						for (j = 17; j < 17 + 4; j++) {
							insert_number_on_display(TRegistrador->Registradores[i].Opcode[j - 17] == true ? 1 : 0, i - scroll_pos + 5, j, DISPLAY_WINDOW2); // porque dessa "gambiarra" ? pois por algum motivo o visual studio 2015 agora trata booleano true com valor de 205 e false com outro valor...
						}
						if (TRegistrador->Registradores[i].Tamanho == _8BITS) {
							insert_string_on_display("8 BITS", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
						}
						else {
							insert_string_on_display("16 BITS", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);

						}
						if (TRegistrador->Registradores[i].FazReferenciaMemoria == SIM) {
							insert_string_on_display("SIM", i - scroll_pos + 5, 47, DISPLAY_WINDOW2);
						}
						else {
							insert_string_on_display("NAO", i - scroll_pos + 5, 47, DISPLAY_WINDOW2);

						}

					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < TRegistrador->Quantidade) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;
			case 4: // INSTRUÇÕES
				do {
					transfer_window1_to_window2();
					create_window_layout(2, 2, 75, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Tabela de Instrucoes", 2, (40 + 2) / 2, DISPLAY_WINDOW2);
					insert_string_on_display("Instrucao", 3, 3, DISPLAY_WINDOW2);
					insert_string_on_display("Opcode", 3, 17, DISPLAY_WINDOW2);
					insert_string_on_display("Operacao", 3, 37, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos,22, 10, DISPLAY_WINDOW2);
					insert_number_on_display(TInstrucao->QntInstrucao, 22,30, DISPLAY_WINDOW2);
					for (i = scroll_pos; i < TInstrucao->QntInstrucao && i - scroll_pos + 5 < 22; i++) {
						insert_string_on_display(TInstrucao->Instrucoes[i].Nome, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
						for (j = 17; j < 17 + 8; j++) {
							insert_number_on_display(TInstrucao->Instrucoes[i].Opcode[j - 17] == true ? 1 : 0, i - scroll_pos + 5, j, DISPLAY_WINDOW2); // porque dessa "gambiarra" ? pois por algum motivo o visual studio 2015 agora trata booleano true com valor de 205 e false com outro valor...
						}
						switch (TInstrucao->Instrucoes[i].Operacao) {
						case VAZIO:
							insert_string_on_display("Vazio", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
						break;
						case MEMORIA_MEMORIA:
							insert_string_on_display("Memoria Memoria", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
						break;
						case MEMORIA_REGISTRADOR16:						
							insert_string_on_display("Memoria Registrador 2Bytes", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case MEMORIA_REGISTRADOR8:
							insert_string_on_display("Memoria Registrador 1Byte", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR16_MEMORIA:
							insert_string_on_display("Registrador 2Bytes Memoria", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR8_MEMORIA:
							insert_string_on_display("Registrador 1Byte Memoria", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR16_REGISTRADOR16:
							insert_string_on_display("Registrador 2Bytes Registrador 2Bytes", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR8_REGISTRADOR8:
							insert_string_on_display("Registrador 1Byte Registrador 1Byte", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR16_VAZIO:
							insert_string_on_display("Registrador 2Bytes", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR8_VAZIO:
							insert_string_on_display("Registrador 1Byte", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case MEMORIA_VAZIO:// memória/label
							insert_string_on_display("Memoria", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR16_VALOR:
							insert_string_on_display("Registrador 2Bytes Numero", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case REGISTRADOR8_VALOR:
							insert_string_on_display("Registrador 1Byte Numero", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case VALOR_VAZIO:
							insert_string_on_display("Numero", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case VALOR_REGISTRADOR16: //modo invalida
							insert_string_on_display("Numero Registrador 2Bytes", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case VALOR_REGISTRADOR8: // modo invalido
							insert_string_on_display("Numero Registrador 1Byte", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case VALOR_MEMORIA:      // modo invalido
							insert_string_on_display("Numero Memoria", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						case VALOR_VALOR:        // modo invalido
							insert_string_on_display("Numero Numero", i - scroll_pos + 5, 37, DISPLAY_WINDOW2);
							break;
						}
					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < TInstrucao->QntInstrucao) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;
			case 5: // VARIAVEIS
				do {
					transfer_window1_to_window2();
					create_window_layout(2, 2, 70, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Tabela de Variaveis", 2, (40 + 2) / 2, DISPLAY_WINDOW2);
					insert_string_on_display("Variavel", 3, 3, DISPLAY_WINDOW2);
					insert_string_on_display("Opcode", 3, 41, DISPLAY_WINDOW2);
					insert_string_on_display("Tamanho", 3, 51, DISPLAY_WINDOW2);
					insert_string_on_display("Valor Inicial", 3, 59, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos, 22,10, DISPLAY_WINDOW2);
					insert_number_on_display(TVariavel->Quantidade, 22,30, DISPLAY_WINDOW2);
					for (i = scroll_pos; i < TVariavel->Quantidade && i - scroll_pos + 5 < 22; i++) {
						insert_string_on_display(TVariavel->Variaveis[i].Nome, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
						for (j = 36; j < 36 + 16; j++) {
							insert_number_on_display(TVariavel->Variaveis[i].Opcode[j - 36] == true ? 1 : 0, i - scroll_pos + 5, j, DISPLAY_WINDOW2); // porque dessa "gambiarra" ? pois por algum motivo o visual studio 2015 agora trata booleano true com valor de 205 e false com outro valor...
						}
						if (TVariavel->Variaveis[i].Tamanho == _8BITS) {
							insert_string_on_display("8 BITS", i - scroll_pos + 5, 53, DISPLAY_WINDOW2);
						}
						else {
							insert_string_on_display("16 BITS", i - scroll_pos + 5, 53, DISPLAY_WINDOW2);

						}
						insert_number_on_display(TVariavel->Variaveis[i].Dado, i - scroll_pos + 5, 62, DISPLAY_WINDOW2);

					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < TVariavel->Quantidade) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;
			case 6: // OPCODE
				do{
					transfer_window1_to_window2();
					create_window_layout(2, 2, 30, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Tabela de Opcode", 2, 4, DISPLAY_WINDOW2);
					insert_string_on_display("Opcode", 3, 3, DISPLAY_WINDOW2);
					insert_string_on_display("Tipo", 3, 21, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos, 22,10, DISPLAY_WINDOW2);
					insert_number_on_display(TOBJ->Quantidade, 22,28, DISPLAY_WINDOW2);
					for (i = scroll_pos; i  < TOBJ->Quantidade && i - scroll_pos + 5 < 22; i++) {
						switch (TOBJ->pilhaobj[i].Tipo) {
							case REGISTRADOR:
								insert_string_on_display("Registrador",i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
								Rtmp = (Registrador*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de char
								if (TOBJ->pilhaobj[i].RefMemoria == false) {
									for (j = 0; j < 4; j++) {
										insert_number_on_display(Rtmp->Opcode[j] == true? 1 : 0, i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
									}
								}
								else {
									for (j = 0; j < 16; j++) {
										if (j < 12) {
											insert_number_on_display(0, i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
										}
										else {
											insert_number_on_display(Rtmp->Opcode[j - 12] == true ? 1 : 0 , i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
										}
									}
								}
								break;
							case NUMERO:
							case LIXO:
								if (TOBJ->pilhaobj[i].Tipo == NUMERO) {
									insert_string_on_display("Numero", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
								}
								else {
									insert_string_on_display("Lixo Memori", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);

								}
								Ntmp = (char*)TOBJ->pilhaobj[i].Ponteiro;
								insert_string_on_display(Ntmp, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
								break;
							case LABEL:
								insert_string_on_display("Label", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
								Ltmp = (Label*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Label
								for (j = 0; j < 16; j++) {
									insert_number_on_display(Ltmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
								}
								break;
							case VARIAVEL:
								insert_string_on_display("Variavel", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
								Vtmp = (Variavel*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Variavel
								for (j = 0; j < 16 - (1 - Vtmp->Tamanho) * 8 ; j++) {
									insert_number_on_display(Vtmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
								}
								break;
							case INSTRUCAO:
								insert_string_on_display("Instrucao", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
								Itmp = (Instrucao*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Variavel
								for (j = 0; j < 8; j++) {
									insert_number_on_display(Itmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
								}
								break;
						}
					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < TOBJ->Quantidade) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;
			case 7: // SALVAR OPCODE
				do {
					EndSalvo = get_string_popup("Digite o endereco a ser salvo");
				} while (!EndSalvo);
				Arquivo = fopen(EndSalvo, "w");
				for (i = 0; i < TOBJ->Quantidade; i++) {
					//Desenha uma barra de progresso.
					for (j = 0, l = 60.0 / TOBJ->Quantidade; j < 60 && l*j < 60; j++) {
						WINDOW2[2][10 + j] = 177;
					}
					for (j = l*i; j < 60; j++) {
						WINDOW2[2][10+j] = ' ';
					}
					print_display(DISPLAY_WINDOW2);

					switch (TOBJ->pilhaobj[i].Tipo) {
					case REGISTRADOR:
//						insert_string_on_display("Registrador", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
						Rtmp = (Registrador*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de char
						if (TOBJ->pilhaobj[i].RefMemoria == false) {
							for (j = 0; j < 4; j++) {
								fputc(Rtmp->Opcode[j] == true ? '1' : '0',Arquivo);
//								insert_number_on_display(Rtmp->Opcode[j] == true ? 1 : 0, i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
							}
						}
						else {
							for (j = 0; j < 16; j++) {
								if (j < 12) {
									fputc('0', Arquivo);
//									insert_number_on_display(0, i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
								}
								else {
									fputc(Rtmp->Opcode[j - 12] == true ? '1' : '0', Arquivo);
//									insert_number_on_display(Rtmp->Opcode[j - 12] == true ? 1 : 0, i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
								}
							}
						}
						break;
					case NUMERO:
					case LIXO:
//						insert_string_on_display("Numero", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
						Ntmp = (char*)TOBJ->pilhaobj[i].Ponteiro;
						fputs(Ntmp, Arquivo);
//						insert_string_on_display(Ntmp, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
						break;
					case LABEL:
//						insert_string_on_display("Label", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
						Ltmp = (Label*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Label
						for (j = 0; j < 16; j++) {
							fputc(Ltmp->Opcode[j] + '0', Arquivo);
//							insert_number_on_display(Ltmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
						}
						break;
					case VARIAVEL:
//						insert_string_on_display("Variavel", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
						Vtmp = (Variavel*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Variavel
						for (j = 0; j < 16 - (1 - Vtmp->Tamanho) * 8; j++) {
							fputc(Vtmp->Opcode[j] + '0', Arquivo);
///							insert_number_on_display(Vtmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
						}
						break;
					case INSTRUCAO:
//						insert_string_on_display("Instrucao", i - scroll_pos + 5, 21, DISPLAY_WINDOW2);
						Itmp = (Instrucao*)TOBJ->pilhaobj[i].Ponteiro;//converte o ponteiro void para o ponteiro de Variavel
						for (j = 0; j < 8; j++) {
							fputc(Itmp->Opcode[j] + '0', Arquivo);
//							insert_number_on_display(Itmp->Opcode[j], i - scroll_pos + 5, 3 + j, DISPLAY_WINDOW2);
						}
						break;
					}
				}
				print_popup("Opcode Salvo");
				fclose(Arquivo);
				free(EndSalvo);
				CodigoSalvo = true;
				break;
			case 8: //código fonte final
				do {
					transfer_window1_to_window2();
					create_window_layout(2, 2, 50, 20, SIM, SIM, NAO, ' ', DISPLAY_WINDOW2);
					insert_string_on_display("Codigo fonte final", 2, 12, DISPLAY_WINDOW2);
					insert_number_on_display(scroll_pos, 22,10, DISPLAY_WINDOW2);
					insert_number_on_display(ArquivoFonteFinal->QntLinhas, 22,30, DISPLAY_WINDOW2);

					for (i = scroll_pos; i < ArquivoFonteFinal->QntLinhas && i - scroll_pos + 5 < 22; i++) {
						insert_string_on_display(ArquivoFonteFinal->Linhas[i].Caracteres, i - scroll_pos + 5, 3, DISPLAY_WINDOW2);
					}
					print_display(DISPLAY_WINDOW2);
					Menu_Opcao = Keyboard_keytouched();
					if (Menu_Opcao == _key) {
						Menu_Opcao = Keyboard_keytouched();
						if (Menu_Opcao == key_up) {
							if (scroll_pos > 0) {
								scroll_pos--;
							}
						}
						else if (Menu_Opcao == key_down) {
							if (scroll_pos + 5 < ArquivoFonteFinal->QntLinhas) {
								scroll_pos++;
							}
						}
					}
					else if (Menu_Opcao == enter) {
						Menu_Opcao = 0;
					}
				} while (Menu_Opcao != 0);
				break;

			}
		} while (Menu_Opcao != 1);
#pragma endregion
	}
	LimpaTabela(TLabels);
	LimpaTabela(TRegistrador);
	LimpaTabela(TInstrucao);
	LimpaTabela(TVariavel);
	LimpaTabela(TOBJ);
	AtualizaTelaMontador();
	Destroi_Texto(ArquivoFonteFinal);

}