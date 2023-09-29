#include <M5StickCPlus.h>
#include <M5GFX.h>

M5GFX display;
M5Canvas canvas(&display);

const int SCREEN_W = 135;  // screen width
const int SCREEN_H  = 240;  // screen height

const int ROAD_START = 10;
const int ROAD_WIDTH = 116;
const int DASH_COL_1_START = 45;
const int DASH_COL_2_START = 86;
const int GRASSWIDTH = 9;
const int DASHWIDTH = 6;
const int DASHHEIGHT = 18;
const int DASHGAP = 18;
const int LANEWIDTH = 35;
const int LANE_1_X = 15;
const int LANE_2_X = 56;
const int LANE_3_X = 97;

const uint8_t TRANSPARENT = display.color332(255, 255, 0); //yellow is transparent
const uint8_t ROADCOLOR = display.color332(192, 192, 192); //gray
const uint8_t GRASSCOLOR = display.color332(173, 255, 47); //green-yellow
const uint8_t DASHCOLOR = display.color332(255, 255, 255); //white
const uint8_t LINECOLOR = display.color332(0, 0, 0); //black
const uint8_t GLASSCOLOR = display.color332(0, 255, 255); //cyan

const uint8_t ENEMYCOLOR_1 = display.color332(0, 128, 128); //dark cyan
const uint8_t C19 = display.color332(0, 68, 68); //enemy 1 dark color
const uint8_t C20 = display.color332(58, 168, 198); //enemy 1 light color

const uint8_t ENEMYCOLOR_2 = display.color332(0, 0, 128); //navy
const uint8_t C29 = display.color332(0, 0, 88); //enemy 2 dark color
const uint8_t C30 = display.color332(0, 98, 168); //enemy 2 light color

const uint8_t ENEMYCOLOR_3 = display.color332(128, 128, 0); //olive
const uint8_t C39 = display.color332(88, 88, 0); //enemy 3 dark color
const uint8_t C40 = display.color332(168, 168, 0); //enemy 3 light color

uint8_t C8 = display.color332(0, 0, 255); //player color - blue (default)
uint8_t C9 = display.color332(0, 0, 188); //player dark color - darker blue (default)
uint8_t C10 = display.color332(0, 128, 255); //player light color - lighter blue (default)

bool gameStarted = false;
bool gameOver = false;
bool waiting_for_input = false;
bool pause_scoring = true;
bool gamePaused = false;

int prevpoints = 0;
int points = 0;
int PLAYER_X = 56;
const int PLAYER_Y = 179;
const int PLAYER_WIDTH = 24;
const int PLAYER_HEIGHT = 51;
int speed = 0;
int prevSpeed = 0;
const int max_speed = 750;
const int dash_speed_increment = 100;

int dash_Y = 10; //default start for game begin
int prev_dash_Y = 10;

const int enemy_start_Y = -100;
int enemy_speed = 1;
int enemy_1_Y = 0; //place way off screen
int enemy_1_X = 0;
int enemy_2_Y = 0; //place way off screen
int enemy_2_X = 0;
int enemy_1_type = 0;
int enemy_2_type = 0;
int enemy_2_active = 0;

const float GAME_TICK = 25.0; // 1000 / 25 = 40fps

int battery_timer = 0;
double vbat = 0.0;
double prevbat = 0.0;
int discharge;

int batteryX = 95;
int batteryY = 5;

float accX = 0;
float accY = 0;
float accZ = 0;
float prevaccX = 0;

/*
TODO: add function to change player color on button press
lcd.color565(0, 255, 0) //green
lcd.color565(255, 0, 0)//red
lcd.color565(255, 215, 0)//gold
lcd.color565(255, 192, 203)//pink
lcd.color565(255, 0, 255)//magenta
*/

# define C0 TRANSPARENT
# define C1 GRASSCOLOR
# define C2 DASHCOLOR
# define C3 LINECOLOR
# define C4 GLASSCOLOR
# define C18 ENEMYCOLOR_1
# define C28 ENEMYCOLOR_2
# define C38 ENEMYCOLOR_3

