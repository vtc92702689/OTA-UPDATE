#include <WiFi.h>
#include <ArduinoOTA.h>
#include <U8g2lib.h>
#include <LittleFS.h> // Thêm thư viện LittleFS

const char* ssid = "OTA";
const char* password = "12345678$$";
const char* ota_password = "9999"; // Mật khẩu cho OTA

IPAddress local_IP(192, 168, 137, 254); // Địa chỉ IP tĩnh
IPAddress gateway(192, 168, 137, 1);    // Địa chỉ Gateway
IPAddress subnet(255, 255, 255, 0);     // Subnet Mask

U8G2_SH1106_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);

void createIndexHtml() {
    const char* indexHtml = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Upload and Download File</title>
</head>
<body>
    <h2>Upload File to ESP32</h2>
    <form method="POST" action="/upload" enctype="multipart/form-data">
        <input type="file" name="file">
        <input type="submit" value="Upload">
    </form>
    <h2>Download config.json</h2>
    <a href="/download" download="config.json">Download config.json</a>
</body>
</html>
)rawliteral";

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Creating index.html...");
    u8g2.sendBuffer();
    
    File file = LittleFS.open("/index.html", "w");
    if (!file) {
        Serial.println("Failed to create index.html");
        u8g2.clearBuffer();
        u8g2.drawStr(0, 10, "Failed to create");
        u8g2.drawStr(0, 30, "index.html");
        u8g2.sendBuffer();
        return;
    }
    file.print(indexHtml);
    file.close();
    
    Serial.println("index.html created");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "index.html created");
    u8g2.sendBuffer();
    delay(5000); // Hiển thị thông báo 5 giây trước khi vào loop
}

void setup() {
  Serial.begin(115200);
  
  // Cấu hình địa chỉ IP tĩnh
  if (!WiFi.config(local_IP, gateway, subnet)) {
    Serial.println("Failed to configure STA");
  }

  WiFi.begin(ssid, password);
  
  // Hiển thị quá trình kết nối WiFi lên màn hình OLED
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "Connecting to WiFi");
  u8g2.sendBuffer();

  static int dotCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); // Giảm thời gian delay để tăng tốc kết nối
    Serial.print(".");

    // Cập nhật màn hình OLED với dấu chấm
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "Connecting to WiFi");
    String dots = String(".");
    for (int i = 0; i < dotCount; i++) {
      dots += ".";
    }
    u8g2.drawStr(0, 30, dots.c_str());
    u8g2.sendBuffer();
    dotCount = (dotCount + 1) % 4; // Tạo hiệu ứng xoay vòng
  }
  
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.drawStr(0, 10, "WiFi connected");
  u8g2.drawStr(0, 30, "IP address:");
  String url = WiFi.localIP().toString();
  u8g2.drawStr(0, 50, url.c_str());
  u8g2.sendBuffer();
  
  // Mount LittleFS và tạo tệp index.html
  if (!LittleFS.begin()) {
    Serial.println("An Error has occurred while mounting LittleFS");
    u8g2.clearBuffer();
    u8g2.drawStr(0, 10, "Failed to mount");
    u8g2.drawStr(0, 30, "LittleFS");
    u8g2.sendBuffer();
    return;
  }
  createIndexHtml(); // Tạo tệp index.html

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
    u8g2.drawStr(0, 10, "Updating...");
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
    u8g2.drawStr(0, 10, "OTA Error:");
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
    u8g2.drawStr(0, 10, "WiFi disconnected");
    u8g2.drawStr(0, 20, "Reconnecting...");
    u8g2.sendBuffer();

    // Kết nối lại WiFi
    WiFi.begin(ssid, password);
    static int dotCount = 0;
    while (WiFi.status() != WL_CONNECTED) {
      delay(500); // Giảm thời gian delay để tăng tốc kết nối
      Serial.print(".");

      // Cập nhật màn hình OLED với dấu chấm
      u8g2.clearBuffer();
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 10, "Reconnecting...");
      String dots = String(".");
      for (int i = 0; i < dotCount; i++) {
        dots += ".";
      }
      u8g2.drawStr(0, 30, dots.c_str());
      u8g2.sendBuffer();
      dotCount = (dotCount + 1) % 4; // Tạo hiệu ứng xoay vòng
    }

    Serial.println("\nWiFi reconnected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.drawStr(0, 10, "WiFi reconnected");
    u8g2.drawStr(0, 20, WiFi.localIP().toString().c_str());
    u8g2.sendBuffer();
  }

  //delay(1000);  // Đợi 1 giây trước khi kiểm tra lại
}