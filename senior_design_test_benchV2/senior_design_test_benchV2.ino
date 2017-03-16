const byte bitBangPin0  =  0;
const byte bitBangPin1  =  1;
const byte bitBangPin2  =  2;
const byte bitBangPin3  =  3;
const byte bitBangPin4  =  4;
const byte bitBangPin5  =  5; 
const byte bitBangPin6  =  6; 
const byte bitBangPin7  =  7; 
const byte bitBangPin8  =  8; 
const byte bitBangPin9  =  9; 
const byte bitBangPin10 = 10; 
const byte bitBangPin11 = 11; 
const byte bitBangPin12 = 12; 
const byte bitBangPin13 = 13; 

int WaitTime = .000066667;

//keep track of time 
unsigned long previousMillis = 0;
unsigned long currentMillis  = 0;

const int interval = 10000;
  
void setup() {
  // put your setup code here, to run once:

  //digital pin set up
  pinMode(bitBangPin0,  OUTPUT);
  pinMode(bitBangPin1,  OUTPUT);
  pinMode(bitBangPin2,  OUTPUT);
  pinMode(bitBangPin3,  OUTPUT);
  pinMode(bitBangPin4,  OUTPUT);
  pinMode(bitBangPin5,  OUTPUT);
  pinMode(bitBangPin6,  OUTPUT);
  pinMode(bitBangPin7,  OUTPUT);
  pinMode(bitBangPin8,  OUTPUT);
  pinMode(bitBangPin9,  OUTPUT);
  pinMode(bitBangPin10, OUTPUT);
  pinMode(bitBangPin11, OUTPUT);
  pinMode(bitBangPin12, OUTPUT);
  pinMode(bitBangPin13, OUTPUT);

 
  Serial.begin(9600);
  while(! Serial);
  Serial.println("Hello");
}

void loop() {
  
  
  // get current time 
  currentMillis = millis();
  
  if((unsigned long) (currentMillis - previousMillis) >= interval){

    Serial.println("here");
    // put your main code here, to run repeatedly:
    digitalWrite(bitBangPin0,  HIGH);
    digitalWrite(bitBangPin2,  HIGH);
    digitalWrite(bitBangPin4,  HIGH);
    digitalWrite(bitBangPin6,  HIGH);
    digitalWrite(bitBangPin8,  HIGH);
    digitalWrite(bitBangPin10, HIGH);
    digitalWrite(bitBangPin12, HIGH);

    delay(WaitTime);

    digitalWrite(bitBangPin1,  HIGH);
    digitalWrite(bitBangPin3,  HIGH);
    digitalWrite(bitBangPin5,  HIGH);
    digitalWrite(bitBangPin7,  HIGH);
    digitalWrite(bitBangPin9,  HIGH);
    digitalWrite(bitBangPin11, HIGH);
    digitalWrite(bitBangPin13, HIGH);

    delay(WaitTime);

    digitalWrite(bitBangPin0,  LOW);
    digitalWrite(bitBangPin2,  LOW);
    digitalWrite(bitBangPin4,  LOW);
    digitalWrite(bitBangPin6,  LOW);
    digitalWrite(bitBangPin8,  LOW);
    digitalWrite(bitBangPin10, LOW);
    digitalWrite(bitBangPin12, LOW);

    delay(WaitTime);

    digitalWrite(bitBangPin1,  LOW);
    digitalWrite(bitBangPin3,  LOW);
    digitalWrite(bitBangPin5,  LOW);
    digitalWrite(bitBangPin7,  LOW);
    digitalWrite(bitBangPin9,  LOW);
    digitalWrite(bitBangPin11, LOW);
    digitalWrite(bitBangPin13, LOW);

    delay(WaitTime);
    previousMillis = currentMillis;
    Serial.println(currentMillis);
  }
  else{
    
     //Serial.println("High");
    //blink 13
     digitalWrite(bitBangPin13, HIGH);   // turn the LED on (HIGH is the voltage level)
  
  }
}

