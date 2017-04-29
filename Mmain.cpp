//==========================================
//          MAIN DO MONTADOR
//==========================================
#include <stdio.h>
#include <stdlib.h>
#include "../../MT2D/Window_core.h"
#include "../../MT2D/system_calls.h"
#include "../../MT2D/Keyboard.h"
#include "../../MT2D/building_functions/generic_string_reader.h"
#include "../../MT2D/building_functions/display_popup_with_result.h"
#include "Mmain.h"
#include "Log.h"
#include "Texto.h"
#include "Montador.h"

bool MontadorSelecionado[3] = {true,false,false};
char TamanhoHorJanelas[3] = {MAX_HOR-1,MAX_HOR - 1,MAX_HOR - 1 };
char TamanhoVerJanelas[3] = {2,17,5};
char InicioHorJanelas[3] = {0,0,0};
char InicioVerJanelas[3] = { 0,2,19 };

extern Texto *Log;
extern int  LinhaLog;
extern int ColunaLog;
Texto *ArquivoFonte=0;
int LinhaArquivoFonte = 0;

void Desenha_Bordas_Janelas() {
	if (MontadorSelecionado[0] == SIM) {
		TamanhoVerJanelas[0] = 2;
		TamanhoVerJanelas[1] = 17;
		TamanhoVerJanelas[2] = 5;
		InicioVerJanelas[0] = 0;
		InicioVerJanelas[1] = 2;
		InicioVerJanelas[2] = 19;
		create_window_layout(InicioHorJanelas[1], InicioVerJanelas[1], TamanhoHorJanelas[1], TamanhoVerJanelas[1], MontadorSelecionado[1], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[2], InicioVerJanelas[2], TamanhoHorJanelas[2], TamanhoVerJanelas[2], MontadorSelecionado[2], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[0], InicioVerJanelas[0], TamanhoHorJanelas[0], TamanhoVerJanelas[0], MontadorSelecionado[0], NAO, NAO, ' ', DISPLAY_WINDOW1);
	}
	else if (MontadorSelecionado[1] == SIM) {
		TamanhoVerJanelas[0] = 2;
		TamanhoVerJanelas[1] = 19;
		TamanhoVerJanelas[2] = 3;
		InicioVerJanelas[0] = 0;
		InicioVerJanelas[1] = 2;
		InicioVerJanelas[2] = 21;
		create_window_layout(InicioHorJanelas[0], InicioVerJanelas[0], TamanhoHorJanelas[0], TamanhoVerJanelas[0], MontadorSelecionado[0], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[2], InicioVerJanelas[2], TamanhoHorJanelas[2], TamanhoVerJanelas[2], MontadorSelecionado[2], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[1], InicioVerJanelas[1], TamanhoHorJanelas[1], TamanhoVerJanelas[1], MontadorSelecionado[1], NAO, NAO, ' ', DISPLAY_WINDOW1);

	}
	else {
		TamanhoVerJanelas[0] = 2;
		TamanhoVerJanelas[1] = 11;
		TamanhoVerJanelas[2] = 11;
		InicioVerJanelas[0] = 0;
		InicioVerJanelas[1] = 2;
		InicioVerJanelas[2] = 13;
		create_window_layout(InicioHorJanelas[0], InicioVerJanelas[0], TamanhoHorJanelas[0], TamanhoVerJanelas[0], MontadorSelecionado[1], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[1], InicioVerJanelas[1], TamanhoHorJanelas[1], TamanhoVerJanelas[1], MontadorSelecionado[2], NAO, NAO, ' ', DISPLAY_WINDOW1);
		create_window_layout(InicioHorJanelas[2], InicioVerJanelas[2], TamanhoHorJanelas[2], TamanhoVerJanelas[2], MontadorSelecionado[2], NAO, NAO, ' ', DISPLAY_WINDOW1);
	}
}

void Desenha_Menu1(int OpcaoSelecionado) {// 0 : nenhum, 1, 2, 3
	int Xo = 0, Yo = 0;
	int i = 0;
	insert_string_on_display("ABRIR ARQUIVO", InicioVerJanelas[0] + 1, InicioHorJanelas[0]+1,  DISPLAY_WINDOW1);
	insert_string_on_display("  COMPILAR", InicioVerJanelas[0] + 1, InicioHorJanelas[0] + 20, DISPLAY_WINDOW1);
	insert_string_on_display("  SALVAR LOG",  InicioVerJanelas[0] + 1, InicioHorJanelas[0]+40, DISPLAY_WINDOW1);
	insert_string_on_display("   FECHAR    ", InicioVerJanelas[0] + 1, InicioHorJanelas[0]+60, DISPLAY_WINDOW1);
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
			Xo = InicioHorJanelas[0]+39;
			Yo = InicioVerJanelas[0];
			break;
		case 4:
			Xo = InicioHorJanelas[0] + 59;
			Yo = InicioVerJanelas[0];
			break;
		}
		//desenha bordas para indicar qual opção foi selecionada.
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

