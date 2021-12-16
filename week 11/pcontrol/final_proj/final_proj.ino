#include <PID_v1.h>
#include <Servo.h>
#define PIN_IR A0
#define MG946_Pin 10
#define MG946_L 580
#define MG946_M 1450
#define MG946_H 2200

Servo MG946;

float Kp = 2.5;                //P게인 값
float Ki = 0;                  //I게인 값 
float Kd = 1;                  //D게인 값
double Setpoint, Input, Output, ServoOutput;                                       
#define DIST_10C 8
#define DIST_40C 36.5
static long apt = 0;
#define interval 2
unsigned long oldmil;

int fc = 9;
float dt = interval / 1000.0;
float lambda = 2 * PI * fc * dt;
float filter = 0.0, prev = 0.0;
float rdist,fdist,pt,it,dit,prevdif;

int servo_duty(int a){
  return map(a,0,180,MG946_L,MG946_H);
}



PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);   //PID객체 생성

void setup() {
  Serial.begin(115200);
  MG946.attach(MG946_Pin);
  MG946.writeMicroseconds(MG946_M); //초기화 각도삽입

  myPID.SetMode(AUTOMATIC);               //PID모드를 AUTOMATIC으로 설정
  myPID.SetOutputLimits(-30,50);          //PID의 값을 최소 -80부터 최대 80까지 설정
}
                   //막대 중앙 위치(Set Point를 15cm로 설정)
void loop() {
  Setpoint = 23;
  float dist = lpfilter();
  Input = dist;
  myPID.Compute(); 
  ServoOutput=servo_duty(87+Output);
  Serial.print("Min:0,Low:200,dist:");
  Serial.print(dist*10);
  Serial.print(",pterm:"); 
  Serial.print(myPID.GetKp());
  Serial.print(",duty_target:");
  Serial.print(Setpoint*10);
  Serial.print(",dutycurr:");
  Serial.print(ServoOutput);
  Serial.println(",High:310,Max:2000");
  MG946.writeMicroseconds(ServoOutput);
  
}
float ir_distance(void){ // return value unit: mm
  float irval=float(analogRead(PIN_IR));
  float tmp = ((6762.0/(irval-9.0))-4.0);
  float calidist = 10.0+30.0/(DIST_40C-DIST_10C)*(tmp-DIST_10C);
  return calidist;
}
float lpfilter()
{
    unsigned long dmil = 0; 
    unsigned long mil = millis();
    if (mil != oldmil)
    {
        dmil = mil - oldmil;
        oldmil = mil;
    }
    apt -= dmil;
    if (apt <= 0)
    {
        apt += interval;
        filter = lambda / (1 + lambda) *  ir_distance()+ 1 / (1 + lambda) * prev; //필터된 값
        prev = filter;
    }
    return filter;
}
