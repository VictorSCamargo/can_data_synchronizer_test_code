#include "funcoes_usb.h"

DadosUsb::DadosUsb(void)
{
	statusUsb = APPLICATION_IDLE;
	memSelecionada = 0;
	numArquivoMem = 0;
}

ApplicationTypeDef DadosUsb::getStatus(void)
{
	return statusUsb;
}

uint8_t DadosUsb::getMemSelecionada(void)
{
	return memSelecionada;
}

//Get file number - string
char* DadosUsb::getNumArquivoStr(void){
	sprintf(str_num_arquivo_mem, "%d", numArquivoMem);	
	return str_num_arquivo_mem;
}

//Get file number - int
uint8_t DadosUsb::getNumArquivoInt(void){ 
	return numArquivoMem;
}
