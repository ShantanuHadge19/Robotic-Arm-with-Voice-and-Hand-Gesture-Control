#include <HardwareSerial.h>
#include <ESP32Servo.h>

String inputString;

Servo pinch_servo;
Servo bend_servo;
Servo base_servo;
Servo elbow_servo;
Servo wrist_servo;
Servo useless_servo;

int current_bend_angle = 15;
int current_pinch_angle = 120;
int current_base_angle = 180;
int current_elbow_angle = 15;
int current_wrist_angle = 105;
int current_useless_angle = 65;

int x = 105;  
int b = 90;

bool handGestureMode = false;

#define RELAY_PIN 18  

#define LED_PIN 19  // ESP32 LED

HardwareSerial VC02(1);  // Use UART1 for VC02 module

void setup() {
  Serial.begin(115200);
  VC02.begin(9600, SERIAL_8N1, 16, 17);
  
  pinMode(RELAY_PIN, OUTPUT);

  delay(50);
  pinMode(LED_PIN, OUTPUT);
  delay(50);
  digitalWrite(LED_PIN, LOW);
  delay(50);

  pinch_servo.attach(2);
  delay(100);
  bend_servo.attach(4);
  delay(100);
  base_servo.attach(5);
  delay(100);
  elbow_servo.attach(21);
  delay(100);
  wrist_servo.attach(23);
  delay(100);
  useless_servo.attach(15);

  pinch_servo.write(current_pinch_angle);
  bend_servo.write(current_bend_angle);
  base_servo.write(current_base_angle);
  elbow_servo.write(current_elbow_angle);
  wrist_servo.write(current_wrist_angle);
  useless_servo.write(current_useless_angle);

  Serial.println("VC02 Voice Control Ready...");
}



