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

/** Definición de la Clase 'FallDetector' **/

#include "Arduino.h"
#ifndef FallDetector_h
#define FallDetector_h

/** Dependencias del Proyecto **/
#include <Adafruit_FONA.h>
#include <Wire.h>
#include <helper_3dmath.h>
#include <SoftwareSerial.h>
#include <MPU6050.h>
#include <I2Cdev.h>

/** Definición de los Umbrales de aceleración y ángulo de giro **/
#define UMBRAL_AC 18
#define UMBRAL_GC 25

class FallDetector
{
public:
	FallDetector();
	void introducirInfoContacto(char *nombre, char *tlf);
	void configuracionInicial();
	void leerValores();
	void mostrarValores();
	void comprobarValores();
	void tareasCaidaActiva();
	void accionesBoton();
	void ejecucion();
	/** Ampliación - Funciones del Módulo SIM **/
	void initFONA();
	void configSIM();
	void enviarSMS();
	char obtenerLocalizacion();

private:
	/** Definición de las variables utilizadas
	    en el sistema **/
	int ax, ay, az; // Lecturas acelerómetro
	int gx, gy, gz; //Lecturas giroscopio
	float AC,GC; //Valores absolutos de las medidas acelerómetro/giroscopio
	//Valores de las escalas aplicadas a las medidas aceler/girosc 
	const float accScale = (2.0 * 9.81 / 32768.0), gyroScale = (250.0 / 32768.0);
	const int buzzer = 9, boton = 5; //Pines digitales para el zumbador y el pulsador
	unsigned long int start_millis; //Milisegundos de inicio del evento 'caída'
	bool caida; //Bandera que controla si el evento está activo (true) o no (false)
	/* Variables del dispositivo MPU-6050 (Acelerómetro + Giróscopo) */
	const int mpuAddress = 0x68;  // Puede ser 0x68 o 0x69
	MPU6050 mpu; 
	int8_t FONA_RST="4";
	int8_t FONA_TX="3";
	int8_t FONA_RX="2";
	Adafruit_FONA fona = Adafruit_FONA(FONA_RST); //Instancia para el manejo del módulo GPRS
	SoftwareSerial fonaSS = SoftwareSerial(FONA_TX, FONA_RX);
	SoftwareSerial *fonaSerial = &fonaSS;
	//SoftwareSerial *fonaSerial = &fonaSS;
	/** Información sobre el usuario **/
	char* nomUsuario;
	char* tlfContacto;
	char *pinSIM;
	
};
#endif