//==========================================
//          MAIN DO SIMULADOR
//==========================================
#include <stdio.h>
#include <stdlib.h>


#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Layout.h>
#include <MT2D/MT2D_Keyboard.h>
#include <MT2D/MT2D_System_Calls.h>
#include <MT2D/InputBox/MT2D_InputBox_String.h>
#include <MT2D/MessageBox/MT2D_MessageBox.h>

#include "CPU.h"
#include "Instrucao.h"
#include "Registrador.h"
#include "Texto.h"
#include "Dump_Memoria.h"
#include "Dump_Registrador.h"
#include "Dump_Simulacao.h"
#include "Dump_Cache.h"
#include "RAM.h"
bool SAIR_SIMULACAO = false;

void Acao(Instrucao * Inst, Registrador * Reg, int Valor) {
	if (strcmp(Inst->Nome, "MOV") == 0) {
		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.MOV(Reg, Valor, true);
		}
		else {
			i8086.MOV(Reg, Valor);
		}
	}
	if (strcmp(Inst->Nome, "ADD") == 0) {
		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.ADD(Reg, Valor, true);
		}
		else {
			i8086.ADD(Reg, Valor);
		}
	}
	if (strcmp(Inst->Nome, "SUB") == 0) {
		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.SUB(Reg, Valor, true);
		}
		else {
			i8086.SUB(Reg, Valor);
		}

	}
	if (strcmp(Inst->Nome, "AND") == 0) {
		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.AND(Reg, Valor, true);
		}
		else {
			i8086.AND(Reg, Valor);
		}
	}
	if (strcmp(Inst->Nome, "OR") == 0) {
/*		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.OR(Reg, Valor, true);
		}
		else {
			i8086.OR(Reg, Valor);
		}
		*/
	}
	if (strcmp(Inst->Nome, "XOR") == 0) {
/*		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.XOR(Reg, Valor, true);
		}
		else {
			i8086.XOR(Reg, Valor);
		}*/
	}
	if (strcmp(Inst->Nome, "CMP") == 0) {
		if (Inst->Operacao == REGISTRADOR16_VALOR || Inst->Operacao == REGISTRADOR8_VALOR) {
			i8086.CMP(Reg, Valor, true);
		}
		else {
			i8086.CMP(Reg, Valor);
		}
	}
}

void Acao(Instrucao * Inst, int Valor, Registrador * Reg) {//Endereço de memória -> Registrador e não Valor -> Registrador
	if (strcmp(Inst->Nome, "MOV") == 0) {
		i8086.MOV(Valor, Reg);
	}
	if (strcmp(Inst->Nome, "ADD") == 0) {
		i8086.ADD(Valor, Reg);
	}
	if (strcmp(Inst->Nome, "SUB") == 0) {
		i8086.SUB(Valor,Reg);
	}
	if (strcmp(Inst->Nome, "AND") == 0) {
		i8086.AND(Valor, Reg);
	}
	if (strcmp(Inst->Nome, "OR") == 0) {
//		i8086.OR(Valor, Reg);
	}
	if (strcmp(Inst->Nome, "XOR") == 0) {
//		i8086.XOR(Valor, Reg);
	}
	if (strcmp(Inst->Nome, "CMP") == 0) {
		i8086.CMP(Valor, Reg);
	}
}

void Acao(Instrucao * Inst, Registrador * Reg) {// 7 8
	if (strcmp(Inst->Nome, "PUSH") == 0) {
		i8086.PUSH(Reg);
	}
	if (strcmp(Inst->Nome, "POP") == 0) {
		i8086.POP(Reg);
	}
	if (strcmp(Inst->Nome, "INC") == 0) {
		i8086.INC(Reg);
	}
	if (strcmp(Inst->Nome, "DEC") == 0) {
		i8086.DEC(Reg);
	}
	if (strcmp(Inst->Nome, "DIV") == 0) {
		i8086.DIV(Reg);
	}
	if (strcmp(Inst->Nome, "NOT") == 0) {
		i8086.NOT(Reg);
	}
	if (strcmp(Inst->Nome, "CALL") == 0) {
		i8086.CALL(Reg);
	}
}

