#include <ArduinoJson.h>
#include "Creature.h"
#include "Adafruit_SSD1306.h"
#include<bitmaps.h>

Creature::Creature(){
  energy = 100;
  happiness = 100;
  fullness = 100;

  energy_timer = creature_timer_delta;
  happiness_timer = creature_timer_delta;
  fullness_timer = creature_timer_delta;

  state = awaken;
  special_state = normal;
}

void Creature::tick(){
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
  }else if(state == asleep){
      fullness_timer -= 1;
      happiness_timer -= 1;
      energy_timer -= 1;
  }else if(state == playing_game){
    fullness_timer -= 5;
    happiness_timer -= 8;
    energy_timer -= 5;
  }else if(state == watching_tv){
    fullness_timer -= 2;
    happiness_timer -= 3;
    energy_timer -= 2;
  }

  if( happiness_timer <= 0){
    if(state == awaken){
      if(happiness > 0){
        happiness -= 1;
      }
    }else if(state == watching_tv){
      if(happiness < 100){
        happiness += 3;
      }
    }else if(state == playing_game){
      if(happiness < 100){
        happiness += 10;
      }
    }
    happiness_timer = creature_timer_delta;

  }
  if(energy_timer <= 0){
    if(state == awaken){
      if(energy > 0){
        energy -= 1;
      }
    }else if(state == asleep){
      if(energy < 100){
        energy += 2;
      }
    }else if(state == watching_tv){
      if(energy > 0){
        energy -= 2;
      }
    }else if(state == playing_game){
      if(energy > 0){
        energy -= 6;
      }
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

void Creature::draw(int frame, Adafruit_SSD1306 &display, bool hat, bool sweater){
  if (state == awaken || state == watching_tv) {
    display.drawBitmap(48, 4, body_frames[frame], 24, 24, SSD1306_WHITE);
    if (happiness > 50){
      display.drawBitmap(56, 6, face_happy_bmp, 8, 8, SSD1306_WHITE);
    }else{
      display.drawBitmap(56, 6, face_sad_bmp, 8, 8, SSD1306_WHITE);
    }
  }else{
    display.drawBitmap(48, 4, body_4_bmp, 24, 24, SSD1306_WHITE);
    display.drawBitmap(56, 6, face_asleep_bmp, 8, 8, SSD1306_WHITE);

  }


  if (special_state == sick){
    display.drawBitmap(72, 4, thermomether, 16, 16, SSD1306_WHITE);
  }

  if(hat){
    display.drawBitmap(56, 0, hat_bmp, 8, 8, SSD1306_WHITE);
  }

  if(sweater){
    display.drawBitmap(52, 12, sweater_black_bmp, 16, 16, SSD1306_BLACK);
    display.drawBitmap(52, 12, sweater_bmp, 16, 16, SSD1306_WHITE);
  }
}

void Creature::put_to_sleep(){
  if(special_state != sick){
    state = asleep;
  }
}

void Creature::awake(){
  state = awaken;
}

void Creature::feed(int hunger_points){
  fullness += hunger_points;
}

void Creature::updateFromJson(JsonObject &json){
  energy = json["energy"];
  happiness = json["happiness"];
  fullness = json["fullness"];
}

void Creature::toJson(JsonObject &json){
  json["energy"] = energy;
  json["happiness"] = happiness;
  json["fullness"] = fullness;
}
