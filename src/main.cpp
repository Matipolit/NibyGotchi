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

const int button_a_pin = 14;
const int button_b_pin = 16;


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

enum CreatureState{awaken, asleep, watching_tv, playing_game};

enum CreatureSpecialState{normal, sick};

const int creature_timer_delta = 100;

class Creature{
  private:
    int excess_fullness;

    int fullness_timer;
    int happiness_timer;
    int energy_timer;

  public:
    CreatureState state;
    CreatureSpecialState special_state;
    int energy;
    int happiness;
    int fullness;

    Creature(){
      energy = 100;
      happiness = 100;
      fullness = 100;

      energy_timer = creature_timer_delta;
      happiness_timer = creature_timer_delta;
      fullness_timer = creature_timer_delta;

      state = awaken;
      special_state = normal;
    }

    void tick(){
      if(state == awaken){
        if(special_state == sick){
          fullness_timer -= 4;
          happiness_timer -= 6;
          energy_timer -=3;
        }else{
          fullness_timer -= 2;
          happiness_timer -= 2;
          energy_timer -= 2;
        }
      }else{
          fullness_timer -= 1;
          happiness_timer -= 1;
          energy_timer -= 1;
      }

      if( happiness_timer <= 0){
        if(state == awaken){
          happiness -= 1;
        }else if(state == watching_tv){
          happiness += 3;
        }else if(state == playing_game){
          happiness += 10;
        }
        happiness_timer = creature_timer_delta;

      }
      if(energy_timer <= 0){
        if(state == awaken){
          energy -= 1;
        }else if(state == asleep){
          energy += 2;
        }else if(state == watching_tv){
          energy -= 2;
        }else if(state == playing_game){
          energy -= 6;
        }
        energy_timer = creature_timer_delta;
      }
      if(fullness_timer <= 0){
        if(state == playing_game){
          fullness -= 3;
        }else{
          fullness -= 1;
        }
        fullness_timer = creature_timer_delta;
      }

      if(fullness > 110){
        special_state = sick;
      }else{
        special_state = normal;
      }
    }

    void draw(int frame){
      if (state == awaken) {
        display.drawBitmap(48, 2, body_frames[frame], 24, 24, SSD1306_WHITE);
        if (happiness > 50){
          display.drawBitmap(56, 4, face_happy_bmp, 8, 8, SSD1306_WHITE);
        }else{
          display.drawBitmap(56, 4, face_sad_bmp, 8, 8, SSD1306_WHITE);
        }
      }else{
        display.drawBitmap(48, 2, body_4_bmp, 24, 24, SSD1306_WHITE);
        display.drawBitmap(56, 4, face_asleep_bmp, 8, 8, SSD1306_WHITE);

      }


      if (special_state == sick){
        display.drawBitmap(72, 4, thermomether, 16, 16, SSD1306_WHITE);
      }
    }

    void put_to_sleep(){
      if(special_state != sick){
        state = asleep;
      }
    }

    void awake(){
      state = awaken;
    }

    void feed(int hunger_points){
      fullness += hunger_points;
    }
};

enum GameState {main_interface, options_interface, feeding, selecting_game, sleep};

void draw_stat(int whichInOrder, int value, const unsigned char *bitmap) {
  if(value > 100){
    value = 100;
  }
  if(value < 0){
    value = 0;
  }
  display.drawRoundRect(12, (2 + 10*whichInOrder), 24, 8, 2, SSD1306_WHITE);
  display.fillRect(14, (4 + 10*whichInOrder), value/5 /* max value is 100, max width is 20*/, 4, SSD1306_WHITE);
  display.drawBitmap(2, (2 + 10*whichInOrder), bitmap,8,8,SSD1306_WHITE);
}

void draw_stats(Creature &creature){
    draw_stat(0, creature.energy, energy_bmp);
    draw_stat(1, creature.happiness, usmieszek_bmp);
    draw_stat(2, creature.fullness, fork_knife_bmp);
}

