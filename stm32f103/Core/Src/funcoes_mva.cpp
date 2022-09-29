#include "funcoes_mva.h"	

char* tipoMat[] = { "Aco ER70S-6", "Aluminio", "Aco Inox" };	// Materiais pre-definidos
char* tipoEnc[] = { "Sick", "MVS", "CMT" };	// Encoders pre-definidos

float dens_linear_float[3][3] = {     																																																														// Densidades lineares de cada material, com cada bitola; 
  {3.93750285246348e-03, 6.15234820697418e-03, 8.85938141804282e-03},    																									// Aco Carbono ER70S-6
  {1.332035285122070e-03, 2.081305133003240e-03, 2.997079391524660e-03},     																						// Aluminio
  {3.930760728171550e-03, 6.141813637768040e-03, 8.844211638385980e-03}      																						// Aco Inoxidavel
};

//Densidade linear multiplicada por 10000000
int dens_linear_int[3][3] = {     																																																														// Densidades lineares de cada material, com cada bitola; 
  {39375, 61523, 88593}, 		// Aco Carbono ER70S-6
  {13320, 20813, 29970},     	// Aluminio
  {39307, 61418, 88442}      	// Aco Inoxidavel
};

float constantes_encoder[3] = {
	CALIB_ENCODER_SICK, //sick
	CALIB_ENCODER_MVS, //MVS
	CALIB_ENCODER_CMT //CMT
};

bool pausa_para_troca = false;

mvaclass::mvaclass( void){ 
	
		div_mva = 0;
	
		numeroDePulsosTotal = 0;
		pulse_counter = 0;
		pulse_counter_anterior = pulse_counter; 	
		controle_save_MVA = 0;
		
		posicaoInstantanea = 0.0;
		posicaoInstantaneaAnterior = 0.0;
	
		variaveis_MVA.veloc = 1.0;
		variaveis_MVA.velocMedia = 1.0;
		variaveis_MVA.consumo = 1.0;
	
		velocidadeInstantanea = 0.0;
		velocidadeFiltrada = 0.0;
	
		tempoConsumo = 0;
	
		totalSegundosEquipLigado = 0.0;
		totalSegundosArcoAberto = 0.0; 
		variaveis_MVA.prodInstantanea = 0.0; 
		prodInstantaneaBackUp = 0.0;
		variaveis_MVA.prodGlobal = 0.0; 
		prodGlobalBack = 0.0;
		variaveis_MVA.prodArcoAberto = 0.0; 
		prodArcoAbertoBack = 0.0;
		
		tipoBit[0] = 0.8; tipoBit[1] = 1.0; tipoBit[2] = 1.2; //Bitola

		flagHabilitaAlarmeMVA = false;
		hour = 0; 
		min = 0;
		seg = 0;
		miliseg = 0;
		hourAA = 0;
		minAA = 0;
		segAA = 0;
		total_tempo = 0;
		flagStartClock = false;
}

void mvaclass::setPulseCounter(uint32_t pulse){ pulse_counter = pulse;}
void mvaclass::setTempoConsumo(uint32_t tempo){ tempoConsumo = tempo;}
void mvaclass::setNumeroDePulsosTotal(long int num){numeroDePulsosTotal = num;}
void mvaclass::setNumeroDePulsos(long int num){pulse_counter_anterior = num;}
void mvaclass::setTotalSegundosEquipLigado(long int totalHoras){totalSegundosEquipLigado = totalHoras;}
void mvaclass::setTotalSegundosArcoAberto(long int totalHoras){totalSegundosArcoAberto = totalHoras;}
void mvaclass::setVeloc(float vel){variaveis_MVA.veloc = vel;}
void mvaclass::setConsumo(float cons){variaveis_MVA.consumo = cons;}
void mvaclass::setProdInstantanea(float prodI){variaveis_MVA.prodInstantanea = prodI;}
void mvaclass::setProdGlobal(float prodG){variaveis_MVA.prodGlobal = prodG;}	
void mvaclass::setProdArcoAberto(float prodA){variaveis_MVA.prodArcoAberto = prodA;}

