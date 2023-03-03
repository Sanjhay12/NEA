#include <SPI.h>  
 #include <MFRC522.h>  
 #define SS_PIN 10  
 #define RST_PIN 9  
 MFRC522 mfrc522(SS_PIN, RST_PIN); 
 void setup()  
 {  
  
  Serial.begin(9600);  // Initiate a serial communication  
  SPI.begin();   // Initiate SPI bus  
  mfrc522.PCD_Init();  // Initiate MFRC522  
  pinMode(5, OUTPUT);  
  pinMode(12, OUTPUT);
  pinMode(2, OUTPUT);
  digitalWrite(5, LOW);
  digitalWrite(12, LOW);
  digitalWrite(2, HIGH);
  pinMode(6, OUTPUT);
  Serial.println("Put your card to the reader...");  
  Serial.println();
   
 }  
 void loop()  
 {  
  // Look for new cards  
  if ( ! mfrc522.PICC_IsNewCardPresent())  
  {  
   return;  
  }  
  // Select one of the cards  
  if ( ! mfrc522.PICC_ReadCardSerial())  
  {  
   return;  
  }  
  //Show UID on serial monitor  
  Serial.print("UID tag :");  
  String content = "";  
  byte letter;  
  for (byte i = 0; i < mfrc522.uid.size; i++)  
  {  
   Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");  
   Serial.print(mfrc522.uid.uidByte[i], HEX);  
   content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));  
   content.concat(String(mfrc522.uid.uidByte[i], HEX));  
  }  
  Serial.println();  
  Serial.print("Message : ");  
  content.toUpperCase();  
  if (content.substring(1) == "73 CF 67 AD") //change here the UID of the card/cards that you want to give access  
  {  
   Serial.println("Authorized access");  
   Serial.println();  
   digitalWrite(5, HIGH);
   digitalWrite(2, LOW);
   digitalWrite(12, HIGH);
   digitalWrite(6, HIGH);
   delay(2000);
   digitalWrite(5, LOW);
   digitalWrite(6, LOW);
   digitalWrite(2, HIGH);
   digitalWrite(12, LOW);
  }  
  else  
  {  
   Serial.println(" Access denied");  
   
     if (content.substring(1) == "43 79 69 AD") //change here the UID of the card/cards that you want to give access  
  {  
    for(int i = 0; i< 5000; i++){
     digitalWrite(2, HIGH);
    digitalWrite(6, HIGH);
    delay(100);
    digitalWrite(2, LOW);
    digitalWrite(6, LOW);
    delay(100);
}
  }    
else  
  {  
   Serial.println(" Access denied");  
   
     while (content.substring(1) == "43 2E 49 92") //change here the UID of the card/cards that you want to give access  
  {  
    digitalWrite(6, LOW);

  }    
 }
}
}