void loop() {
  if (VC02.available() >= 2) {
    byte highByte = VC02.read();
    byte lowByte = VC02.read();
    unsigned int receivedValue = (highByte << 8) | lowByte;

    Serial.print("Received HEX value: 0x");
    Serial.println(receivedValue, HEX);

    switch (receivedValue) {
      case 0xAA01:
        delay(50);
        digitalWrite(LED_PIN, HIGH);
        delay(250);
        digitalWrite(LED_PIN, LOW);
        delay(50);

        break;

      case 0xAA02:
        //relay bulb on
        delay(50);
        digitalWrite(RELAY_PIN, HIGH);
        delay(250);

        break;

      case 0xAA03:
        //relay bulb on
        delay(50);
        digitalWrite(RELAY_PIN, LOW); 
        delay(250);

        break;

      case 0xAA04:
        //put the object in a box
        delay(250);

   
        for (int angle = 105; angle <= 120; angle++) {
            wrist_servo.write(angle);
            delay(10);
        }
        delay(250);

    
        for (int angle = 30; angle >= 0; angle--) {
            elbow_servo.write(angle);
            delay(10);
        }
        delay(250);

        
        for (int angle = 120; angle <= 140; angle++) {
            wrist_servo.write(angle);
            delay(10);
        }
        delay(200);
   
    

    
        for (int angle = 60; angle >= 45; angle--) {
            bend_servo.write(angle);
            delay(10);
        }
        delay(250);

        pinch_servo.write(30);
        delay(500);


        for (int angle = 140; angle >= 110; angle--) {
            wrist_servo.write(angle);
            delay(10);
        }
        delay(500);

   
        for (int angle = 0; angle <= 60; angle++) {
            base_servo.write(angle);
            delay(10);
        }
        delay(500);

   
        pinch_servo.write(120);
        delay(250);

        break;

      case 0xAA05:

        // Say hello
        delay(500);

        for (int angle = 180; angle >= 0; angle--) {
              base_servo.write(angle);
              delay(10);
        }
        delay(250);

        for (int angle = 15; angle <= 30; angle++) {
              elbow_servo.write(angle);
              delay(10);
        }
        delay(250);

        for (int angle = 15; angle <= 80; angle++) {
              bend_servo.write(angle);
              delay(10);
        }
        delay(250);

        for (int angle = 120; angle >= 30; angle--) {
              pinch_servo.write(angle);
              delay(2);
        }
        delay(150);

        for (int angle = 30; angle <= 120; angle++) {
              pinch_servo.write(angle);
              delay(2);
        }
        delay(150);

        for (int angle = 120; angle >= 30; angle--) {
              pinch_servo.write(angle);
              delay(2);
        }
        delay(150);

        for (int angle = 30; angle <= 120; angle++) {
              pinch_servo.write(angle);
              delay(2);
        }
        delay(150);

        break;
      
      case 0xAA06:
        // Jarvis Sleep
        delay(250);

        for (int angle = 0; angle <= 180; angle++) {
                base_servo.write(angle);
                delay(10);
        }
        delay(250);

        for (int angle = 30; angle >= 15; angle--) {
                elbow_servo.write(angle);
                delay(10);
        }
        delay(250);

        for (int angle = 60; angle >= 15; angle--) {
                bend_servo.write(angle);
                delay(10);
        }
        delay(250);

        wrist_servo.write(105);
        delay(250);

        pinch_servo.write(120);


        break;
      
      
      case 0xAA07:
        
        Serial.println("Hand Gesture Mode Enabled.");
        digitalWrite(LED_PIN, HIGH);
        
        handGestureMode = true;

        while (handGestureMode) {
          if(Serial.available())
          {
              inputString = Serial.readStringUntil('\r');
              Serial.println(inputString);

              int firstComma = inputString.indexOf(',');
              int secondComma = inputString.indexOf(',', firstComma + 1);
              int thirdComma = inputString.indexOf(',', secondComma + 1);
              int fourthComma = inputString.indexOf(',', thirdComma + 1);
              int fifthComma = inputString.indexOf(',', fourthComma + 1);
              int sixthComma = inputString.indexOf(',', fifthComma + 1);

              if (firstComma != -1 && secondComma != -1 && thirdComma != -1 && fourthComma != -1) {
                int x_axis = inputString.substring(fifthComma + 1, sixthComma).toInt();
                int y_axis = inputString.substring(sixthComma + 1).toInt();


                 int y = map(y_axis, 0, 1080, 180, 0);
                 int x = map(x_axis, 0, 1920, 0, 180);

                 int x1 = x + 150;
                 int y1 = y - 50;
                 int y2 = y - 115;

  
                 
                 base_servo.write(x1);
                 
                 bend_servo.write(y1);

                 elbow_servo.write(y2);


              }
    
          }
          

          if (VC02.available() >= 2) {
            byte highByte = VC02.read();
            byte lowByte = VC02.read();
            unsigned int newCommand = (highByte << 8) | lowByte;


            if (newCommand == 0xAA08) {
              
              
              handGestureMode = false;
            }
            
          }
        }

        digitalWrite(LED_PIN, LOW);
        delay(100);
        Serial.println("LED Turned OFF!");
        break;
      
      case 0xAA09:
        
        Serial.println("Hand Gesture Mode Enabled.");
        digitalWrite(LED_PIN, HIGH);
        
        handGestureMode = true;

        while (handGestureMode) {
          if (Serial.available()) {
            inputString = Serial.readStringUntil('\r');
            Serial.println("Received Gesture Data: " + inputString);

            int firstComma = inputString.indexOf(',');
            int secondComma = inputString.indexOf(',', firstComma + 1);
            int thirdComma = inputString.indexOf(',', secondComma + 1);
            int fourthComma = inputString.indexOf(',', thirdComma + 1);
            int fifthComma = inputString.indexOf(',', fourthComma + 1);
            
            

            if (firstComma == -1 || secondComma == -1 ) {
              Serial.println("Invalid input format.");
              return;
            }

            int M = inputString.substring(0, firstComma).toInt();
            int angle = inputString.substring(firstComma + 1, secondComma).toInt();
            int B = inputString.substring(secondComma + 1, thirdComma).toInt();
            int E = inputString.substring(thirdComma + 1, fourthComma).toInt();
            int W = inputString.substring(fourthComma + 1, fifthComma).toInt();

            int E1 = map(E, 0, 1080, 180, 0);
            int B1 = map(B, 0, 1920, 180, 0);

            if (angle >= 0 && angle <= 180) {
              int X = 75 - angle;
              bend_servo.write(X);
              delay(15);
            }

           // if (B1 >= 0 && B1 <= 180) {
             // int B = (B1-90)*2;
              
             // base_servo.write(B);
           // }

            if (B == 1) {
              b = b - 1;
              delay(10);
     
            } 
            else if (B == 0) {
              b = b + 1;
              delay(10);
    
            }

            b = constrain(b, 0, 180);
            base_servo.write(b);

            if (E1 >= 0 && E1 <= 180){
              E1= E1-90;

              elbow_servo.write(E1);
            }

            //if (W >= 0 && W <= 180) {
             //  W=W*0.75;
              // int X = 90 + W;
              // wrist_servo.write(X);
          // }
            if (W == 1) {
              x = x + 1;
              delay(10);
     
            } 
            else if (W == 0) {
              x = x - 1;
              delay(10);
    
            }

    // Constrain servo angle between 0 and 180
            x = constrain(x, 0, 180);
            wrist_servo.write(x);

            current_pinch_angle = (M == 10) ? 30 : 120;
            pinch_servo.write(current_pinch_angle);           
            

          }

          if (VC02.available() >= 2) {
            byte highByte = VC02.read();
            byte lowByte = VC02.read();
            unsigned int newCommand = (highByte << 8) | lowByte;


            if (newCommand == 0xAA10) {
              delay(50);
              Serial.println("Exiting Hand Gesture Mode.");
              handGestureMode = false;
            }
          }
        }

        digitalWrite(LED_PIN, LOW);
        delay(100);
        Serial.println("LED Turned OFF!");
        break;


    }
  }
}
