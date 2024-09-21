#ifndef CREATURE_H
#define CREATURE_H

#include <ArduinoJson.h>
#include <Adafruit_SSD1306.h>
enum CreatureState{awaken, asleep, watching_tv, playing_game};

enum CreatureSpecialState{normal, sick};

const int creature_timer_delta = 250;

class Creature{
  private:
    int fullness_timer;
    int happiness_timer;
    int energy_timer;

  public:
    CreatureState state;
    CreatureSpecialState special_state;
    int energy;
    int happiness;
    int fullness;

    Creature();
    void updateFromJson(JsonObject& json);
    void toJson(JsonObject& json);
    void tick();
    void draw(int frame,  Adafruit_SSD1306 &display);
    void put_to_sleep();
    void awake();
    void feed(int hunger_points);
};

#endif