/**
	Trata Memoria e Imediato aqui
*/
void Acao(Instrucao * Inst, int Valor) {
	if (strcmp(Inst->Nome, "MUL") == 0) {
		if (Inst->Operacao == VALOR_VAZIO) {
			i8086.MUL(Valor, true);
		}
		else {
			i8086.MUL(Valor);
		}
	}
	if (strcmp(Inst->Nome, "DIV") == 0) {
		if (Inst->Operacao == VALOR_VAZIO) {
			i8086.DIV(Valor, true);
		}
		else {
			i8086.DIV(Valor);
		}
	}
	if (strcmp(Inst->Nome, "CALL") == 0) {
		if (Inst->Operacao == VALOR_VAZIO) {
			i8086.CALL(Valor, true);
		}
		else {
			i8086.CALL(Valor);
		}
	}
	if (strcmp(Inst->Nome, "ORG") == 0) {
	}
	if (strcmp(Inst->Nome, "INC") == 0) {
		i8086.INC(Valor);
	}
	if (strcmp(Inst->Nome, "DEC") == 0) {
		i8086.DEC(Valor);
	}
	if (strcmp(Inst->Nome, "NOT") == 0) {
		i8086.NOT(Valor);
	}
	if (strcmp(Inst->Nome, "JMP") == 0) {
		if (Inst->Operacao == VALOR_VAZIO) {
			i8086.JMP(Valor, true); // JMP 600h | label
		}
		else {
			i8086.JMP(Valor);// JMP [600h]
		}
	}
	if (strcmp(Inst->Nome, "JE") == 0) {
		i8086.JE(Valor);
	}
	if (strcmp(Inst->Nome, "JZ") == 0) {
		i8086.JZ(Valor);
	}
	if (strcmp(Inst->Nome, "JL") == 0) {
		i8086.JL(Valor);
	}
	if (strcmp(Inst->Nome, "JNGE") == 0) {
		i8086.JNGE(Valor);
	}
	if (strcmp(Inst->Nome, "JA") == 0) {
		i8086.JA(Valor);
	}
	if (strcmp(Inst->Nome, "JNBE") == 0) {
		i8086.JNBE(Valor);
	}
	if (strcmp(Inst->Nome, "JB") == 0) {
		i8086.JB(Valor);
	}
	if (strcmp(Inst->Nome, "JNAE") == 0) {
		i8086.JNAE(Valor);
	}
	if (strcmp(Inst->Nome, "JA") == 0) {
		i8086.JA(Valor);
	}
	if (strcmp(Inst->Nome, "JNBE") == 0) {
		i8086.JNBE(Valor);
	}
	if (strcmp(Inst->Nome, "JB") == 0) {
		i8086.JB(Valor);
	}
	if (strcmp(Inst->Nome, "JNAE") == 0) {
		i8086.JNAE(Valor);
	}
	if (strcmp(Inst->Nome, "JG") == 0) {
		i8086.JG(Valor);
	}
	if (strcmp(Inst->Nome, "JNLE") == 0) {
		i8086.JNLE(Valor);
	}
	if (strcmp(Inst->Nome, "JNE") == 0) {
		i8086.JNE(Valor);
	}
	if (strcmp(Inst->Nome, "JNZ") == 0) {
		i8086.JNZ(Valor);
	}

	if (strcmp(Inst->Nome, "PUSH") == 0) {
		i8086.PUSH(Valor);
	}
	if (strcmp(Inst->Nome, "POP") == 0) {
		i8086.POP(Valor);
	}
}

void Acao(Instrucao * Inst) {
	if (strcmp(Inst->Nome, "RET") == 0) {
		i8086.RET();
	}
}

void Acao(Instrucao * Inst, Registrador * Reg, Registrador * Reg2) {//5 6
	if (strcmp(Inst->Nome, "MOV") == 0) {
		i8086.MOV(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "ADD") == 0) {
		i8086.ADD(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "SUB") == 0) {
		i8086.SUB(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "AND") == 0) {
		i8086.AND(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "OR") == 0) {
		i8086.OR(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "XOR") == 0) {
		i8086.XOR(Reg, Reg2);
	}
	if (strcmp(Inst->Nome, "CMP") == 0) {
		i8086.CMP(Reg, Reg2);
	}
}

