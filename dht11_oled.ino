#include <dht11.h>

#include <Adafruit_SSD1306.h>

static const unsigned char PROGMEM smile16_glcd_bmp[] =
{ B00001111, B11110000,
  B00001111, B11110000,
  B00110000, B00001100,
  B00110000, B00001100,
  B11001100, B00110011,
  B11001100, B00110011,
  B11000000, B00000011,
  B11000000, B00000011,
  B11001100, B00110011,
  B11001100, B00110011,
  B11000011, B11000011,
  B11000011, B11000011,
  B00110000, B00001100,
  B00110000, B00001100,
  B00001111, B11110000,
  B00001111, B11110000 };

#define MEMSIZE 1024

#define SCR_H 64
#define SCR_W 128

#define DHT11PIN 2

#define OLED_RESET 4

#define TEMP_REFRESH 2250

Adafruit_SSD1306 display(OLED_RESET);
dht11 DHT11;

void setup()
{
  Serial.begin(115200);
  
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.clearDisplay();
  doDemo();
}

void loop()
{
  Serial.println("\n");

  int chk = DHT11.read(DHT11PIN);
  
  display.clearDisplay();
  display.setCursor(0, 0);
  
  Serial.print("Read:");
  switch (chk)
  {
    case DHTLIB_OK: 
    Serial.println("OK"); 
    break;
    case DHTLIB_ERROR_CHECKSUM: 
    Serial.println("CE"); 
//    display.println("Sensor checksum error");
    display.display();
    break;
    case DHTLIB_ERROR_TIMEOUT: 
    Serial.println("TO"); 
    display.println("Sensor time out error"); 
    display.display();
    break;
    default: 
    Serial.println("UE");
    display.println("Unknown error");
    display.display();
    break;
  }

  char buf[64];

  sprintf(buf, "Hum:  %d", (int)DHT11.humidity);
  display.println(buf);

  sprintf(buf, "Temp: %3dC, K: %3dK", (int)DHT11.temperature, (int)Kelvin(DHT11.temperature) );
  display.println(buf);

  sprintf(buf, "Dew:  %3dC", (int)dewPointFast(DHT11.temperature, DHT11.humidity));
  display.println(buf);

  Serial.print("Humidity (%): ");
  Serial.println((float)(DHT11.humidity), 2);
  Serial.print("Temp: (°C): ");
  Serial.print((float)(DHT11.temperature), 2);
  Serial.print(",  (°K): ");
  Serial.println(Kelvin(DHT11.temperature), 2);

  Serial.print("Dew PointFast (°C): ");
  Serial.println(dewPointFast(DHT11.temperature, DHT11.humidity));

  display.display();
  delay(TEMP_REFRESH);
}

double Kelvin(double celsius)
{
  return celsius + 273.15;
}

// dewPoint function NOAA
// reference (1) : http://wahiduddin.net/calc/density_algorithms.htm
// reference (2) : http://www.colorado.edu/geography/weather_station/Geog_site/about.htm
//
double dewPoint(double celsius, double humidity)
{
  // (1) Saturation Vapor Pressure = ESGG(T)
  double RATIO = 373.15 / (273.15 + celsius);
  double RHS = -7.90298 * (RATIO - 1);
  RHS += 5.02808 * log10(RATIO);
  RHS += -1.3816e-7 * (pow(10, (11.344 * (1 - 1/RATIO ))) - 1) ;
  RHS += 8.1328e-3 * (pow(10, (-3.49149 * (RATIO - 1))) - 1) ;
  RHS += log10(1013.246);

        // factor -3 is to adjust units - Vapor Pressure SVP * humidity
  double VP = pow(10, RHS - 3) * humidity;

        // (2) DEWPOINT = F(Vapor Pressure)
  double T = log(VP/0.61078);   // temp var
  return (241.88 * T) / (17.558 - T);
}

// delta max = 0.6544 wrt dewPoint()
// 6.9 x faster than dewPoint()
// reference: http://en.wikipedia.org/wiki/Dew_point
double dewPointFast(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity*0.01);
  double Td = (b * temp) / (a - temp);
  return Td;
}


void doDemo() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("Vomit SRAM Memory:");
  display.display();
  delay(2000);
  printMemory(100);
  delay(500);
  display.clearDisplay();
  display.println("Fin ~");
  display.display();
  delay(1000);
  display.clearDisplay();
  delay(500);
  display.println("Show Smiley Face:");
  display.display();
  delay(2000);
  display.clearDisplay();
  delay(500);
  display.drawBitmap(0, 0, smile16_glcd_bmp, 16, 16, 1);
  display.display();
  delay(1000);
  display.clearDisplay();
  delay(500);
  display.setCursor(0,0);
  display.println("Scroll Smiley Face:");
  display.display();
  delay(1000);
  moveSmileyFace(10);
}

void printMemory(int fpms) {
  for (int i = 0; i < MEMSIZE; i += SCR_H) {
    display.clearDisplay();
    delay(fpms);
    display.drawBitmap(0, 0, (uint8_t*)i, SCR_W, SCR_H, 1);
    display.display();
  }
}

void moveSmileyFace(int fpms) {
  for (int i = 0; i < SCR_W; i++) {
    display.clearDisplay();
    delay(fpms);
    display.drawBitmap(i, 8, smile16_glcd_bmp, 16, 16, 1);
    display.display();
  }
}

