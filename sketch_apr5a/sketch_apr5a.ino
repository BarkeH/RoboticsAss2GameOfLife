#include <Adafruit_NeoPixel.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>;
#include <AccelStepper.h>
#include <TimedAction.h>

#define motorPin1  40      // IN1 on the ULN2003 driver
#define motorPin2  39     // IN2 on the ULN2003 driver
#define motorPin3  41     // IN3 on the ULN2003 driver
#define motorPin4  38     // IN4 on the ULN2003 driver

#define MotorInterfaceType 8

const int stepsPerRevolution = 2048;
AccelStepper stepper = AccelStepper(MotorInterfaceType, motorPin1, motorPin3, motorPin2, motorPin4);

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
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(70, 3, NEO_GRB + NEO_KHZ800);
int mainPathLength = 60;
char mainPath[] = "apaapaaepaasaaapaeaesaaapaaeapeaeaepaaapaaeaeepeaeaaepasaaaa";

char uniPath[] = "aaaaaaaaaa";
int uniPathLength = 10;
Adafruit_NeoPixel uniPixel = Adafruit_NeoPixel(10,4, NEO_GRB + NEO_KHZ800);


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
uint32_t tileColours[] = {actionColour,stopColour,paydayColour,eventColour};

struct jobStructure {
  String title;
  int pay;
};

struct playerStructure {
  uint32_t colour;
  int pos;
  int money;
  int actionCards;
  String readableColour;
  bool educated;
  bool finished;
  char path[];
  
};

playerStructure players[4] = {{playerColours[0],0,0,0,"blue",false,false,mainPath},{playerColours[1],0,0,0,"white",false,false,mainPath},{playerColours[2],0,0,0,"teal",false,false,mainPath},{playerColours[3],0,0,0,"pink",false,false,mainPath}};

int numPlayers = 0;
int currentTurn = 0;
bool finished = false;

long randNumber;

void flashLights(){
  for (int i = 0; i < 4; i++){
    if (players[i].pos == 0){continue;}
    for (int j = 0; j < 4-i; j++){
      if (i != j && players[i].pos == players[j].pos && players[i].path == players[j].path){
        Adafruit_NeoPixel pixel;
        if (players[i].path == mainPath){
          pixel = pixels;
        }else if (players[i].path == uniPath){
          pixel = uniPixel;
        }

        if (pixel.getPixelColor(players[i].pos) == players[i].colour) { pixel.setPixelColor(players[i].pos,players[j].colour);}
        else if (pixel.getPixelColor(players[i].pos) == players[j].colour) { setPixel(pixel,players[i].path,players[i].pos);}
        else {
          for (int k = 0; k < 4; k++){
            if (pixel.getPixelColor(players[i].pos) == players[i].colour) { pixel.setPixelColor(players[i].pos,players[j].colour); break;}
          }
        }
        
      }
    }
  }
}

TimedAction flashLightThread = TimedAction(500,flashLights);

