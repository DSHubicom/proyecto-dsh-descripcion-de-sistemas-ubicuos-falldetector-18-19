/** 
 _____     _ _ ____       _            _             
|  ___|_ _| | |  _ \  ___| |_ ___  ___| |_ ___  _ __ 
| |_ / _` | | | | | |/ _ \ __/ _ \/ __| __/ _ \| '__|
|  _| (_| | | | |_| |  __/ ||  __/ (__| || (_) | |   
|_|  \__,_|_|_|____/ \___|\__\___|\___|\__\___/|_|   
                                                     
------------------------------------------------------

Sistema Inteligente de Detección y Aviso ante caídas

------------------------------------------------------
	Desarrollado por:
	Grupo 2:
		- Sergio Laso
		- José Manuel Romero Muelas
		- Sergio Zabala Mesonero
	Asignatura: Descripción de Sistemas Hardware
	Máster en Ingeniería Informática - Curso 2018/19
------------------------------------------------------ **/

/** Implementación de la Clase 'FallDetector' **/

#include "FallDetector.h"


/** Constructor de la clase**/
FallDetector::FallDetector():mpu(0x68){
	ax = 0;
	ay = 0;
	az = 0;
	gx = 0;
	gy = 0;
	gz = 0;
	AC = 0;
	GC = 0;
	start_millis = 0;
	caida = false;

	nomUsuario="Sergio Zabala";
	tlfContacto="+34669100117";
	pinSIM="1010";
	fona = Adafruit_FONA(4);
	fonaSS = SoftwareSerial(3, 2);
	SoftwareSerial *fonaSerial = &fonaSS;
	//*fonaSerial = &fonaSS;
}
/** Función para configurar los parámetros propios del usuario: Nombre y Apellidos, y 
	teléfono de contacto al que se enviará el SMS **/
void FallDetector::introducirInfoContacto(char* nombre, char* tlf){
	nomUsuario = nombre;
	tlfContacto = tlf;
}

/** Inicialización del entorno necesario para el funcionamiento del
	sistema completo **/
void FallDetector::configuracionInicial(){
	Serial.begin(115200);
	Wire.begin();
	
	initFONA();
	mpu.initialize();/*Inicializar Acel + Girosc*/
	Serial.println(mpu.testConnection() ? F("IMU iniciado correctamente") : F("Error al iniciar IMU"));
	pinMode(boton,INPUT);/*Estado del Botón: LOW-Levantado, HIGH- Pulsado*/
	introducirInfoContacto("Sergio Zabala Mesonero", "+34606004376");
	configSIM();
}
/** Función para la lectura de los valores arrojados por el acelerómetro
	y el giroscopio **/
void FallDetector::leerValores(){
	mpu.getAcceleration(&ax, &ay, &az);
	mpu.getRotation(&gx, &gy, &gz);
	AC=abs(ax * accScale)+abs(ay*accScale)+abs(az*accScale);
	GC=abs(gx * accScale)+abs(gy*accScale)+abs(gz*accScale);
}
/** Imprimir valores absolutos de las mediciones **/
void FallDetector::mostrarValores(){
	Serial.print(F("AC: "));
	Serial.println(AC);
	Serial.print(F("GC: "));
	Serial.println(GC);
	Serial.println(F("---------------------"));
}
/** Comprobación de los valores para evaluar si se ha cumplido la 
	condición de caída **/
void FallDetector::comprobarValores(){
		if ( AC>= UMBRAL_AC && GC >= UMBRAL_GC && !caida ){
		Serial.println("AVISO: Detectada condición de caída");
		caida = true;
		start_millis = millis();
	}	  
}
/** Función para la gestión de las distintas tareas que se realizan 
	una vez ha sucedido el evento de caída:
		-Si han pasado menos de 10 segundos desde el inicio de la posible caída,
		 entonces el zumbador sonará.
		-Si se ha superado ese intrevalo de aviso al usuario sin recibir respuesta 
		 (pulsación del botón), entonces se activará el procedimiento de alarma
		 	· Se envía SMS
		 	· Se indica localización del afectado **/
