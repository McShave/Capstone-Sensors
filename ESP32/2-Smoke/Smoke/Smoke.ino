
int smokeA0 = 16;
int sensorThres = 400;

void setup() {
  // put your setup code here, to run once:
  pinMode(smokeA0, INPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int analogSensor = analogRead(smokeA0);

  Serial.print("Pin A0: ");
  Serial.println(analogSensor);
  // Checks if it has reached the threshold value
  if (analogSensor > sensorThres)
  {
    Serial.println("danger!");
  }
  delay(500);
}
