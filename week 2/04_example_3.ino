
#define PIN_LED 13

unsigned int count;
bool toggle;
void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  while (!Serial) {
    ;
  }
  Serial.println("Hello World!");
  count = toggle = 0;
  digitalWrite(PIN_LED, toggle);
}

void loop() {
  Serial.println(++count);
  toggle = toggle_state(toggle);
  digitalWrite(PIN_LED, toggle);
  delay(1000);
}

bool toggle_state(bool toggle) { return !toggle; }
