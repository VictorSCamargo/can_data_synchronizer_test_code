#ifndef __FUNCOES_MTC_H__
	#define __FUNCOES_MTC_H__

	// C header
	#ifdef __cplusplus  // Insert start of extern C construct
	extern "C" {
	#endif
//	#include <math.h> //pow e sqrt
//	#include <string.h> //memset
	#include <stdint.h>
	#include <stdio.h> //snprintf
	#ifdef __cplusplus  // Insert end of extern C construct.
	}                   // The C header file can now be
	#endif              // included in either C or C++ code.
	
	typedef struct { 
		double vEficaz;
		double iEficaz;
		float mediaV;
		float mediaI;
	}structMTC;
	
	
	typedef struct {
		float tensaoMaxima; 
		float tensaoMinima;
		float correnteMaxima; 
		float correnteMinima;
	}structMTC_config;
	
	
	class mtcclass
	{
		structMTC variaveis_MTC;
		structMTC_config config_MTC;
				
		char stringRetorno[10];																			// String para retornar dados em formato de texto
		bool flagHabilitaAlarmeMTC;											//Controle para salvar dados medidos do MTC

	public:
		mtcclass();

		void setEficazI(float);
		void setMediaI(float);
		void setEficazV(float);
		void setMediaV(float);
		void setDefaultConfigMTC(void);

		structMTC* getVariaveisMTC(void);
		int getSizoOfVariaveisMTC(void);

		structMTC_config* getConfigMTC(void);
		unsigned short getSizoOfConfigMTC(void);

		float getEficazI(void);
		float getMediaI(void);
		float getEficazV(void);
		float getMediaV(void);

		float getCorrenteMax(void);
		float getCorrenteMin(void);
		float getTensaoMax(void);
		float getTensaoMin(void);

		char* getStrEficazI(void);
		char* getStrMediaI(void);
		char* getStrEficazV(void);
		char* getStrMediaV(void);

		char* getStrCorrenteMax(void);
		char* getStrCorrenteMin(void);
		char* getStrTensaoMax(void);
		char* getStrTensaoMin(void);

		char* getStrPinoentradaC(void);
		char* getStrPinoentradaT(void);

		bool incTensaoMax(void);
		bool incTensaoMin(void);
		bool decTensaoMax(void);
		bool decTensaoMin(void);
		bool incCorrenteMax(void);
		bool incCorrenteMin(void);
		bool decCorrenteMax(void);
		bool decCorrenteMin(void);

		int alarme_T(void);
		int alarme_I(void);

		bool getFlagHabilitaAlarmeMTC(void);
		void setFlagHabilitaAlarmeMTC(bool);
		
	};
	
#endif
