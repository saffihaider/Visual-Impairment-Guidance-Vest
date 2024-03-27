//Haptic Motor Controls
#include <Wire.h>

// Ultrasonic pins
const int trigPin = 15; // Pin used to trigger ranging
const int echoPin1 = 5; // Ultrasonic 1 output 
const int echoPin2 = 4; // Ultrasonic 2 output 

const int threshold = 50; // Minimum distance for obstacle to be registered
const int delaytime = 500; // Delay between measurements
const int iterations = 1; // Averaging factor (number of measurements to be averaged per cycle)
int distance1, distance2;

//Haptic Pins
#define PWM_PIN_0 22 // Pin motor 1
#define PWM_PIN_1 25 // Pin motor 2  
#define PWM_PIN_2 12 // Pin motor 3
#define PWM_PIN_3 18 // Pin motor 4

//LED Pins
const int leftLED = 6;   
const int rightLED = 7;  
const int middleLED = 8;  

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(echoPin2, INPUT);  
  Serial.begin(9600); // Starts the serial communication
  

  pinMode(PWM_PIN_0, OUTPUT); 
  pinMode(PWM_PIN_1, OUTPUT); 
  pinMode(PWM_PIN_2, OUTPUT);
  pinMode(PWM_PIN_3, OUTPUT);

}

void loop() {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  
  // Output UltraSonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(90);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(970);
  
  // Scan for returned wave and measure pulse widths to calculate distance
  distance1 = pulseIn(echoPin1, HIGH) * 0.034 / 2.0;
  distance2 = pulseIn(echoPin2, HIGH) * 0.034 / 2.0;
  Serial.println(distance1);
  Serial.println(distance2);

  // Compare distance to threshold
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);
  bool middle = left && right;
  bool none = (distance1 > threshold && distance2 > threshold);
  
  digitalWrite(PWM_PIN_0, LOW);
  digitalWrite(PWM_PIN_1, LOW);
  digitalWrite(PWM_PIN_2, LOW);
  digitalWrite(PWM_PIN_3, LOW);
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);
  digitalWrite(middleLED, LOW);
  
  // Trigger respective haptic motor
  
  if (middle) {
    Serial.println("middle");
    analogWrite(PWM_PIN_0, 255);
    analogWrite(PWM_PIN_1, 255);
    analogWrite(PWM_PIN_2, 255);
    analogWrite(PWM_PIN_3, 255);
    digitalWrite(middleLED, HIGH);
  }
  else if (left) {
    Serial.println("left");
    analogWrite(PWM_PIN_0, 255);
    analogWrite(PWM_PIN_1, 255);
    analogWrite(PWM_PIN_2, 0); 
    analogWrite(PWM_PIN_3, 0); 
    digitalWrite(leftLED, HIGH);
  }
  else if (right) {
    Serial.println("right");
    analogWrite(PWM_PIN_0, 0); 
    analogWrite(PWM_PIN_1, 0); 
    analogWrite(PWM_PIN_2, 255);
    analogWrite(PWM_PIN_3, 255);
    digitalWrite(rightLED, HIGH);
  }
  else if (none) {
    Serial.println("No obstacle");
    analogWrite(PWM_PIN_0, 0); 
    analogWrite(PWM_PIN_1, 0); 
    analogWrite(PWM_PIN_2, 0);
    analogWrite(PWM_PIN_3, 0);
    digitalWrite(leftLED, LOW);
    digitalWrite(rightLED, LOW);
    digitalWrite(middleLED, LOW);
  }
  
  delay(10); // Delay for stability
}
