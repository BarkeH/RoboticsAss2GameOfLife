#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(70, 2, NEO_GRB + NEO_KHZ800);
int mainPathLength = 60;
String mainPath[] = {
  "a","p","a","a","p","a","a","e","p","a","a","s","a","a","a","p","a","e","a","e","s","a","a","a","p","a","a","e","a","p","e","a","e","a","e","p","a","a","a",
  "p","a","a","e","a","e","e","p","e","a","e","a","a","e","p","a","s","a","a","a","a"
 };

uint32_t red = pixels.Color(255,0,0);
uint32_t green = pixels.Color(0,255,0);
uint32_t blue = pixels.Color(0,0,255);
uint32_t yellow = pixels.Color(255,255,0);
uint32_t pink = pixels.Color(255,0,100);
uint32_t test = pixels.Color(10,0,5);
uint32_t brown = pixels.Color(128,64,0);
uint32_t teal = pixels.Color(0,128,128);
uint32_t grey = pixels.Color(20,20,20);
uint32_t white = pixels.Color(255,255,255);

uint32_t colour[] = {red, green, blue, yellow, pink, test, brown, teal, grey, white};
uint32_t playerColours[] = {blue,white,teal,pink};
uint32_t actionColour = yellow;
uint32_t stopColour = red;
uint32_t paydayColour = green;
uint32_t eventColour = grey;

struct jobStructure {
  String title;
  int pay;
};

struct playerStructure {
  uint32_t playerColour;
  int pos;
  int money;
  int actionCards;
  
  String path[];
  
};



long randNumber;
 
void setup()
{  
  pinMode(2,OUTPUT);
  pixels.setBrightness(200);
  pixels.begin();
  
  Serial.begin(9600);
  setBoard(pixels,mainPath,mainPathLength);
  
}

void loop()
{
  
 
  
}
void setBoard(Adafruit_NeoPixel pixels, String path[], int len){
  
  for (int i = 0; i < len; i++){
    if (path[i] == "a"){pixels.setPixelColor(i,actionColour);}
    if (path[i] == "s"){pixels.setPixelColor(i,stopColour);}
    if (path[i] == "e"){pixels.setPixelColor(i,eventColour);}
    if (path[i] == "p"){pixels.setPixelColor(i,paydayColour);}
    pixels.show();
  }
}
