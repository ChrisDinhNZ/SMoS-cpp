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
static uint8_t data;

void setup()
{
   done = false;
   data = 1;
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
         switch(smosObject.smos_EncodeGetMessage(1, SMOS_CONTENT_TYPE_GENERIC, 0, 1, &data, hexString))
         {
            case SMOS_ERROR_OK:
               Serial.println(hexString);
               break;
            
            default:
               Serial.println("SMOS_ERROR_INVALID_MESSAGE");
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
