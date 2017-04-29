#ifndef _DUMP_SIM_H
#define _DUMP_SIM_H

/**
Mostra a simula��o graficalmente
Origem: Mostra de onde est� no nivel de hardware a simula��o
Destino: Mostra onde est� indo no nivel de hardware a simula��o
Origem/Destino s�o:
0: Memoria, 1: BIO, 2: Queue, 3: Control Timer, 4: IP, 5: Inc IP, 6: Registrador, 7: ULA
Registrador: Qual registrador vai receber/enviar o dado:
0 : DX,DL,DH  1: CX,CL,CJ 2: BX,BH,BL 3: AX,BH,BL
Tx: Qual entrade da Ula vai receber os dado
1: T1, 2: T2, 3: Instru��o aplicada na ula
Tx tamb�m pode assumir o valor de um registrador caso esteja movendo um registrador para registrador,
neste caso o valor de Tx ser� equivalente ao de Registrador

String1 e 2 s�o informa��es que voc� deseja informar na simula��o
*/
void Dump_Simulacao(int Origem, int Destino, int Registrador, int Tx, char *String1, char *String2);
int Dump_Simulacao_registrador_indice(char *Nome_Registrador);
#endif