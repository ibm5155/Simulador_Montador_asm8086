#include <MT2D/Tools/Video/MT2D_Generic_Line.h>
#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Keyboard.h>
#include <MT2D/MT2D_System_Calls.h>

#include "Mmain.h"
#include "Smain.h"
#include "Texto.h"
#include <string.h>
#include <stdlib.h>
//#include "Smain.h"

void Desenha_Ruido() {
	int i = 0;
	int j = rand() % 3;
	while (i < MAX_VER)
	{
		if (j == 3) {
			j = 0;
		}
		MT2D_Line_Draw_Vertical(i, j + 176);
		i++;
		j++;
	}
}
void Desenha_Espaco_Limpo() {
	for (int Y = 5; Y < 20; Y++) {
		for (int X = 10; X < 70; X++) {
			WINDOW1[Y][X] = ' ';
		}
	}
}


void main() {
	unsigned char tecla=0;
	do {
		MT2D_Clear_Main_Window();
		Desenha_Ruido();
		Desenha_Espaco_Limpo();
		insert_string_on_display("Simulador\\Montador por Lucas Zimerman e Caio Emidio", 5, 10, DISPLAY_WINDOW1);
		insert_string_on_display("------------------------------------------------------------", 6, 10, DISPLAY_WINDOW1);
		
		insert_string_on_display("Opcoes de Entrada:", 10, 20, DISPLAY_WINDOW1);
		insert_string_on_display("(1)MONTADOR", 11, 15, DISPLAY_WINDOW1);
		insert_string_on_display("(2)SIMULADOR", 12, 15, DISPLAY_WINDOW1);
		insert_string_on_display("(3)SAIR", 13, 15, DISPLAY_WINDOW1);

		insert_string_on_display("CONTROLES:", 15, 20, DISPLAY_WINDOW1);
		insert_string_on_display(" TAB (altera entre um objeto em foco), Setas, ENTER (Acao)", 16, 10, DISPLAY_WINDOW1);
		insert_string_on_display("P (proxima acao no simulador)", 17, 11, DISPLAY_WINDOW1);

		MT2D_Draw_Window(DISPLAY_WINDOW1);
		if (MT2D_Keyboard_touched()) {
			do {
				tecla = MT2D_Keyboard_keytouched();
			} while (tecla < '1' && tecla > '3');
			if (tecla == '1') {
				Mmain();
			}
			else if (tecla == '2') {
				Smain();
			}
		}
		else {
			MT2D_System_Delay(30);
		}
	} while (tecla != '3');
}