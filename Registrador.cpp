#include "Registrador.h"
#include "Texto.h"
#include "Log.h"
#include "Mmain.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
Retorna a posição da tabela onde se encontra o registrador requisitado
Caso não encontrado retorna -1.
Caso a tabela seja invalida(nao existe), é retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Opcode,bool Por_Opcode) {
	int Saida = -2;
	if (Tab) {
		int i = 0;
		int j = 0;
		for (i = 0; i < Tab->Quantidade; i++) {

			j = 0;
			while (j < 4) {
				if (Opcode[j] != (char)Tab->Registradores[i].Opcode[j] + '0') {
					j = 16;
				}
				if (j == 3) {
					Saida = i;
				}
				j++;
			}
		}
		if (Saida == -2) {
			Saida = -1;
		}
	}
	return Saida;
}

/**
	Retorna a posição da tabela onde se encontra o registrador requisitado
	Caso não encontrado retorna -1.
	Caso a tabela seja invalida(nao existe), é retornado -2.
*/
int Registrador_PegaID(TabelaRegistrador *Tab, char *Nome) {
	int Saida = -2;
	if (Tab) {
		int i = 0;
		for (i= 0; i < Tab->Quantidade; i++) {
			if (_stricmp(Nome, Tab->Registradores[i].Nome) == 0) {
				Saida = i;
			}
		}
		if (Saida == -2) {
			Saida = -1;
		}
	}
	return Saida;
}

int Registrador_RecebeDado(TabelaRegistrador *Tab, Registrador *Reg) {
	if (Reg->Tamanho == _8BITS) {
		return Reg->Dado;
	}
//	senão é de 16 bits
	int Dado = 0;
	char *Dado1=0, *Dado2=0, Dado_Juntado[17];
	Dado1 = InteiroParaCharBinario(Tab->Registradores[Reg->High_offset].Dado, _8BITS);
	Dado2= InteiroParaCharBinario(Tab->Registradores[Reg->Low_offset].Dado, _8BITS);
	for (int i = 0; i < 8; i++) {
		Dado_Juntado[i] = Dado1[i];
	}
	for (int i = 0; i < 8; i++) {
		Dado_Juntado[i+8] = Dado2[i];
	}
	Dado_Juntado[16] = '\0';
	Dado = CharBinarioParaInteiro(Dado_Juntado);
	return Dado;
}

void Registrador_AdicionaDado(TabelaRegistrador *Tab, Registrador *Reg, int Dado) {
	if (Reg->Tamanho == _16BITS) {
		char *DadoCompleto;
		char Dado1[9], Dado2[9];
		DadoCompleto = InteiroParaCharBinario(Dado, 16);// converte o dado para binario
		for (int i = 0; i < 8; i++) {
			Dado1[i] = DadoCompleto[i];
		}
		Dado1[8] = '\0';
		for (int i = 0; i < 8; i++) {
			Dado2[i] = DadoCompleto[i+8];
		}
		Dado2[8] = '\0';
		//Transfere os dados entre os registradores de 8 bits
		Tab->Registradores[Reg->High_offset].Dado = CharBinarioParaInteiro(Dado1);
		Tab->Registradores[Reg->Low_offset].Dado = CharBinarioParaInteiro(Dado2);

	}
	else {
		Reg->Dado = Dado;
	}
}

