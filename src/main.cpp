#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ArduinoJson.h>
#include <Ticker.h>

#include<Creature.h>
#include<bitmaps.h>
#include<Game.h>


#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int button_a_pin = 14;
const int button_b_pin = 16;

Creature creature;

int hi_score = 0;

// NETWORK SHIT

const char* server = "matipolit.ovh";
const int port = 80;

// Password is injected during compile time
const char* hashed_password = NIBYGOTCHI_PASS_HASH;

String serverPath = String("/nibygotchi?passwd=") + hashed_password;

class Network {
  public:
    String name;
    String ssid;
    String password;
};

Network networks[4];

void connect_to_network(Network &network){
  WiFi.begin(network.ssid, network.password);
}

int current_network;


AsyncClient client;
String jsonData = "";

void uploadStatsToServer();
void sendPostData();
void onData(void* arg, AsyncClient* client, void* data, size_t len);
void onDisconnect(void* arg, AsyncClient* client);

bool first_download_from_server = false;

void uploadStatsToServer() {
  Serial.println("Running 'upload stats to server'");
  if (WiFi.status() == WL_CONNECTED && !client.connected() && first_download_from_server) {
    Serial.println("Client not connected");
    sendPostData();
  } else {
    Serial.println("WiFi disconnected or client already connected.");
  }
}

void sendPostData() {
  // Prepare the JSON data
  StaticJsonDocument<200> jsonDoc;
  jsonDoc["energy"] = creature.energy;
  jsonDoc["happiness"] = creature.happiness;
  jsonDoc["fullness"] = creature.fullness;
  jsonDoc["hi_score"] = hi_score;

  String jsonStr;
  serializeJson(jsonDoc, jsonStr);
  
  // Create HTTP POST request
  String request = String("POST ") + serverPath + " HTTP/1.1\r\n" +
                   "Host: " + server + "\r\n" +
                   "Content-Type: application/json\r\n" +
                   "Content-Length: " + String(jsonStr.length()) + "\r\n" +
                   "Connection: close\r\n\r\n" +
                   jsonStr;

  client.onData(onData);
  client.onDisconnect(onDisconnect);
  client.connect(server, port);

  // Send the POST request once connected
  client.onConnect([request](void* arg, AsyncClient* client) {
    Serial.println("Connected to server");
    client->write(request.c_str());
  });
}

void onData(void* arg, AsyncClient* client, void* data, size_t len) {
  jsonData += String((char*)data).substring(0, len);
  Serial.println("Received response:");
  Serial.println(jsonData);
}

void onDisconnect(void* arg, AsyncClient* client) {
  Serial.println("Disconnected from server");

  jsonData = "";
}

void retrieveStatsFromServer() {
  Serial.println("Running 'retrieve stats from server'");
  AsyncClient* client = new AsyncClient();  // Create a new AsyncClient

  if (!client) {
    Serial.println("Failed to create client");
    return;
  }

  // Connect callback
  client->onConnect([](void* arg, AsyncClient* client) {
    Serial.println("Connected to server");

    // Send the GET request to the server
    String request = String("GET ") + serverPath + " HTTP/1.1\r\n" +
                     "Host: " + server + "\r\n" +
                     "Connection: close\r\n\r\n";

    client->write(request.c_str());

  }, nullptr);

  // Data receive callback
  client->onData([](void* arg, AsyncClient* client, void* data, size_t len) {
    static String jsonData = "";  // Hold the complete JSON data
    String response = String((char*)data).substring(0, len);  // Get data chunk

    Serial.println("Received data chunk:");
    Serial.println(response);

    // Handle chunked encoding: strip the chunk size and only append valid data
    int chunkSizeStart = response.indexOf("\r\n");
    if (chunkSizeStart != -1) {
      // Strip the chunk size if it's present
      String chunkData = response.substring(chunkSizeStart + 6);  // Skip the chunk size
      jsonData += chunkData;  // Append valid chunk data to jsonData

      // Check if the response ends with the last chunk ("0\r\n")
      if (jsonData.indexOf("0\r\n") != -1) {
        jsonData.replace("0\r\n", "");  // Remove the last chunk size indicator

        // Now, handle the JSON content
        int jsonStartIdx = jsonData.indexOf("\r\n\r\n");
        if (jsonStartIdx != -1) {
          String jsonBody = jsonData.substring(jsonStartIdx + 6);  // Skip HTTP headers
          Serial.println("Final Response: ");
          Serial.println(jsonBody);

          // Parse the JSON data
          StaticJsonDocument<512> doc;
          DeserializationError error = deserializeJson(doc, jsonBody);
          if (!error) {
            JsonObject json = doc.as<JsonObject>();
            creature.updateFromJson(json);  // Assuming creature.updateFromJson is defined
            hi_score = json["hi_score"];
            Serial.println("Creature updated with server data.");
          } else {
            Serial.print("Failed to parse JSON: ");
            Serial.println(error.c_str());
          }
        }
        jsonData = "";  // Clear the buffer after parsing
      }
    } else {
      Serial.println("No chunk size available");
      // In case there's no chunk size, just append the response
      jsonData += response;
    }
  }, nullptr);

  // Error callback
  client->onError([](void* arg, AsyncClient* client, int8_t error) {
    Serial.print("Connection error: ");
    Serial.println(error);
    delete client;  // Clean up the client
  }, nullptr);

  // Disconnect callback
  client->onDisconnect([](void* arg, AsyncClient* client) {
    Serial.println("Disconnected from server");
    delete client;  // Clean up the client
  }, nullptr);

  // Timeout callback
  client->onTimeout([](void* arg, AsyncClient* client, uint32_t time) {
    Serial.println("Connection timed out");
    delete client;  // Clean up the client
  }, nullptr);

  // Start connection
  client->connect(server, port);  // Connect to the server
}

