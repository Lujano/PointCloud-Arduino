
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
 
const char* ssid = "LINK-NET-U31";
const char* password = "narbel11157444";
String server_ip = "192.168.1.101";
String server_port = "80";
String url_client = "/ESP8266";

const int Buffer_size = 30;
byte Buffer[Buffer_size] = {}   ;      // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte n_canales;
String param1 = "";
String param2 = "";
byte band;
byte anuncio;
byte counter;
byte data;
 
void setup () {
 
  Serial.begin(9600);
  band = 0; // No se a recivido anuncio
  stringComplete = false;
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print(".");
 
  }
 
}
 
void loop() {

    if (stringComplete) {
    int sente = Serial.write(Buffer, n_canales+n_canales);
    unsigned int p1 = (int(Buffer[0])<<7)+int(Buffer[1]);
    unsigned int p2 = (int(Buffer[2])<<7)+int(Buffer[3]);
    param1 = String(p1, DEC);
    param2 = String(p2, DEC );
    // clear Buffer:
    clear_buffer(Buffer, Buffer_size);
    get_request(param1, param2);
    stringComplete = false;
  }
  else{Event();}
 
  //delay(30000);    //Send a request every 30 seconds
 
}

void get_request(String param1, String param2)
{

   String url = "";
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    url = "http://"+server_ip+":"+server_port+url_client+"?param1="+param1+"&param2="+param2;
    http.begin(url);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String payload = http.getString();   //Get the request response payload
      //Serial.println(payload);                     //Print the response payload
 
    }
 
    http.end();   //Close connection
 
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
void Event() {
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