const uint8_t enemySprite3[] = {
  C0, C0, C0, C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C0, C3, C3, C39, C39, C39, C39, C39, C39, C39, C39, C39, C39, C3, C3, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C3, C39, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C0, C0, C0, C0,
  C0, C0, C0, C3, C38, C40, C40, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C38, C38, C39, C3, C0, C0, C0,
  C0, C0, C3, C38, C40, C3, C3, C40, C40, C38, C38, C38, C38, C38, C39, C39, C39, C3, C3, C38, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C38, C3, C0, C0,
  C0, C0, C3, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C38, C38, C38, C38, C3, C3, C3, C3, C3, C3, C38, C38, C38, C38, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C40, C3, C3, C3, C4, C4, C4, C4, C4, C4, C3, C3, C3, C39, C39, C39, C3, C0, C0,
  C0, C0, C3, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C0,
  C3, C38, C38, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C38, C38, C3,
  C0, C3, C3, C4, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C4, C3, C3, C0,
  C0, C0, C3, C4, C3, C4, C4, C4, C4, C3, C3, C3, C3, C3, C3, C4, C4, C4, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C4, C3, C3, C3, C38, C38, C38, C38, C38, C38, C3, C3, C3, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C40, C40, C40, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C3, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C39, C3, C3, C0, C0,
  C0, C0, C3, C40, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C3, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C40, C3, C40, C40, C40, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C39, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C3, C3, C3, C3, C38, C38, C38, C38, C38, C38, C3, C3, C3, C3, C3, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C38, C38, C3, C3, C3, C3, C3, C3, C38, C38, C39, C39, C3, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C3, C39, C3, C0, C0,
  C0, C0, C3, C40, C3, C40, C40, C40, C38, C38, C38, C38, C38, C38, C38, C38, C39, C39, C39, C3, C39, C3, C0, C0,
  C0, C0, C0, C3, C40, C3, C40, C40, C40, C38, C38, C38, C38, C39, C39, C39, C39, C39, C3, C38, C3, C0, C0, C0,
  C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0
};

const uint8_t enemySprite2[] = {
  C0, C0, C0, C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C0, C3, C3, C29, C29, C29, C29, C29, C29, C29, C29, C29, C29, C3, C3, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C3, C29, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C0, C0, C0, C0,
  C0, C0, C0, C3, C28, C30, C30, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C28, C28, C29, C3, C0, C0, C0,
  C0, C0, C3, C28, C30, C3, C3, C30, C30, C28, C28, C28, C28, C28, C29, C29, C29, C3, C3, C28, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C28, C3, C0, C0,
  C0, C0, C3, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C28, C28, C28, C28, C3, C3, C3, C3, C3, C3, C28, C28, C28, C28, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C30, C3, C3, C3, C4, C4, C4, C4, C4, C4, C3, C3, C3, C29, C29, C29, C3, C0, C0,
  C0, C0, C3, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C0,
  C3, C28, C28, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C28, C28, C3,
  C0, C3, C3, C4, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C4, C3, C3, C0,
  C0, C0, C3, C4, C3, C4, C4, C4, C4, C3, C3, C3, C3, C3, C3, C4, C4, C4, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C4, C3, C3, C3, C28, C28, C28, C28, C28, C28, C3, C3, C3, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C30, C30, C30, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C3, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C29, C3, C3, C0, C0,
  C0, C0, C3, C30, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C3, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C30, C3, C30, C30, C30, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C29, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C3, C3, C3, C3, C28, C28, C28, C28, C28, C28, C3, C3, C3, C3, C3, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C28, C28, C3, C3, C3, C3, C3, C3, C28, C28, C29, C29, C3, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C3, C29, C3, C0, C0,
  C0, C0, C3, C30, C3, C30, C30, C30, C28, C28, C28, C28, C28, C28, C28, C28, C29, C29, C29, C3, C29, C3, C0, C0,
  C0, C0, C0, C3, C30, C3, C30, C30, C30, C28, C28, C28, C28, C29, C29, C29, C29, C29, C3, C28, C3, C0, C0, C0,
  C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0
};

