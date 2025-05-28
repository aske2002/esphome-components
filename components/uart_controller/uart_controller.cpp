#include "uart_controller.h"
#include "esphome/core/application.h"
#include "esphome/core/log.h"

namespace esphome
{
  namespace uart_controller
  {

    static const char *const TAG = "uart_controller";
    static const uint8_t TX_START_GPIO = 0x01;

    void UartController::setup()
    {
      // Short Ta/Tb using join pin
      join_pin_->digital_write(true);
      delay(2000); // simulate the "join" signal duration

      // Release the join line
      join_pin_->digital_write(false);

      // Wait for line voltage to rise (optional ADC check)
      delay(1000);

      // Enable the transmit line (connect 20 ohms)
      transmit_enable_pin_->digital_write(true);

      // Now tx_data_pin_ can be used to simulate data pulses
    }

    void UartController::update()
    {
    }

    void UartController::dump_config()
    {
      ESP_LOGCONFIG(TAG, "UartController:");
#if ESPHOME_LOG_LEVEL >= ESPHOME_LOG_LEVEL_VERBOSE
      ESP_LOGCONFIG(TAG, "sensormap");
      for (auto &it : sensorset_)
      {
        ESP_LOGCONFIG(TAG, "  Sensor on_char=%c off_char=%c", it->ctrl_char_on, it->ctrl_char_off);
      }
#endif
    }

    void UartController::loop()
    {
      // Incoming data to process?
      if (available() > 0)
      {
        // read the incoming byte:
        uint8_t incomingByte = 0;
        this->read_byte(&incomingByte);
        if (incomingByte >= 0)
        {
          char cmdByte = (char)incomingByte;
          // ask all sensors to process the byte
          for (auto &it : sensorset_)
          {
            it->parse_input(cmdByte);
          }
        }
      }
    }

    void UartController::dump_sensors_()
    {
      ESP_LOGV(TAG, "sensors");
      for (auto &it : sensorset_)
      {
        ESP_LOGV(TAG, "  Sensor 0x%x", it);
        it->dump_config();
      }
    }

  } // namespace uart_controller
} // namespace esphome
