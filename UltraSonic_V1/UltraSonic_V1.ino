// Ultrasonic pins
const int trigPin = 15; // Pin used to trigger ranging
const int echoPin1 = 2; // Ultrasonic 1 output 
const int echoPin2 = 4; // Ultrasonic 2 output 

const int threshold = 50; // Minimum distance for obstacle to be registered
const int delaytime = 500; // Delay between measurements
const int iterations = 1; // Averaging factor (number of measurements to be averaged per cycle)
int distance1, distance2;

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin1, INPUT); 
  pinMode(echoPin2, INPUT);  
  Serial.begin(9600); // Starts the serial communication
}

void loop() {
  // Clear trigPin
  digitalWrite(trigPin, LOW);
  delayMicroseconds(10);
  
  // Output UltraSonic wave
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(30);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(970);
  
  // Scan for returned wave and measure pulse widths to calculate distance
  distance1 = pulseIn(echoPin1, HIGH)* 0.034 / 2.0;
  distance2 = pulseIn(echoPin2, HIGH)* 0.034 / 2.0;
  Serial.println(distance1);
  Serial.println(distance2);

  // Compare distance to threshold
  bool middle = (distance1 < threshold) && (distance2 < threshold);
  bool left = (distance1 < threshold);
  bool right = (distance2 < threshold);

  // Trigger respective haptic motor
  if (middle) {
    Serial.println("middle");
    // Add haptic controls here
  }
 
  else if (left) {
    Serial.println("left");
    // Add haptic controls here
  }
  
  else if (right) {
    Serial.println("right");
    // Add haptic controls here
  }
  
  else {
    Serial.println("N/A");
    // Add haptic controls here
  }
}
