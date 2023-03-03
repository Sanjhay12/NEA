

#include <ArduinoBLE.h>
#define Button_PIN 3

#include <SparkFun_RV8803.h> //Get the library here:http://librarymanager/All#SparkFun_RV-8803
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
RV8803 rtc;
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define NUMFLAKES     10 // Number of snowflakes in the animation example

#define LOGO_HEIGHT   16
#define LOGO_WIDTH    16
static const unsigned char PROGMEM logo_bmp[] =
{ 0b00000000, 0b11000000,
  0b00000001, 0b11000000,
  0b00000001, 0b11000000,
  0b00000011, 0b11100000,
  0b11110011, 0b11100000,
  0b11111110, 0b11111000,
  0b01111110, 0b11111111,
  0b00110011, 0b10011111,
  0b00011111, 0b11111100,
  0b00001101, 0b01110000,
  0b00011011, 0b10100000,
  0b00111111, 0b11100000,
  0b00111111, 0b11110000,
  0b01111100, 0b11110000,
  0b01110000, 0b01110000,
  0b00000000, 0b00110000 };

byte value = 0;
long start_time = 0;
String last_time = "";
int last_duration;
long prev_screen = 0;

void setup() {
  Serial.begin(9600);
  pinMode(3, INPUT_PULLUP);
  
  
  // begin initialization
  if (!BLE.begin()) {
    Serial.println("starting Bluetooth® Low Energy module failed!");
    while (1);
  }

  Serial.println("Bluetooth® Low Energy Central - Peripheral Explorer");

  // start scanning for peripherals
  BLE.scan();
  Wire.begin();
  rtc.begin();
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  display.display();
  delay(2000); // Pause for 2 seconds
}

void loop() {
  // check if a peripheral has been discovered
  BLEDevice peripheral = BLE.available();
  LCD_display();
  if (peripheral) {
   
    // see if peripheral is a LED
    if (peripheral.localName() == "key_chain") {
      // stop scanning
      BLE.stopScan();
      readHallSensor(peripheral);

      BLE.scan();
    
    
    }
  }
}

void readHallSensor(BLEDevice peripheral){
  if(peripheral.connect()){
    Serial.println("Connected");
  }
  else{
    Serial.println("Failed to connect!");
  }
  if (peripheral.discoverService("1811")){
    Serial.println("Discovered service");
  }
  else{
    Serial.println("Failed discovery");
  }
  BLECharacteristic HallValue = peripheral.characteristic("2A3F");
  Serial.println(HallValue.uuid());
  BLECharacteristic Reset = peripheral.characteristic("2A40");
  Serial.println(Reset.uuid());
  if(!HallValue.canSubscribe()){
    Serial.println("Hall value is not subscribable");
  }
  
  HallValue.subscribe();
  Reset.subscribe();
  bool screen_page = false;
  prev_screen = millis();
  while(peripheral.connected()){
    if(millis()-prev_screen > 3000){
      screen_page = !screen_page;
      prev_screen = millis();
    }
    if(!screen_page){
      LCD_display();
    }
    else{
      display_memory();
    }
    
    HallValue.readValue(value);
    Serial.println(value);
    if(value == 1){
      start_time = millis();
      last_time = rtc.stringTime();
    }
    while(value == 1){
      int time_elapsed = (millis()-start_time)/1000;
      Serial.println(time_elapsed);
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0,0);
      display.println("Your bag is currently open for" + (String)time_elapsed + "sec");
      display.display();
      
      digitalWrite(5, HIGH);
      HallValue.readValue(value);
      if(digitalRead(3)==0){
        Reset.writeValue((byte)1);
        digitalWrite(5,LOW);
        last_duration = time_elapsed;
      }
      
    }
  }
  peripheral.disconnect();
 
}
void LCD_display(){  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(10,0);
  if (rtc.updateTime() == true) //Updates the time variables from RTC
  {
    String currentDate = rtc.stringDateUSA(); //Get the current date in mm/dd/yyyy format (we're weird)
    //String currentDate = rtc.stringDate(); //Get the current date in dd/mm/yyyy format
    String currentTime = rtc.stringTime(); //Get the time
    display.println(currentTime);
    display.display();
  }
}

void display_memory(){
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextSize(2);
  display.println(last_time);
  display.println("Your bag was open for "+(String)last_duration + "sec");
  display.display();
}
