#include <WiFi.h>        // Thư viện WiFi để kết nối với mạng WiFi
#include <ArduinoOTA.h>  // Thư viện OTA để cập nhật chương trình qua mạng

const char* ssid = "CAI TIEN";         // SSID của mạng WiFi của bạn
const char* password = "12345678$$"; // Mật khẩu của mạng WiFi của bạn

void setup() {
  Serial.begin(115200);  // Khởi động giao tiếp Serial với tốc độ 115200 bps
  
  WiFi.begin(ssid, password);  // Kết nối với mạng WiFi bằng SSID và mật khẩu
  while (WiFi.status() != WL_CONNECTED) {  // Đợi đến khi kết nối thành công
    delay(500);
    Serial.print(".");  // In ra dấu chấm mỗi 500ms để báo hiệu đang kết nối
  }
  Serial.println("\nWiFi connected");  // In ra thông báo kết nối WiFi thành công
  Serial.print("IP address: ");  // In ra địa chỉ IP của ESP32
  Serial.println(WiFi.localIP());  // Lấy và in ra địa chỉ IP của ESP32
  
  ArduinoOTA.begin();  // Bắt đầu OTA
  
  // Thiết lập các sự kiện cho OTA
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";  // Nếu cập nhật mã chương trình
    } else { // U_SPIFFS
      type = "filesystem";  // Nếu cập nhật hệ thống file
    }
    Serial.println("Start updating " + type);  // In ra thông báo bắt đầu cập nhật
  });

  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");  // In ra thông báo kết thúc cập nhật
  });

  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));  // In ra tiến trình cập nhật
  });

  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);  // In ra thông báo lỗi nếu có lỗi xảy ra
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");  // Lỗi xác thực
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");  // Lỗi bắt đầu cập nhật
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");  // Lỗi kết nối
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");  // Lỗi nhận dữ liệu
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");  // Lỗi kết thúc cập nhật
    }
  });
}

void loop() {
  ArduinoOTA.handle();  // Xử lý các sự kiện OTA
}
