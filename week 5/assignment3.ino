// Arduino pin assignment
#define PIN_LED 9
#define PIN_TRIG 12
#define PIN_ECHO 13

// configurable parameters
#define SND_VEL 346.0 // sound velocity at 24 celsius degree (unit: m/s)
#define INTERVAL 25 // sampling interval (unit: ms)
#define _DIST_MIN 100 // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300 // maximum distance to be measured (unit: mm)

float timeout; 
float dist_min, dist_max, dist_raw;
unsigned long last_sampling_time;
float scale, last_data;
int ldb;
void setup() {
  pinMode(PIN_LED,OUTPUT);
  pinMode(PIN_TRIG,OUTPUT);
  digitalWrite(PIN_TRIG, LOW); 
  pinMode(PIN_ECHO,INPUT);

  dist_min = _DIST_MIN; 
  dist_max = _DIST_MAX;
  timeout = (INTERVAL / 2) * 1000.0; 
  dist_raw = 0.0; 
  scale = 0.001 * 0.5 * SND_VEL;
  Serial.begin(115200);

  last_sampling_time = 0;
}

void loop() {
  if(millis() < last_sampling_time + INTERVAL) return;
  
  dist_raw = USS_measure(PIN_TRIG,PIN_ECHO);
  last_data=dist_raw;

  Serial.print("Min:0,");
  Serial.print("raw:");
  Serial.print(dist_raw);
  Serial.print(",");
  Serial.println("Max:400");
  dist_raw=round(dist_raw);
  if(dist_raw < dist_min || dist_raw > dist_max|| dist_raw==100|| dist_raw==300)
    ldb=0;
  if(dist_raw < 200) ldb=(dist_raw-100)*2.55;
  else if(dist_raw > 200) ldb=(100-(dist_raw-200))*2.55;
  else if (dist_raw == 200) ldb=255;
  analogWrite(PIN_LED, ldb);

  last_sampling_time += INTERVAL;
}

float USS_measure(int TRIG, int ECHO)
{
  float reading;
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);
  
  reading = pulseIn(ECHO, HIGH, timeout) * scale; // unit: mm
  if(reading < dist_min || reading > dist_max) reading = last_data; // return 0 when out of range.
  return reading;
}
