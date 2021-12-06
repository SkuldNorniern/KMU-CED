#include <Servo.h>

#define PIN_SERVO 10
#define PIN_IR A0
#define PIN_LED 9
#define SVMN 553
#define SVMX 2399
#define DIST_10C 80
#define DIST_40C 370
float Kp = 0.1;              
float Ki = 0.0;                  
float Kd = 0.376;   
#define tgdist 230

const int dutyneu = 1410; // servo neutral position (90 degree)
#define MXM -35
#define MXP 45

static long apt = 0;
#define interval 2
unsigned long oldmil;

int fc = 20;
float dt = interval / 1000.0;
float lambda = 2 * PI * fc * dt;
float filter = 0.0, prev = 0.0;
float rdist,fdist,pt,it,dit,prevdif;

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
    float tt= millis();
    fdist = lpfilter();
    float curdif=tgdist-fdist;
    pt = Kp * curdif;               
    dit = Kd * (curdif - prevdif);                                           
    it = Ki * curdif;
    
    //if(pt> MXP) pt= MXP;
    //else if(pt<MXM) pt= MXM;
    float ctrl = 1410+pt+it+dit; 
    ctrl=SVMN + SVMX / (2257 - 845) * (ctrl - 845);
  
    if(ctrl > SVMX) ctrl = SVMX;
    if(ctrl < SVMN) ctrl = SVMN;

    Serial.print("dist_ir:");
    Serial.print(fdist);
    Serial.print(",pterm:"); 
    Serial.print(map(pt,-1000,1000,510,610));
    Serial.print(",dterm:"); 
    Serial.print(map(dit,-1000,1000,510,610));
    Serial.print(",duty_target:");
    Serial.print(map(tgdist,1000,2000,410,510));
    Serial.print(",dutycurr:");
    Serial.print(map(ctrl,1000,2000,410,510));
    Serial.println(",Min:100,Low:200,dist_target:255,High:310,Max:4100");
    
    myservo.writeMicroseconds(ctrl); 

}

float ir_distance(void)
{ // return value unit: mm
    float val;
    float volt = float(analogRead(PIN_IR));
    val = ((6762.0 / (volt - 9.0)) - 4.0)*10 ;
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