void Acao(Instrucao * Inst, int Memoria, int Imediato) {
	if (strcmp(Inst->Nome, "MOV") == 0) {
		i8086.MOV(Memoria, Imediato);
	}
	if (strcmp(Inst->Nome, "ADD") == 0) {
		i8086.ADD(Memoria, Imediato);
	}
	if (strcmp(Inst->Nome, "SUB") == 0) {
		i8086.SUB(Memoria, Imediato);
	}
	if (strcmp(Inst->Nome, "AND") == 0) {
	}
	if (strcmp(Inst->Nome, "OR") == 0) {
	}
	if (strcmp(Inst->Nome, "XOR") == 0) {
	}
	if (strcmp(Inst->Nome, "CMP") == 0) {
		i8086.CMP(Memoria,Imediato);
	}
}

/**
0 : nenhum, 1, 2, 3
*/
void Desenha_Menu1(int OpcaoSelecionado);

/**
0 : nenhum, 1, 2, 3
*/
void SDesenha_Menu(int OpcaoSelecionado);

/**
Retorno:
false: sair
true: continuar a simulação
*/
bool Menu_Simulador();
char *ArquivoFonte = 0;
FILE *Arquivo=0;
CPU i8086;
int Velocidade_Simulador = 50;


void Transfere_Dado_Queue_Para_Char(int Inicio_Queue, char *PNT, int Quantidade_Dados, int Offset) {
	int i = Offset;
	int j = 0;
	char *Queue_Binario;
	if (i8086.QUEUE_ULTIMO > Inicio_Queue) {
		for (j = 0; j < Quantidade_Dados;) {
			Queue_Binario = InteiroParaCharBinario(i8086.QUEUE[Inicio_Queue], _8BITS); // a queue é salvo em um valor inteiro então precisamos converte-la para binario
			for (; i < 8 && j < Quantidade_Dados; i++, j++) {
				PNT[j] = Queue_Binario[i];
			}
			i = 0;
			Inicio_Queue++;
			free(Queue_Binario);

		}
		PNT[j] = '\0';
	}
}

void Carrega_Memoria_BIU() {
	i8086.Bus_Int = Memoria[i8086.IP->Registradores[0].Dado] << 8;
	i8086.Bus_Int += Memoria[i8086.IP->Registradores[0].Dado +1];
}

void Carrega_BIU_Queue() {
	//essa função é sempre chamada quando deu um cache miss então primeiro atualiza o cache que esta entre a biu e o cache e manda o dado pra queue
	int tmp;
	for (int i = 0; i < 8; i++)
	{
		tmp = Memoria[i8086.IP->Registradores[0].Dado + i] ;
		i8086.Cache_Salva_Dado(tmp, i8086.IP->Registradores[0].Dado + i, _8BITS);
	}


	i8086.QUEUE[i8086.QUEUE_ULTIMO] = i8086.Bus_Int >> 8;
	i8086.QUEUE[i8086.QUEUE_ULTIMO + 1] = i8086.Bus_Int - (i8086.QUEUE[i8086.QUEUE_ULTIMO] << 8);

	i8086.QUEUE_ULTIMO+=2;
}

void Carrega_Cache_Queue()
{

	i8086.QUEUE[i8086.QUEUE_ULTIMO] = i8086.Bus_Int >> 8;
	i8086.QUEUE[i8086.QUEUE_ULTIMO + 1] = i8086.Bus_Int - (i8086.QUEUE[i8086.QUEUE_ULTIMO] << 8);

	i8086.QUEUE_ULTIMO += 2;
}

bool Carrega_Cache() {
	bool DeuHit = i8086.Cache_Deu_Hit();
	if (DeuHit == false)
	{
		i8086.cache.Miss++;
	}
	else {
		i8086.Bus_Int = i8086.cache.Memoria[i8086.IP->Registradores[0].Dado % sizeofCache];
		i8086.Bus_Int = i8086.Bus_Int  << 8;
		i8086.Bus_Int += i8086.cache.Memoria[ (i8086.IP->Registradores[0].Dado + 1) % sizeofCache];

		i8086.cache.Hit++;

	}
	return DeuHit;
}

