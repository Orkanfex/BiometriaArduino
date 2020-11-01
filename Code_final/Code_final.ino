#include <Adafruit_Fingerprint.h>

#if defined(__AVR__) || defined(ESP8266)
// For UNO and others without hardware serial, we must use software serial...
// pin #2 is IN from sensor (GREEN wire)
// pin #3 is OUT from arduino  (WHITE wire)
// Set up the serial port to use softwareserial..
SoftwareSerial mySerial(3, 4);

#else
// On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// #0 is green wire, #1 is white
#define mySerial Serial1

#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

uint8_t id;

int vermelho = 9, verde = 8;

void setup() {
  Serial.begin(9600);
  while (!Serial);  // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nProcurando Leitor Biométrico...");

  // set the data rate for the sensor serial port
  finger.begin(57600);
  
  delay(5);
  
  if (finger.verifyPassword()) {
    Serial.println("Sensor de impressão digital encontrado!");
  } else {
    Serial.println("Não encontrou o sensor de impressão digital :(");
    while (1) { delay(1); }
  }
  
  finger.LEDcontrol(true);
  
  Serial.println(F("Lendo parâmetros do sensor"));
  finger.getParameters();
  Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacidade: ")); Serial.println(finger.capacity);
  Serial.print(F("Nível de segurança: ")); Serial.println(finger.security_level);
  Serial.print(F("Endereço do dispositivo: ")); Serial.println(finger.device_addr, HEX);
  Serial.print(F("Pacote len: ")); Serial.println(finger.packet_len);
  Serial.print(F("Taxa de transmissão: ")); Serial.println(finger.baud_rate);
  
  pinMode(vermelho, OUTPUT);
  pinMode(verde, OUTPUT);

}

uint8_t readnumber(void) {
  uint8_t num = 0;
  
  while (num == 0) {
    while (! Serial.available());
    num = Serial.parseInt();
  }
  return num;
}
  
void loop() {
  Serial.println("----------------------------");
  Serial.println("Selecione uma opção");
  Serial.println("1)Cadastrar digital");
  Serial.println("2)Buscar digital");
  Serial.println("3)Delete digital");
  Serial.println("----------------------------");

  int option = readnumber();
//CADASTRO
  if(option == 1){
    Serial.println("Pronto para registrar uma impressão digital!");
    Serial.println("Digite o número de ID (de 1 a 127) para cadastrar um dedo.");
    id = readnumber();
    while (!  getFingerprintEnroll() );
  }

//MATCHING
  if(option == 2){
    
//LED VERMELHO E VERDE
    digitalWrite(vermelho, HIGH);
    digitalWrite(verde, LOW);
    
//VERIFICA SE TEM DITAL CADASTRADA, CASO SIM, VERIFICA, CASO NÃO, CADASTRE    
    finger.getTemplateCount();

    if (finger.templateCount == 0) {
      Serial.print("O sensor não contém dados de impressão digital. Por favor, execute um cadastro.\n");
      return;
    } 
    else {
      Serial.println("Aguardando uma digital válida...");
      Serial.print("Sensor contém "); Serial.print(finger.templateCount); Serial.println(" modelos");
    }
    getFingerprintID();
  }
  

}

uint8_t getFingerprintEnroll() {
  
  int p = -1;
  Serial.print("Aguardando um dedo válido para cadastrar como #"); Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem Coletada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de imagem");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagem muito bagunçada");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar os recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar os recursos de impressão digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(id);
  p = -1;
  Serial.println("Coloque o mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem Coletada");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de imagem");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }

  // OK success!

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Imagem convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Imagem muito bagunçada");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar os recursos de impressão digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar os recursos de impressão digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  // OK converted!
  Serial.print("Criando modelo para #");  Serial.println(id);
  
  p = finger.createModel();
  Serial.println(finger.createModel());
  if (p == FINGERPRINT_OK) {
    Serial.println("Impressões correspondentes!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As impressões digitais não coincidem");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível armazenar nesse local");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro ao gravar no flash");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  } 

  return true;
}

uint8_t getFingerprintID() {
  while(1!=2){
    uint8_t p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Imagem Coletada");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println("Nenhum dedo detectado");
        continue;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro de comunicação");
        continue;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Erro de imagem");
        return p;
      default:
        Serial.println("Erro desconhecido");
        return p;
    }
    // OK success!
  
    p = finger.image2Tz();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Imagem convertida");
        break;
      case FINGERPRINT_IMAGEMESS:
        Serial.println("Imagem muito bagunçada");
        continue;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Erro de comunicação");
        continue;
      case FINGERPRINT_FEATUREFAIL:
        Serial.println("Não foi possível encontrar os recursos de impressão digital");
        return p;
      case FINGERPRINT_INVALIDIMAGE:
        Serial.println("Não foi possível encontrar os recursos de impressão digital");
        return p;
      default:
        Serial.println("Erro desconhecido");
        return p;
    }
    
    // OK converted!
    p = finger.fingerSearch();
    if (p == FINGERPRINT_OK) {
      Serial.println("Digital encontrada!");
      digitalWrite(vermelho, LOW);
      for(int x = 0; x<2; x++){
        digitalWrite(verde, HIGH);
        delay(100);
        digitalWrite(verde, LOW);
        delay(100);
      }
    } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
      Serial.println("Erro de comunicação");
      continue;
    } else if (p == FINGERPRINT_NOTFOUND) {
      Serial.println("Nenhuma digital correspondente cadastrada");
      for(int x = 0; x<2; x++){
        digitalWrite(vermelho, LOW);
        delay(300);
        digitalWrite(vermelho, HIGH);
        delay(300);
      }
      
      return p;
    } else {
      Serial.println("Erro desconhecido");
      return p;
    }   
    
    // found a match!
    Serial.print("ID encontrado: ID #"); Serial.print(finger.fingerID); 
    Serial.print(" com confiança de "); Serial.println(finger.confidence); 
  
    return finger.fingerID;
  }
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  // found a match!
  Serial.print("ID encontrado: ID #"); Serial.print(finger.fingerID); 
  Serial.print(" com confiança de "); Serial.println(finger.confidence);
  return finger.fingerID; 
}
