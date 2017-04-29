#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Texto.h"
#include <MT2D/MT2D.h>
#include <MT2D/InputBox/MT2D_InputBox_String.h>

extern Texto *Log = 0;
int  LinhaLog = 0;
int ColunaLog = 0;

void AdicionaLog(char *frase) {
	AdicionaLinha(Log, frase);
}

void CriaLog() {
	Log = (Texto*)malloc(sizeof(Texto));
	Log->Fonte = 0;
	Log->Linhas = 0;
	Log->QntLinhas = 0;
	Log->Salvo = SIM;
}
void DestroiLog() {
	Destroi_Texto(Log);
	CriaLog();
}
void SalvaLog() {
	char *endereco = MT2D_InputBox_String("Digite endereco e o nome do arquivo (ou somente o nome dele caso queira salvar na pasta do arquivo...)");
	FILE *f = fopen(endereco, "a");
	if (!f) {
		AdicionaLog("ERRO: Arquivo de log nao foi criado...");
	}
	else {
		fprintf(f, "=============== INICIO ==============\n");
		for (int i = 0; i < Log->QntLinhas; i++) {
			fprintf(f, "%s\n", Log->Linhas[i].Caracteres);
		}
		fprintf(f, "================ FIM ================\n");
		fclose(f);
		AdicionaLog("Arquivo de log foi criado");

	}
}