int draw_menu_option(bool selected, String text, int start_coord, int row){
  int length = 0;
  display.setTextSize(1);      // Normal 1:1 pixel scale
  if(selected){
    display.fillRoundRect(start_coord+2, 36 + row * 14, text.length()*6 + 1, 9, 2, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  }else{
    display.setTextColor(SSD1306_WHITE);
  }
  display.drawRoundRect(start_coord, 34 + row * 14, text.length()*6 + 5, 13, 2, SSD1306_WHITE);
  display.setCursor(start_coord+3, 37 + row * 14);

  for(char& c : text) {
    length ++;
    display.write(c);
  }
  return length;
}


void draw_menu(GameState &game_state, int &selected, bool &accept_queue, Creature &creature){
  switch(game_state){
    case options_interface:{
      int menu_selected = selected % 3;
      int len = draw_menu_option(menu_selected==1, "uspij", 2, 0);
      len += draw_menu_option(menu_selected==2, "powrot", 8+len*6, 0);
      if(accept_queue){
        switch(menu_selected){
          case 1: {
            game_state = sleep;
            display.clearDisplay();
            display.display();
            menu_selected = 0;
            break;
          }
          case 2: {
            game_state = main_interface;
            menu_selected = 0;
            break;
          }
        }
        accept_queue = false;
      }
      break;
    };
    case main_interface:{
      int menu_selected;
      int len;
      if(creature.state == awaken){
        menu_selected = selected % 6;
        len = draw_menu_option(menu_selected==1, "uspij", 2, 0);
        len += draw_menu_option(menu_selected==2, "nakarm", 8+len*6, 0);
        len += draw_menu_option(menu_selected==3, "tv", 14+len*6, 0);
        len += draw_menu_option(menu_selected==4, "gry", 20+len*6, 0);
        len += draw_menu_option(menu_selected==5, "opcje", 2, 1);

        if(accept_queue){
          switch(menu_selected){
            case 1: {
              creature.put_to_sleep();
              selected = 0;
              break;
            }
            case 2: {
              game_state = feeding;
              selected = 0;
              break;
            }
            case 3: {
              creature.state = watching_tv;
              selected = 0;
              break;
            }
            case 5: {
              game_state = options_interface;
              selected = 0;
              break;
            }
          }
          accept_queue = false;
        }
      }else if(creature.state == asleep){
        menu_selected = selected % 3;
        len = draw_menu_option(menu_selected==1, "obudz", 2, 0);
        len += draw_menu_option(menu_selected==2, "opcje", 2, 1);

        if(accept_queue){
          switch(menu_selected){
            case 1: {
              creature.awake();
              selected = 0;
              break;
            }
            case 2: {
              game_state = options_interface;
              selected = 0;
              break;
            }
          }
        }
      }else if(creature.state == watching_tv){
        int menu_selected = selected % 4;
        int len = draw_menu_option(menu_selected==1, "przerwij", 2, 0);
        len += draw_menu_option(menu_selected==2, "nakarm", 8+len*6, 0);
        len += draw_menu_option(menu_selected==3, "opcje", 2, 1);
        if(accept_queue){
          switch(menu_selected){
            case 1: {
              game_state = main_interface;
              creature.state = awaken;
              menu_selected = 0;
              break;
            }
            case 2: {
              game_state = feeding;
              menu_selected = 0;
              break;
            }
            case 3: {
                game_state = options_interface;
                selected = 0;
                break;
              }
          }
          accept_queue = false;
        }
      };
      break;
    }
    case feeding:{
      int menu_selected = selected % 4;
      int len = draw_menu_option(menu_selected==1, "burger", 2, 0);
      len += draw_menu_option(menu_selected==2, "pizza", 8+len*6, 0);
      len = draw_menu_option(menu_selected==3, "powrot", 2, 1);
      if(accept_queue){
        switch(menu_selected){
          case 1: {
            creature.feed(10);
            break;
          }
          case 2: {
            creature.feed(20);
            break;
          }
          case 3: {
            game_state = main_interface;
            selected = 0;
            break;
          }
        }
        accept_queue = false;
      }
      break;
    }
  }
}


void draw_tv(int frame){
  display.drawRoundRect(90, 2, 24, 16, 3, SSD1306_WHITE);
  if( frame > 2){
    display.fillRect(92, 4, 20, 12, SSD1306_WHITE);
  }
}


Creature creature;
GameState game_state;
int option_selected;

void setup() {
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  creature = Creature();

  game_state = main_interface;
  display.display();

  pinMode(button_a_pin, INPUT);
  pinMode(button_b_pin, INPUT);

}

int frame = 0;
unsigned long previous_frame_time = 0;
unsigned long current_time;
unsigned long frame_delta;

unsigned long previous_tick_time = 0;
unsigned long tick_delta;


unsigned long last_a_button_press = 0;
int button_a_state;
bool button_a_queued = false;

unsigned long last_b_button_press = 0;
int button_b_state;
bool button_b_queued = false;

void loop() {
  current_time = millis();
  button_a_state = digitalRead(button_a_pin);
  button_b_state = digitalRead(button_b_pin);


  if(button_a_state == HIGH){
    if(current_time - last_a_button_press > 200){
      last_a_button_press = current_time;
      button_a_queued = true;
    }
  }

  if(button_b_state == HIGH){
    if(current_time - last_b_button_press > 200){
      button_b_queued = true;
      last_b_button_press = current_time;
    }
  }

  if(game_state != sleep){
    if(button_a_queued){
      option_selected += 1;
      button_a_queued = false;
    }


    frame_delta = current_time - previous_frame_time;
    tick_delta = current_time - previous_tick_time;
    if(frame_delta > 100){
      previous_frame_time = current_time;
      display.clearDisplay();
      if(game_state == main_interface || game_state == feeding){
        draw_stats(creature);
        creature.draw(frame);
        if(creature.state == watching_tv){
          draw_tv(frame);
        }
      }
      draw_menu(game_state, option_selected, button_b_queued, creature);
      display.display();
      if (frame < 5){
          frame++;
      }else{
          frame = 0;
      }
    }
  }else{
    if(button_a_queued || button_b_queued){
      game_state = main_interface;
      button_a_queued = false;
      button_b_queued = false;
    }
  }

  if(tick_delta > 1000){
    creature.tick();
    previous_tick_time = current_time;
    Serial.println("Tick");
  }

}