const uint8_t enemySprite1[] = {
  C0, C0, C0, C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C0, C3, C3, C19, C19, C19, C19, C19, C19, C19, C19, C19, C19, C3, C3, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C3, C19, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C0, C0, C0, C0,
  C0, C0, C0, C3, C18, C20, C20, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C18, C18, C19, C3, C0, C0, C0,
  C0, C0, C3, C18, C20, C3, C3, C20, C20, C18, C18, C18, C18, C18, C19, C19, C19, C3, C3, C18, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C18, C3, C0, C0,
  C0, C0, C3, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C18, C18, C18, C18, C3, C3, C3, C3, C3, C3, C18, C18, C18, C18, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C20, C3, C3, C3, C4, C4, C4, C4, C4, C4, C3, C3, C3, C19, C19, C19, C3, C0, C0,
  C0, C0, C3, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C0,
  C3, C18, C18, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C18, C18, C3,
  C0, C3, C3, C4, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C4, C3, C3, C0,
  C0, C0, C3, C4, C3, C4, C4, C4, C4, C3, C3, C3, C3, C3, C3, C4, C4, C4, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C4, C3, C3, C3, C18, C18, C18, C18, C18, C18, C3, C3, C3, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C20, C20, C20, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C3, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C19, C3, C3, C0, C0,
  C0, C0, C3, C20, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C3, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C20, C3, C20, C20, C20, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C19, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C3, C3, C3, C3, C18, C18, C18, C18, C18, C18, C3, C3, C3, C3, C3, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C18, C18, C3, C3, C3, C3, C3, C3, C18, C18, C19, C19, C3, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C3, C19, C3, C0, C0,
  C0, C0, C3, C20, C3, C20, C20, C20, C18, C18, C18, C18, C18, C18, C18, C18, C19, C19, C19, C3, C19, C3, C0, C0,
  C0, C0, C0, C3, C20, C3, C20, C20, C20, C18, C18, C18, C18, C19, C19, C19, C19, C19, C3, C18, C3, C0, C0, C0,
  C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0
};

uint8_t playerSprite[] = {
  C0, C0, C0, C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C0, C3, C3, C9, C9, C9, C9, C9, C9, C9, C9, C9, C9, C3, C3, C0, C0, C0, C0, C0,
  C0, C0, C0, C0, C3, C9, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C0, C0, C0, C0,
  C0, C0, C0, C3, C8, C10, C10, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C8, C8, C9, C3, C0, C0, C0,
  C0, C0, C3, C8, C10, C3, C3, C10, C10, C8, C8, C8, C8, C8, C9, C9, C9, C3, C3, C8, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C8, C3, C0, C0,
  C0, C0, C3, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C8, C8, C8, C8, C3, C3, C3, C3, C3, C3, C8, C8, C8, C8, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C10, C3, C3, C3, C4, C4, C4, C4, C4, C4, C3, C3, C3, C9, C9, C9, C3, C0, C0,
  C0, C0, C3, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C0, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C0, C0,
  C0, C3, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C3, C0,
  C3, C8, C8, C3, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C3, C8, C8, C3,
  C0, C3, C3, C4, C3, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C4, C3, C4, C3, C3, C0,
  C0, C0, C3, C4, C3, C4, C4, C4, C4, C3, C3, C3, C3, C3, C3, C4, C4, C4, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C4, C3, C3, C3, C8, C8, C8, C8, C8, C8, C3, C3, C3, C4, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C10, C10, C10, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C3, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C4, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C4, C3, C0, C0,
  C0, C0, C3, C3, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C9, C3, C3, C0, C0,
  C0, C0, C3, C10, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C3, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C10, C3, C10, C10, C10, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C9, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C3, C3, C3, C3, C8, C8, C8, C8, C8, C8, C3, C3, C3, C3, C3, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C8, C8, C3, C3, C3, C3, C3, C3, C8, C8, C9, C9, C3, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C3, C9, C3, C0, C0,
  C0, C0, C3, C10, C3, C10, C10, C10, C8, C8, C8, C8, C8, C8, C8, C8, C9, C9, C9, C3, C9, C3, C0, C0,
  C0, C0, C0, C3, C10, C3, C10, C10, C10, C8, C8, C8, C8, C9, C9, C9, C9, C9, C3, C8, C3, C0, C0, C0,
  C0, C0, C0, C0, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C3, C0, C0, C0, C0
};

