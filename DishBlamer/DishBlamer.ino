#include <Wtv020sd16p.h>
#include <PololuLedStrip.h>

#define LED_COUNT  22
#define RESET_PIN  2
#define CLOCK_PIN  3
#define DATA_PIN  4
#define BUSY_PIN  6

PololuLedStrip<5> ledStrip;
rgb_color colors[60];

Wtv020sd16p soundCtrl(RESET_PIN, CLOCK_PIN, DATA_PIN, BUSY_PIN);

uint8_t r = 100;
uint8_t g = 100;
uint8_t b = 0;

int sum = 0;
int counter = 0;
int threshold = 10;

int lastHeight = 0;

void setColors(int distanceRead) {
  distanceRead -= 150;
  float percentage = 1;
  if (distanceRead > 400) {
    percentage = 1;
  } else {
    percentage = (float) distanceRead / 400;
  }
  
  for (int i = 0; i < 38; i += 1) {
    colors[i] = (rgb_color){0, 0, 0};
  }
  
  int barAmount = LED_COUNT * (percentage);
  
  for (int i = 38; i < barAmount + 38; i += 1) {
    float half = LED_COUNT / 2;
    
    if ((i - 38) < half) {
      float percent = ((i - 38) / half);
      r = 255 - (25 * percent);
      g = 0 + (230 * percent);
    } else {
      float percent = (((i - 38) - half) / half);
      r = 230 - (230 * percent);
      g = 230 + (25 * percent);
    }
    colors[i] = (rgb_color){r, g, b};
  }
  
  for (int j = barAmount + 38; j < 60; j += 1) {
    colors[j] = (rgb_color){0, 0, 0};
  }
  
  ledStrip.write(colors, 60);
}

void setup()
{ 
  soundCtrl.reset();
  setColors(650);
  
  Serial.begin(9600);
}

unsigned char buf[16] = {0};
unsigned char len = 0;

int stabilizedHeight = 0;
boolean stabilized = false;

void loop()
{
  int val = analogRead(4);
  sum += val;
  counter += 1;
  
  if (abs(stabilizedHeight - val) < 25) {
    stabilized = true;
  } else {
    stabilizedHeight = val;
  }
  
  if (counter > threshold) {
    int dishes = sum / counter;
    
//    Serial.println("");
//    Serial.print("Dishes height:" );
//    Serial.println(dishes);
//    Serial.println("");
//    Serial.println("=======");

    setColors(dishes);

    counter = 0;
    sum = 0;
    
    if ((lastHeight - dishes) > 30 && dishes < 250) {
      if (stabilized) {
        soundCtrl.asyncPlayVoice(1);
        stabilized = false;
      }
      // Bad
      Serial.println("b");
      Serial.println("");
    } else if ((lastHeight - dishes) < -30) {
      if (stabilized) {
        soundCtrl.asyncPlayVoice(0);
        stabilized = false;
      }
      // Good
      Serial.println("g");
      Serial.println("");
    }
    
    lastHeight = dishes;
  }
  
  delay(150);
}

