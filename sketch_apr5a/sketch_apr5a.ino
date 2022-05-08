#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>;
#include <Stepper.h>
#include <TimedAction.h>

const int stepsPerRevolution = 2048;
Stepper myStepper = Stepper(stepsPerRevolution, 38, 37, 39, 36);

LiquidCrystal_I2C lcd(0x27, 16, 2);
LiquidCrystal_I2C lcd2(0x28,16,2);
const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {13, 12, 11, 10}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {9, 8, 7}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(70, 2, NEO_GRB + NEO_KHZ800);
int mainPathLength = 60;
char mainPath[] = "apaapaaepaasaaapaeaesaaapaaeapeaeaepaaapaaeaeepeaeaaepasaaaa";

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
  uint32_t colour;
  int pos;
  int money;
  int actionCards;
  
  char path[];
  
};

playerStructure players[4] = {{playerColours[0],0,0,0,mainPath},{playerColours[1],0,0,0,mainPath},{playerColours[2],0,0,0,mainPath},{playerColours[3],0,0,0,mainPath}};

int numPlayers = 0;


long randNumber;
 
void setup()
{  
  myStepper.setSpeed(15);
  pinMode(2,OUTPUT);
  pixels.setBrightness(200);
  pixels.begin();
  
  Serial.begin(9600);
  setBoard(pixels,mainPath,mainPathLength);
  
  lcd.begin();
  lcd.backlight();
  
  lcd2.begin();
  lcd2.backlight();
  beforeGame();

//  for (int i = 0; i < 4; i++){
//    players[i].colour = playerColours[i];
//    
//  }
}

void loop()
{
  myStepper.step(stepsPerRevolution * 10);
  delay(10000); 
  
}
void beforeGame(){
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("welcome to the ");
  lcd.setCursor(0,1);
  lcd.print("game of life");

  lcd2.clear();
  lcd2.setCursor(0,0);
  lcd2.print("press button to begin");
  getInput("#");

  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("choose how many");
  lcd.setCursor(0,1);
  lcd.print("players");

  lcd2.clear();
  lcd2.print("press 3 or 4");
  char temp = getInput("34");
  numPlayers = temp - '0';

  
}

void setBoard(Adafruit_NeoPixel pixels, char path[], int len){
  
  for (int i = 0; i < len; i++){
    if (path[i] == 'a'){pixels.setPixelColor(i,actionColour);}
    if (path[i] == 's'){pixels.setPixelColor(i,stopColour);}
    if (path[i] == 'e'){pixels.setPixelColor(i,eventColour);}
    if (path[i] == 'p'){pixels.setPixelColor(i,paydayColour);}
    
  }
  pixels.show();
}

char getInput(char options[]){
  
  bool done = false;
  char input = " ";
  while (done == false) {
    input = keypad.getKey();
    if (input){
      for (int i = 0; i <= sizeof(options); i++){
      
        if (options[i] == input){
          done == true;
          return input;
        }
      }
    }
    
  }
  
}

void pressButton(){

}

