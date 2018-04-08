
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
const char* MAC = "60-01-94-21-D1-FF";
const char* ssid = "LINK-NET-U31";
const char* password = "narbel11157444";
String server_ip = "192.168.1.125";
String server_port = "8000";
String url_client = "/ESP8266";

const int Buffer_size = 30;
byte Buffer[Buffer_size] = {}   ;      // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
byte n_canales;
String command = "";
String param1 = "";
String param2 = "";
String param3 = "";
String param4 = "";

byte band;
byte anuncio;
byte counter;
byte data;
byte estado;

byte step1, step2 ,phi_start, phi_end, theta_start, theta_end;
byte Trama_FREERUN[5] = {0xf1, 0x00, 0x01, step1, step2};
byte Trama_POINTCLOUD[7] = {0xf2, 0x00, 0x02, phi_start, phi_end, theta_start, theta_end};

String payload = "0";
#define ESPERAR 1
#define FREERUN 2
#define POINTCLOUD 4

void deco(String response);
void clear_buffer(byte *Buffer, const int Buffer_size);
void Event();
void get_request(String param1, String param2);

void setup () {
 
  Serial.begin(9600);
  band = 0; // No se a recivido anuncio
  stringComplete = false;
  
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
 
    delay(1000);
    Serial.print(".");
 
  }

 estado = ESPERAR;
 


 
}
 
void loop() {

   switch (estado){
    case ESPERAR:
    {
    while (payload != "OK"){
      if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      
      
      HTTPClient http;  //Declare an object of class HTTPClient
      String url = "http://"+server_ip+":"+server_port+url_client+"?param1=OK&param2=OK"; // Conexion con servidor
      http.begin(url);  //Specify request destination
      int httpCode = http.GET();      
      
      if (httpCode > 0) { //Check the returning code
        payload = http.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload
   
      }
      http.end();   //Close connection
   
        }
        Serial.print("-");//Send the request
        delay(1000);
        // Se conecto con servidor cambiar a modo FREERUN e Iniciar servidor
   
    }
     estado = FREERUN;
    }
    break;
    case FREERUN:
    {
      HTTPClient http2;  //Declare an object of class HTTPClient
      String url2 = "http://"+server_ip+":"+server_port+url_client+"?param1=FREERUN&param2=FREERUN"; // Conexion con servidor
      http2.begin(url2);  //Specify request destination
      int httpCode = http2.GET();  
      if (httpCode > 0) { //Check the returning code
        payload = http2.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload
   
      }//Send the request
      http2.end();   //Close connection

      deco(payload);

      if (command == "FREERUN"){ // Comando igual a FREERUN
        Trama_FREERUN[3] = (byte) (param1.toInt() & 0xff);
        Trama_FREERUN[4] = (byte) (param2.toInt() & 0xff);
        command = "";
        Serial.write(Trama_FREERUN, 5);
    }
      else if (command == "POINTCLOUD"){ // Comando igual a POINTCLOUD
        Trama_POINTCLOUD[3] = (byte) (param1.toInt() & 0xff);  // phi_start;
        Trama_POINTCLOUD[4] =(byte) (param2.toInt() & 0xff);  // phi_end;
        Trama_POINTCLOUD[5] = (byte) (param3.toInt() & 0xff);  // theta_start;
        Trama_POINTCLOUD[6] = (byte) (param4.toInt() & 0xff);  // theta_end;
        Serial.write(Trama_POINTCLOUD, 7);
        estado = POINTCLOUD;
         command = "";
      }
      param1 = "";
      param2 = "";
      param3 = "";
      param4 = "";
    }
    delay(80);
    
    
    break;

    case POINTCLOUD: // Mandando data recibida de pointcloud a servidor
    {
         if (stringComplete) {
        unsigned int p1 = (int(Buffer[0])<<15)+ ((int(Buffer[1]))<<8) + ((int(Buffer[2]))<<7)+ (int(Buffer[3]));
        unsigned int p2 = (int(Buffer[4])<<15)+ ((int(Buffer[5]))<<8) + ((int(Buffer[6]))<<7)+ (int(Buffer[7]));
        unsigned int p3 = (int(Buffer[8])<<7)+int(Buffer[9]);
        unsigned int p4 = (int(Buffer[10])<<7)+int(Buffer[11]);
        param1 = String(p1, DEC);
        param2 = String(p2, DEC );
        param3 = String(p3, DEC);
        param4 = String(p4, DEC );
        // clear Buffer:
        clear_buffer(Buffer, Buffer_size);
        get_request(param1, param2, param3, param4);
        stringComplete = false;
      }
      else{Event();}
    }
    break;
    
    }
   
 
  //delay(30000);    //Send a request every 30 seconds
 
}


void deco(String response) {
  byte band = 0;  // bandera para contar el numero de seperadores "&" identificados
  int i;
 char separator = '&';
  for (i= 0; i < response.length(); i++){
    switch(band){
      case 0:
            if(response.charAt(i) != separator){
              command = command+response.charAt(i);
              }
            else{
              band = 1;
              }
             break;
      
      case 1:
            if(response.charAt(i) != separator){
              param1 = param1+response.charAt(i);
              }
            else{
              band = 2;
              }
             break;
       case 2:
               if(response.charAt(i) != separator){
              param2 = param2+response.charAt(i);
              }
            else{
              band = 3;
              }
             break;
       case 3:
               if(response.charAt(i) != separator){
              param3 = param3+response.charAt(i);
              }
            else{
              band = 4;
              }
             break;
        case 4:
              param4 = param4+response.charAt(i);
             break;
      
      }

  

  }

}

void get_request(String param1, String param2, String param3, String param4)
{

 String url = "";
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    url = "http://"+server_ip+":"+server_port+url_client+"?param1="+param1+"&param2="+param2+"&param3="+param3+"&param4="+param4;
    http.begin(url);  //Specify request destination
    int httpCode = http.GET();
    if (httpCode > 0) { //Check the returning code
        payload = http.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload
   
      }//Send the request
      http.end();   //Close connection

      deco(payload);

      if (command == "FREERUN"){ // Comando igual a FREERUN
        Trama_FREERUN[3] = (byte) (param1.toInt() & 0xff);
        Trama_FREERUN[4] = (byte) (param2.toInt() & 0xff);
        command = "";
        Serial.write(Trama_FREERUN, 5);
        estado = FREERUN; // Cambiar estado a FREERUN
    }
      else if (command == "POINTCLOUD"){ // Comando igual a POINTCLOUD
      }
      param1 = "";
      param2 = "";
      param3 = "";
      param4 = "";
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
    if (anuncio != 0xff &(anuncio & 0xf0)== 0xf0 ){
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


 
