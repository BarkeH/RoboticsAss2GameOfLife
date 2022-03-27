include wire module
include liquid crystal I2C module
include fast LED module
include stepper module
//Define variables
 
integer btnPin
 
integer In1
integer In2
integer In3
integer In4
 
integer kPadPin // analog pin
 
//mainstrip is the long LED strip and it is the main path for the game, path 1 and path 2 are paths that split off from the main path
define mainStrip
define path1
define path2
 
define LEDtype WS2811
define colour order GRB
define largeLength  48
define smallLength 16
 
//array of the different strips
CRGB ledsMain of length largeLength
CRGB ledsPath1 of length smallLength
CRGB ledsPath2 of length smallLength
 
//arrays of the different tiles on the path
//example array ["a","a","p","a","a"] where a stands for action tile and p stands for payday
tilesMain of length largeLength
tilesPath1 of length smallLength
tilesPath2 of length smallLength
 
LiquidCrystalI2C lcd(0x27, 16, 2) // Liquid Crystal I2C is a variable type 0x27 is the address
 
const integer stepsPerTurn
const integer gearReduction
const integer stepsPerOutTurn = stepsPerTurn * gearReduction
 
char state[] is "beforeGame"
 
Stepper steppermotor with properties stepsPerOutTurn and pins [put pins here]
 
integer numPlayers = 0
 
integer numSteps = 0
 
struct {
   char[] title
   int pay
} jobType
 
jobType jobList[]
 
struct {
   char[] message
   char do
   int reward
} actionCardStruct
 
actionCardStruct actionCardList[]
 
struct {
   int money = 0
   char[] path = tilesMain
   CRGB lightStrip = ledsMain
   int position = 0
   int actionCards = 0
   CRGB colour
   jobType job = null
   bool retired = false
} playerType
 
playerType players[6]
player colours array[6] = {insert colours}
 
integer numRetired = 0
 
const integer thresholds of length 16 = {thresholds go here} // thresholds for keypad
const char keypad of length 16 = {characters go here}// characters on keypad
 
CRGB actionColour
CRGB paydayColour
CRGB stopColour
CRGB taxesColour
CRGB moneyColour
CRGB retireColour
 
 
function setup
   pinmode btnPin is input
  
   pinmode In1 is output
   pinmode In2 is output
   pinmode In3 is output
   pinmode In4 is output
 
   pinmode mainStrip is output
   pinmode path1 is output
   pinmode path2 is output
 
   lcd begin
   lcd backlight start
   lcd clear
   lcd cursor at (0,0)
 
   Fast LED add LEDs with properties LEDtype, mainstrip, colour order and variables ledsMain, largeLength
   Fast LED add LEDs with properties LEDtype, path1, colour order and variables ledsPath1, smallLength
   Fast LED add LEDs with properties LEDtype, path2, colour order and variables ledsPath2, smallLength
 
   setupBoard()
 
   Fast Led show
 
   steppermotor setspeed(60) //60 rpm
 
   for i = 0; i < 6; i++
       players[i].colour = colours[i]
 
function loop
   if state == "beforeGame"
       state = beforeGame()
   else if state == "duringGame"
       state = game()
   else if game == "endGame"
       endGame()
 
 
function beforeGame
   lcd print welcome message, press button to start
 
   waitForButtonPress()
 
   lcd clear
   lcd cursor(0,0)
   lcd print choose player number message
 
   while numPlayers == 0
       char value = getInput
       if value in ["3","4","5","6"]
           numPlayers = int(value)
  
   return "duringGame"
 
 
function game
   //choose jobs
   for i=0; i < numPlayers; i++
       job1 = random element of jobList
       job2 = random element of jobList
       lcd print choose job message for playeri and wait for button
       waitForButtonPress()
 
       lcd print job1.title: $job1.pay
       lcd print job2.title: $job2.pay
       int x = 0
       while x != "1" or x != "2"
           getInput()
      
       if x == 1
           player[i].job = job1
       else
           player[i].job == job2
      
   display different players colours to lcd
 
   integer currentTurn = 1
 
   while numRetired < numPlayers
       for i = 0; i < numPlayers; i++
           if players[i].retired == false
               lcd print player i turn message
               waitForButtonPress()
               score = spinWheel()
               movePlayer(players[i],score)
               lcd print players[i] stats
               waitForButtonPress()    
   return "endGame"
         