void Decodifica_Queue() {
	int Buffer = i8086.QUEUE[0];
	int Operando1, Operando2;

	//<DADOS DO PROCESSADOR>
	Instrucao *instrucao = &i8086.Instrucoes->Instrucoes[Instrucao_PegaID(i8086.Instrucoes, InteiroParaCharBinario(Buffer, _8BITS))];
	Registrador * Reg1=0, *Reg2=0;
	int Valor=0,Valor2=0;
	int EndMemoria = 0;
	char Conteudo_4BITS[5], Conteudo2_4BITS[5], Conteudo_8BITS[9], Conteudo_16BITS[17];
	//</DADOS DO PROCESSADOR>
	int Tamanho_Instrucao;
	int Qnt_Elementos = 0;
	switch (instrucao->Operacao) {
	case 1: // 16 bits -> 4 bits
	case 2: // 16 bits -> 4 bits
		Qnt_Elementos = 4;
		break;//+ 3 leituras
	case 3: // 4 bits -> 16 bits
	case 4: // 4 bits -> 16 bits
		Qnt_Elementos = 4;
		break;//+ 3 leituras
	case 5: // 4 bits -> 4 bits
	case 6: // 4 bits -> 4 bits
		Qnt_Elementos = 2;
		break;// + 1 leitura
	case 7: // 4 bits
	case 8: // 4 bits
		Qnt_Elementos = 2;
		break;// + 1 leitura
	case 9: // 16 bits
		Qnt_Elementos = 3;
		break; // + 2 leituras
	case 10: // sem mais leituras
		Qnt_Elementos = 1;
		break;
		Qnt_Elementos = 4;
	case 11:// 4 bits -> 16 bits
		Qnt_Elementos = 4;
		break;// + 3 leituras
	case 12:// 4 bits -> 8 bits
		Qnt_Elementos = 3;
		break; // + 2 leituras
	case 13:// 16 bits
		Qnt_Elementos = 3;
		break;// + 2 leituras
	case 18: // 16 bits -> 16 bits
		Qnt_Elementos = 5;
		break;// + 4 leituras
	}
	if (Qnt_Elementos <= i8086.QUEUE_ULTIMO) {// temos todos os elementos 
		int zz = 0;
		int f = 0;
		switch (instrucao->Operacao) {
		case 1: // 16 bits -> 4 bits
		case 2: // 16 bits -> 4 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_16BITS, 16, 0);
			Transfere_Dado_Queue_Para_Char(3, Conteudo_4BITS, 4, 0);
			Reg1 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Valor = CharBinarioParaInteiro(Conteudo_16BITS);
			Acao(instrucao, Valor, Reg1);
			Qnt_Elementos = 4;
			break;//+ 3 leituras
		case 3: // 4 bits -> 16 bits
		case 4: // 4 bits -> 16 bits
		case 11:// 4 bits -> 16 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_4BITS, 4, 0);
			Transfere_Dado_Queue_Para_Char(1, Conteudo_16BITS, 16, 4);
			Reg1 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Valor = CharBinarioParaInteiro(Conteudo_16BITS);
			Acao(instrucao, Reg1, Valor);
			Qnt_Elementos = 4;
			break;//+ 3 leituras
		case 5: // 4 bits -> 4 bits
		case 6: // 4 bits -> 4 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_4BITS, 4, 0);
			Reg1 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Transfere_Dado_Queue_Para_Char(1, Conteudo_4BITS, 4, 4);
			Reg2 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Acao(instrucao, Reg1, Reg2);
			Qnt_Elementos = 2;
			break;// + 1 leitura
		case 7: // 4 bits
		case 8: // 4 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_4BITS, 4, 0);
			Reg1 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Acao(instrucao, Reg1);
			Qnt_Elementos = 2;
			break;// + 1 leitura
		case 9: // 16 bits
		case 13:// 16 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_16BITS, 16, 0);
			Valor = CharBinarioParaInteiro(Conteudo_16BITS);
			Acao(instrucao, Valor);
			Qnt_Elementos = 3;
			break; // + 2 leituras
		case 10: // sem mais leituras
			Acao(instrucao);
			Qnt_Elementos = 1;
			break;
		case 12:// 4 bits -> 8 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_4BITS, 4, 0);
			Transfere_Dado_Queue_Para_Char(1, Conteudo_8BITS, 8, 4);
			Reg1 = &i8086.Registradores_Geral->Registradores[Registrador_PegaID(i8086.Registradores_Geral, Conteudo_4BITS, true)];
			Valor = CharBinarioParaInteiro(Conteudo_8BITS);
			Acao(instrucao, Reg1, Valor);
			Qnt_Elementos = 3;
			break; // + 2 leituras
		case 18: // 16 bits -> 16 bits
			Transfere_Dado_Queue_Para_Char(1, Conteudo_16BITS, 16, 0);
			Valor = CharBinarioParaInteiro(Conteudo_16BITS);
			Transfere_Dado_Queue_Para_Char(3, Conteudo_16BITS, 16, 0);
			Valor2 = CharBinarioParaInteiro(Conteudo_16BITS);
			Acao(instrucao, Valor, Valor2);
			Qnt_Elementos = 5;
			break;// + 4 leituras
		}
		i8086.QUEUE_ULTIMO = 0; // "Limpa" a queue
	}
	else {
		// vai pegar o resto
	}
}