void mvaclass::setDefaultConfigMVA(void)
{
	config_MVA.materialativado = 0;
	config_MVA.bitolaativada = 0;
	config_MVA.encoderativado = 1;
	config_MVA.sentido = 1;	
}

structMVA_config* mvaclass::getConfigMVA(void){return &config_MVA;}
unsigned short mvaclass::getSizoOfConfigMVA(void){return sizeof(config_MVA);}

uint32_t mvaclass::getPulseCounter(void){return pulse_counter;}
uint32_t mvaclass::getTempoConsumo(void){return tempoConsumo;}
long int mvaclass::getNumeroDePulsosTotal(void){return numeroDePulsosTotal;}
long int mvaclass::getNumeroDePulsos(void){return pulse_counter_anterior;}
float mvaclass::getTotalSegundosSalvar(void){return total_tempo/10000.0;} //o .0 é importante para que a divisao retorne número com decimal (float)
long int mvaclass::getTotalSegundosArcoAberto(void){return totalSegundosArcoAberto;}
float mvaclass::getVeloc(void){return variaveis_MVA.veloc;}
float mvaclass::getVelocMedia(void){return variaveis_MVA.velocMedia;}
float mvaclass::getConsumo(void){return variaveis_MVA.consumo;}
float mvaclass::getProdInstantanea(void){return variaveis_MVA.prodInstantanea;}
float mvaclass::getProdGlobal(void){return variaveis_MVA.prodGlobal;}	
float mvaclass::getProdArcoAberto(void){return variaveis_MVA.prodArcoAberto;}

bool mvaclass::getFlagHabilitaAlarmeMVA(void){ return flagHabilitaAlarmeMVA;}
void mvaclass::setFlagHabilitaAlarmeMVA(bool flag){flagHabilitaAlarmeMVA = flag; }

int mvaclass::getSentido(void){return config_MVA.sentido;}
void mvaclass::trocaSentido(void)
{
	if(mvaclass::getSentido() == 1){
		config_MVA.sentido = -1;
	}else{
		config_MVA.sentido = 1;
	}
}

char* mvaclass::getStrVeloc(void){
	sprintf(stringRetorno, "%.1f", variaveis_MVA.veloc);
	return stringRetorno;
}
char* mvaclass::getStrVelocMedia(void){
	sprintf(stringRetorno, "%.1f", variaveis_MVA.velocMedia);
	return stringRetorno;
}
char* mvaclass::getStrConsumo(){	
	sprintf(stringRetorno, "%.1f", variaveis_MVA.consumo);
	return stringRetorno;
}
char* mvaclass::getStrProdInstantanea(void){	
	sprintf(stringRetorno, "%.2f ", variaveis_MVA.prodInstantanea);
	return stringRetorno;
}
char* mvaclass::getStrProdGlobal(void){	
	sprintf(stringRetorno, "%.2f", variaveis_MVA.prodGlobal);
	return stringRetorno;
}	
char* mvaclass::getStrProdArcoAberto(void){	
	sprintf(stringRetorno, "%.2f", variaveis_MVA.prodArcoAberto);
	return stringRetorno;
}
char* mvaclass::getStrtotalSegundosArcoAberto(void){	
	sprintf(stringRetorno, "%02d:%02d:%02d", hourAA, minAA, int(segAA/1000));
	return stringRetorno;
}
char* mvaclass::getStrConstEncoder(void){
	sprintf(stringRetorno, "%.11f", constantes_encoder[config_MVA.encoderativado]);
	return stringRetorno;
}


void mvaclass::trocaMaterial(int sinal){
	if(sinal == 1){
		if(++config_MVA.materialativado >= MATERIAL_MAX){
			config_MVA.materialativado = 0;
		}
	}else
	if(sinal == -1){
		if(--config_MVA.materialativado < 0){
			config_MVA.materialativado = MATERIAL_MAX - 1;
		}
	}
}