void AdicionaTabela(TabelaRegistrador * tabela, char * Nome, bool Tamanho, bool FazReferenciaMemoria, char * High, char * Low,char *Opcode) {
	int i = 0;
	char Erro[100];
	if (tabela->Quantidade > 0) {
		tabela->Registradores = (Registrador*)realloc(tabela->Registradores, (tabela->Quantidade+1) * sizeof(Registrador));
	}
	else {
		tabela->Registradores = (Registrador*)malloc(sizeof(Registrador));
	}
	//nome de tamanho fixo
	i = strlen(Nome);
	strcpy(tabela->Registradores[tabela->Quantidade].Nome, Nome);
	tabela->Registradores[tabela->Quantidade].Nome[i + 1] = '\0';
	tabela->Registradores[tabela->Quantidade].Dado = 0;
	tabela->Registradores[tabela->Quantidade].FazReferenciaMemoria = FazReferenciaMemoria;
	tabela->Registradores[tabela->Quantidade].Tamanho = Tamanho;
	tabela->Registradores[tabela->Quantidade].High_offset = -1;
	tabela->Registradores[tabela->Quantidade].Low_offset = -1;
	for (i = 0; i < 4; i++) {
		tabela->Registradores[tabela->Quantidade].Opcode[i] = Opcode[i] == '0' ? false : true;
	}

	if (High) {
		//Procura registrador referenciado em high e copia seu ponteiro
		for (i = 0; i < tabela->Quantidade; i++) {
			if (strcmp(High, tabela->Registradores[i].Nome) == 0) {
				//achou o registrador
				tabela->Registradores[tabela->Quantidade].High_offset  = i;
				i = tabela->Quantidade + 10;
			}
		}if (i == tabela->Quantidade) {
			sprintf(Erro, "Erro registrador.txt:referencia de %s nao encontrada no registrador %s",High,Nome);
			AdicionaLog(Erro);
			AtualizaTelaMontador();
		}
	}

	if (Low) {
		//Procura registrador referenciado em high e copia seu ponteiro
		for (i = 0; i < tabela->Quantidade; i++) {
			if (strcmp(Low, tabela->Registradores[i].Nome) == 0) {
				//achou o registrador
				tabela->Registradores[tabela->Quantidade].Low_offset = i;
				i = tabela->Quantidade + 10;
			}
		}if (i == tabela->Quantidade) {
			sprintf(Erro, "Erro registrador.txt:referencia de %s nao encontrada no registrador %s", Low, Nome);
			AdicionaLog(Erro);
			AtualizaTelaMontador();
		}

	}

	tabela->Quantidade++;
}

TabelaRegistrador * CriaTabelaRegistradorVazia()
{
	TabelaRegistrador *tabela = 0;
	tabela = (TabelaRegistrador*)malloc(sizeof(TabelaRegistrador));
	tabela->Quantidade = 0;
	tabela->Registradores = 0;
	tabela->TabelaValida = SIM;

	return tabela;
}

