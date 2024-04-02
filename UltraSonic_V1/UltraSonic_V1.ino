
// Ultrasonic pins
const int trigPin = 15; // Pin used to trigger ranging
const int echoPin1 = 5; // Ultrasonic 1 output 
const int echoPin2 = 4; // Ultrasonic 2 output 

const int threshold = 50; // Minimum distance for obstacle to be registered (in cm)
const int iterations = 5;
int distance1, distance2;

//Haptic Pins
#define PWM_PIN_0 22 // Pin motor 1: Left
#define PWM_PIN_1 25 // Pin motor 2: Left  
#define PWM_PIN_2 12 // Pin motor 3: Right
#define PWM_PIN_3 18 // Pin motor 4: Right

//LED Pins
const int leftLED = 23;   
const int rightLED = 14;  
const int middleLED = 21;  

void setup() {
  pinMode(trigPin, OUTPUT);  
  pinMode(echoPin1, INPUT); 
  pinMode(echoPin2, INPUT);  
  Serial.begin(9600); // Starts the serial communication
  
  pinMode(PWM_PIN_0, OUTPUT); 
  pinMode(PWM_PIN_1, OUTPUT); 
  pinMode(PWM_PIN_2, OUTPUT);
  pinMode(PWM_PIN_3, OUTPUT);
  pinMode(leftLED, OUTPUT); 
  pinMode(rightLED, OUTPUT); 
  pinMode(middleLED, OUTPUT);

}

void loop() {  
  distance1 = 0;
  distance2 = 0;
  for (int i = 0; i < iterations; i++)
  {
    // Output UltraSonic wave
    digitalWrite(trigPin, HIGH);
    
    // Scan for returned wave and measure pulse widths to calculate distance
    distance1 += pulseIn(echoPin1, HIGH) * 0.034 / 2.0;
    distance2 += pulseIn(echoPin2, HIGH) * 0.034 / 2.0;
  
    // Stop outputting UltraSonic wave
    digitalWrite(trigPin, LOW);
  }
  // Take average distance
  distance1 /= iterations;
  distance2 /= iterations;

  // Print
  Serial.println(distance1);
  Serial.println(distance2);

  // Compare distance to threshold
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);
  bool middle = left && right;
  
  // Clear haptic PWM signals
  digitalWrite(PWM_PIN_0, LOW);
  digitalWrite(PWM_PIN_1, LOW);
  digitalWrite(PWM_PIN_2, LOW);
  digitalWrite(PWM_PIN_3, LOW);
  digitalWrite(leftLED, LOW);
  digitalWrite(rightLED, LOW);
  digitalWrite(middleLED, LOW);
  
  // Trigger respective haptic motor and LED
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
  else {
    Serial.println("No obstacle");
    analogWrite(PWM_PIN_0, 0); 
    analogWrite(PWM_PIN_1, 0); 
    analogWrite(PWM_PIN_2, 0);
    analogWrite(PWM_PIN_3, 0);
    digitalWrite(leftLED, LOW);
    digitalWrite(rightLED, LOW);
    digitalWrite(middleLED, LOW);
  }
  
  //delay(delaytime); // Delay for stability
}
