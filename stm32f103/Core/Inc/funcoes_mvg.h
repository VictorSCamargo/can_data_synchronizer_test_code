#ifndef __FUNCOES_MVG_H__
	#define __FUNCOES_MVG_H__
	
	// C header
	#ifdef __cplusplus  // Insert start of extern C construct
	extern "C" {
	#endif

	#include <string.h> //strcpy
	#include <stdint.h>
	#include <stdio.h> //snprintf		

	#ifdef __cplusplus  // Insert end of extern C construct.
	}                   // The C header file can now be
	#endif              // included in either C or C++ code.
	
	/*-----Defines-----*/
	#define INC_VAZAO 0.1		// Valor de incremento para mudar vasão
	#define VAZAO_MIN 0.0   // Valor minimo limite de vazão estipulada
	#define VAZAO_MAX 21.0  // Valor maximo limite de vazão estipulada
	/*-----------------*/
	
	
	typedef struct{        // Define uma estrutura para guardar os valores nome, minimo, maximo, incremento e fatores
		char gas[10];        // Nome do gas
		float vazaoMinAju;   // Valor vazão minimo ajustado
		float vazaoMaxAju;   // Valor vazão maximo ajustado
		float fatores;       // Fatores de conversao da vazao de acordo com cada gas
		float fatores2;      // Fatores de conversao da vazao de acordo com cada gas
	}structGas;
	
	//Medidas
	typedef struct {
		float valorLido;				// Valor lido instantaneo
		float acumuladaTotal;		//Vazão acumulada total
		float acumulada;				//Vazão acumulada atual
	}structMVG;
	
	//Configurações
	typedef struct {
		float vazaoMaxima[4]; 
		float vazaoMinima[4];		
		int gasselecionado; //Nome de gás selecionado
	}structMVG_config;
	
class mvgclass
{
	structMVG variaveis_mvg;
	structMVG_config config_MVG;
	int gasAtivado;            																	// Indice do gas ativado

	uint32_t tempoVazao;
	int indiceGas;          																			// Indice do gas selecionado
	
	float vazaoMinimaSalvar;																			// Valor da vazão mínima para salver
	float vazaoMaximaSalvar;																			// Valor da vazão máxima para salver
	
	bool flagHabilitaAlarmeMVG;											//Controle para salvar dados medidos do MVG

	char stringRetorno[10];						// String para retornar dados em formato de texto
	
	public:	
		mvgclass();
	
		void setVazaoMax(float);
		void setVazaoMin(float);
		void setAcumulo(float);
		void setAcumTotal(float);
		void trocaGas(int);
		void setDefaultConfigMVG(void);
		
		structMVG_config* getConfigMVG(void);
		unsigned short getSizoOfConfigMVG(void);
		
		float getVazaoMax(void);
		float getVazaoMin(void);
		float getAcumulo(void);
		float getAcumTotal(void);
		int getGas(void);
		float getValorLido(void);
		
		char* getStrValorLido(void);
		char* getStrAcumulo(void);
		char* getStrAcumTotal(void);
		char* getStrVazaoMax(void);
		char* getStrVazaoMin(void);
		char* getStrGas(void);
		
		bool incVazaoMax(void);
		bool incVazaoMin(void);
		bool decVazaoMax(void);
		bool decVazaoMin(void);
		
		void Zerar(void);

		void inc_tempoVazao(void);
		void clear_tempoVazao(void);

		void Config(void);
		void altera_valor(int sinal);
		void vazao(void);
		void Enviar_USART3_WIFI_MVG(void);
		void Enviar_USART2_MVG(void);
		void vazao_acumulada(void);
		void converteAD(void);
	
		char* get_tipoGasAtivado( void);
		
		int alarme(void);
		
		bool getFlagHabilitaAlarmeMVG(void);
		void setFlagHabilitaAlarmeMVG(bool);


};
	
#endif
