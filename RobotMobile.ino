#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
//Set Konfigurasi WiFi
#define WIFI_SSID "Y..."
#define WIFI_PASS "sembarang"

#define ENA   0           // Enable/speed motors Kanan        GPIO0(D3)
#define ENB   15          // Enable/speed motors Kiri         GPIO15(D8)
#define IN_1  2           // L298N in1 motors Kanan           GPIO2(D4)
#define IN_2  14          // L298N in2 motors Kanan           GPIO14(D5)
#define IN_3  12          // L298N in1 motors Kiri            GPIO12(D6)
#define IN_4  13          // L298N in1 motors Kiri            GPIO13(D7)
#define LEDR1  16
#define LEDR2  5
#define LEDL1  4
#define LEDL2  3

#define UDP_PORT 4210
WiFiUDP UDP;
char packet[255];
String myString;
char reply[] = "Perintah diterima!";

int speedCar = 600;         // 400 - 1023.
int speedSlow = 100;

void setup() {
  Serial.begin(115200);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(LEDR1, OUTPUT);
  pinMode(LEDR2, OUTPUT);
  pinMode(LEDL1, OUTPUT);
  pinMode(LEDL2, OUTPUT);
  digitalWrite(LEDR1, LOW);
  digitalWrite(LEDR2, LOW);
  digitalWrite(LEDL1, LOW);
  digitalWrite(LEDL2, LOW);
  delay(200);
  for (long x = 0; x < 6; x++)
   {
      digitalWrite(LEDR1, !digitalRead(LEDR1));
      digitalWrite(LEDR2, !digitalRead(LEDR2));
      digitalWrite(LEDL1, !digitalRead(LEDL1));
      digitalWrite(LEDL2, !digitalRead(LEDL2));
      delay(500);
   }
  Serial.println();
  Serial.print("Mencoba menghubungkan ke ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.print("connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  // Begin listening to UDP port
  UDP.begin(UDP_PORT);
  Serial.print("Listening on UDP port ");
  Serial.println(UDP_PORT);

  for (long x = 0; x < 6; x++)
   {
      digitalWrite(LEDR1, !digitalRead(LEDR1));
      digitalWrite(LEDR2, !digitalRead(LEDR2));
      digitalWrite(LEDL1, !digitalRead(LEDL1));
      digitalWrite(LEDL2, !digitalRead(LEDL2));
      delay(500);
   }
//  digitalWrite(LEDR1, LOW);
//  digitalWrite(LEDR2, LOW);
//  digitalWrite(LEDL1, LOW);
//  digitalWrite(LEDL2, LOW);

}
void Maju(){ 

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar+15);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar);
  }
void Mundur(){ 

      digitalWrite(IN_1, HIGH);
      digitalWrite(IN_2, LOW);
      analogWrite(ENA, speedCar-speedSlow+10);

      digitalWrite(IN_3, HIGH);
      digitalWrite(IN_4, LOW);
      analogWrite(ENB, speedCar-speedSlow);
  }
void BelokKiri(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar+speedSlow+15);
 
      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar-speedSlow);
   }

void BelokKanan(){
      
      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, HIGH);
      analogWrite(ENA, speedCar-speedSlow+15);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, HIGH);
      analogWrite(ENB, speedCar+speedSlow);
  }
void stopRobot(){  

      digitalWrite(IN_1, LOW);
      digitalWrite(IN_2, LOW);
//      analogWrite(ENA, speedCar);

      digitalWrite(IN_3, LOW);
      digitalWrite(IN_4, LOW);
//      analogWrite(ENB, speedCar);
 }

void loop() {
  // If packet received...
  int packetSize = UDP.parsePacket();
  if (packetSize) {
    Serial.print("Received packet! Size: ");
    Serial.println(packetSize); 
    int len = UDP.read(packet, 255);
    if (len > 0)
    {
      packet[len] = '\0';
    }
    Serial.print("Packet received: ");
    Serial.println(packet);
    myString = packet;
    if (myString.startsWith("MAJU")) {
      Serial.println("Action : GAS POLL !!!");
      Maju();
      digitalWrite(LEDR1, HIGH);
      digitalWrite(LEDL1, HIGH);
      digitalWrite(LEDR2, LOW);
      digitalWrite(LEDL2, LOW);
    }
    else if (myString.startsWith("MUNDUR")) {
      Serial.println("Action : MUNDUR COY !!!");
      Mundur();
      digitalWrite(LEDR1, LOW);
      digitalWrite(LEDL1, LOW);
      digitalWrite(LEDR2, HIGH);
      digitalWrite(LEDL2, HIGH);
    }
    else if (myString.startsWith("KANAN")) {
      Serial.println("Action : BELOK KANAN GRAK");
      BelokKanan();
      digitalWrite(LEDR1, HIGH);
      digitalWrite(LEDL1, HIGH);
      digitalWrite(LEDR2, LOW);
      digitalWrite(LEDL2, LOW);
    }
    else if (myString.startsWith("KIRI")) {
      Serial.println("Action : BELOK KIRI GRAK");
      BelokKiri();
      digitalWrite(LEDR1, HIGH);
      digitalWrite(LEDL1, HIGH);
      digitalWrite(LEDR2, LOW);
      digitalWrite(LEDL2, LOW);
    }
    else if (myString.startsWith("STOP")) {
      Serial.println("Action : HOOOP");
      stopRobot();
      digitalWrite(LEDR1, LOW);
      digitalWrite(LEDL1, LOW);
      digitalWrite(LEDR2, LOW);
      digitalWrite(LEDL2, LOW);
    }
    UDP.beginPacket(UDP.remoteIP(), UDP.remotePort());
    UDP.write(reply);
    UDP.endPacket();
    Serial.println();
  }
  
}
