/*
 * Pong
 * Original Code from https://github.com/rparrett/pongclock
 *
 */

// Demo only - not playable

#define BLACK 0x0000
#define WHITE 0xFFFF
#define GREY  0x5AEB

#include <TFT_eSPI.h> // Hardware-specific library
#include <SPI.h>

TFT_eSPI tft = TFT_eSPI();       // Invoke custom library

int16_t h = 135;
int16_t w = 220;

int dly = 5;

int16_t paddle_h = 30;
int16_t paddle_w = 4;

int16_t lpaddle_x = 0;
int16_t rpaddle_x = w - paddle_w;

int16_t lpaddle_y = 0;
int16_t rpaddle_y = h - paddle_h;

int16_t lpaddle_d = 1;
int16_t rpaddle_d = -1;

int16_t lpaddle_ball_t = w - w / 4;
int16_t rpaddle_ball_t = w / 4;

int16_t target_y = 0;

int16_t ball_x = 2;
int16_t ball_y = 2;
int16_t oldball_x = 2;
int16_t oldball_y = 2;

int16_t ball_dx = 1;
int16_t ball_dy = 1;

int16_t ball_w = 6;
int16_t ball_h = 6;

int16_t dashline_h = 4;
int16_t dashline_w = 2;
int16_t dashline_n = h / dashline_h;
int16_t dashline_x = w / 2 - 1;
int16_t dashline_y = dashline_h / 2;

int16_t lscore = 12;
int16_t rscore = 4;

class Button
{
  int pin_;
  public:
  Button(int pin):pin_(pin) {
    pinMode(pin, INPUT);
  }

  bool isPressed() {
    // buttons are pulled up.
    return digitalRead(pin_) == LOW;
  }

};


Button left_button_(0);
Button right_button_(35);
void setup(void) {
  
  //randomSeed(analogRead(0)*analogRead(1));
   
  tft.init();

  tft.setRotation(1);

  tft.fillScreen(BLACK);
  //tft.fillScreen(GREY);
  
  initgame();

  tft.setTextColor(WHITE, BLACK);
}

void loop() {
  delay(10);

  // lpaddle();
  rpaddle();

  midline();

  ball();
  tft.setRotation(0);
  showPoints();
  tft.setRotation(1);
  printf("left %s\n", left_button_.isPressed()?"on":"off");
  printf("right %s\n", right_button_.isPressed()?"on":"off");
}

int points_ = 0;
int last_points_ = -1;

void showPoints() {

  if (last_points_ == points_) return;
  last_points_ = points_;
  
  char str_p[20] = {0};
  // tft.fillRect(0,h-26,w,239,GREY);

  tft.setTextDatum(TC_DATUM);
  tft.setTextColor(WHITE,GREY);
  snprintf(str_p, 20, "%d", points_);
  tft.drawString(str_p, 26, 240-20 , 4);
}

void initgame() {
  rpaddle_y = random(0, h - paddle_h);

  // ball is placed on the center of the left paddle
  ball_y = w / 2;
  ball_x = random(5);
  
  // calc_target_y();

  midline();
}

void midline() {

  // If the ball is not on the line then don't redraw the line
  if ((ball_x<dashline_x-ball_w) && (ball_x > dashline_x+dashline_w)) return;

  tft.startWrite();

  // Quick way to draw a dashed line
  tft.setAddrWindow(dashline_x, 0, dashline_w, h);
  
  for(int16_t i = 0; i < dashline_n; i+=2) {
    tft.pushColor(WHITE, dashline_w*dashline_h); // push dash pixels
    tft.pushColor(BLACK, dashline_w*dashline_h); // push gap pixels
  }

  tft.endWrite();
}

// void lpaddle() {
  
//   if (lpaddle_d == 1) {
//     tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, 1, BLACK);
//   } 
//   else if (lpaddle_d == -1) {
//     tft.fillRect(lpaddle_x, lpaddle_y + paddle_h - 1, paddle_w, 1, BLACK);
//   }

//   lpaddle_y = lpaddle_y + lpaddle_d;

