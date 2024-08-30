#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <map>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


static const unsigned char PROGMEM usmieszek_bmp[] = {
 0b00000000,
 0b00000000,
 0b01100110,
 0b01000010,
 0b00000000,
 0b10000001,
 0b01000010,
 0b00111100,
};

static const unsigned char PROGMEM energy_bmp[] = {
 0b00011100,
 0b00100100,
 0b00101000,
 0b01001100,
 0b01100100,
 0b00010100,
 0b00101000,
 0b00110000,
};

static const unsigned char PROGMEM fork_knife_bmp[] = {
 0b10100110,
 0b10100101,
 0b10100101,
 0b11100101,
 0b01000110,
 0b01000100,
 0b01000100,
 0b01000100,
};

static const unsigned char PROGMEM face_happy_bmp[] = {
 0b00000000,
 0b00000000,
 0b10000001,
 0b00000000,
 0b00000000,
 0b11111111,
 0b01111110,
 0b00000000,
};

static const unsigned char PROGMEM face_sad_bmp[] = {
 0b00000000,
 0b00000000,
 0b10000001,
 0b00000000,
 0b00000000,
 0b01111110,
 0b11111111,
 0b00000000,
};

static const unsigned char PROGMEM face_asleep_bmp[] = {
 0b00000000,
 0b00000000,
 0b11000011,
 0b00000000,
 0b00000000,
 0b11111111,
 0b00001100,
 0b00000000,
};

// 16x16
static const unsigned char PROGMEM thermomether[] = {
 0b00000010, 0b00000000,
 0b00000101, 0b00000000,
 0b00000101, 0b00000000,
 0b00000100, 0b10000000,
 0b00000010, 0b10000000,
 0b00000010, 0b01000000,
 0b00000001, 0b01000000,
 0b00000001, 0b00100000,
 0b00000000, 0b10100000,
 0b00000000, 0b10010000,
 0b00000000, 0b01010000,
 0b00000000, 0b01001000,
 0b00000000, 0b10000100,
 0b00000000, 0b10000100,
 0b00000000, 0b01001000,
 0b00000000, 0b00110000,
};

static const unsigned char PROGMEM body_1_bmp[] = {
 0b00000000, 0b11100111, 0b00000000,
 0b00000001, 0b00011000, 0b10000000,
 0b00000001, 0b00000000, 0b10000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00110010, 0b00000000, 0b01001100,
 0b01001010, 0b00000000, 0b01010010,
 0b10000110, 0b00000000, 0b01100001,
 0b01000100, 0b00000000, 0b01000010,
 0b01000000, 0b00000000, 0b00000010,
 0b00110000, 0b00000000, 0b00001100,
 0b00001100, 0b00000000, 0b00110000,
 0b00000100, 0b00000000, 0b00100000,
 0b00000100, 0b00000000, 0b00100000,
 0b00001000, 0b00000000, 0b00010000,
 0b00001000, 0b00000000, 0b00010000,
 0b00010000, 0b00000000, 0b00001000,
 0b00100000, 0b00000000, 0b00000100,
 0b00100000, 0b00000000, 0b00000100,
 0b01000000, 0b00000000, 0b00000010,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b11000000, 0b00000000, 0b00000011,
 0b00110001, 0b11000011, 0b10001100,
 0b00001110, 0b00111100, 0b01110000,
};

static const unsigned char PROGMEM body_2_bmp[] = {
 0b00000000, 0b11100111, 0b00000000,
 0b00000001, 0b00011000, 0b10000000,
 0b00000001, 0b00000000, 0b10000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b01110010, 0b00000000, 0b01011110,
 0b10001010, 0b00000000, 0b00100001,
 0b10000100, 0b00000000, 0b01000001,
 0b10000000, 0b00000000, 0b00000001,
 0b01000000, 0b00000000, 0b00000010,
 0b00111100, 0b00000000, 0b00111100,
 0b00000100, 0b00000000, 0b00100000,
 0b00000100, 0b00000000, 0b00100000,
 0b00001000, 0b00000000, 0b00010000,
 0b00001000, 0b00000000, 0b00010000,
 0b00010000, 0b00000000, 0b00001000,
 0b00100000, 0b00000000, 0b00000100,
 0b00100000, 0b00000000, 0b00000100,
 0b01000000, 0b00000000, 0b00000010,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b11000000, 0b00000000, 0b00000011,
 0b00111000, 0b11100001, 0b11000100,
 0b00000111, 0b00011110, 0b00111000,
};

static const unsigned char PROGMEM body_3_bmp[] = {
 0b00000000, 0b11100111, 0b00000000,
 0b00000001, 0b00011000, 0b10000000,
 0b00000001, 0b00000000, 0b10000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b01111100, 0b00000000, 0b00111110,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b01111100, 0b00000000, 0b00011110,
 0b00000100, 0b00000000, 0b00100000,
 0b00001000, 0b00000000, 0b00010000,
 0b00001000, 0b00000000, 0b00010000,
 0b00010000, 0b00000000, 0b00001000,
 0b00100000, 0b00000000, 0b00000100,
 0b00100000, 0b00000000, 0b00000100,
 0b01000000, 0b00000000, 0b00000010,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000011,
 0b01011100, 0b01110000, 0b11100100,
 0b00100011, 0b10001111, 0b00011000,
};

