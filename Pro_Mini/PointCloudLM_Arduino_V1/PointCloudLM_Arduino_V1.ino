/* #################################INFORMACION###############################
*     Filename    : PointCloudLM_Arduino_V1.ino
*     Project     : PointCloudLM
*     Board       : Arduino Mini Pro (ATmega328P, 3.3V, 8 MHz)
*     Autor       : Luis Lujano (13-10775)    
*     GitHub      : https://github.com/Lujano
*     Sensors     : SRF04 (Ultrasonico), GP2Y0A21YK (Infrared) [Distance] 
*     Drivers     : Mini SSC II Serial Servo Controller
* ###########################################################################*/


/* #################################LIBRERIAS################################*/

// Librerias adicionales
#include <NewPing.h> // Libreria para el manejo del ultrasonido

/* #################################VARIABLES################################*/

// Variables Maquina de estados
unsigned char State  ; // Estado del microcontrolador
typedef enum {       // Estados del microcontrolador
  WAIT,              /* Estado en el cual no se realiza ningun proceso */
  FREERUN,           /* Estado en el cual se reenvia la posicion de los motores recibida por serial */
  POINTCLOUD_START,  /* Comenzar proceso de medicion PointCloud*/
  POINTCLOUD_END,    /* Terminar proceso de medicion PointCloud*/
  MOTOR,             /* Enviar pasos al driver  de los motores */
  MEASURE,           /* Medicion de distancias con los sensores */
  SEND_WIFI          /* Enviar datos medidos al modulo wifi */
} US_State;


// Variables Comunicacion Serial
unsigned int Enviados = 13;    // Esta variable no aporta nada más sino el número de elementos del arreglo a enviar.
unsigned char anuncio;
unsigned char anuncio2;
unsigned char found_band;
unsigned char n_canales;
unsigned char command; // Comando enviado desde pc para cambiar estado del sistema
unsigned char Trama_PC[13]={0xf6, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};   // Esta es una primera trama que yo hice de ejemplo.

// Variable estado POINTCLOUD END
unsigned char Trama_end[3]={0xf1,0x00,0x00}; // Trama enviada cuando se termina el proceso de PointCloud
unsigned int env_end = 3;

// Variable ADC
unsigned int ADC16;
unsigned int MADC16;
unsigned int i = 0;

// Variables de los pasos  de los motores
unsigned char step1 ;
unsigned char step2 ;
unsigned char last_step1;
unsigned char last_step2;
bool dir = 0; // Direccion en aumento de angulo phi (0) (cambio de direccion del motor 2)
unsigned char Trama_Motores[3]={0xFF,0x00,0x00};
unsigned int EnviadosM = 3; 
unsigned char band = 0; // bandera de direccion cambiada
unsigned char reset_band = 0;

// Variables PointCloud Start
unsigned char phi_start;
unsigned char phi_end ;
unsigned char theta_start ;
unsigned char theta_end ;

// Equivalencia angulos-pasos de la calibracion de los motores 
const byte phi_0 = 228;
const byte phi_180 = 36;
const byte theta_0 = 245;
const byte theta_90 = 131;
const byte theta_min = 100;  // minimo angulo sin que el motor choque con la base

unsigned int tiempo; // 16 bits


/* ########################################PINES#####################################*/
//Set pin numbers
#define PING_PIN  9
#define ECHO_PIN  8
//Maximum distance(cm) the HC-SR04 can detect accurately
#define MAX_DISTANCE 400
//Time delay to wait for the receiver to register the echo
#define DELAY_LOOP  3


/* #################################DECLARACION_FUNCIONES################################*/
void serial_send();
void servo1_send(byte posicion);
void servo2_send(byte posicion);


/* ################################CONFIGURACION_INICIAL#################################*/
NewPing sonar(PING_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

void setup(){
  Serial.begin(9600); /*9600 es la Velocidad maxima permitidad por el  driver de los motores*/
  pinMode(9, OUTPUT); /*activación del pin 9 como salida: trigger para el pulso ultrasónico*/
  pinMode(8, INPUT); /*activación del pin 8 como entrada: tiempo del echo del ultrasonido*/

  dir = 0; // sentido de giro por defatult
}

/* #####################################MAIN#######################################*/
void loop(){
}

/* ##############################IMPLEMENTACION_FUNCIONES##############################*/
void serial_send(){
    Trama_PC[3] = (tiempo >> 15) & 0x01; // bit mas significativo upper
    Trama_PC[4] = (tiempo >> 8) & 0x7F;  // bits restantes upper
    Trama_PC[5] = (tiempo >> 7) & 0x01;  // bit mas significativo lower
    Trama_PC[6] = (tiempo) & 0x7F;  // bits restantes lower
    Trama_PC[7] = (step1 >> 7) & 0x01;  // bit mas significativo upper
    Trama_PC[8] = (step1) & 0x7F;  // bits restantes
    Trama_PC[9] = (step2 >> 7) & 0x01;  // bit mas significativo upper
    Trama_PC[10] = (step2) & 0x7F;  // bits restantes
    Serial.write(Trama_PC, 11);
    
  }

void servo1_send(byte posicion){// el servo que controla phi (plano xy)
    byte direccion = 1 ; // conector del driver al que esta conectado el motor
    Trama_Motores [1] = direccion;
    Trama_Motores [2] = posicion;
    Serial.write(Trama_Motores, 3);
 }
  
void servo2_send(byte posicion){ //el servo que controla theta (respecto al eje z)
    byte direccion = 2 ;  // conector del driver al que esta conectado el motor
    Trama_Motores [1] = direccion;
    Trama_Motores [2] = posicion;
    Serial.write(Trama_Motores, 3);
    }
