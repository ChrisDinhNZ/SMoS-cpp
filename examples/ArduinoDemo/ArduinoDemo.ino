/*
 * ArduinoDemo:
 *
 * Copyright Chris Dinh 2020
 */

#include "smos.h"
#include "Arduino.h"

static SMoS smosObject;
static char *hexString;
unsigned int secSinceBoot;
static bool done;
static uint8_t data[SMOS_MAX_DATA_BYTE_LEN];

void setup()
{
   done = false;

   memset(data, 0, sizeof(data));
   data[0] = 1;
   data[1] = 1;
   hexString = "";

   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
   Serial.begin(9600);
   Serial.println("Begin SMoS Demo");
}

void loop()
{
   secSinceBoot = millis() / 1000;

   /* Do something every 30 secs */
   if (secSinceBoot % 30 == 0)
   {
      digitalWrite(LED_BUILTIN, HIGH);

      if (!done)
      {
         switch (smosObject.smos_EncodePiggyBackAckMessage(2,
                                                           SMOS_CONTENT_TYPE_GENERIC,
                                                           0,
                                                           SMOS_CODE_CLASS_RESP_SUCCESS,
                                                           SMOS_CODE_DETAIL_SUCCESS_CONTENT,
                                                           1,
                                                           data,
                                                           hexString))
         {
            case SMOS_RESULT_SUCCESS:
               Serial.println(hexString);
               break;

            case SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE:
            case SMOS_RESULT_ERROR_NULL_POINTER:
            case SMOS_RESULT_ERROR_ENCODE_MESSAGE:
            case SMOS_RESULT_UNKNOWN:
            default:
               Serial.println("SMOS_RESULT_ERROR");
               break;
         }

         done = true;
      }
   }
   else
   {
      digitalWrite(LED_BUILTIN, LOW);
      done = false;
   }
}
