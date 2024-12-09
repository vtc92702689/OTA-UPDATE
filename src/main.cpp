#include <WiFi.h>
#include <ArduinoOTA.h>
#include <U8g2lib.h> 

const char* ssid = "OTA";
const char* password = "12345678$$";
const char* ota_password = "9999"; // Mật khẩu cho OTA

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void setup() {
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0,10,"WiFi connected");
  u8g2.drawStr(0, 20, WiFi.localIP().toString().c_str());
  u8g2.sendBuffer();
  
  ArduinoOTA.setPassword(ota_password); // Thiết lập mật khẩu cho OTA
  ArduinoOTA.begin();

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { 
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"Updating...");
    u8g2.drawStr(0, 20, ("Progress: " + String((progress / (total / 100))) + "%").c_str());
    u8g2.sendBuffer();
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    String errorMessage;

    if (error == OTA_AUTH_ERROR) {
      errorMessage = "Auth Failed";
      Serial.println(errorMessage);
    } else if (error == OTA_BEGIN_ERROR) {
      errorMessage = "Begin Failed";
      Serial.println(errorMessage);
    } else if (error == OTA_CONNECT_ERROR) {
      errorMessage = "Connect Failed";
      Serial.println(errorMessage);
    } else if (error == OTA_RECEIVE_ERROR) {
      errorMessage = "Receive Failed";
      Serial.println(errorMessage);
    } else if (error == OTA_END_ERROR) {
      errorMessage = "End Failed";
      Serial.println(errorMessage);
    }

    // Hiển thị lỗi lên màn hình OLED
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"OTA Error:");
    u8g2.drawStr(0, 20, errorMessage.c_str());
    u8g2.sendBuffer();
  });
}

void loop() {
  ArduinoOTA.handle();

  // Kiểm tra kết nối WiFi
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected. Reconnecting...");
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"WiFi disconnected");
    u8g2.drawStr(0, 20, "Reconnecting...");
    u8g2.sendBuffer();

    // Kết nối lại WiFi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.println("\nWiFi reconnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0,10,"WiFi reconnected");
    u8g2.drawStr(0, 20, WiFi.localIP().toString().c_str());
    u8g2.sendBuffer();
    delay(1000);  // Đợi 1 giây trước khi kiểm tra lại
  }

  
}
