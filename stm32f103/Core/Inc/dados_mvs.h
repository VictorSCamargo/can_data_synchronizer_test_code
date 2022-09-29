#ifndef __DADOS_MVS_H
#define __DADOS_MVS_H

#include "funcoes_mva.h"
#include "funcoes_mvg.h"
#include "funcoes_mtc.h"
#include "funcoes_usb.h"	

//	#include "wifi.h"	
//	#include "rtc.h"
//  eeprom

#ifdef __cplusplus
 extern "C" {
#endif
		
	class DadosMvs
	{
		 
	public:
		DadosMvs();
	
		mtcclass mtc;
		mvgclass mvg;
		mvaclass mva;
		DadosUsb usb;

	};
	 
	 
#ifdef __cplusplus
}
#endif

#endif