void Incremementa_IP() {
	i8086.IP->Registradores[0].Dado+=2;
}
void Smain() {
	int i,j, k;
	char BUFFER_BYTE[9];
#pragma region PARTE1
	MT2D_Create_Window_Layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
	insert_string_on_display("Simulador", 0, 60, DISPLAY_WINDOW1);
	MT2D_Draw_Window(DISPLAY_WINDOW1);
	i8086.Instrucoes = CriaTabelaInstrucao();
	i8086.Registradores_Geral = CriaTabelaRegistrador(NULL);
	
	i8086.Registradores_Segmento = CriaTabelaRegistrador("Segmento.txt");

	i8086.Registrador_Flags = CriaTabelaRegistrador("Flags.txt");

	i8086.IP = CriaTabelaRegistradorVazia();
	AdicionaTabela(i8086.IP, "IP", _16BITS, SIM, NULL, NULL, "0000");
	for (i = 1; i < 30; i++) {
		AdicionaTabela(i8086.IP, "PILHA_IP", _16BITS, SIM, NULL, NULL, "0000");
		i8086.IP->Registradores[i].Dado = 0;
	}

	i8086.IP->Registradores[0].Dado = 256;


#pragma endregion - Inicializa as estruturas da simulação
	if (!i8086.Instrucoes && !i8086.Registradores_Geral) {
		MT2D_InputBox_String("Instrucoes ou Registradores nao encontrados, simulador abortado");
	}
	else {
		do {
			ArquivoFonte = MT2D_InputBox_String("Digite o endereco e\\ou o nome do arquito");
			if (!ArquivoFonte) {
				MT2D_InputBox_String("Nada foi digitado");
			}
			else {
				Arquivo = fopen(ArquivoFonte, "r");
				if (!Arquivo) {
					free(ArquivoFonte);
					MT2D_InputBox_String("Arquivo nao encontrado");
				}
			}

		} while (!Arquivo);
#pragma region PARTE 2
		for (i = 0; i < 65535; i++) {//limpa a memória
			Memoria[i] = 0;
		}
		k = 256;//100h
		while (!feof(Arquivo)) {
			for (j = 0; j < 8 && !feof(Arquivo); j++) {// captura 2 bytes
				BUFFER_BYTE[j] = fgetc(Arquivo);
			}
			BUFFER_BYTE[j] = '\0';
			Memoria[k] = CharBinarioParaInteiro(BUFFER_BYTE);//carrega o programa na memória
			k++;
		}
#pragma endregion - Carrega o programa na memória

		SAIR_SIMULACAO = false;
		Menu_Simulador();
		while (i8086.IP->Registradores[0].Dado < k - 1 && i8086.IP->Registradores[0].Dado >= 16) {

			if (Carrega_Cache() == false)
			{
				if (SAIR_SIMULACAO == true) break;
				Carrega_Memoria_BIU();
				Dump_Simulacao(0, 1, 0, 0, "Carrega dado da memoria para o BUS", "Cache Miss");

				if (SAIR_SIMULACAO == true) break;
				Carrega_BIU_Queue();
				Dump_Simulacao(1, 2, 0, 0, "Carrega o dado do BUS para a QUEUE", "");

			}
			else
			{
				Carrega_Cache_Queue();
				Dump_Simulacao(1, 2, 0, 0, "Carrega o dado do BUS para a QUEUE", "Cache Hit");
			}


			if (SAIR_SIMULACAO == true) break;
			Dump_Simulacao(2, 3, 0, 0, "Decodifica QUEUE", "");
			Decodifica_Queue();


			if (SAIR_SIMULACAO == true) break;
			Incremementa_IP();
			Dump_Simulacao(5, 4, 0, 0, "Incrementa IP", "");
		}
		MT2D_InputBox_String("FIM DA SIMULACAO");
		Menu_Simulador();
		LimpaTabela(i8086.Instrucoes);
		LimpaTabela(i8086.Registradores_Segmento);
		LimpaTabela(i8086.Registrador_Flags);
		LimpaTabela(i8086.IP);
	}
}

