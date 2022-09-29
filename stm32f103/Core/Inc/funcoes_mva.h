#ifndef __FUNCOES_MVA_H__
	#define __FUNCOES_MVA_H__

	// C header
	#ifdef __cplusplus  // Insert start of extern C construct
	extern "C" {
	#endif
		#include <string.h> //memset
		#include <stdint.h>
		#include <stdio.h> //snprintf
	#ifdef __cplusplus  // Insert end of extern C construct.
	}                   // The C header file can now be
	#endif              // included in either C or C++ code.


// ENCODERS DE ARAME --------------------------------------------------------------------
			
	#define CALIB_ENCODER_SICK 2.63653E-07
	/*
	PPR = 65536
	DIAMETRO = 0.0224 m
	2.63653E-07
	*/
	
	#define CALIB_ENCODER_MVS 8.59029E-06
	/*
	PPR = 2048
	DIAMETRO = 0.0224 m
	*/
	
	#define CALIB_ENCODER_CMT 5.68340E-06
	/*
	PPR 2048
	DIAMETRO = 0,01482 m
	*/
	
  /*
	*    CALCULO PARA CALIBRACAO:
	*    ( PI * DIAMETRO) / (PPR*ENC_MODE))
	*
	*			PI = 3.14159...
	*			ENC_MODE = 4 (pois o encoder está configurado para captar sinal nas curvas de subida e descida de ambos pinos)
	*
	*/

// FIM DE ENCODERS DE ARAME --------------------------------------------------------------------

/*
Densidades (p) obtidas em http://www.scielo.br/pdf/si/v19n1/a11v19n1.pdf

			p - g/cm^3 - densidade do material
			p = g/cm^3 -> kg/m^3 * 1e3


		arame - secao transversal = pi*r^2

		massa = volume * p

		x - consumo de arame [m]

		volume = pi*r^2  *  x  [m^3)

producao de arame = volume*p = (pi*r^2  *  x)*p = x * (p*pi*r^2)*1e3

		Aco Inoxidavel: p = 7,82 g/cm^3 = 7820 kg/m^3
		Bitola:
		0.8 mm - r = 0.4mm  -> producao = x*3,930760728171550E-03
		1.0 mm - r = 0.5mm  -> producao = x*6.141813637768040E-03
		1.2 mm - r = 0.6mm  -> producao = x*8.844211638385980E-03

		Aluminio: p = 2,65 g/cm^3 = 2650 kg/m^3
		Bitola:
		0.8 mm - r = 0.4mm  -> producao = x*1.332035285122070E-03
		1.0 mm - r = 0.5mm  -> producao = x*2.081305133003240E-03
		1.2 mm - r = 0.6mm  -> producao = x*2.997079391524660E-03

		Aco Carbono ER70S-6: p =  lbs/cu in 0.283  = 7833.4130301 kg/m^3 (obtido em http://www.unibraze.com/DataSheets/Data70S-6.pdf)
		Bitola:
		0.8 mm - r = 0.4mm  -> producao = x*3.93750285246348E-03
		1.0 mm - r = 0.5mm  -> producao = x*6.15234820697418E-03
		1.2 mm - r = 0.6mm  -> producao = x*8.85938141804282E-03
*/

	typedef struct {
		double veloc;        						// Velocidade do arame
		double velocMedia;
		float consumo;      						// Consumo de arame
		double prodInstantanea;    			// Producaoo instantanea de arame
		float prodGlobal;  							// Producao Global de arame
		float prodArcoAberto;  					// Producao de arame durante Arco Aberto     
		int segundofuncionando;
		int minutofuncionando;
		int horafuncionando;       
		int segundosarcoaberto;
		int minutosarcoaberto;
		int horasarcoaberto;
	}structMVA;
	
	typedef struct { 
		int materialativado;
		int bitolaativada;
		int encoderativado;
		signed char sentido;		
	}structMVA_config;
	
	typedef enum {
				SICK = 0,
				MVS,
				CMT,
				TIPO_ENCODER_MAX
	}tipo_encoder;
	
	 typedef enum {
				ACO = 0,
				ALUMINIO,
				INOX,
				MATERIAL_MAX
		}Material;
	 
		typedef enum {
				B0_8_mm = 0,
				B1_0_mm,
				B1_2_mm,
				BITOLA_MAX
		}Bitola;