void Desenha_Codigo_Fonte() {
//LinhaArquivoFonte
	int i = 0,j=0;
	if (ArquivoFonte) {//só escreve se tiver um texto carregado...
		if (LinhaArquivoFonte < ArquivoFonte->QntLinhas) {//se não estivermos fora do campo de visão..

			for (i = LinhaArquivoFonte, j = InicioVerJanelas[1] + 1 + MontadorSelecionado[0]; i < ArquivoFonte->QntLinhas && j < InicioVerJanelas[2]; i++, j++) {
				insert_number_on_display(i + 1, j, InicioHorJanelas[1] + 1, DISPLAY_WINDOW1); // insere o número da linha
				insert_string_on_display(ArquivoFonte->Linhas[i].Caracteres, j, InicioHorJanelas[1] + 6, DISPLAY_WINDOW1);
			}
		}
	}
}

void Desenha_Log() {
	//LinhaArquivoFonte
	int i = 0, j = 0;
	if (Log) {//só escreve se tiver um texto carregado...
		if (LinhaLog < Log->QntLinhas) {//se não estivermos fora do campo de visão..

//			for (i = LinhaLog , j = InicioVerJanelas[2] + 1 + MontadorSelecionado[1]; i < Log->QntLinhas && j < InicioVerJanelas[2] + TamanhoVerJanelas[2] ; i++, j++) {
			for (i = LinhaLog, j = InicioVerJanelas[2] + TamanhoVerJanelas[2] -1; i < Log->QntLinhas && j >= InicioVerJanelas[2] + 1 + MontadorSelecionado[1]; i++, j--) {
			insert_number_on_display(i + 1, j, InicioHorJanelas[2] + 1 - ColunaLog, DISPLAY_WINDOW1); // insere o número da linha
				insert_string_on_display(Log->Linhas[i].Caracteres, j, InicioHorJanelas[2] + 6 - ColunaLog, DISPLAY_WINDOW1);
			}
		}
	}
}

bool Mmenu1() {//controla as animações e ações do menu 1, retorna SIM se é pra sair do montador ou NAO se é pra continuar no montador
	bool Saida = NAO;
	unsigned char Tecla = 0;
	char Duracao = 0;
	char Selecionado = 1;
	MontadorSelecionado[0] = SIM;
	MontadorSelecionado[1] = NAO;
	MontadorSelecionado[2] = NAO;
	do {
		//Leitura de teclas
		if (Keyboard_touched()) {
			Tecla = Keyboard_keytouched();
			if (Tecla == _key) {//uma das setas foram acionadas. Precisa checar novamente o teclado pra saber qual delas foi acionada
				Tecla = Keyboard_keytouched();
				if (Tecla == key_left) {
					if (Selecionado == 1) {
						Selecionado = 4;
					}
					else {
						Selecionado--;
					}
					Duracao = 0;
				}else if (Tecla == key_right) {
					if (Selecionado == 4) {
						Selecionado = 1;
					}
					else {
						Selecionado++;
					}
					Duracao = 0;
				}
			}
			else if (Tecla == enter) {//entra na opção marcada
				switch (Selecionado) {
				case 4: 
					//botão sair
					Saida = SIM;
					break;
				case 3:
					//botão Salva log
					SalvaLog();
					break;
				case 2:
					// botão compila
					Montador(ArquivoFonte);
					break;	
				case 1:
					//botão abrir arquivo fonte
					Destroi_Texto(ArquivoFonte);
					ArquivoFonte = CarregaTexto(get_string_popup("Digite o nome e o endereco do arquivo fonte"));
					if (ArquivoFonte) {
						AdicionaLog("Arquivo Fonte Carregado");
					}
					else {
						AdicionaLog("Arquivo Fonte Nao Carregado");
					}
					LinhaArquivoFonte = 0;
					break;
				}
			}
		}
		else {
			//faz animação do menu (PISCA)
			if (Duracao == 0) {
				Desenha_Bordas_Janelas();
				Desenha_Menu1(Selecionado);
			}
			else if (Duracao < 10) {
				Desenha_Menu1(Selecionado);
			}
			else if (Duracao == 10) {
				Desenha_Bordas_Janelas();
				Desenha_Menu1(0);

			}else if (Duracao < 20) {
				Desenha_Menu1(0);
			}
			else {
				Duracao = -1;
			}
			Duracao++;
		}
		Desenha_Codigo_Fonte();
		Desenha_Log();
		Delay(50);
		print_display(DISPLAY_WINDOW1);
	} while (Tecla != 9 && !Saida);//TAB
	if (Tecla == 9) {
		MontadorSelecionado[0] = false;
		MontadorSelecionado[1] = true;
		MontadorSelecionado[2] = false;
	}
	return Saida;
}

