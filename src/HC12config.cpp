#include "Arduino.h"
#include "HC12config.h"

hc12Config::hc12Config(HardwareSerial *SerialHC12, uint8_t setPin) {
  this->SerialHC12 = SerialHC12;
  this->setPin = setPin;
}

void hc12Config::programming_mode(bool enter){
  if (enter) {
#ifdef HC12_DEBUG
    Serial.printf("HC12: entering programming mode (SET pin = %d)\n", setPin);
#endif    

    pinMode(setPin, OUTPUT);
    digitalWrite(setPin, LOW);
    delay(HC12_MS_DELAY);

  } else {
#ifdef HC12_DEBUG
    Serial.printf("HC12: leaving programming mode\n");
#endif    

    pinMode(setPin, INPUT);
    delay(HC12_MS_DELAY);    
  }
  SerialHC12->flush();
}

int hc12Config::sendCommandAndTestResult(const char* command, bool silent){
#ifdef HC12_DEBUG
  Serial.printf("HC12: sending: %s\n", command);
#endif

  SerialHC12->println(command);
  String hc12Response = SerialHC12->readStringUntil('\n');

#ifdef HC12_DEBUG
  Serial.printf("HC12: received: %s\n", hc12Response.c_str());
#endif

  if (hc12Response.startsWith("OK") ) {
    return HC12_SUCCESS;
  } else {
    if (silent == false) {
      Serial.printf("HC12: %s -> %s\n", command, hc12Response.c_str());
    }
    return HC12_ERROR;
  }
}

int hc12Config::testBaud(){
#ifdef HC12_DEBUG
  Serial.printf("HC12: Testing BaudRate %d\n", SerialHC12->baudRate());
#endif

  programming_mode(true);
  int r = sendCommandAndTestResult("AT");
  programming_mode(false);
  return r;
}

int hc12Config::findBaud(){
  unsigned long allBauds[] = HC12_ALL_BAUDS;

  if ( testBaud() == HC12_SUCCESS) {
    return HC12_SUCCESS;
  }

  unsigned long startingBaud = SerialHC12->baudRate();
  int n = sizeof(allBauds) / sizeof(allBauds[0]);

  for (int i = 0; i < n; i++) {
    if (allBauds[i] != startingBaud) {
      SerialHC12->updateBaudRate(allBauds[i]);
      if ( testBaud() == HC12_SUCCESS) {
        return HC12_SUCCESS;
      }
    }
  }

  SerialHC12->updateBaudRate(startingBaud);
  return HC12_ERROR;
}

int hc12Config::start(){
  Serial.print("Starting HC12...");
  SerialHC12->setTimeout(HC12_MS_TIMEOUT);
#ifdef HC12_DEBUG
  Serial.println();
#endif  

  if (findBaud() == HC12_SUCCESS) {
    Serial.println("done.");
    return HC12_SUCCESS;
  } else {
    Serial.println("unable to verify connection.");
    return HC12_ERROR;
  }
}

int hc12Config::begin(){
  char command[15];
  unsigned long baudTarget = SerialHC12->baudRate();

  if (start() != HC12_SUCCESS) {
    return HC12_ERROR;
  }

  if (baudTarget != SerialHC12->baudRate()) {
    programming_mode(true);
    sprintf(command, "AT+B%d", baudTarget);
    if (sendCommandAndTestResult(command) != HC12_SUCCESS){
      return HC12_ERROR;
    }
    programming_mode(false);
    SerialHC12->updateBaudRate(baudTarget);
  }
  return HC12_SUCCESS;
}

int hc12Config::begin(unsigned long baud, int8_t channel, int mode, int power){
  char command[15];

  SerialHC12->updateBaudRate(baud);
  if (start() != HC12_SUCCESS) {
    return HC12_ERROR;
  }

  programming_mode(true);
  sprintf(command, "AT+FU%d", mode);
  if (sendCommandAndTestResult(command) != HC12_SUCCESS){
    return HC12_ERROR;
  }

  sprintf(command, "AT+C%03d", channel);
  if (sendCommandAndTestResult(command) != HC12_SUCCESS){
    return HC12_ERROR;
  }

  /* maximal erlaubtes Powerlevel = 5? 12mW (ist fast wie 10mW) */
  sprintf(command, "AT+P%d", power);
  if (sendCommandAndTestResult(command) != HC12_SUCCESS){
    return HC12_ERROR;
  }

  sprintf(command, "AT+B%d", baud);
  if (sendCommandAndTestResult(command) != HC12_SUCCESS){
    return HC12_ERROR;
  }
/*  uart->println("AT+RX");
  delay(100);
  while (uart->available()) {
    Serial.write(uart->read());
  }*/  
  programming_mode(false);
  SerialHC12->updateBaudRate(baud);
  return HC12_SUCCESS;
}

int hc12Config::sleep(){
  Serial.print("Sending HC12 to sleep...");    
  programming_mode(true);
  int r = sendCommandAndTestResult("AT+SLEEP");
  programming_mode(false);
  if (r == HC12_SUCCESS ) {
    Serial.println("done.");
  } else {
    Serial.println("error.");
  }
  return r;
}

int hc12Config::wakeup(){
  Serial.print("Waking up HC12...");  
  int r = testBaud();
  if (r == HC12_SUCCESS ) {
    Serial.println("done.");
  } else {
    Serial.println("unable to verify connection.");
  }
  return r;
}

int hc12Config::set_defaults(){
  if (start() != HC12_SUCCESS) {
    return HC12_ERROR;
  }  
  programming_mode(true);
  int r = sendCommandAndTestResult("AT+DEFAULT");
  programming_mode(false);  

  SerialHC12->updateBaudRate(HC12_DEFAULT_BAUD);  
  testBaud();
  return r;
}