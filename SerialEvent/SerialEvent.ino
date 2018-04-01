/*
  Serial Event example

 When new serial data arrives, this sketch adds it to a String.
 When a newline is received, the loop prints the string and
 clears it.

 A good test for this is to try it with a GPS receiver
 that sends out NMEA 0183 sentences.

 Created 9 May 2011
 by Tom Igoe

 This example code is in the public domain.

 http://www.arduino.cc/en/Tutorial/SerialEvent

 */
const int Buffer_size = 30;
byte Buffer[Buffer_size] = {}   ;      // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte n_canales;
byte band;
byte anuncio;
byte counter;
byte data;

void setup() {
  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  band = 0; // No se a recivido anuncio
  stringComplete = false;
}

void loop() {
  // print the string when a newline arrives:
  if (stringComplete) {
    int sente = Serial.write(Buffer, n_canales+n_canales);
    // clear Buffer:
    clear_buffer(Buffer, Buffer_size);
    stringComplete = false;
  }
  
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void clear_buffer(byte *Buffer, const int Buffer_size){
  byte i;
  for (i = 0; i<Buffer_size; i++){
    Buffer[i] = 0x00;
  }
  }
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
  

  if (band ==  0){
     anuncio= (byte)Serial.read();
    if ((anuncio & 0xf0)== 0xf0){
        n_canales     = anuncio & 0x0f;
      band = 1;
      counter = 0;
    }
   }
   else 
    {
      
      if (counter == (n_canales+n_canales)-1){
        stringComplete = true;
        band = 0;
        }
        data = (byte)Serial.read();
        Buffer[counter] = data;
        counter += 1;
      
    }
   
  
}
}


