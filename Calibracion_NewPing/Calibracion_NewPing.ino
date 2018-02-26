#include <NewPing.h>

unsigned int tiempo; // 16 bits

// Datos de motores calibrados
byte phi_180 =228;
byte phi_0 = 36;

byte theta_90 = 250;
byte theta_0 = 135;
byte theta_min = 100;  // minimo angulo sin que el motor choque con la base


byte step1; // Paso del motor 1
byte step2; // Paso del motor 2

byte Trama_Motores[3] = {0xff, 0x00, 0x00};
byte Trama_PC[11] = {0xf5, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}; // Canales

bool i; // Direccion

//Set pin numbers
#define PING_PIN  9
#define ECHO_PIN  8
//Maximum distance(cm) the HC-SR04 can detect accurately
#define MAX_DISTANCE 270
//Time delay to wait for the receiver to register the echo
#define DELAY_LOOP  3

NewPing sonar(PING_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pin and maximum distance.

// Funciones
void trigger();
int echo();
void serial_send();
void servo1_send(byte posicion);
void servo2_send(byte posicion);

// Configuracion inicial
void setup(){
  Serial.begin(115200);
  Serial3.begin(9600);
  pinMode(9, OUTPUT); /*activación del pin 9 como salida: trigger para el pulso ultrasónico*/
  pinMode(8, INPUT); /*activación del pin 8 como entrada: tiempo del echo del ultrasonido*/

  i = 0; // sentido de giro por defatult
}


/* MAIN */
void loop(){

    for (step2 = theta_0; step2 <=theta_90 ; step2 = step2+1){
        servo2_send(step2);
        delay(100); // delay en milisegundos
        if (i == 0){
          for (step1=phi_180 ; step1>= phi_0 ; step1 = step1 -1 ){
                  servo1_send(step1);
                  delay(250); // delay en milisegundos
                  tiempo =  sonar.ping();
                  serial_send();
          }
          i = 1;
        }
        else{
           for (step1 = phi_0 ; step1 <= phi_180 ; step1 = step1 +1 ){
                servo1_send(step1);
                delay(250); // delay en milisegundos
                tiempo =  sonar.ping();
                serial_send();
           }
           i = 0;
        }
     }
 


  delay(1000);
}

void trigger(){
   digitalWrite(9,LOW); /* Por cuestión de estabilización del sensor*/
   delayMicroseconds(5);
   digitalWrite(9, HIGH); /* envío del pulso ultrasónico*/
   delayMicroseconds(10);
   digitalWrite(9,LOW); /* Por cuestión de estabilización del sensor*/
  }
  
int echo(){
  int tiempo;
  unsigned long tiempo_temp;
  tiempo_temp=pulseIn(8, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/
  tiempo = (0x0000ffff & tiempo_temp); // conversion a 16 bits

  return tiempo;
  }

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
    Serial3.write(Trama_Motores, 3);
 }
  
void servo2_send(byte posicion){ //el servo que controla theta (respecto al eje z)
    byte direccion = 2 ;  // conector del driver al que esta conectado el motor
    Trama_Motores [1] = direccion;
    Trama_Motores [2] = posicion;
    Serial3.write(Trama_Motores, 3);
    }void setup() {
  // put your setup code here, to run once:

}

void loop() {
  // put your main code here, to run repeatedly:

}
