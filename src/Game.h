#ifndef GAME_H
#define GAME_H
#include <Adafruit_SSD1306.h>

class Cactus {
  private: 
    bool up;
    int height;

  public:
    Cactus();
    Cactus(long start_coord, long end_coord);
    float position;
    void draw(Adafruit_SSD1306& display);
    bool does_collide_with_player(int character_y);
};

class Game {
  private:
    int score;
    float speed;
    int speed_delta;
    float character_velocity;
    float character_y;
    int rounded_character_y;
    Cactus cactuses[4];

  public:
    Game();
    int tick_and_draw(bool& jump_button_queued, Adafruit_SSD1306& display);
};

#endif
