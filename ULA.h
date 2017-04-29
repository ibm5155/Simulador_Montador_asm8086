#ifndef _ULA_H
#define ULA

class ULA {
private: 
	int entrada1, entrada2,saida;
public:
	int Out() {
		return saida;
	}
	void MOV();
	void MOV();
	void PUSH();
	void POP();
	void ADD();
	void SUB();
	void INC();
	void DEC();
	void MUL();
	void DIV();
	void NOT();
	void AND();
	void OR();
	void XOR();
	void CMP();
	void CALL();
	void JMP();
	void RET();
	void JE();
	void JZ();
	void JL();
	void JNGE();
	void JA();
	void JNBE();
	void JB();
	void JNA();
	void JG();
	void ORG();
};

#endif