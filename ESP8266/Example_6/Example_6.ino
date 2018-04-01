
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
 
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
#define POINTCLOUD 3


ESP8266WebServer server(80); // Server


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
 server.on("/Server", handleRoot);


 
}
 
void loop() {

   switch (estado){
    case ESPERAR:
    while (payload != "OK"){
      if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
      
      
      HTTPClient http;  //Declare an object of class HTTPClient
      String url = "http://"+server_ip+":"+server_port+url_client+"?param1=OK&param2=OK"; // Conexion con servidor
      http.begin(url);  //Specify request destination
      int httpCode = http.GET();                                                                  //Send the request
      if (httpCode > 0) { //Check the returning code
        payload = http.getString();   //Get the request response payload
        //Serial.println(payload);                     //Print the response payload
   
      }
      http.end();   //Close connection
   
        }
    }
    // Se conecto con servidor cambiar a modo FREERUN e Iniciar servidor
    server.begin();
    estado = FREERUN;
    break;
    case FREERUN:
        server.handleClient(); // Servidor Corriendo
    break;

    case POINTCLOUD: // Mandando data recibida de pointcloud a servidor
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
      
    break;
    
    }
   
 
  //delay(30000);    //Send a request every 30 seconds
 
}




void handleRoot() {

  step1 = server.arg(0).toInt() & 0xff;
  step2 = server.arg(1).toInt() & 0xff;
  String command = server.arg(2);
  if (command == "OK"){
    Trama_FREERUN[3] =  step1;
    Trama_FREERUN[4] =  step2;
    Serial.write(Trama_FREERUN, 5);
    server.send(200, "text/plain", "OK");       //Response to the HTTP request
    }
  else if (command == "POINTCLOUD"){
    phi_start = server.arg(0).toInt() & 0xff;
    phi_end = server.arg(1).toInt() & 0xff;
    theta_start = server.arg(3).toInt() & 0xff;
    theta_end = server.arg(4).toInt() & 0xff;

    Trama_POINTCLOUD[3] =  phi_start;
    Trama_POINTCLOUD[4] =  phi_end;
    Trama_POINTCLOUD[5] =  theta_start;
    Trama_POINTCLOUD[6] =  theta_end;
    Serial.write(Trama_POINTCLOUD, 7);
    estado = POINTCLOUD;
    server.send(200, "text/plain", "POINTCLOUD");       //Response to the HTTP request
    server.stop(); // Detener servidor, empezar cliente
  }

}

void get_request(String param1, String param2, String param3, String param4)
{

 String url = "";
  if (WiFi.status() == WL_CONNECTED) { //Check WiFi connection status
 
    HTTPClient http;  //Declare an object of class HTTPClient
    url = "http://"+server_ip+":"+server_port+url_client+"?param1="+param1+"&param2="+param2+"&param3="+param3+"&param4="+param4;
    http.begin(url);  //Specify request destination
    int httpCode = http.GET();                                                                  //Send the request
 
    if (httpCode > 0) { //Check the returning code
 
      String command = http.getString();   //Get the request response payload
      //Serial.println(payload);                     //Print the response payload
      if (command == "FREERUN"){
        server.begin(); // Prender Servidor
        estado = FREERUN;
        } 
 
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


 
