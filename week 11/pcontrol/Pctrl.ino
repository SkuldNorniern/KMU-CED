#include <Servo.h>

#define PIN_SERVO 10
#define PIN_IR A0
#define PIN_LED 9
#define SVMN 553
#define SVMX 2399
#define DIST_10C 80
#define DIST_40C 370
float Kp = 2.2;                
float Ki = 0;                  
float Kd = 1;   
#define tgdist 23

const int dutyneu = 1410; // servo neutral position (90 degree)
#define MXM -35
#define MXP 45

static long apt = 0;
#define interval 2
unsigned long oldmil;

int fc = 15;
float dt = interval / 1000.0;
float lambda = 2 * PI * fc * dt;
float filter = 0.0, prev = 0.0;
float rdist,fdist,pt,it,dit,prevdist;

Servo myservo;
void setup()
{
    // initialize GPIO pins
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, 1);
    myservo.attach(10);
    myservo.writeMicroseconds(dutyneu);
    delay(1000);
    
        // initialize serial port
    Serial.begin(115200);
}



void loop()
{
    //float tt= millis();
    fdist = lpfilter();
    float curdif=tgdist-fdist;
    pt = Kp * curdif;               
    //it = kd * (curdif - prevdif)/tt;                                           
    //dit += ki * curdif*millis(); 
    if(pt> MXP) pt= MXP;
    else if(pt<MXM) pt= MXM;
    float ctrl = pt+1520; 
    //prevdist = fdist; 
    // if(ctrl> MXP) ctrl= MXP;
    //else if(ctrl<MXM) ctrl= MXM;
    //float con=servo_duty(87+ctrl*0.9);
    if(ctrl > 1670) duty_curr = 1670;
    else if(ctrl < 1150) duty_curr = 1150;

    Serial.print("Min:0,Low:200,dist:");
    Serial.print(fdist*10);
    Serial.print(",pterm:"); 
    Serial.print(pt);
    Serial.print(",duty_target:");
    Serial.print(tgdist*10);
    Serial.print(",dutycurr:");
    Serial.print(ctrl);
    Serial.println(",High:310,Max:2000");

        myservo.writeMicroseconds(ctrl); 

}

float ir_distance(void)
{ // return value unit: mm
    float val;
    float volt = float(analogRead(PIN_IR));
    val = ((6762.0 / (volt - 9.0)) - 4.0) ;
    float calidist = 100.0 + 300.0 / (DIST_40C - DIST_10C) * (val - DIST_10C);
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
int servo_duty(int a){
  return map(a,0,180,SVMN,SVMX);
}
