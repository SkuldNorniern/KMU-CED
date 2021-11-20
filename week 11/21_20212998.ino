#include <Servo.h>

#define PIN_SERVO 10
#define PIN_IR A0
#define PIN_LED 9
#define _DUTY_MIN 553
#define _DUTY_MAX 2399// servo full clockwise position (0 degree)
const int dutyneu = 1410; // servo neutral position (90 degree)

int a, b; // unit: mm
Servo myservo;
static long apt = 0; 
// Set the sampling time
#define interval 2 // milliseconds
unsigned long oldmil;


int fc = 7; // cutoff frequency 5~10 Hz 정도 사용해보시기 바랍니다
float dt = interval/1000.0; // sampling time
float lambda = 2*PI*fc*dt;
float filter = 0.0, prev = 0.0;

void setup() {
// initialize GPIO pins
  pinMode(PIN_LED,OUTPUT);
  digitalWrite(PIN_LED, 1);
  myservo.attach(10); 
  myservo.writeMicroseconds(dutyneu);
  delay(1000);
// initialize serial port
  Serial.begin(115200);

  a = 96;
  b = 314;
}

float ir_distance(void){ // return value unit: mm
  float val;
  float volt = float(analogRead(PIN_IR));
  val = ((6762.0/(volt-9.0))-4.0) * 10.0;
  return val;
}

void loop() {
  float rdist = ir_distance();
  float calidist = 100 + 300.0 / (b - a) * (rdist - a);
  unsigned long dmil = 0; // clear last result
  unsigned long mil = millis();  
  if (mil != oldmil) { 
    dmil = mil-oldmil; 
    oldmil = mil;   
  } 
  
  apt -= dmil; 
  
  if (apt <= 0) {  
    apt += interval;  // 아날로그값 읽기
    filter = lambda / (1 + lambda) * calidist + 1 / (1 + lambda) * prev; //필터된 값
    prev = filter; // 센서 필터 이전값 업데이트
    Serial.print("min:0,max:500,dist:");
    Serial.print(rdist);
    Serial.print(",calidist:");
    Serial.print(calidist);
    Serial.print(",fileterd:");
    Serial.println(filter);
  }
  if(rdist > 156 && rdist <224) digitalWrite(PIN_LED, 0);
  else digitalWrite(PIN_LED, 255);
  if(filter>255)myservo.writeMicroseconds(1200);
  else if(filter<255)myservo.writeMicroseconds(1800);
  delay(20);
}