void(* resetFunc) (void) = 0;
void setup()
{  
  stepper.setMaxSpeed(2200);
  stepper.setAcceleration(1500);
  
  pinMode(3,OUTPUT);
  pinMode(4,OUTPUT);

  uniPixel.setBrightness(50);
  uniPixel.begin();  
  setBoard(uniPixel,uniPath,uniPathLength);

  pixels.setBrightness(50);
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

void finishGame(){
  int currentMax = 0;
  int currentWinner = 0;
  for (int i = 0; i < numPlayers; i++){
    int money = 0;
    money +=  players[i].money;
    money += players[i].actionCards * 100;
    if (money > currentMax){ currentMax = money; currentWinner = i;}
    printBothLCD("Player " + String(i+1) " you", "have $" +String(money), "press # to", "continue");
    getInput("#"); 
  }
  printBothLCD("The winner is","Player " + String(currentWinner + 1) +" with", "$" + String(currentMax), "Press # to", "restart");
  getInput("#");
  resetFunc();
}

void loop()
{
  if (finished == true){
    finishGame();
  }
  finished = true;
  for (int i = 0; i < numPlayers; i++){
    if (players[i].finished == false){
      finished = false;
      printBothLCD("Player " + String(i+1) + " it is", "your turn, press","# to spin","");
      getInput("#");
      int distance = spinWheel();
      for (int j=0; j < distance; j++){
        delay(500);
        players[i].pos ++;
        if (players[i].path == mainPath){
           if (players[i].pos == mainPathLength){players[i].finished = true;
           setPixel(pixels, mainPath, players[i].pos - 1);
           pixels.setPixelColor(players[i].pos,players[i].colour);
           
        }
        if (players[i].path[players[i].pos] == "s"){stopSquare(players[i]);}
        
        
      }
      if (players[i].path[players[i].pos] == "a"){actionSquare(players[i]);}
      if (players[i].path[players[i].pos] == "e"){eventSquare(players[i]);}

    }
  }
  
}

void actionSquare(playerStructure player){}
void eventSquare(playerStructure player){}

void beforeGame(){


  printBothLCD(
      "welcome to the ",
      "game of life",
      "press button",
      "to begin"
  );
  getInput("#");

  printBothLCD(
      "choose how many",
      "players",
      "press 3 or 4",
      ""
  );

  char temp = getInput("34");
  numPlayers = temp - '0';

  for (int i = 0; i < numPlayers; i++){
    printBothLCD(
        "Player " + String(i+1) + " Your",
        "colour is " + players[i].readableColour,
        "Colour is shown",
        "firsttilepress#"
    );

    pixels.setPixelColor(0,players[i].colour);
    pixels.clear();
    pixels.show();
    delay(1000);
    getInput("#");
  }

  for (int i = 0;i<numPlayers;i++){
      
      printBothLCD(
          "Player " + String(i+1) + " choose", 
          "uni or fastpath",
          "press 1 for uni", 
          "2 for fast path"
      );
      char in = getInput("12");
      if (in == '1'){
          players[i].path = uniPath;
          players[i].pos = 0;
          players[i].educated = true;
      }else if (in == '2'){
           
      }
  }
  
    
  
}


int spinWheel(){
  int result;
  randomSeed(analogRead(A0));
  int randNum = random(4200,20000);
  stepper.moveTo(randNum);
  stepper.runToPosition();
  delay(randNum/2+500);
  float decimal = ((float)randNum / 4200) * 3;
  result = floor((decimal-floor(decimal))*10);

  Serial.println(result);
  
  stepper.moveTo(0);
  stepper.runToPosition();
  
  return result;
}

void setBoard(Adafruit_NeoPixel pixels, char path[], int len){
  
  for (int i = 0; i < len; i++){
    Serial.println("hi");
    if (path[i] == 'a'){pixels.setPixelColor(i,actionColour);}
    if (path[i] == 's'){pixels.setPixelColor(i,stopColour);}
    if (path[i] == 'e'){pixels.setPixelColor(i,eventColour);}
    if (path[i] == 'p'){pixels.setPixelColor(i,paydayColour);}
    
  }
  pixels.show();
}
void setPixel(Adafruit_NeoPixel pixels, char path[], int i){
  if (path[i] == 'a'){pixels.setPixelColor(i,actionColour);}
  if (path[i] == 's'){pixels.setPixelColor(i,stopColour);}
  if (path[i] == 'e'){pixels.setPixelColor(i,eventColour);}
  if (path[i] == 'p'){pixels.setPixelColor(i,paydayColour);}
  pixels.show()
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



void printBothLCD(String line1, String line2, String line3, String line4){
    lcd.clear();
    lcd2.clear();

    lcd.setCursor(0,0);
    lcd2.setCursor(0,0);
    
    lcd.print(line1);
    lcd.setCursor(0,1);
    lcd.print(line2);

    lcd2.print(line3);
    lcd2.setCursor(0,1);
    lcd2.print(line4);
}
