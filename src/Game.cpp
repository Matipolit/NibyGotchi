#include <Game.h>
#include <Arduino.h>
#include <Adafruit_SSD1306.h>
#include <bitmaps.h>

Cactus::Cactus() {}

Cactus::Cactus(long start_coord, long end_coord){
  position = random(start_coord, end_coord);
  up = random(0, 2);
  height = random(1, 4);
}

bool Cactus::does_collide_with_player(int character_y){
  if(10.5 < position && position < 17.5){
    if(up){
      if(character_y < (height * 8 + 4)){
        Serial.println("Lost game up!");
        return true;
      }
    }else{
      if(character_y > (56 - (height  * 8))){
        Serial.println("Lost game down!");
        return true;
      }
    }
  }
  return false;
}

void Cactus::draw(Adafruit_SSD1306 &display){
  int current_height;
  int height_diff;
  if(up){
    current_height = 0;
    height_diff = 8;
  }else{
    current_height = 56;
    height_diff = -8;
  }
  for(int i = 0; i < height; i++){
    display.drawBitmap(round(position), current_height, cactus_body_bmp, 8, 8, SSD1306_WHITE);
    current_height += height_diff;
  }
  if(up){
    display.drawBitmap(round(position), current_height, cactus_head_upside_down_bmp, 8, 8, SSD1306_WHITE);
  }else{
    display.drawBitmap(round(position), current_height, cactus_head_bmp, 8, 8, SSD1306_WHITE);
    }
}

Game::Game() 
  : score(0), 
    speed(1.0), 
    speed_delta(0), 
    character_velocity(0.0), 
    character_y(0.0), 
    rounded_character_y(0) 
{
    // Initialize the cactuses array
    for (int i = 0; i < 4; i++) {
        long start = 12 + 30 * (i + 1);
        long end = start + 18;
        cactuses[i] = Cactus(start, end); // Assuming Cactus has this constructor
    }
}


int Game::tick_and_draw(bool& jump_button_queued, Adafruit_SSD1306& display){
  /*
  Serial.printf("Character velocity: ");
  Serial.println(character_velocity);
  Serial.printf("Character y: ");
  Serial.println(character_y);
  Serial.printf("Rounded character y: ");
  Serial.println(rounded_character_y);
  */

  speed_delta ++;

  
  if(jump_button_queued){
    character_velocity = -4.5;
    jump_button_queued = false;
  }
  if(character_velocity < 0){
    if(character_y >= 0){
      character_y += character_velocity;
    }
  }else{
    if(character_y <= 56){
      character_y += character_velocity;
    }
  }

  if(character_velocity < 3){
    character_velocity += 0.7;
  }

  rounded_character_y = round(character_y);

  for(int i = 0; i < 4; i++){
    cactuses[i].position -= speed;
    if(cactuses[i].position <= 0){
      long new_position = cactuses[i].position + 128;
      cactuses[i] = Cactus(new_position, new_position);
      score++;
    }
    cactuses[i].draw(display);
    if(cactuses[i].does_collide_with_player(rounded_character_y)){
      return score;
    }
  }

  display.drawBitmap(10, rounded_character_y, body_small_bmp , 8, 8, SSD1306_WHITE);
  display.setCursor(102, 56);
  display.print(score, DEC);
  if(speed_delta > 100){
    speed += 0.1;
    speed_delta = 0;
  }
  return -1;
}
