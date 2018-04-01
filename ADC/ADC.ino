
//Pines
int sensor_pin = A3;
int trigger_pin = 10;
int echo_pin = 11;
int tiempo; // 16 bits
int ADC16=0;

// Datos de motores calibrados
byte phi_180 =228;
byte phi_0 = 36;

byte theta_90 = 250;
byte theta_0 = 135;
byte theta_min = 100;  // minimo angulo sin que el motor choque con la base


byte step1; // Paso del motor 1
byte step2; // Paso del motor 2

byte Trama_Motores[3] = {0xff, 0x00, 0x00};
byte Trama_PC[3] = {0xf1, 0x00, 0x00}; // Canales

bool i; // Direccion



// Funciones
void trigger();
int echo();
void serial_send();
void servo1_send(byte posicion);
void servo2_send(byte posicion);

// Configuracion inicial
void setup(){
  Serial.begin(115200);
  //Serial3.begin(9600);
  pinMode(trigger_pin, INPUT); /*activación del pin 9 como salida: trigger para el pulso ultrasónico*/
  pinMode(echo_pin, INPUT); /*activación del pin 8 como entrada: tiempo del echo del ultrasonido*/

  i = 0; // sentido de giro por defatult
}


/* MAIN */
void loop(){

  ADC16 = analogRead(sensor_pin);
  serial_send();
  delay(1);
  
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
   Trama_PC[1] =  ((ADC16 >> 7) & (0x1F));
   Trama_PC[2] = (ADC16) & (0x7F);
    /*
    Trama_PC[3] = (tiempo >> 15) & 0x01; // bit mas significativo upper
    Trama_PC[4] = (tiempo >> 8) & 0x7F;  // bits restantes upper
    Trama_PC[5] = (tiempo >> 7) & 0x01;  // bit mas significativo lower
    Trama_PC[6] = (tiempo) & 0x7F;  // bits restantes lower
    Trama_PC[7] = (step1 >> 7) & 0x01;  // bit mas significativo upper
    Trama_PC[8] = (step1) & 0x7F;  // bits restantes
    Trama_PC[9] = (step2 >> 7) & 0x01;  // bit mas significativo upper
    Trama_PC[10] = (step2) & 0x7F;  // bits restantes
    */
    //Serial.print(ADC16);
    //Serial.print("\n");
    Serial.write(Trama_PC, 3);
    
  }

void servo1_send(byte posicion){// el servo que controla phi (plano xy)
    byte direccion = 1 ; // conector del driver al que esta conectado el motor
    Trama_Motores [1] = direccion;
    Trama_Motores [2] = posicion;
//    Serial3.write(Trama_Motores, 3);
 }
  
void servo2_send(byte posicion){ //el servo que controla theta (respecto al eje z)
    byte direccion = 2 ;  // conector del driver al que esta conectado el motor
    Trama_Motores [1] = direccion;
    Trama_Motores [2] = posicion;
    //Serial3.write(Trama_Motores, 3);
    }
