#include "Texto.h"
#include <MT2D/MT2D.h>
#include <MT2D/MT2D_Display.h>
#include <MT2D/MT2D_Layout.h>
#include <MT2D/MT2D_Keyboard.h>
#include "CPU.h"
#include "RAM.h"
#include <stdlib.h>
#include <string.h>


void Dump_Cache() {
	char BUFFER[200];
	int i = 0;
	int Y = 2;
	int Saida = false;
	float Resultado;
	char *Booleano;
	do {
		MT2D_Clear_Main_Window();
		MT2D_Create_Window_Layout(0, 0, MAX_HOR - 1, MAX_VER - 1, false, false, 0, ' ', DISPLAY_WINDOW1);
		insert_string_on_display("Dump Cache", 0, 10, DISPLAY_WINDOW1);
		MT2D_Create_Window_Layout(8, 1, 50, 19, true, true, false, ' ', DISPLAY_WINDOW1);
		insert_string_on_display("ID    ID MEMORIA         DADO BINARIO", 2, 11, DISPLAY_WINDOW1);
		for (i = 0; i < 16; i++)
		{
			insert_number_on_display(i + 1, 4 + i, 11, DISPLAY_WINDOW1);
			insert_string_on_display(InteiroParaCharBinario((int)i8086.cache.EndMemoria[i], _16BITS), i + 4, 15, DISPLAY_WINDOW1);
			insert_string_on_display(InteiroParaCharBinario((int)i8086.cache.Memoria[i], _8BITS), i + 4, 36, DISPLAY_WINDOW1);

		}
		if (i8086.cache.Hit + i8086.cache.Miss == 0)
		{
			insert_string_on_display("Nenhum hit ou miss encontrados", 20, 10, DISPLAY_WINDOW1);
		}
		else
		{
			Resultado = ((i8086.cache.Hit) / float(i8086.cache.Hit + i8086.cache.Miss))*100;
			sprintf(BUFFER, "Total Hits: %d  %d%%", i8086.cache.Hit,(int)Resultado);
			Resultado = ((i8086.cache.Miss) / float(i8086.cache.Hit + i8086.cache.Miss)) * 100;
			insert_string_on_display(BUFFER, 22, 10, DISPLAY_WINDOW1);
			sprintf(BUFFER, "Total Miss: %d  %d%%", i8086.cache.Miss, (int)Resultado);
			insert_string_on_display(BUFFER, 23, 10, DISPLAY_WINDOW1);

		}

		MT2D_Draw_Window(DISPLAY_WINDOW1);

		Saida = MT2D_Keyboard_keytouched();
		if (Saida == tab_pressed) {//ta hora de dizer tchau
			Saida = true;
		}
		else
		{
			Saida = false;
		}

	} while (!Saida);
}