enum GameState {main_interface, options_interface, feeding, sleep, wifi_selection, game_selection};

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
    display.fillRoundRect(start_coord+2, 4 + row * 15, text.length()*6 + 1, 9, 2, SSD1306_WHITE);
    display.setTextColor(SSD1306_BLACK);
  }else{
    display.setTextColor(SSD1306_WHITE);
  }
  display.drawRoundRect(start_coord, 2 + row * 15, text.length()*6 + 5, 13, 2, SSD1306_WHITE);
  display.setCursor(start_coord+3, 5 + row * 15);

  for(char& c : text) {
    length ++;
    display.write(c);
  }
  return length;
}

Ticker upload_stats_ticker(uploadStatsToServer, 15000);

void draw_menu(GameState &game_state, int &selected, bool &accept_queue, Game &game){
  switch(game_state){
    case options_interface:{
      int menu_selected = selected % 4;
      int len = draw_menu_option(menu_selected==1, "uspij", 2, 0);
      len += draw_menu_option(menu_selected==2, "wifi", 8+len*6, 0);
      len += draw_menu_option(menu_selected==3, "powrot", 14+len*6, 0);
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
              game_state = wifi_selection;
              menu_selected = 0;
              break;
          }
          case 3: {
            game_state = main_interface;
            menu_selected = 0;
            break;
          }
        }
        accept_queue = false;
      }
      break;
    };
    case wifi_selection: {
      int menu_selected = selected % 6;
      int len = draw_menu_option(menu_selected==1 || current_network == 0, networks[0].name, 2, 0);
      draw_menu_option(menu_selected==2 || current_network == 1, networks[1].name, 2, 1);
      draw_menu_option(menu_selected==3 || current_network == 2, networks[2].name, 2, 2);
      draw_menu_option(menu_selected==4 || current_network == 3, networks[3].name, 2, 3);
      draw_menu_option(menu_selected==5, "powrot", 8+len*6, 0);
      if(accept_queue){
        if(menu_selected == 5){
          game_state = main_interface;
          menu_selected = 0;
        }else{
          current_network = menu_selected - 1;
          connect_to_network(networks[current_network]);
        }
        accept_queue = false;
      }
      break;
    };
    case game_selection: {
      int menu_selected = selected % 3;
      int len = draw_menu_option(menu_selected==1, "graj", 2, 3);
      len += draw_menu_option(menu_selected==2, "powrot", 8+len*6, 3);
      if(accept_queue){
        switch(menu_selected){
          case 1: {
            creature.state = playing_game;
            selected = 0;
            upload_stats_ticker.pause();
            game = Game();
            break;
          }
          case 2: {
            game_state = main_interface;
            creature.state = awaken;
            selected = 0;
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
        len = draw_menu_option(menu_selected==1, "uspij", 2, 2);
        len += draw_menu_option(menu_selected==2, "nakarm", 8+len*6, 2);
        len += draw_menu_option(menu_selected==3, "tv", 14+len*6, 2);
        len += draw_menu_option(menu_selected==4, "gry", 20+len*6, 2);
        len += draw_menu_option(menu_selected==5, "opcje", 2, 3);

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
            case 4: {
                game_state = game_selection;
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
        len = draw_menu_option(menu_selected==1, "obudz", 2, 2);
        len += draw_menu_option(menu_selected==2, "opcje", 2, 3);

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
        int len = draw_menu_option(menu_selected==1, "przerwij", 2, 2);
        len += draw_menu_option(menu_selected==2, "nakarm", 8+len*6, 2);
        len += draw_menu_option(menu_selected==3, "opcje", 2, 3);
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
      int len = draw_menu_option(menu_selected==1, "burger", 2, 2);
      len += draw_menu_option(menu_selected==2, "pizza", 8+len*6, 2);
      len = draw_menu_option(menu_selected==3, "powrot", 2, 3);
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

Game game;
GameState game_state;
int option_selected;

void setup() {
  randomSeed(analogRead(0));
  Serial.begin(9600);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  Serial.println("Using server: " + serverPath);
  display.clearDisplay();
  creature = Creature();

  game_state = main_interface;
  display.display();

  pinMode(button_a_pin, INPUT);
  pinMode(button_b_pin, INPUT);

  current_network = 2;

  networks[0].name = "Kasia Wroclaw";
  networks[0].ssid = "T-Mobile_Swiatlowod_0895";
  networks[0].password = "";
  
  networks[1].name = "Kasia Telatyn";
  networks[1].ssid = "Orange_Swiatlowod_2B40";
  networks[1].password = "Internet9";

  networks[2].name = "Mateusz Zielona";
  networks[2].ssid = "vfrnet";
  networks[2].password = "alamakota";

  networks[3].name = "Matusz Wroclaw";
  networks[3].ssid = "PLAY_Swiatlowodowy_58D2";
  networks[3].password = "6FqTVw@KWBf%";

  connect_to_network(networks[current_network]);
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
  if(first_download_from_server){
    upload_stats_ticker.update();
  }
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

  tick_delta = current_time - previous_tick_time;

  if(game_state != sleep){

    frame_delta = current_time - previous_frame_time;
    if(creature.state != playing_game){

      if(button_a_queued){
        option_selected += 1;
        button_a_queued = false;
      }
      
      if(frame_delta > 100){
        previous_frame_time = current_time;
        display.clearDisplay();
        if(game_state == main_interface || game_state == feeding){
          draw_stats(creature);
          creature.draw(frame, display);
          if(creature.state == watching_tv){
            draw_tv(frame);
          }
        }


        if(WiFi.status() != WL_CONNECTED){
          display.drawBitmap(118, 54, no_wifi_icon_bmp, 8, 8, SSD1306_WHITE);

        }else{
          display.drawBitmap(118, 54, wifi_icon_bmp, 8, 8, SSD1306_WHITE);
          if(!first_download_from_server){
            retrieveStatsFromServer();
            first_download_from_server = true;
            Serial.println("Downloaded stats from server, starting upload ticker");
            upload_stats_ticker.start();
          }
        }

        draw_menu(game_state, option_selected, button_b_queued, game);

        if(game_state == game_selection){
          display.setCursor(2,2);
          display.setTextSize(2);
          display.println("Flappy\nDitto");
          display.setTextSize(1);
          display.setCursor(76, 2);
          display.print("Top: ");
          display.print(hi_score);
        }

        display.display();
        if (frame < 5){
            frame++;
        }else{
            frame = 0;
        }
      }
    }else{
      if(button_a_queued){
        creature.state = awaken;
        game_state = game_selection;
        upload_stats_ticker.resume();
      }
      if(frame_delta > 50){
        previous_frame_time = current_time;
        display.clearDisplay();

        int score = game.tick_and_draw(button_b_queued, display);

        if(score > 0){
          if(score > hi_score){
            hi_score = score;
          }
          creature.state = awaken;
          game_state = game_selection;
          upload_stats_ticker.resume();
        };
        display.display();
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
  }

}
