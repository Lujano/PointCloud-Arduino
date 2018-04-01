
const int Buffer_size = 30;
byte Buffer[Buffer_size] = {}   ;      // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte n_canales;
byte band;
byte anuncio;
byte counter;
byte data;
 
void setup () {
 
  Serial.begin(9600);
  band = 0; // No se a recivido anuncio
  stringComplete = false;
  /*
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print("Connecting..");
 
  }
 */
}
 
void loop() {
  /*
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
 
    http.begin("http://192.168.173.1:80/login?nm=jorge");  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
  }*/

    if (stringComplete) {
    int sente = Serial.write(Buffer, n_canales+n_canales);
    // clear Buffer:
    clear_buffer(Buffer, Buffer_size);
    stringComplete = false;
  }
 
  //delay(30000);    //Send a request every 30 seconds
 
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



