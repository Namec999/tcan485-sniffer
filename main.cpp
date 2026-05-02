/*
  =============================================
  TCAN485 v1.1 - J1939 / CAN Bus Sniffer
  =============================================
  Board  : LilyGo T-CAN485 v1.1
  MCU    : ESP32 (original, not S3)
  Speed  : 250 kbps  (J1939 / heavy truck standard)
  Frames : ALL accepted - 11-bit standard + 29-bit extended
  Pins   : TX=GPIO27  RX=GPIO26  Enable=GPIO23
  Serial : 115200 baud via USB (CH340K)
  =============================================
*/

#include <Arduino.h>
#include "driver/twai.h"

#define CAN_TX_PIN    GPIO_NUM_27
#define CAN_RX_PIN    GPIO_NUM_26
#define CAN_SE_PIN    23      // SN65HVD23x RS/enable pin -> must be LOW
#define LED_PIN       4       // WS2812 data pin (not used here, just kept off)

unsigned long frameCount = 0;

void setup() {
    Serial.begin(115200);
    delay(800);

    // Activate CAN transceiver (LOW = enabled on SN65HVD23x)
    pinMode(CAN_SE_PIN, OUTPUT);
    digitalWrite(CAN_SE_PIN, LOW);

    Serial.println();
    Serial.println("=========================================");
    Serial.println("  TCAN485 J1939 CAN Sniffer  v1.0");
    Serial.println("  250 kbps | 11-bit + 29-bit | Listen");
    Serial.println("=========================================");
    Serial.println("  #      TIME(ms)  ID         EXT DLC  DATA");
    Serial.println("-----------------------------------------");

    // TWAI general config - LISTEN ONLY (safe, no ACK on bus)
    twai_general_config_t g_config = TWAI_GENERAL_CONFIG_DEFAULT(CAN_TX_PIN, CAN_RX_PIN, TWAI_MODE_LISTEN_ONLY);
    g_config.rx_queue_len = 128;
    g_config.tx_queue_len = 0;

    // 250 kbps - J1939 standard
    twai_timing_config_t t_config = TWAI_TIMING_CONFIG_250KBITS();

    // Accept ALL frames - no filter
    twai_filter_config_t f_config = TWAI_FILTER_CONFIG_ACCEPT_ALL();

    esp_err_t res;

    res = twai_driver_install(&g_config, &t_config, &f_config);
    if (res != ESP_OK) {
        Serial.printf("ERROR: TWAI install failed: %d\n", res);
        while(1) { delay(1000); Serial.println("HALTED - check wiring"); }
    }

    res = twai_start();
    if (res != ESP_OK) {
        Serial.printf("ERROR: TWAI start failed: %d\n", res);
        while(1) { delay(1000); Serial.println("HALTED"); }
    }

    Serial.println("CAN bus active - waiting for frames...");
    Serial.println();
}

void loop() {
    twai_message_t msg;

    if (twai_receive(&msg, pdMS_TO_TICKS(50)) == ESP_OK) {
        frameCount++;

        // Frame number + timestamp
        Serial.printf("%-6lu  %-10lu  ", frameCount, millis());

        // CAN ID
        if (msg.extd) {
            Serial.printf("%08X  YES  %d    ", msg.identifier, msg.data_length_code);
        } else {
            Serial.printf("%03X       NO   %d    ", msg.identifier, msg.data_length_code);
        }

        // Data bytes in hex
        for (int i = 0; i < msg.data_length_code; i++) {
            Serial.printf("%02X ", msg.data[i]);
        }
        Serial.println();
    }

    // Print stats every 5 seconds
    static unsigned long lastStats = 0;
    if (millis() - lastStats > 5000) {
        lastStats = millis();
        twai_status_info_t status;
        twai_get_status_info(&status);
        Serial.printf("-- Stats: %lu frames rx | bus_err=%lu | arb_lost=%lu --\n",
            status.msgs_to_rx + frameCount,
            status.bus_error_count,
            status.arb_lost_count);
    }
}