function endGame
   winner = 0
   winnernNumber
   for int i=0; i < largeLength; i++
       players[i].cash += players[i].actionCards * 100
       lcd display players[i] stats
       if winner < players[i].cash
           winner = players[i].cash
           winnernNumber = i
       waitForButtonPress()
   lcd print winner is player i+1
  
   restart arduino
 
function setupBoard
   for i = 0; i < largeLength; i++
      
       ledsMain[i] = squareColour(tilesMain,i)
       if i < smallLength
           ledsPath1[i] = squareColour(tilesPath1,i)
           ledsPath2[i] = squareColour(tilesPath2,i)
 
function getInput
   int value = analogRead(kPadPin)
 
   for i = 0; i < 16; i++
       if absolute value of (value - thresholds[i]) < 5
           return characters[i]
      
function spinWheel
   integer spin = random number between 10 and stepsPerOutTurn*3
   numSteps += spin
   steppermotor.step(spin)
   delay(3000)
   integer score = int((numSteps%stepsPerOutTurn)/stepsPerOutTurn*10)
   return score
 
function waitForButtonPress
   while digitalRead(btnPin) == LOW
       delay(2)
 
function squareColour(path,position)
   if path[position] == "a"
       return actionColour
   else if path[position] == "p"
       return = paydayColour
   else if path[position] == "s"
       return stopColour
   else if path[position] == "t"
       return taxesColour
   else if path[position] == "m"
       return moneyColour
   else if path[position] == "r"
       return retireColour
 
 
function movePlayer(player, score)
   for int i=0; i < score; i++
       player.lightStrip[i+player.position] = squareColour(player.path,player.position)
       player.position += 1
       player.lightStrip[i+player.position] = player.colour
       FastLED show
       delay(500)
       if player.path[player.position] == "s" or player.path[player.position] == "r"
           break
       else if player.path[player.position] == "p"
           player.cash += player.job.pay
 
 
   square = player.path[player.position]
   if square == "a"
       actionSquare(player)
   else if square == "s"
       stopSquare(player)
   else if square == "t"
       player.cash -= 100
   else if square == "m"
       player.cash += 300
   else if square == "r"
       retireSquare(player)
 
function retireSquare(player)
   player.retired = true
   if numRetired == 0
       player.cash += 400
   else if numRetired == 2
       player.cash += 200
   else if numRetired == 3 or numRetired == 4
       player.cash += 100
   numRetired ++
 
function actionSquare(player)
   actionCardChosen = random element in actionCardList
   lcd print actionCardChosen.message
   waitForButtonPress()
 
   if actionCardChosen.do == "n"
       player.cash += actionCardChosen.reward
   else if actionCardChosen.do == "w"
       otherPlayerNum = random number from 0 to numPlayers - 1
       otherPlayer = players[otherPlayerNum]
 
       lcd print player press button to spin wheel
       waitForButtonPress()
       playerScore = spinWheel()
       lcd print otherPlayer press button to spin wheel
       waitForButtonPress()
       otherPlayerScore = spinWheel()
 
       if playerScore > otherPlayerScore
           player.cash += actionCardChosen.reward
       else if playerScore < otherPlayerScore
           otherPlayer.cash += actionCardChosen.reward
       else
           player.cash += actionCardChosen.reward/2
           otherPlayer.cash += actionCardChosen.reward/2
   player.actionCards ++
 
 
  
function stopSquare(player)
   lcd print player press 1 for risky path press 2 for safe path
   choice = 0
   while choice != 1 and choice != 2
       choice = getInput()
   if choice == 1
       player.path = tilesPath1
   else
       player.path = tilesPath2
   player.position = 0
