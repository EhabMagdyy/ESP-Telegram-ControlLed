/********************************************************************************************************************
*****************************    Author  : Ehab Magdy Abdullah                      *********************************
*****************************    Linkedin: https://www.linkedin.com/in/ehabmagdyy/  *********************************
*****************************    Youtube : https://www.youtube.com/@EhabMagdyy      *********************************
*********************************************************************************************************************/

#ifdef ESP32
  #include <WiFi.h>
#else
  #include <ESP8266WiFi.h>
#endif
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Wi-Fi credentials
#define SSID          "SSID"
#define PASSWORD      "PASSWORD"

// bot token (from BotFather)
#define BOTtoken      "BOTtoken"

// Telegram user ID (obtained from IDBot)
#define CHAT_ID       "CHAT_ID"

// Initialize secure client and Telegram Bot instance
WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Check for new messages every 1000 milliseconds
int botDelay = 1000;
unsigned long lastTimeBotRan;

// Define LED pin and initial state
#define LED_PIN       2
bool ledState = LOW;

// Function to handle incoming messages
void handleNewMessages(int numNewMessages)
{
  Serial.println("Handling New Messages:");
  Serial.println(numNewMessages);

  for (int i = 0; i < numNewMessages; i++) {
    // Get the chat ID from the incoming message
    String incomingChatId = String(bot.messages[i].chat_id);
    if (incomingChatId != CHAT_ID) {
      bot.sendMessage(incomingChatId, "Unauthorized user", "");
      continue;
    }

    // Get the text command from the message
    String text = bot.messages[i].text;
    Serial.println("Received: " + text);
    String senderName = bot.messages[i].from_name;

    // If /start command, send a welcome message with available commands
    if (text == "/start") {
      String welcome = "Welcome, " + senderName + ".\n";
      welcome += "Commands:\n";
      welcome += "/led_on - Turn LED ON\n";
      welcome += "/led_off - Turn LED OFF\n";
      welcome += "/state - Get current LED state\n";
      bot.sendMessage(incomingChatId, welcome, "");
    }
    // Turn LED ON
    if (text == "/led_on") {
      bot.sendMessage(incomingChatId, "Turning LED ON", "");
      ledState = HIGH;
      digitalWrite(LED_PIN, ledState);
    }
    // Turn LED OFF
    if (text == "/led_off") {
      bot.sendMessage(incomingChatId, "Turning LED OFF", "");
      ledState = LOW;
      digitalWrite(LED_PIN, ledState);
    }
    // Report LED state
    if (text == "/state") {
      if (digitalRead(LED_PIN)) {
        bot.sendMessage(incomingChatId, "LED is ON", "");
      } else {
        bot.sendMessage(incomingChatId, "LED is OFF", "");
      }
    }
  }
}

void setup()
{
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState);

  // Connect to Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected!");

  // Set the root certificate for api.telegram.org (included with the board package)
  client.setCACert(TELEGRAM_CERTIFICATE_ROOT);
}

void loop()
{
  if (millis() > lastTimeBotRan + botDelay) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      Serial.println("New message received");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}
