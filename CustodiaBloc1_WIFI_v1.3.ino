/*******************  APP RSOC2020 CODE:CUSTODIA   *******************************
   
********************  CODE BOITIER CAPTEUR 1 WIFI  *******************************
12:31  20/01/2019

TODO :

valeur analogique
test gaz
mettre en fonction les commandes wifi et dht ?
mettre les adresses mac en identifiant
code couleur led :
vert rouge clignotant : calibrage
vert fixe / rouge cligno:  calibré /tentative co wifi
vert fixe rouge eteint fonctionnement normale


**********************************LIBRAIRIES****************************************/
#include <dht.h>
#include <Wire.h> 
#include <ESP8266WiFi.h>
#include <WiFiClient.h>


/************************Hardware Related Macros************************************/
#define         MQ_PIN                       A0      //Lecture analogique du capteur de fumée
#define         DHT11_PIN                    D3      //Lecture digitale du capteur de température et humidité
#define         LED_INFO_GREEN               D1      //Led d'information verte
#define         LED_INFO_RED                 D2      //Led d'information gauche

#define         SIZE_TABLE_MEMORY   12

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
                                                     //cablibration phase


/*****************************Globals***********************************************/

     
                                                                                            
bool            trigger_MQ2   = false;
bool            init_compteur = false;
dht             DHT11;

float           memory_DHT11[SIZE_TABLE_MEMORY];
float             memory_MQ2[SIZE_TABLE_MEMORY];

int             marker        = 0;
float           temperature   = 0;
int             humidity      = 0;
int             smoke         = 0;

//VARIABLE WIFI
const char* ssid = "wifilego";
const char* pass = "matlablego";

//VARIABLE SERVEUR
const char* host = "193.48.125.70";
const int port = 50008;



/************************** FONCTION SETUP *****************************/
void setup()
{
  Serial.begin(115200);//UART setup, baudrate = 115200bps
  
  connectWifi(); // fonction qui connecte l'esp au wifi


  //SETUP LED
  Serial.print("Setup LED:");
  pinMode(LED_INFO_GREEN, OUTPUT);    //reglage mode
  delay(100);                         //on le laisse tranquille 0.1s
  digitalWrite(LED_INFO_GREEN, LOW);  //on la passe en LOW en attendant la calibration du capteur de fumee

  pinMode(LED_INFO_RED, OUTPUT);      //reglage mode
  delay(100);                         //on le laisse tranquille 0.1s
  digitalWrite(LED_INFO_RED, LOW);    //on la passe en LOW en attendant la calibration du capteur de fumee
  Serial.print(" ............done");

  //SETUP MQ2
  //init memory
  for(int i=0; i<13; i++){
    memory_MQ2[i]=0; 
  }
  
  Serial.print("Setup MQ2:");
  for(int j = 0; marker <= 12; j++){
    for(int i = 1; i < SIZE_TABLE_MEMORY; i++){
      memory_MQ2[i-1] = memory_MQ2[i];
    }
    memory_MQ2[SIZE_TABLE_MEMORY-1] = (analogRead(A0)/1024)*5000; //permet d'obtenir la valeur en millivolt entre 0-1V, puis * 5000 pour obtenir la valeur réel renvoyé par le capteur en V
    marker = marker + 1;
    
    digitalWrite(LED_INFO_GREEN, HIGH);
    delay(CALIBRATION_SAMPLE_INTERVAL);
    digitalWrite(LED_INFO_GREEN, LOW);
    delay(CALIBRATION_SAMPLE_INTERVAL);
    
    Serial.print(".");
    
  }  // 12-13sec pour remplir les 12 premières valeurs
  Serial.print("done ");
  
  

  //SETUP DHT11
  //init memory
  Serial.print("Setup DHT11:");
  for(int i=0; i<13; i++){
    memory_DHT11[i]=0; 
  }
  float chk = DHT11.read11(DHT11_PIN); // a garder ?
  
  for(int j = 0; marker <= 12; j++){
    for(int i = 1; i < SIZE_TABLE_MEMORY; i++){
      memory_DHT11[i-1] = memory_DHT11[i];
    }
    memory_DHT11[SIZE_TABLE_MEMORY-1] = DHT11.temperature;
    marker = marker + 1;
    
    digitalWrite(LED_INFO_GREEN, HIGH);
    delay(CALIBRATION_SAMPLE_INTERVAL);
    digitalWrite(LED_INFO_GREEN, LOW);
    delay(CALIBRATION_SAMPLE_INTERVAL);
    
    Serial.print(".");
    
  }  // 12-13sec pour remplir les 12 premières valeurs
  Serial.print("done ");
}



