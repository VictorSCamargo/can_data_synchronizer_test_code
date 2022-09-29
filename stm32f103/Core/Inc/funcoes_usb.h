#ifndef __FUNCOES_USB_H
#define __FUNCOES_USB_H

#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
 extern "C" {
#endif

	typedef enum {
		APPLICATION_IDLE = 0,
		APPLICATION_START,
		APPLICATION_READY,
		APPLICATION_DISCONNECT
	}ApplicationTypeDef;
	
	class DadosUsb{
		
		ApplicationTypeDef statusUsb;
		uint8_t memSelecionada;				//SD, USB ou nenhuma.
		
		uint8_t numArquivoMem;
		char str_num_arquivo_mem[3];
		
		uint8_t flagLimparMemoria;
		
		public:
			DadosUsb();
		
			ApplicationTypeDef getStatus(void);
			uint8_t getMemSelecionada(void);
		
			char* getNumArquivoStr(void);
			uint8_t getNumArquivoInt(void);
			
			uint8_t getFlagLimpouMemoria(void);
			void setFlagLimparMemoria(uint8_t);
	};

#ifdef __cplusplus
}
#endif

#endif