void draw_battery(int b) {

  /*
  //hide lightning bolt
  M5.Lcd.fillRect(batteryX + 5, batteryY - 5, 20, 31, BLACK);
  */

  //draw basics
  canvas.fillRoundRect(batteryX, batteryY, 30, 18, 3, BLACK);
  canvas.drawRoundRect(batteryX, batteryY, 30, 18, 3, WHITE);
  canvas.drawRect(batteryX + 29, batteryY + 3, 4, 12, WHITE);
  canvas.fillRect(batteryX + 29, batteryY + 4, 3, 10, BLACK);
  canvas.drawRoundRect(batteryX + 2, batteryY + 3, 8, 12, 1, WHITE);
  canvas.drawRoundRect(batteryX + 11, batteryY + 3, 8, 12, 1, WHITE);
  canvas.drawRoundRect(batteryX + 20, batteryY + 3, 8, 12, 1, WHITE);

  //fill the bars based on battery level
  canvas.fillRoundRect(batteryX + 3, batteryY + 4, 6, 10, 1, GREEN);

  if (b >= 2) {
    canvas.fillRoundRect(batteryX + 12, batteryY + 4, 6, 10, 1, GREEN);
  }

  if (b == 3) {
    canvas.fillRoundRect(batteryX + 21, batteryY + 4, 6, 10, 1, GREEN);
  }
  /*
  //if charging, draw lightning bolt
  if (M5.Axp.GetIchargeData() > 0) {
    //black sillouhettes
    M5.Lcd.fillTriangle(batteryX + 20, batteryY - 8, batteryX + 1, batteryY + 12, batteryX + 19, batteryY + 12, BLACK);
    M5.Lcd.fillTriangle(batteryX + 10, batteryY + 28, batteryX + 29, batteryY + 8, batteryX + 11, batteryY + 8, BLACK);
    //yellow foreground
    M5.Lcd.fillTriangle(batteryX + 19, batteryY - 5, batteryX + 5, batteryY + 10, batteryX + 17, batteryY + 10, YELLOW);
    M5.Lcd.fillTriangle(batteryX + 11, batteryY + 25, batteryX + 25, batteryY + 10, batteryX + 13, batteryY + 10, YELLOW);
  }
  */
}

void battery_status() {
  vbat = M5.Axp.GetVbatData() * 1.1 / 1000;
  if (vbat >= 4) {
    draw_battery(3);
  } else if (vbat >= 3.7) {
    draw_battery(2);
  } else if (vbat < 3.7) {
    draw_battery(1);
  } else {
    return;
  }
}

void draw_background(){
  //draw grass
  canvas.fillScreen(GRASSCOLOR);
  //draw road
  canvas.fillRect(ROAD_START,0,ROAD_WIDTH,SCREEN_H,ROADCOLOR);
}

void draw_dashes(){

  if (!gamePaused){
    dash_Y = prev_dash_Y + (speed / dash_speed_increment);
  } else {
    dash_Y = prev_dash_Y;
  }

  if (dash_Y >= (DASHGAP + DASHHEIGHT)){
    dash_Y = 0;
  }

  prev_dash_Y = dash_Y;

  while (dash_Y <= SCREEN_H){
    //draw partial dashes at top
    if (((dash_Y - DASHGAP) > 0) && ((DASHGAP + DASHHEIGHT) > dash_Y)){
      //column 1
      canvas.fillRect(DASH_COL_1_START,0,DASHWIDTH,(dash_Y - DASHGAP),DASHCOLOR);
      //column 2
      canvas.fillRect(DASH_COL_2_START,0,DASHWIDTH,(dash_Y - DASHGAP),DASHCOLOR);
    }

    //draw partial dashes at bottom
    if ((dash_Y + DASHHEIGHT)>SCREEN_H){
      //column 1
      canvas.fillRect(DASH_COL_1_START,dash_Y,DASHWIDTH,(SCREEN_H - dash_Y),DASHCOLOR);
      //column 2
      canvas.fillRect(DASH_COL_2_START,dash_Y,DASHWIDTH,(SCREEN_H - dash_Y),DASHCOLOR);
      //drawn the last dashes, exit code
      return;
    }

    //draw full dashes
    if ((dash_Y + DASHHEIGHT)<=SCREEN_H){
      //column 1
      canvas.fillRect(DASH_COL_1_START,dash_Y,DASHWIDTH,DASHHEIGHT,DASHCOLOR);
      //column 2
      canvas.fillRect(DASH_COL_2_START,dash_Y,DASHWIDTH,DASHHEIGHT,DASHCOLOR);
    }
    dash_Y = dash_Y + DASHHEIGHT + DASHGAP;
  }
}