/************************** FONCTION LOOP ***************************/
void loop()
{
  float mean_temperature;
  float mean_smoke;


  for(int i = 1; i < SIZE_TABLE_MEMORY; i++){
      memory_DHT11[i-1] = memory_DHT11[i];
      memory_MQ2[i-1]   = memory_MQ2[i];
    }
    memory_DHT11[SIZE_TABLE_MEMORY-1] = DHT11.temperature;
    memory_MQ2[SIZE_TABLE_MEMORY-1]   = analogRead(A0);   
     
  for(int i = 0; i < SIZE_TABLE_MEMORY; i++){
    mean_temperature = mean_temperature + memory_DHT11[i];
    mean_smoke       = mean_smoke + memory_MQ2[i];
  }
  mean_temperature = mean_temperature/SIZE_TABLE_MEMORY;
  mean_smoke       = mean_smoke/SIZE_TABLE_MEMORY;
  
   
   Serial.print("RELEVE N°"); 
   Serial.print(marker);
   Serial.print("\n");

   Serial.print("\n--DHT11 Sensor--\n");
   Serial.print("Temperature: ");
   Serial.print(memory_DHT11[SIZE_TABLE_MEMORY-1],0);
   Serial.print("°C\n");
   Serial.print("Moyenne Glissante: ");
   Serial.print(mean_temperature); 
   Serial.print("°C\n");  
   Serial.print("Humidity: ");
   Serial.print(DHT11.humidity,0); //print the humidity on lcd
   humidity = DHT11.humidity;
   Serial.println("%\n"); 
  
   Serial.print("\n--MQ2 Sensor--\n");
   Serial.print("Valeur analogique: ");
   Serial.print(memory_MQ2[SIZE_TABLE_MEMORY-1]);
  
   marker++;
   sendJson(memory_DHT11[SIZE_TABLE_MEMORY-1], humidity, memory_MQ2[SIZE_TABLE_MEMORY-1]);
   delay(10000);
   //fin de la loop
}

void serialMonitor(){
  
}

/**********************************FONCTION COMMUNICATION WIFI****************************/

//CONNEXION WIFI
void connectWifi(){
  Serial.println();
  Serial.print("Connexion à l'IP : ");
  Serial.println(ssid);
  Serial.print("Port du serveur :");
  Serial.println(host);
  WiFi.begin(ssid, pass);
  delay(1000);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Mon IP : ");
  Serial.println(WiFi.localIP());
  Serial.println("Adresse MAC :");
  Serial.println(WiFi.macAddress());  
}


//ENVOI DU STRING JSON
void sendJson(float mean, int humidity, bool smoke){
  
   WiFiClient client; //creation d'une variable client
   
   if (!client.connect(host, port)) {                    //verification tentative de connexion, si echec faire :
      Serial.println("Connexion au serveur impossible"); //informer du probleme de connexion
      return;
   }
   
   Serial.println("Envoie des informations au serveur"); // on  stringifie le Json 
   client.println("{\"type\":\"sensors\",\"data\":{\"temperature\":\" + String(temperature) + \",\"humidity\":\" + String(humidity) + \",\"smoke\":\" + String(smoke) + \"},\"source\":\"WiFi.macAddress()\",\"captureTime\":\"\",\"user\":{\"email\":\"franck.battu@gmail.com\"}}"); //attention au json coupé
   
   /* client.println("{\"type\":\"sensors\",\"data\":{\"temperature\":" + String(mean) + , \"humidity\" : " + String(humidity)" +",\
   \"smoke\" : " + String(smoke) + " }}"); ANCIEN JSON*/
   client.flush();
   client.stop();

  
}

