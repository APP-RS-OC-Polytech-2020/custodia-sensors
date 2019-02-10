/*
CUSTODIA BLOC2 
*/

#define SS_PIN D2  
#define RST_PIN D3 
#define LED_INFO_SECURITY D0
#define LASER_RESPONSE D1

#include <SPI.h>
#include <MFRC522.h>

MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
int statuss = 0;
int out = 0;
bool enable_security = false;
void setup() 
{
  Serial.begin(57600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  pinMode(LED_INFO_SECURITY, OUTPUT);
  digitalWrite(LED_INFO_SECURITY, LOW);
}
void loop() 
{
  Serial.println("loop");
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
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == "53 6B 24 D9") //change UID of the card that you want to give access
  {
    Serial.println(" Carte Reconnu ");
    
    delay(1000);
    //traitement du schnaps 
    if(enable_security){//Si la securite etait activé, on la coupe
      Serial.println("Désactivation mode sécurité");
      digitalWrite(LED_INFO_SECURITY, LOW);
      enable_security = false;
          
    }
    else{//Si elle etait pas activé, on active la led et le laser
      Serial.println("Activation mode sécurité");
      digitalWrite(LED_INFO_SECURITY, HIGH);
      enable_security = true;
      while(enable_security){
        if(digitalRead(LASER_RESPONSE)==LOW)
        {
          Serial.println("NO Obstacles!");
        }
        else
        {
          Serial.println("Obstacles!");
        }
          delay(200);
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
  Serial.println();
  Serial.print(" UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  content.toUpperCase();
  Serial.println();
  if (content.substring(1) == "53 6B 24 D9") //change UID of the card that you want to give access
  {
    Serial.println(" Carte Reconnu ");
          
      }
    }
    Serial.println();
    statuss = 1;
  }
  
  else   {
    Serial.println(" Carte non enregistrée ");
    delay(3000);
  }
} 