static const unsigned char PROGMEM body_4_bmp[] = {
 0b00000000, 0b11100111, 0b00000000,
 0b00000001, 0b00011000, 0b10000000,
 0b00000001, 0b00000000, 0b10000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00000010, 0b00000000, 0b01000000,
 0b00001100, 0b00000000, 0b00110000,
 0b01110000, 0b00000000, 0b00001110,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b10001100, 0b00000000, 0b00010001,
 0b01110100, 0b00000000, 0b00101110,
 0b00001000, 0b00000000, 0b00010000,
 0b00001000, 0b00000000, 0b00010000,
 0b00010000, 0b00000000, 0b00001000,
 0b00100000, 0b00000000, 0b00000100,
 0b00100000, 0b00000000, 0b00000100,
 0b01000000, 0b00000000, 0b00000010,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b10000000, 0b00000000, 0b00000001,
 0b01001100, 0b00111000, 0b01110010,
 0b00110011, 0b11000111, 0b10001100,
};

static const unsigned char* const PROGMEM body_frames[] = {
  body_1_bmp,
  body_2_bmp,
  body_3_bmp,
  body_4_bmp,
  body_3_bmp,
  body_2_bmp
};


enum GameState {main_interface, selecting_option, feeding, selecting_game, playing_game};

enum CreatureState{awake, asleep};

enum CreatureSpecialState{normal, sick};

// options from the main interface
enum MainMenuOptions{
  sleep,
  feed,
  play
};

String get_option_name(MainMenuOptions menu_option){
  switch (menu_option){
    case sleep: return "Uśpij";
    case feed: return "Nakarm";
    case play: return "Graj";
    default: panic();
  }
}


struct CreatureStats {
  int energy;
  int happiness;
  int fullness;
  CreatureState state;
  CreatureSpecialState special_state;
};

void draw_stat(int whichInOrder, int value, const unsigned char *bitmap) {
  display.drawRoundRect(12, (2 + 10*whichInOrder), 24, 8, 2, SSD1306_WHITE);
  display.fillRect(14, (4 + 10*whichInOrder), value/5 /* max value is 100, max width is 20*/, 4, SSD1306_WHITE);
  display.drawBitmap(2, (2 + 10*whichInOrder), bitmap,8,8,SSD1306_WHITE);
}

void draw_creture(int frame, CreatureStats creature_stats){
  if (creature_stats.state == awake) {
    display.drawBitmap(48, 2, body_frames[frame], 24, 24, SSD1306_WHITE);
  }else{
    display.drawBitmap(48, 2, body_frames[0], 24, 24, SSD1306_WHITE);
  }
  if (creature_stats.happiness > 50){
    display.drawBitmap(56, 4, face_happy_bmp, 8, 8, SSD1306_WHITE);
  }else{
    display.drawBitmap(56, 4, face_sad_bmp, 8, 8, SSD1306_WHITE);
  }
}

void draw_stats(CreatureStats stats){
    draw_stat(0, stats.energy, energy_bmp);
    draw_stat(1, stats.happiness, usmieszek_bmp);
    draw_stat(2, stats.fullness, fork_knife_bmp);
}

int draw_menu_option(bool selected, String text, int start_coord){
  int length = 0;
  display.setTextSize(1);      // Normal 1:1 pixel scale
  if(selected){
    display.setTextColor(SSD1306_BLACK);
  }else{
    display.setTextColor(SSD1306_WHITE);
  }
  display.drawRoundRect(start_coord, 34, text.length()*6 + 4, 12, 2, SSD1306_WHITE);
  display.setCursor(start_coord+3, 36);

  for(char& c : text) {
    length ++;
    display.write(c);
  }
  return length;
}

void draw_menu(GameState game_state, int selected){
  switch(game_state){
    case main_interface:{
      int len = draw_menu_option(selected==1, "uspij", 2);
      len += draw_menu_option(selected==2, "nakarm", 8+len*6);
      break;
    }
  }
}


CreatureStats creature_stats;
GameState game_state;
int option_selected;

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  creature_stats.energy = 100;
  creature_stats.fullness = 100;
  creature_stats.happiness = 100;
  creature_stats.state = awake;
  creature_stats.special_state = normal;

  game_state = main_interface;
  display.display();
}

int frame = 0;

void loop() {
    display.clearDisplay();
    draw_stats(creature_stats);
    draw_creture(frame, creature_stats);
    draw_menu(game_state, option_selected);
    display.display();
    delay(100);
    if (frame < 5){
        frame++;
    }else{
        frame = 0;
    }
}