/**
	Retorno:
	false: sair
	true: continuar a simulação
*/
bool Menu_Simulador() {
	bool Saida = false;
	int opcao = 1;
	int tecla = 0;
	int show_cnt = 0;
	do {
		if (show_cnt < 30) {
			SDesenha_Menu(opcao);
			show_cnt++;
		}
		else if (show_cnt < 60) {
			show_cnt++;
			SDesenha_Menu(5);
		}
		else {
			show_cnt = 0;
		}
		if (MT2D_Keyboard_touched()) {
			show_cnt = 0;
			tecla = MT2D_Keyboard_keytouched();
			if (tecla == arrow_key_pressed) {
				tecla = MT2D_Keyboard_keytouched();
				if (tecla == arrow_left_pressed) {
					opcao--;
					if (opcao == 0) {
						opcao = 4;
					}
				}
				if (tecla == arrow_right_pressed) {
					opcao++;
					if (opcao == 5) {
						opcao = 1;
					}
				}
			}
			else if (tecla == enter_pressed) {
				if (opcao == 1) {
					Dump_Memoria();
				}
				else if (opcao == 2) {
					Dump_Registrador();
				}
				else if (opcao == 3) {
					Dump_Cache();
				}
				else if (opcao == 4) {
					tecla = 'p';
				}
			}
			else if (tecla == '+') {
				if (Velocidade_Simulador < 500) {
					Velocidade_Simulador += 10;
					MT2D_System_Delay(100);
				}
			}
			else if (tecla == '-') {
				if (Velocidade_Simulador > 0) {
					Velocidade_Simulador -= 10;
					MT2D_System_Delay(100);
				}
			}
		}
		MT2D_Draw_Window(DISPLAY_WINDOW1);
		MT2D_System_Delay(10);
	} while (tecla != 'p' && tecla != 'P' && Saida == false);
	if (opcao != 4) {
		Saida = true;
	}
	else {
		Saida = false;
		SAIR_SIMULACAO = true;
	}
	return Saida;
}


void Desenha_Janela_Simulador() {
	MT2D_Create_Window_Layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
	insert_string_on_display("Simulador", 0, 50, DISPLAY_WINDOW1);

}