//   if (ball_dx == 1) lpaddle_d = 0;
//   else {
//     if (lpaddle_y + paddle_h / 2 == target_y) lpaddle_d = 0;
//     else if (lpaddle_y + paddle_h / 2 > target_y) lpaddle_d = -1;
//     else lpaddle_d = 1;
//   }

//   if (lpaddle_y + paddle_h >= h && lpaddle_d == 1) lpaddle_d = 0;
//   else if (lpaddle_y <= 0 && lpaddle_d == -1) lpaddle_d = 0;

//   tft.fillRect(lpaddle_x, lpaddle_y, paddle_w, paddle_h, WHITE);
// }

void rpaddle() {
  rpaddle_d = 0;
  if (right_button_.isPressed()) {
    rpaddle_d = -1;
  } 
  else if (left_button_.isPressed()) {
    rpaddle_d = 1;
  }
  
  if (rpaddle_d == 1) {
    tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, 1, BLACK);
  } 
  else if (rpaddle_d == -1) {
    tft.fillRect(rpaddle_x, rpaddle_y + paddle_h - 1, paddle_w, 1, BLACK);
  }

  rpaddle_y = rpaddle_y + rpaddle_d;

  if (ball_dx == -1) rpaddle_d = 0;
  else {
    if (rpaddle_y + paddle_h / 2 == target_y) rpaddle_d = 0;
    else if (rpaddle_y + paddle_h / 2 > target_y) rpaddle_d = -1;
    else rpaddle_d = 1;
  }

  if (rpaddle_y + paddle_h >= h && rpaddle_d == 1) rpaddle_d = 0;
  else if (rpaddle_y <= 0 && rpaddle_d == -1) rpaddle_d = 0;

  tft.fillRect(rpaddle_x, rpaddle_y, paddle_w, paddle_h, WHITE);
}

// void calc_target_y() {
//   int16_t target_x;
//   int16_t reflections;
//   int16_t y;

//   if (ball_dx == 1) {
//     target_x = w - ball_w;
//   } 
//   else {
//     target_x = -1 * (w - ball_w);
//   }

//   y = abs(target_x * (ball_dy / ball_dx) + ball_y);

//   reflections = floor(y / h);

//   if (reflections % 2 == 0) {
//     target_y = y % h;
//   } 
//   else {
//     target_y = h - (y % h);
//   }
// }

void ball() {
  ball_x = ball_x + ball_dx;
  ball_y = ball_y + ball_dy;

  // if (ball_dx == -1 && ball_x == paddle_w && ball_y + ball_h >= lpaddle_y && ball_y <= lpaddle_y + paddle_h) {
  //   ball_dx = ball_dx * -1;
  //   dly = random(5); // change speed of ball after paddle contact
  //   calc_target_y(); 
  // }
  if (ball_dx == 1 && ball_x + ball_w == w - paddle_w && ball_y + ball_h >= rpaddle_y && ball_y <= rpaddle_y + paddle_h) {
    ball_dx = ball_dx * -1;
    dly = random(5); // change speed of ball after paddle contact
    // calc_target_y();
  } else if ((ball_dx == 1 && ball_x >= w) || (ball_dx == -1 && ball_x + ball_w < 0)) {
    dly = 5;
  }

  if (ball_y > h - ball_w || ball_y < 0) {
    ball_dy = ball_dy * -1;
    ball_y += ball_dy; // Keep in bounds
  }

  if (ball_x >= w - ball_h) {
    // ball is placed on the center of the left paddle
    ball_x = random(5, w/2);
    ball_y = random(5, h-ball_w);
    points_+=1;
    // initgame();
    // ball_dx = ball_dx * -1;
    // ball_x += ball_dx; // Keep in bounds
    tft.fillRect(oldball_x, oldball_y, ball_w, ball_h, BLACK);
    return;
  } else if (ball_x < 0) {
    ball_dx = ball_dx * -1;
    ball_x += ball_dx; // Keep in bounds
  }

  //tft.fillRect(oldball_x, oldball_y, ball_w, ball_h, BLACK);
  tft.drawRect(oldball_x, oldball_y, ball_w, ball_h, BLACK); // Less TFT refresh aliasing than line above for large balls
  tft.fillRect(   ball_x,    ball_y, ball_w, ball_h, WHITE);
  oldball_x = ball_x;
  oldball_y = ball_y;
}

