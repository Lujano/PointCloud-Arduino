long distancia;
long tiempo;
int tiempo2; // 16 bits
void setup(){
  Serial.begin(9600);
  pinMode(9, OUTPUT); /*activación del pin 9 como salida: para el pulso ultrasónico*/
  pinMode(8, INPUT); /*activación del pin 8 como entrada: tiempo del rebote del ultrasonido*/
}

void loop(){
 digitalWrite(9,LOW); /* Por cuestión de estabilización del sensor*/
 delayMicroseconds(5);
 digitalWrite(9, HIGH); /* envío del pulso ultrasónico*/
 delayMicroseconds(10);
 
  tiempo=pulseIn(8, HIGH); /* Función para medir la longitud del pulso entrante. Mide el tiempo que transcurrido entre el envío
  del pulso ultrasónico y cuando el sensor recibe el rebote, es decir: desde que el pin 12 empieza a recibir el rebote, HIGH, hasta que
  deja de hacerlo, LOW, la longitud del pulso entrante*/
  tiempo2 = (0x0000ffff & tiempo)
  
  

  /*Monitorización en centímetros por el monitor serial*/
  Serial.println("Distancia ");
  Serial.println(distancia);
  Serial.println(" cm");
  delay(1000);
}