class mvaclass
{
		structMVA variaveis_MVA;
		structMVA_config config_MVA;

		unsigned short div_mva;
	
		uint32_t pulse_counter;								// Contador de leitura do encoder. 32bits pois o encoder Sick usa um tamanho maior.
		uint32_t pulse_counter_anterior;      // numeros de pulsos utilizados 
		long int delta_pulsos;								// pode ser positivo ou negativo
		long int numeroDePulsosTotal;         // numero total de pulsos lidos
	  
		float posicaoInstantanea;
		float posicaoInstantaneaAnterior;			// Backup do valor do consumo de arame
		float velocidadeInstantanea;
		float velocidadeFiltrada;
	
		uint32_t tempoConsumo;								// Contador para calcular consumo de arame
	
		long int totalSegundosEquipLigado;            										// total de horas do equipamento ligado
		long int totalSegundosArcoAberto;            											// total de horas de arco aberto
		float prodInstantaneaBackUp;																// Backup da prudu??o instant?nea de arame
		float prodGlobalBack;																				// Backup produ??o global de arame 
		float prodArcoAbertoBack;																		// Backup da produ??o de arame durante Arco Aberto  
		float tipoBit[3];  																					// bitolas pr?-definidas

		char stringRetorno[15];																			// String para retornar dados em formato de texto
		char clock[15];
	
		bool flagHabilitaAlarmeMVA;											//Controle para salvar dados medidos do MVA
		
		int hour, min, seg, miliseg, controle_mili;
		int hourAA, minAA, segAA, controle_segAA;
		int controle_save_MVA;
		unsigned long int total_tempo;
		
		bool flagStartClock;


	public:	
		mvaclass();
		void setPulseCounter(uint32_t);
		void setTempoConsumo(uint32_t);
		void setNumeroDePulsosTotal(long int);
		void setNumeroDePulsos(long int);
		void setTotalSegundosEquipLigado(long int);
		void setTotalSegundosArcoAberto(long int);
		void setVeloc(float);
		void setConsumo(float);
		void setProdInstantanea(float);
		void setProdGlobal(float);	
		void setProdArcoAberto(float);
		void setDefaultConfigMVA(void);
		
		structMVA_config* getConfigMVA(void);
		unsigned short getSizoOfConfigMVA(void);
	
		int getSentido(void);
		void trocaSentido(void);
	
		uint32_t getPulseCounter(void);
		uint32_t getTempoConsumo(void);
		long int getNumeroDePulsosTotal(void);
		long int getNumeroDePulsos(void);
		float getTotalSegundosSalvar(void);
		long int getTotalSegundosArcoAberto(void);
		float getVeloc(void);
		float getVelocMedia(void);
		float getConsumo(void);
		float getProdInstantanea(void);
		float getProdGlobal(void);	
		float getProdArcoAberto(void);
		
		char* getStrVeloc(void);
		char* getStrVelocMedia(void);
		char* getStrConsumo(void);
		char* getStrProdInstantanea(void);
		char* getStrProdGlobal(void);	
		char* getStrProdArcoAberto(void);
		char* getStrtotalSegundosArcoAberto(void);
		char* getStrConstEncoder(void);
		
		void trocaMaterial(int sinal);
		char* getMaterial(void);
		void trocaBitola(int sinal);
		char* getStrBitola(void);
		void iniciaEncoder(void);
		void trocaEncoder(int sinal);
		char* getEncoder(void);
		
		void Zerar(void);
		void cal_consumo_velocidade(void);
		void Producao(void); //extern text
		void tempo_MVA(void);
		
		//envolve telas ou usart
		void Medicoes(/*BOOL habM*/); 
		void Enviar_USART3_MVA(void); //WIFI
		void Enviar_USART2_MVA(void);
		
		bool getFlagHabilitaAlarmeMVA(void);
		void setFlagHabilitaAlarmeMVA(bool);
		
		void setFlagStartClock(bool);
		void clearClock(void);
		char* getStrClock(void);
		

		

};
	
#endif