void draw_enemies(){

  if (enemy_1_Y > SCREEN_H){

    pause_scoring = false;
    enemy_1_Y = -PLAYER_HEIGHT;
    enemy_2_Y = -PLAYER_HEIGHT;
    enemy_2_active = random(0,2);

    enemy_1_X = random(1,4); //find column, assign x later
    enemy_1_type = random(1,4); //get image type

    switch (enemy_1_X){
      case 1:
        enemy_1_X = LANE_1_X;
        break;
      case 2:
        enemy_1_X = LANE_2_X;
        break;
      case 3:
        enemy_1_X = LANE_3_X;
        break;
    }

    if (enemy_2_active == 1){
      enemy_2_X = random(1,4);
      enemy_2_type = random(1,4); //get image type

      switch (enemy_2_X){
        case 1:
          enemy_2_X = LANE_1_X;
          break;
        case 2:
          enemy_2_X = LANE_2_X;
          break;
        case 3:
          enemy_2_X = LANE_3_X;
          break;
      }
    }
  }

  if (enemy_1_Y <= SCREEN_H){
    //draw enemies
    switch (enemy_1_type){
      case 1:
        canvas.pushImage(enemy_1_X,enemy_1_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite1,TRANSPARENT);
        break;
      case 2:
        canvas.pushImage(enemy_1_X,enemy_1_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite2,TRANSPARENT);
        break;
      case 3:
        canvas.pushImage(enemy_1_X,enemy_1_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite3,TRANSPARENT);
        break;
    }

    if ((enemy_2_active == 1) && (enemy_2_Y <= SCREEN_H)){
      switch (enemy_2_type){
        case 1:
          canvas.pushImage(enemy_2_X,enemy_2_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite1,TRANSPARENT);
          break;
        case 2:
          canvas.pushImage(enemy_2_X,enemy_2_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite2,TRANSPARENT);
          break;
        case 3:
          canvas.pushImage(enemy_2_X,enemy_2_Y,PLAYER_WIDTH,PLAYER_HEIGHT,enemySprite3,TRANSPARENT);
          break;
      }
    }
  }

  if (!gamePaused){
    enemy_1_Y += enemy_speed;
    if (enemy_2_active == 1){
      enemy_2_Y += enemy_speed;
    }
  }

}


void draw_player(){
  canvas.pushImage(PLAYER_X, PLAYER_Y,PLAYER_WIDTH,PLAYER_HEIGHT,playerSprite,TRANSPARENT);
}

void game_setup(){
  if (gameStarted){
    return;
  }

  if (!waiting_for_input){
    //paint initial scene
    draw_background();
    draw_dashes();
    draw_player();

    canvas.setTextColor(BLACK);
    canvas.setTextSize(3);
    canvas.drawCenterString("PRESS A",68,40);
    canvas.drawCenterString("TO",68,80);
    canvas.drawCenterString("START",68,120);

    waiting_for_input = true;
  }
}

void check_IMU() {
  M5.IMU.getAccelData(&accX, &accY, &accZ);
}


