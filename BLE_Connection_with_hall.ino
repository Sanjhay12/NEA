

#include  <ArduinoBLE.h>

BLEService KeyChain{"1811"};

const int hallPin = 1;
volatile bool hallState = LOW;

BLEUnsignedIntCharacteristic HallEffectValue ("2A3F", BLERead | BLENotify);
BLEBoolCharacteristic Reset("2A40", BLERead | BLEWrite);
void setup() {
    digitalWrite(12, HIGH);
    digitalWrite(6, LOW);
    delay(100);
    
  Serial.begin(9600);
  pinMode(hallPin,INPUT);
 // attachInterrupt(digitalPinToInterrupt(hallPin), hall_ISR,CHANGE); 
  !BLE.begin();
  BLE.setLocalName("key_chain");
  BLE.setAdvertisedService(KeyChain);
  KeyChain.addCharacteristic(HallEffectValue);
  KeyChain.addCharacteristic(Reset);
  BLE.addService(KeyChain);
  Reset.setEventHandler(BLEWritten, ResetWritten);

  Reset.setValue(0);
  HallEffectValue.writeValue(0);

  BLE.advertise();

  Serial.println("Bluetooth device advertised, waiting for connections");
  
}

void loop() {
  // put your main code here, to run repeatedly:
  BLEDevice central = BLE.central();
  if (central){
    Serial.print("Connceted to central: ");
    Serial.println(central.address());
    digitalWrite(6, HIGH);
    digitalWrite(12, LOW);
    while(central.connected()){
      int hall_value = analogRead(hallPin);
      if(hall_value>800 || hall_value<500){
        HallEffectValue.writeValue(1);
      }
      Serial.println(hall_value);
      delay(500);
    }

    Serial.print("Disconnected from central:");
    Serial.println(central.address());
    digitalWrite(6, LOW);
    delay(100);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
    delay(100);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
    digitalWrite(6, LOW);
    delay(100);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
    delay(100);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(6, LOW);
    digitalWrite(12, HIGH);
    
  }
}

void ResetWritten(BLEDevice central, BLECharacteristic characteristic){
  bool ResetInput = Reset.value();
  if (ResetInput == 1){
    Serial.println("Reset instruction received");
    HallEffectValue.writeValue(0);
    Reset.writeValue(0);
    Serial.println("Reset Characteristic Value set back to 0");
  }
  
}
