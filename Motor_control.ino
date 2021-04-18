#include <NewPing.h>


#include <NewPing.h>
#define MAX_DISTANCE_POSSIBLE 70 // sets maximum useable sensor measuring distance to 400cm

int M1 = 4 ; //M1 Direction Control   right wheels
int E1 = 5; //M1 Speed Control

int M2 = 7; //M1 Direction Control   left wheels
int E2 = 6; //M2 Speed Control

unsigned long previousMillis = 0; //microsecond at which the pin was last writen

unsigned int pingSpeed = 100; // How frequently are we going to send out a ping (in milliseconds). 50ms would be 20 times a second.
unsigned long pingTimer;     // Holds the next ping time.

int distance;
//****************************** Raspberry pi - arduino communication

const int D0 = 8;       //Raspberry pin 21    LSB
const int D1 = 9;       //Raspberry pin 22
const int D2 = 10;       //Raspberry pin 23
const int D3 = 11;       //Raspberry pin 24    MSB

// ******************************************** Indicator
const int indicatorR=2;    // right Blinker
const int indicatorL=3;    // left Blinker

const int trigger=12;
const int echo=13;

int a,b,c,d,data;

NewPing sonar(trigger, echo, MAX_DISTANCE_POSSIBLE);

//****************************************************************

void stop(void) //stop
 { 
 digitalWrite(E1,LOW);
 digitalWrite(E2,LOW);
 }
void forward(char a,char b) //Move forward
 {
 analogWrite (E1,a); //PWM Speed Control
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 }
void backward (char a,char b) //Move backward
 {
 analogWrite (E1,a); 
 digitalWrite(M1,LOW);
 analogWrite (E2,b);
 digitalWrite(M2,LOW);
}
void right (char a,char b) //Turn Left
 {
 analogWrite (E1,a);
 digitalWrite(M1,LOW);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 }
void right1 (char a,char b) //Turn Left
 {
 analogWrite (E1,a);
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 }
 
void left (char a,char b) //Turn Right
 {
 analogWrite (E1,a);
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,LOW);
 } 

void left1 (char a,char b) //Turn Right
 {
 analogWrite (E1,a);
 digitalWrite(M1,HIGH);
 analogWrite (E2,b);
 digitalWrite(M2,HIGH);
 }  


int Data()
{
   a = digitalRead(D0);
   b = digitalRead(D1);
   c = digitalRead(D2);
   d = digitalRead(D3);
   data = 8*d+4*c+2*b+a;
//  int objDistance = checkDistance();
 //if(objDistance>10 && objDistance<55)data = 99;
   
  
   return data;
}

int checkDistance()                   
{ 
  //delay(70);
  unsigned int uS = sonar.ping();
  int cm = uS / US_ROUNDTRIP_CM;
  return cm;        
}        


// ******************* Indicator **********************
int signal_right()
{
  digitalWrite(indicatorR, HIGH);  
  delay(500);                       
  digitalWrite(indicatorR,LOW);   
  delay(500);                      
  }
int signal_left(int Delay)
{
  digitalWrite(indicatorL, HIGH);  
  delay(Delay/2);                       
  digitalWrite(indicatorL,LOW);   
  delay(Delay/2);   
  }

void changeLane()
{
  stop();signal_left(500);signal_left(500);
  
  left(200,200); delay(500);
  
 // stop();delay(2000);
  forward(200,200);delay(350);
  stop();delay(400);
  right(100,255);delay(450);
 // right(0,255);delay(1000);
  //right1(150,255);delay(300);
//stop();delay(500);
  //forward(150,150);delay(500);
 // stop();

  
}
void echoCheck() { // Timer2 interrupt calls this function every 24uS where you can check the ping status.
  // Don't do anything here!
  
  if (sonar.check_timer()) { // This is how you check to see if the ping was received.
     distance= sonar.ping_result / US_ROUNDTRIP_CM;
 
  // Serial.print("Ping: ");
   // Serial.print(sonar.ping_result / US_ROUNDTRIP_CM); // Ping returned, uS result in ping_result, convert to cm with US_ROUNDTRIP_CM.
   // Serial.println("cm");
 //  if (distance>=40 && distance <50){changeLane();} 
  }
  // Don't do anything here!
  
}
void setup() {
  // put your setup code here, to run once:
  stop();
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  Serial.begin(115200); //Set Baud Rate 
 
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);



  pinMode(indicatorL, OUTPUT);
  pinMode(indicatorR, OUTPUT);

  pingTimer = millis();
}

void loop() {
//  if(Serial.available()>0) stop();
   //Serial.println(Data());
//if(checkDistance<50)changeLane();
//else while (checkDistance()>50);

    if (millis() >= pingTimer) {   // pingSpeed milliseconds since last ping, do another ping.
    pingTimer += pingSpeed;      // Set the next ping time.
    sonar.ping_timer(echoCheck); // Send out the ping, calls "echoCheck" function every 24uS where you can check the ping status.
 
  }
if (Data()==7)    // IF RED LIGHT IS ON
   stop();



else {
/*
 unsigned long currentMillis = millis(); //time in milliseconds from which the code was started
 if (currentMillis-previousMillis >= 100) {

  int distance = checkDistance();
  if (distance>=40 && distance <50){changeLane();}
   } */
  if (distance>=40 && distance <50){changeLane(); distance=0;}
   
 switch (Data())
 
  {
   
    case 0: forward(200,200);break;
    case 1: right1(200,250);break;
    case 2: right(150,250);break;
    case 3: right(80,250);break;
    case 4: left1(250,230);break;
    case 5: left(240,120);break;
    case 6: left(240,80);break;   
/*
 case 0: forward(200,200);break;
    case 1: right1(200,250);break;
    case 2: right(150,250);break;
    case 3: right(80,250);break;
    case 4: left1(250,230);break;
    case 5: left1(250,150);break;
    case 6: left(250,80);break;*/
/*
    case 0: forward(200,200);break;
    case 1: right1(200,180);break;
    case 2: right(100,200);break;
    case 3: right(50,250);break;
    case 4: left1(200,180);break;
    case 5: left(200,100);break;
    case 6: left(250,50);break;    */
     
    case 99: changeLane();break;
    default: stop();  
  }
}
 //stop();
  
}
