#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>

// Thông tin WiFi
const char* ssid = "Colnaux";
const char* password = "987654321";

// Thông tin MQTT Broker
const char* mqtt_server = "1bf241f0b032457b90e6f4cdbe33cae2.s1.eu.hivemq.cloud"; // thay bằng host của bạn
const int mqtt_port = 8883; // nếu dùng TLS, nếu không thì 1883
const char* mqtt_user = "GasIOT2025";
const char* mqtt_pass = "Gasiot2025";

// Tên topic muốn gửi
const char* topic = "esp32/gassensor";

// UART config
#define STM32_RX 16  // ESP32 RX2 (connect STM32 TX)
#define STM32_TX 17  // ESP32 TX2 (not used here, chỉ cần khi truyền 2 chiều)

WiFiClientSecure espClient;
PubSubClient client(espClient);

String uart_msg = "";

void setup_wifi() {
  Serial.begin(115200); // Serial debug cho máy tính
  delay(100);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void reconnect() {
  // Đảm bảo WiFi luôn kết nối
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print("WiFi lost. Reconnecting...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    delay(1000);
  }
  // Kết nối lại MQTT
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_user, mqtt_pass)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
  setup_wifi();
  espClient.setInsecure(); // Chỉ dùng để test với TLS, KHÔNG an toàn khi làm thật!
  client.setServer(mqtt_server, mqtt_port);

  // Initialize UART2 (STM32 -> ESP32) với baudrate 9600
  Serial2.begin(9600, SERIAL_8N1, STM32_RX, STM32_TX);
  delay(100);
  Serial.println("Ready to receive UART from STM32...");
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  while (Serial2.available()) {
    char inChar = (char)Serial2.read();
    // Debug tùy chọn:
    // Serial.print("UART "); Serial.println((int)inChar); // In mã ASCII
    // Serial.print("Char: "); Serial.println(inChar);    // In ký tự nhận được

    if (inChar == '\n') {
      uart_msg.trim();
      if (uart_msg.length() > 0) {
        client.publish(topic, uart_msg.c_str());
        Serial.print("Published: ");
        Serial.println(uart_msg);
        delay(100);
      }
      uart_msg = "";
    } else {
      uart_msg += inChar;
    }
  }

  delay(10); // Để tránh chiếm CPU
}