void FallDetector::tareasCaidaActiva(){
	if (caida){
	if ( (millis() -start_millis) < 10000 ){ /*Se avisa al usuario durante 10 segundos a la espera de respuesta*/
    	tone(buzzer,1000); // Send 1KHz sound signal... 
    	Serial.println("---------------------> El buzzer está sonando");
		}
		else { /*Si el usuario no ha cancelado el aviso, se activan las tareas de alerta.*/
    		Serial.println("#####Enviando Mensaje....");
    		enviarSMS();
    		tone(buzzer,1000,1); //TODO Funcion apagar avisador
    		caida = false;
    		delay(5000);
    	}
	}		
		
}
/**  Procedimiento encargado de comprobar si el botón ha sido
	 o no pulsado, para cancelar el procedimiento de alerta ante caídas. **/
void FallDetector::accionesBoton(){
	if ( digitalRead(boton) == HIGH ){
    	Serial.println("############### Boton Pulsado ##############");
    	if (caida) {
    		caida = false;
    		tone(buzzer,1000,1);
    	} //Dejar de emitir sonido cuando se pulsa el botón 
	}		
}
/** AMPLIACIÓN de la Librería 'FallDetector' para utilizar el modulo SIM **/

/** Inicializar modulo 'AdaFruit FONA SIM800L' **/
void FallDetector::initFONA(){
	fonaSerial->begin(4800);
	if (! fona.begin(*fonaSerial)) {
		Serial.println(F("Couldn't find FONA"));
		while (1);
	}
	Serial.println(F("FONA is OK"));

	 // Print module IMEI number.
  	char imei[16] = {0}; // MUST use a 16 character buffer for IMEI!
  	uint8_t imeiLen = fona.getIMEI(imei);
  	if (imeiLen > 0) {
    	Serial.print("Module IMEI: "); Serial.println(imei);
    }
}


/**Configuracion de la tarjeta SIM: Desbloqueo + Activacion GPRS **/
void FallDetector::configSIM(){
	//Desbloquear tarjeta con el PIN
	Serial.println(F("---------> Desbloqueando SIM  <------------"));

	
	if (! fona.unlockSIM(pinSIM)) {
          Serial.println(F("---------> ERROR: Fallo al desbloquear SIM  <------------"));
    } else {
        Serial.println(F("----------> SIM Desbloqueda <------------"));
          //Activar GPRS
    	if (!fona.enableGPRS(true))
        	Serial.println(F("---------> ERROR: Fallo al activar GPRS  <------------"));
        else
        	Serial.println(F("---------> OK: Activado GPRS  <------------"));

    }
   
    
}


/** Procedimiento para el envío de un SMS en caso de caída. Este contendrá el nombre del usuario,
	así como las coordenadas donde se produjo dicha caída */

void FallDetector::enviarSMS(){
	char message[141]="";
	//Elaborando el SMS a enviar con la estructura:
	// {Nombre Apellidos del usuario} ha sufrido una caída en la localización:
	// {Latitud, Longitud}
	// Los servicios de emergencia han sido avisados.
	strcat(message,nomUsuario);
	strcat(message," ha sufrido una caída en la localización: \n");
	strcat(message,obtenerLocalizacion());
	strcat(message, "\nLos servicios de emergencia han sido avisados.");
	 
	if (!fona.sendSMS(tlfContacto, message)) {
	  Serial.println(F("Envio Fallido de SMS"));
	} else {
	  Serial.println(F("------------------> SMS enviado con éxito <--------------------------"));
	}
}

char FallDetector::obtenerLocalizacion(){
	uint16_t returncode;
	char replybuffer [255];
	
    if (!fona.getGSMLoc(&returncode, replybuffer, 250))
          Serial.println(F("---------> ERROR: Fallo al obtener Localización  <------------"));
    if (returncode == 0) {
          Serial.println(replybuffer);
    } 
    return replybuffer;
}




/** Procedimiento principal del sistema detector de caídas 'FallDetector'.
	Realiza todas las funciones (leer mediciones, comprobar si exceden el umbral etc)
	con un intervalo de parada de 300ms. **/

void FallDetector::ejecucion(){
	leerValores();
	mostrarValores();
	comprobarValores();
	tareasCaidaActiva();
	accionesBoton();
	delay(300);
}