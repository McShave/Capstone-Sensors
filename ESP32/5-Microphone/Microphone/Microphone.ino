void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(39, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.println(analogRead(39));
  delay(500); 
}