/**
  0 : nenhum, 1, 2, 3
*/
void SDesenha_Menu(int OpcaoSelecionado) {
	int Xo = 0, Yo = 0;
	int i = 0;
	const int  InicioHorJanelas[1] = { 0 };
	const int  InicioVerJanelas[1] = { 0 };
	Desenha_Janela_Simulador();

	MT2D_Create_Window_Layout(10, 5, 9, 8, true, false, false, ' ', DISPLAY_WINDOW1);
	char *Buffer=0;
	for (int i = 0; i < i8086.QUEUE_ULTIMO; i++) {
		Buffer = InteiroParaCharBinario(i8086.QUEUE[i], _8BITS);
		insert_string_on_display(Buffer, 5 + 7 - i, 11, DISPLAY_WINDOW1);
		free(Buffer);
	}
	insert_string_on_display("QUEUE", 4, 12, DISPLAY_WINDOW1);

	if (i8086.QUEUE_ULTIMO > 0) {
		char *Inst = InteiroParaCharBinario(i8086.QUEUE[0], _8BITS);
		int Inst_ID = Instrucao_PegaID(i8086.Instrucoes, Inst);
		Buffer = (char*)malloc(46 * sizeof(char));
		switch (i8086.Instrucoes->Instrucoes[Inst_ID].Operacao) {
		case MEMORIA_REGISTRADOR16:
			sprintf(Buffer, "%s Memoria Registrador 16 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case MEMORIA_REGISTRADOR8:
			sprintf(Buffer, "%s Memoria Registrador 8 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR16_MEMORIA:
			sprintf(Buffer, "%s Registrador 16 bits Memoria", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR8_MEMORIA:
			sprintf(Buffer, "%s Registrador 16 bits Memoria", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR16_REGISTRADOR16:
			sprintf(Buffer, "%s Registrador 16 bits Registrador 16 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR8_REGISTRADOR8:
			sprintf(Buffer, "%s Registrador 8 bits Registrador 8 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR16_VAZIO:
			sprintf(Buffer, "%s Registrador 16 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR8_VAZIO:
			sprintf(Buffer, "%s Registrador 8 bits", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case MEMORIA_VAZIO:
			sprintf(Buffer, "%s Memoria", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case VAZIO:
			sprintf(Buffer, "%s", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR16_VALOR:
			sprintf(Buffer, "%s Registrador 16 bits Imediato", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case REGISTRADOR8_VALOR:
			sprintf(Buffer, "%s Registrador 8 bits Imediato", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case VALOR_VAZIO:
			sprintf(Buffer, "%s Imediato", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		case MEMORIA_VALOR:
			sprintf(Buffer, "%s Memoria Imediato", i8086.Instrucoes->Instrucoes[Inst_ID].Nome);
			break;
		default:
			sprintf(Buffer, "Instrucao Desconhecida");
		}
		insert_string_on_display(Buffer, 13, 22, DISPLAY_WINDOW1);
		free(Buffer);
	}
	insert_string_on_display("Aperte + ou - para alterar a velocidade da simulacao", 21, 1, DISPLAY_WINDOW1);
	insert_string_on_display("Velocidade da Simulacao:", 22, 1, DISPLAY_WINDOW1);
	insert_number_on_display(Velocidade_Simulador, 22, 26, DISPLAY_WINDOW1);
	insert_string_on_display("Aperte (P) para iniciar a proxima etapa da simulacao", 23, 1, DISPLAY_WINDOW1);
	insert_string_on_display(" VER MEMORIA ", 1, InicioHorJanelas[0] + 1, DISPLAY_WINDOW1);
	insert_string_on_display(" REGISTRADOR ", 1, InicioHorJanelas[0] + 20, DISPLAY_WINDOW1);
	insert_string_on_display(" VER CACHE   ", 1, InicioHorJanelas[0] + 40, DISPLAY_WINDOW1);
	insert_string_on_display("   FECHAR    ",1, InicioHorJanelas[0] + 60, DISPLAY_WINDOW1);
	if (OpcaoSelecionado) {
		switch (OpcaoSelecionado) {
		case 1:
			Xo = InicioHorJanelas[0];
			Yo = InicioVerJanelas[0];
			break;
		case 2:
			Xo = InicioHorJanelas[0] + 19;
			Yo = InicioVerJanelas[0];
			break;
		case 3:
			Xo = InicioHorJanelas[0] + 39;
			Yo = InicioVerJanelas[0];
			break;
		case 4:
			Xo = InicioHorJanelas[0] + 59;
			Yo = InicioVerJanelas[0];
			break;
		}
		//desenha bordas para indicar qual opção foi selecionada.
		if (OpcaoSelecionado < 5) {
			i = Xo;
			WINDOW1[Yo + 1][i] = '>';
			while (i < Xo + 15) {
				WINDOW1[Yo][i] = 'v';
				i++;
			}
			WINDOW1[Yo + 1][i - 1] = '<';
			i = Xo;
			while (i < Xo + 15) {
				WINDOW1[Yo + 2][i] = '^';
				i++;
			}
		}
	}

}