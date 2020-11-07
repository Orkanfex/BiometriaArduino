

#include <SPI.h>
#include <UIPEthernet.h>
byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED }; //Setting MAC Address

int valorLuz;
int pinoSensorLuz = A0;

char server[] = "192.168.0.102";
IPAddress ip(192,168,0,110); 
EthernetClient client; 

/* Setup for Ethernet and RFID */

void setup() {
  Serial.begin(9600);
  
  
  Ethernet.begin(mac, ip);
  
  delay(1000);

  char encerra = '0';
  uint32_t starttime = millis();
  while ((millis() - starttime) < 5000 && encerra == '0') {
    encerra = Sending_To_phpmyadmindatabase();
    delay(500);
      
    
   // interval
  }
}
//------------------------------------------------------------------------------


/* Infinite Loop */
void loop(){
  //valorLuz = analogRead(pinoSensorLuz);
  
  
}


  char Sending_To_phpmyadmindatabase()   //CONNECTING WITH MYSQL
 {
   if (client.connect(server, 80)) {
    //Serial.print("connected");
    client.print("GET /biometria/template.php?template=");     //YOUR URL
    client.print("3648888");
    client.print("&raAluno=");
    client.print("123456");
    client.print(" ");      //SPACE BEFORE HTTP/1.1
    client.print("HTTP/1.1");
    client.println();
    client.println("Host: 192.168.0.104");
    client.println("Connection: close");
    client.println();

    return '1';
  } else {
    // if you didn't get a connection to the server:
    return '0';
  }
 }
