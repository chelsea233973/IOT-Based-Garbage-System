#include <NewPing.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>
SoftwareSerial mySerial(4, 5); //rx, tx

#define TRIGGER_PIN  2  // Arduino pin tied to trigger pin on the ultrasonic sensor.
#define ECHO_PIN     3  // Arduino pin tied to echo pin on the ultrasonic sensor.
#define MAX_DISTANCE 200 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.

NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE); // NewPing setup of pins and maximum distance.

LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE); 


const byte led = 7;
const byte buzzer = 6;
int level = 0;
unsigned long currentMillis;
unsigned long previousMillis=0;
long interval = 40000;
const int binDepth = 76;

void setup() {
  pinMode(led, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(115200);
  mySerial.begin(9600); 
  lcd.begin(20,4);         
  lcd.setBacklight(HIGH);
  lcd.clear();
  lcd.print("GARBAGE MONITORING");
  lcd.setCursor(0, 1);
  lcd.print("SYSTEM");
  delay(10000);  // give time to log on to network.
  mySerial.print("AT+CMGF=1\r"); // set SMS mode to text
  delay(100);
  mySerial.print("AT+CNMI=2,2,0,0,0\r"); 
  delay(100);
  lcd.clear();

}

void loop() {

  currentMillis = millis();               
  unsigned int uS = sonar.ping(); // Send ping, get ping time in microseconds (uS).
  int distance = uS / US_ROUNDTRIP_CM;
  Serial.println(String(distance) + "cm"); // Convert ping time to distance in cm and print result (0 = outside set distance range)
  level = binDepth - distance;
  Serial.println("Waste Level: " + String(level));
  lcd.home();
  lcd.print("WASTE LEVEL:");
  lcd.setCursor(9, 2);
  
  if(level >= 0)
    lcd.print(String(level) + "cm  ");
  
  delay(1000);
 
// if(currentMillis-previousMillis >= interval)
// {
//  upload();
//  previousMillis = currentMillis;
// }
  
  if(level >= 70)
  {  
      digitalWrite(led, 1);
      digitalWrite(buzzer, 1);
      sendMessage();
  }

}

void sendMessage()
{
  mySerial.println("AT+CMGS=\"+233557487864\"");                                    
  delay(100);
  getResponse();
  mySerial.println("bin at KNUST is full, please come for me, lets keep Ghana clean!!!"); 
  delay(100);
  getResponse();
  mySerial.println((char)26);
  delay(100); 
  getResponse();
  mySerial.println();
  digitalWrite(led, 0);
  digitalWrite(buzzer, 0);
  delay(100);
  delay(15000);
}

void getResponse()
{
  while (mySerial.available())
  {
    Serial.write(mySerial.read());
  }
}

void upload()
{
  Serial.println("uploading to server ******************************");
  mySerial.println("AT");
  getResponse();
  delay(200);
  mySerial.println("AT+CFUN=1");
  getResponse();
  delay(200);
  mySerial.println("AT+CPIN?");
  getResponse();
  delay(200);
  mySerial.println("AT+CSTT=\"MTN\",\"\",\"\"");
  getResponse();
  delay(200);
  mySerial.println("AT+CIICR");
  getResponse();
  delay(500);
  mySerial.println("AT+CIFSR");
  getResponse();
  delay(500);
  mySerial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",\"80\"");
  getResponse();
  delay(1000);
  getResponse();
  mySerial.println("AT+CIPSEND");
  getResponse();
  delay(1000);
  getResponse();
  //mySerial.println("GET http://api.thingspeak.com/update?key=GSU96L72FD1JBLK2&field1=" + String(gateway) +"&field2=" + String(node) + "&field3=" +String(_pressure)+ "&field4=" +String(_power));
  mySerial.println("GET http://api.thingspeak.com/update?key=JCMICR4JM297V1JY&field1=" + String(level));// +"&field2=" + String(gasVal) + "&field3=" +String(ldrVal)+ "&field4=" +String(lati) + "&field5=" +String(longi));
  getResponse();
  delay(1000);
  getResponse();
  mySerial.println((char)26);
}

