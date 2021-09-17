#define PIN_LED 7
void setup() {
  // put your setup code here, to run once:
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, 0);
  
}
int i;
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(PIN_LED, 1);
  delay(1000);
  digitalWrite(PIN_LED, 0); 
  for(i=0;i<5;i++)
  {
    digitalWrite(PIN_LED, 1);
    delay(100);
    digitalWrite(PIN_LED, 0);
    delay(100);
  }
}