char* mvaclass::getMaterial(void){
	if(config_MVA.materialativado >= ACO){
		if(config_MVA.materialativado < MATERIAL_MAX)
			return tipoMat[config_MVA.materialativado];
	}else{
			config_MVA.materialativado = 0;
	}
	return tipoMat[config_MVA.materialativado];
}

void mvaclass::trocaBitola(int sinal){
	if(sinal == 1 && config_MVA.bitolaativada < 2)
		config_MVA.bitolaativada++;
	else if(sinal == 1 && config_MVA.bitolaativada == 2){
		config_MVA.bitolaativada = 0;
	}
	if(sinal == -1 && config_MVA.bitolaativada > 0)
		config_MVA.bitolaativada--;
	else if(sinal == -1 && config_MVA.bitolaativada == 0){
		config_MVA.bitolaativada = 2;
	}
}

char* mvaclass::getStrBitola(void){
	if(config_MVA.bitolaativada >= B0_8_mm){
		if(config_MVA.bitolaativada < BITOLA_MAX)
			sprintf(stringRetorno, "%.1f", tipoBit[config_MVA.bitolaativada]);
	}else{
			config_MVA.bitolaativada = 0;
			sprintf(stringRetorno, "%.1f", tipoBit[config_MVA.bitolaativada]);
	}
	return stringRetorno;
}

void mvaclass::trocaEncoder(int sinal){
	if(sinal == 1){
		if(++config_MVA.encoderativado >= TIPO_ENCODER_MAX){
			config_MVA.encoderativado = 0;
		}
	}else
	if(sinal == -1){
		if(--config_MVA.encoderativado < 0){
			config_MVA.encoderativado = TIPO_ENCODER_MAX - 1;
		}
	}
}


char* mvaclass::getEncoder(void){
	if(config_MVA.encoderativado >= 0){
		if(config_MVA.encoderativado < TIPO_ENCODER_MAX)
			return tipoEnc[config_MVA.encoderativado];
	}else{
			config_MVA.encoderativado = 0;
	}
	return tipoEnc[config_MVA.encoderativado];
}

void mvaclass::Zerar(void) {
	posicaoInstantanea = 0.0f;
	posicaoInstantaneaAnterior = 0.0f;
	variaveis_MVA.consumo = 0.0f;
	variaveis_MVA.prodArcoAberto = 0.0f;
	numeroDePulsosTotal = 0;
	totalSegundosEquipLigado = 0.0;
	totalSegundosArcoAberto = 0.0;  														
	segAA = 0;
	minAA = 0;
	hourAA = 0;    
	seg = 0;
	min = 0;
	hour = 0;   
	total_tempo = 0;
	miliseg = 0;
}

void mvaclass::setFlagStartClock(bool flag){
	flagStartClock = flag;
}

void mvaclass::clearClock(void){
	min = 0;
	hour = 0;
	seg = 0;
	miliseg = 0;
	controle_mili = 0;
	total_tempo = 0;
}

char* mvaclass::getStrClock(void){
	sprintf(clock, "%02d:%02d:%02d ", hour, min, seg);
	return clock;
}

void mvaclass::tempo_MVA(void){
	
	//	Tempo de salvamento
	
	if(flagStartClock == true){
		
		total_tempo++;
		controle_mili++;
		
		if(controle_mili >= 10){
			controle_mili = 0;
			miliseg++;
		}	
    if(miliseg >= 1000) {							
      miliseg = 0;      							
      seg++;										
    }
		if(seg >= 60){									
      seg = 0;										
      min++;										
    }
    if(min >= 60){									
      min = 0;										
      hour++;										
    }
	}

	//	Tempo de arco aberto
	
  if ( (variaveis_MVA.veloc >= 0.1f) || (variaveis_MVA.veloc <= -0.1f) ){
    
		controle_segAA++;
		
		if(controle_segAA >= 10000){
			segAA++;
			controle_segAA = 0;
		}
		if(segAA >= 60) {							
			segAA = 0;      							
			minAA++;								
    }
		if(minAA >= 60){
			minAA = 0;
			hourAA++;
		}
  }
}
