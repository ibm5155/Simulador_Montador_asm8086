#include "Texto.h"
#include "../../MT2D/Window_core.h"
#include "../../MT2D/Keyboard.h"
#include "CPU.h"
#include <stdlib.h>

extern unsigned char Memoria[65535];// 1 byte por char -> 64KB ram

void Dump_Memoria() {
	int pnt = i8086.IP->Registradores[0].Dado;
	int Y = 2;
	int Saida = false;
	char *Booleano;
	do {
		clear_display();
		create_window_layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
		insert_string_on_display("Memoria", 0, 10, DISPLAY_WINDOW1);
		for (Y = 0; Y < MAX_VER -2;Y++) {//imprime 24 informações a partir de pnt
			insert_number_on_display(pnt+Y, Y+1, 2, DISPLAY_WINDOW1);
			insert_number_on_display(Memoria[pnt+Y], Y+1, 10, DISPLAY_WINDOW1);
			Booleano = InteiroParaCharBinario(Memoria[pnt + Y], _8BITS);
			insert_string_on_display(Booleano, Y+1, 20, DISPLAY_WINDOW1);
			free(Booleano);
			if (pnt+Y == i8086.IP->Registradores[0].Dado) {
				insert_string_on_display("<---", Y+1, 29, DISPLAY_WINDOW1);
			}
		}
		print_display(DISPLAY_WINDOW1);
		Saida = Keyboard_keytouched();
		if (Saida == tab) {//ta hora de dizer tchau
		}
		else  if (Saida == _key) {
			Saida = Keyboard_keytouched();
			if (Saida == key_up && pnt > 0) {
				pnt--;
			}
			else if (Saida == key_down && pnt < 65510) {
				pnt++;
			}
			Saida = false;
		}
		else {
			Saida = false;
		}

	} while (!Saida);
}