TabelaRegistrador * CriaTabelaRegistrador(char *Endereco)
{
	FILE *f = 0;
	char *nome = 0;
	char *HIGH = 0;
	char *LOW = 0;
	char *Tamanho = 0;
	bool TamanhoBIN = false;
	char *RefMem = 0;
	bool RefMemBIN = false;
	char *Opcode = 0;
	bool OpcodeBIN = false;
	char Erro[100];
	int linha = 1;
	int i = 0;
	bool ParserValido = false;
	TabelaRegistrador *tabela = 0;
	if (Endereco == NULL) {
		f = fopen("Registrador.txt", "r");
	}
	else {
		f = fopen(Endereco, "r");
	}
	if (f) {
		//só é criado a tabela se tiver certeza de ter pelo menos um arquivo aberto...
		tabela = (TabelaRegistrador*)malloc(sizeof(TabelaRegistrador));
		tabela->Quantidade = 0;
		tabela->Registradores = 0;
		tabela->TabelaValida = SIM;

		while (!feof(f)) {
			//todos os elementos devem estar bem definidos;
			//é checado elemento por elemento para ver se em alguma parte está faltando alguma informação
			nome = CarregaPalavra(f);
			if (nome == 0) {
				sprintf(Erro, "Erro %s:nome do registrador nao encontrado",(Endereco == NULL? "Registrador,txt" : Endereco));
				AdicionaLog(Erro);
				AtualizaTelaMontador();
				tabela->TabelaValida = NAO;
			}
			else {
				Tamanho = CarregaPalavra(f);
				if (Tamanho == 0) {
					sprintf(Erro, "Erro %s:Tamanho nao encontrado do registrador %s", (Endereco == NULL ? "Registrador,txt" : Endereco), nome);
					AdicionaLog(Erro);
					AtualizaTelaMontador();
					tabela->TabelaValida = NAO;
				}
				else {
					RefMem = CarregaPalavra(f);
					if (RefMem == 0) {
						sprintf(Erro, "Erro %s:ref memoria nao encontrada de %s", (Endereco == NULL ? "Registrador,txt" : Endereco),nome);
						AdicionaLog(Erro);
						AtualizaTelaMontador();
						tabela->TabelaValida = NAO;
					}
					else{
						Opcode = CarregaPalavra(f);
						if (Opcode == 0) {
							sprintf(Erro, "Erro %s: Opcode nao encontrado de %s", (Endereco == NULL ? "Registrador,txt" : Endereco), nome);
							AdicionaLog(Erro);
							AtualizaTelaMontador();
							tabela->TabelaValida = NAO;
						}
						else {
							HIGH = CarregaPalavra(f);
							if (HIGH == 0) {
								sprintf(Erro, "Erro %s:ref HIGH nao encontrada de %s", (Endereco == NULL ? "Registrador,txt" : Endereco), nome);
								AdicionaLog(Erro);
								AtualizaTelaMontador();
								tabela->TabelaValida = NAO;
							}
							else {
								LOW = CarregaPalavra(f);
								if (LOW == 0) {
									sprintf(Erro, "Erro %s:ref LOW nao encontrada de %s", (Endereco == NULL ? "Registrador,txt" : Endereco),  nome);
									AdicionaLog(Erro);
									AtualizaTelaMontador();
									tabela->TabelaValida = NAO;
								}
							}
						}
					}
				}
			}
			if (nome && Tamanho && RefMem && HIGH && LOW && Opcode) {//se todos eles foram carregados
				ParserValido = false;
				//nome do registrador não tem como verificar...
				//ETAPA 1: checar se o tamanho do registrador é binario
				if (strlen(Tamanho) == 1 && (Tamanho[0] == '0' || Tamanho[0] == '1')) {//operação é bool então não pode ter mais de um caracter
					TamanhoBIN = Tamanho[0] - '0'; //TESTE 1 PASSOU, a instrução é booleana e de tamanho 1
					//ETAPA 2: verificar se a referência a memória é booleana
					if (strlen(RefMem) == 1 && (RefMem[0] == '0' || RefMem[0] == '1')) {//operação é bool então não pode ter mais de um caracter
						//ETAPA 3: Testa o Opcode
						if (strlen(Opcode) == 4 && (Opcode[0] == '0' || Opcode[0] == '1') && (Opcode[1] == '0' || Opcode[1] == '1') && (Opcode[2] == '0' || Opcode[2] == '1') && (Opcode[3] == '0' || Opcode[3] == '1')) {
							RefMemBIN = RefMem[0] - '0'; //TESTE 3 PASSOU, agora você sabe se o registrador faz referência a memória
							ParserValido = true;
						}
						else {
							sprintf(Erro, "Erro %s:opcao invalida para o opcode %s em %s", (Endereco == NULL ? "Registrador,txt" : Endereco), Opcode,nome);
							AdicionaLog(Erro);
							AtualizaTelaMontador();
							tabela->TabelaValida = NAO;
						}
					}else {
						sprintf(Erro, "Erro %s:opcao invalida para o tamanho de %s", (Endereco == NULL ? "Registrador,txt" : Endereco), nome);
						AdicionaLog(Erro);
						AtualizaTelaMontador();
						tabela->TabelaValida = NAO;
					}
				}else {
					sprintf(Erro, "Erro %s:opcao invalida para o tamanho de %s", (Endereco == NULL ? "Registrador,txt" : Endereco),  nome);
					AdicionaLog(Erro);
					AtualizaTelaMontador();
					tabela->TabelaValida = NAO;
				}
				//Será verificado se o HIGH e LOW estão  registrados dentro da função "AdicionaTabela", caso sejam invalidos será mostrado uma mensagem de erro
				//e será marcado como nulo o valor de HIGH e LOW

				if (ParserValido == true) {//se tudo ocorrer corretamente, carregue a instrucao
					if (strlen(HIGH) == 1 && HIGH[0] == '0') {
						free(HIGH);
						HIGH = 0;
					}
					if (strlen(LOW) == 1 && LOW[0] == '0') {
						free(LOW);
						LOW = 0;
					}
					AdicionaTabela(tabela, nome,TamanhoBIN,RefMemBIN,HIGH,LOW,Opcode);
				}
				else {
					tabela->TabelaValida = NAO;
				}
			}
			linha++;
		}
		fclose(f);
	}
	else {
		sprintf(Erro, "Erro arquivo que contem os registradores (%s) nao foi encontrado", (Endereco == NULL ? "Registrador,txt" : Endereco));
		AdicionaLog(Erro);
		AtualizaTelaMontador();
	}
	return tabela;
}

void LimpaTabela(TabelaRegistrador * tabela)
{
	int c = 0;
	if (tabela) {
		free(tabela->Registradores);
		free(tabela);
		tabela = 0;
	}
}