void Mmenu2() {//controla basicamente a posição do texto...
	int Tecla = 0;
	do {
		Desenha_Bordas_Janelas();
		Desenha_Menu1(0);
		Desenha_Codigo_Fonte();
		Desenha_Log();
		insert_number_on_display(ArquivoFonte ? ArquivoFonte->QntLinhas : 0 , InicioVerJanelas[2], InicioHorJanelas[1] + 1, DISPLAY_WINDOW1);// imprime  quantidade linhas contidos no arquivofonte, caso o arquivo fonte não exista imprima 0 (senão da null pointer,...)
		print_display(DISPLAY_WINDOW1);
		while (!Keyboard_touched()) Delay(10);
		Tecla = Keyboard_keytouched();
		if (Tecla == _key) {
			Tecla = Keyboard_keytouched();
			if (Tecla == key_down) {
				if (ArquivoFonte) {
					if (LinhaArquivoFonte <= ArquivoFonte->QntLinhas) {//vai para a parte de baixo do texto e para até sumir a ultima linha
						LinhaArquivoFonte++;
					}
				}
			}else if (Tecla == key_up) {//sobe o texto
				if (LinhaArquivoFonte > 0) {
					LinhaArquivoFonte--;
				}

			}
			else if (Tecla == 71) {// botão HOME
				LinhaArquivoFonte = 0;
			}
			else if (Tecla == 79) {// botão END
				if (ArquivoFonte) {
					LinhaArquivoFonte = ArquivoFonte->QntLinhas - 1 > 0 ? ArquivoFonte->QntLinhas - 1 : 0;
					//move para a posição da ultima linha, caso a ultima linha seja zero (texto em branco) deixe na
					//posição inicial.
				}
			}
		}
	} while (Tecla != 9);
	MontadorSelecionado[0] = false;
	MontadorSelecionado[1] = false;
	MontadorSelecionado[2] = true;
}

bool Mmenu3() {//controla basicamente a posição do log...
	int Tecla = 0;
	do {
		Desenha_Bordas_Janelas();
		Desenha_Menu1(0);
		Desenha_Codigo_Fonte();
		Desenha_Log();
		print_display(DISPLAY_WINDOW1);
		while (!Keyboard_touched()) Delay(10);
		Tecla = Keyboard_keytouched();
		if (Tecla == _key) {
			Tecla = Keyboard_keytouched();
			if (Tecla == key_up) {
				if (Log) {
					if (LinhaLog < Log->QntLinhas -1) {//vai para a parte de baixo do log e para até sumir a ultima linha
						LinhaLog++;
					}
				}
			}
			else if (Tecla == key_down) {//sobe o log
				if (LinhaLog > 0) {
					LinhaLog--;
				}
			}
			else if (Tecla == key_right) {
				ColunaLog++;
			}
			else if (Tecla == key_left && ColunaLog > 0) {
				ColunaLog--;
			}
			else if (Tecla == 71) {// botão HOME
				LinhaLog = 0;
			}
			else if (Tecla == 79) {// botão END
				LinhaLog = Log->QntLinhas - 1;
			}
		}
		else if (Tecla == enter) {
			if (print_popup_wiht_result("Deseja apagar o arquivo de log?", "", "SIM", "NAO") == SIM) {
				DestroiLog();
			}
		}

	} while (Tecla != 9);
	MontadorSelecionado[0] = true;
	MontadorSelecionado[1] = false;
	MontadorSelecionado[2] = false;
}

void Mmain()
{
	bool saida = NAO;
	Desenha_Bordas_Janelas();
	DestroiLog();
	CriaLog();
	print_display(DISPLAY_WINDOW1);
	do {

		if (MontadorSelecionado[0] == SIM) {
			saida = Mmenu1();// somente o menu das opções tem o poder de sair do montador...
		}else if (MontadorSelecionado[1] == SIM) {
			Mmenu2();
		}
		else {
				Mmenu3();
		}
	} while (saida != SIM);
//	Destroi_Texto(ArquivoFonte);
	DestroiLog();
}

void AtualizaTelaMontador(){
	LinhaLog = Log->QntLinhas - 4 >= 0 ? Log->QntLinhas - 4 : LinhaLog;
	Desenha_Bordas_Janelas();
	Desenha_Menu1(0);
	Desenha_Codigo_Fonte();
	Desenha_Log();
	print_display(DISPLAY_WINDOW1);
}