void game_loop(){
  if ((!gameStarted) || (gameOver)){
    return;
  }

  double next_game_tick;
  next_game_tick = millis()+GAME_TICK;

  while (millis()>next_game_tick){
    //wait for the next frame
  }

  if (!gamePaused) {

    check_IMU();

    if (abs(accX)>0.1){
      if (accX < 0){
        //tilting right
        if (PLAYER_X + enemy_speed <= (ROAD_START + ROAD_WIDTH - PLAYER_WIDTH)) {
          PLAYER_X = PLAYER_X + enemy_speed;
        } else {
          PLAYER_X = ROAD_START + ROAD_WIDTH - PLAYER_WIDTH;
        }
      } else {
        //tilting left
        if (PLAYER_X - enemy_speed >= ROAD_START){
          PLAYER_X = PLAYER_X - enemy_speed;
        } else {
          PLAYER_X = ROAD_START;
        }
      }
    }

    if (speed==0){
      speed = 100;
    }

    if (speed != max_speed){
      speed += 1;
    }

  }

  draw_background();
  draw_dashes();
  draw_enemies();
  draw_player();
  show_points();
  
  if (gamePaused){
    canvas.setTextSize(3);
    canvas.drawCenterString("GAME",68,50);
    canvas.drawCenterString("PAUSED",68,90);
  }

}

void game_over(){
  
  //check for collision
  if ((gameStarted) && (PLAYER_Y <= enemy_1_Y + PLAYER_HEIGHT) && (enemy_1_Y <= PLAYER_Y + PLAYER_HEIGHT) ){
    //check enemy 1 first
    if ((enemy_1_X<= PLAYER_X + PLAYER_WIDTH - 4) && (enemy_1_X + PLAYER_WIDTH - 4 >= PLAYER_X)){
      gameOver = true;
    }

    if ((enemy_2_X<= PLAYER_X + PLAYER_WIDTH - 4) && (enemy_2_X + PLAYER_WIDTH - 4 >= PLAYER_X) && (enemy_2_active == 1)) {
      gameOver = true;
    }
  }
  
  if (!gameOver){
    return;
  }

  if (gameOver){
    speed = 0;
    canvas.setTextSize(3);
    canvas.drawCenterString("GAME",68,50);
    canvas.drawCenterString("OVER",68,90);
  }
}

void draw_battery(){
  battery_status();
}

void check_buttons(){

  M5.update();

  if ((M5.BtnA.wasReleased()) && (waiting_for_input) && (!gameStarted)){
    //start game
    gameStarted = true;
    return;
  }

  if ((M5.BtnA.wasReleased()) && (gameOver)){
    //play again
    game_reset();
    return;
  }

  if ((M5.BtnA.wasReleased()) && (!gamePaused) && (gameStarted) && (!gameOver)){
    //pause
    prevSpeed = speed;
    speed = 0;
    gamePaused = true;
    return;
  }

  if ((M5.BtnA.wasReleased()) && (gamePaused) && (gameStarted) && (!gameOver)){
    //un-pause
    speed = prevSpeed;
    gamePaused = false;
    return;
  }

  if ((M5.BtnA.wasReleasefor(1000)) && (gamePaused) && (gameStarted) && (!gameOver)){
    //restart
    game_reset();
    return;
  }

}

void game_reset(){
  enemy_speed = 1;
  enemy_1_Y = enemy_start_Y;
  enemy_1_X = 0;
  enemy_2_Y = enemy_start_Y;
  enemy_2_X = 0;
  enemy_1_type = 0;
  enemy_2_type = 0;
  enemy_2_active = 0;
  gameOver = false;
  gameStarted = false;
  waiting_for_input = false;
  points = 0;
  gamePaused = false;
  pause_scoring = true;
}

void show_points(){

  if ((!gameOver) && (gameStarted)){
    if ((enemy_1_Y > PLAYER_Y + PLAYER_HEIGHT) && (!pause_scoring)){
      points += 1;
      if (enemy_2_active == 1){
        points += 1;
      }
      pause_scoring = true;
    }
    if ((points != prevpoints) && (points != 0) && (points % 10 == 0)){
      enemy_speed += 1;
      prevpoints = points;
    }
  }

  canvas.setTextSize(3);
  canvas.drawString(String(points),10,5);
}

void setup() {
  M5.begin();
  display.begin();
  canvas.createSprite(135,240);
  canvas.setColorDepth(16);
  M5.IMU.Init();
  display.setRotation(0);
  game_reset();
}

void loop() {
  game_over();
  game_setup();
  check_buttons();
  game_loop();
  draw_battery();
  
  //refresh display
  display.startWrite(); 
  canvas.pushSprite(0, 0,TRANSPARENT);
  display.endWrite();
}