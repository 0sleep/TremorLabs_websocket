///////////////////////////////////////////////////////////////////
//                       power management                        //
//                 for esp32 for use in tremor senspor          //
//                                                             //
/////////////////////////////////////////////////////////////////

#define uS_TO_S_conversion 1000000  /* Converert micro seconds to seconds */
#define TIME_TO_SLEEP  4        /* Time ESP32 will go to sleep (in seconds) */

RTC_DATA_ATTR int bootCount = 0;

void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_EXT0 : Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1 : Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD : Serial.println("Wakeup caused by touchpad"); break;

    default : Serial.printf("Wakeup was not caused by deep sleep: %d\n",wakeup_reason); break;
  }
}

void setup(){
  Serial.begin(115200);
  delay(1000)
  ++bootCount; // ++ incrament operator +1 to boot number
  Serial.println("Boot: " + String(bootCount)); //  then print
  print_wakeup_reason(); // debug
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_conversion); //goto sleep for ()Seconds
    Serial.println("Setup ESP32 to sleep for every " + String(TIME_TO_SLEEP) +
    " Seconds");
    Serial.println("Going to sleep now");
    delay(1000);
    Serial.flush();  // wait for data to complete
    esp_deep_sleep_start(); // bedtimne
    Serial.println("placeholder"); // should not be printed as esp is sleeping
  }
