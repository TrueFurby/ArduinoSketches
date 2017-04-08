#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 3, /* data=*/ 4, /* CS=*/ 5, /* reset=*/ U8X8_PIN_NONE);

String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete
u8g2_uint_t height, width;

const int linenum = 8;
String lines = "";

#include <SoftwareSerial.h>
SoftwareSerial mySerial(A1, A2); // RX, TX

void setup() {
  pinMode(A0, OUTPUT);
  digitalWrite(A0, LOW);
  pinMode(6, OUTPUT);
  
  digitalWrite(6, HIGH);
  delay(20);
  digitalWrite(6, LOW);

  u8g2.begin(/*Select=*/ 9, /*Right/Next=*/ 7, /*Left/Prev=*/ 8, /*Up=*/ U8X8_PIN_NONE, /*Down=*/ U8X8_PIN_NONE, /*Home/Cancel=*/ A5);
  u8g2.setFont(u8g2_font_5x7_mf); // u8g2_font_6x10_tf
  height = 7;
  width = 5; //u8g2.getUTF8Width(text);
  u8g2.setFontMode(0);

  // initialize serial:
  Serial.begin(9600);
  // reserve 200 bytes for the inputString:
  int linecount = u8g2.getDisplayWidth()/width;
  inputString.reserve(linecount);
  lines.reserve(linecount*linenum);
  Serial.println("starting..");

  mySerial.begin(9600);

  refreshDisplay();
}

void loop() {  
  // print the string when a newline arrives:
  mySerialEvent();
  if (stringComplete) {
    Serial.print("INCOMING: ");
    Serial.print(inputString);
    lines = inputString + lines;

    int n = 0;
    for (int i = 0;i < lines.length();i++) {
      if (lines.charAt(i)=='\n') {
        n++;
      }
    }
    Serial.println(String(n)+" lines:");

    if (n > linenum) {
      Serial.println("removing oldest line");
      lines = lines.substring(0, lines.substring(0, lines.length()-1).lastIndexOf('\n'));
    }
    //Serial.println(lines);
    
    refreshDisplay();
    inputString = "";
    stringComplete = false;
  }
  delay(10);
}

void refreshDisplay() {
    //Serial.println("refresh display");
    int i;
    u8g2_uint_t x;  
    u8g2.firstPage();
    do {
      x = u8g2.getDisplayHeight()-1;
      i = 0;
      while( x > 0 && i < lines.length() ) { 
        //Serial.print("i: ");Serial.println(i);
        u8g2.setCursor(1, x);
        String l = lines.substring(i);
        int nl = l.indexOf('\n');
        l = lines.substring(i, i+nl);
        if (l.length() > 0) {
          u8g2.print(l.c_str());
        }
        i += nl + 1;
        x -= height;        
      } 
            
    } while ( u8g2.nextPage() );
    //Serial.println("refreshed");
}

/*
  SerialEvent occurs whenever a new data comes in the
 hardware serial RX.  This routine is run between each
 time loop() runs, so using delay inside loop can delay
 response.  Multiple bytes of data may be available.
 */
void mySerialEvent() {
  digitalWrite(A0, HIGH);
  while (mySerial.available()) {
    // get the new byte:
    char inChar = (char)mySerial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }else{
      digitalWrite(A0, LOW);
      delay(20);
      digitalWrite(A0, HIGH);
    }
  }
  digitalWrite(A0, LOW);
}


