/*
 * ArduinoDemo:
 *
 * Copyright Chris Dinh 2020
 */

#include "smos.h"
#include "Arduino.h"

typedef enum DataArrayIndexInfo_e
{
   DATA_ARRAY_INDEX_INFO_RESOURCE,
   DATA_ARRAY_INDEX_INFO_RESOURCE_INFO,
   DATA_ARRAY_MAX_SIZE
};

typedef enum ResourceType_e
{
   RESOURCE_TYPE_SWITCH = 0x01,
   RESOURCE_TYPE_ALARM = 0x02
};

typedef enum ResourceState_e
{
   RESOURCE_STATE_OFF,
   RESOURCE_STATE_ON
};

typedef struct Resource
{
   ResourceType_e    type;
   ResourceState_e   state;
};

static SMoS smosService;
static SMoSObject smosObject;
static char *hexString;
static uint16_t hexStringLength;
static Resource builtInLedResource;

static uint8_t data[DATA_ARRAY_MAX_SIZE];

static void ResetBuiltInLedResource(void)
{
   builtInLedResource.type = RESOURCE_TYPE_SWITCH;
   builtInLedResource.state = RESOURCE_STATE_OFF;
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
}

static void ResetHexString(void)
{
   hexString = "";
   hexStringLength = 0;
}

static void SetBuiltInLedState(ResourceState_e state)
{
   builtInLedResource.state = state;

   if (state == RESOURCE_STATE_ON)
   {
      digitalWrite(LED_BUILTIN, HIGH);
   }
   else
   {
      digitalWrite(LED_BUILTIN, LOW);
   }
}

static ResourceState_e GetBuiltInLedState(void)
{
   return builtInLedResource.state;
}

void setup()
{
   memset(data, 0, sizeof(data));
   memset(&smosObject, 0, sizeof(smosObject));
   ResetHexString();
   ResetBuiltInLedResource();

   Serial.begin(9600);
   Serial.println("Begin SMoS Demo");
}

void loop()
{
   if (Serial.available())
   {
      char c = Serial.read();

      /* We have already started processing a hex string so carry on processing it. */
      if (hexStringLength != 0)
      {
         SMoSResult_e result;

         hexString[hexStringLength] = c;
         hexStringLength++;

         result = smosService.smos_DecodeHexString(hexString, hexStringLength, &smosObject);

         switch (result)
         {
            case SMOS_RESULT_SUCCESS:
               sdf
               return;

            case SMOS_RESULT_ERROR_NULL_POINTER:
               Serial.println("This shouldn't happen");
               return;

            case SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE:
               ResetHexString();
               return;

            case SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING:
            case SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE:
               /* String not long enough, do nothing for now. */
               return;

            case SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE:
            case SMOS_RESULT_ERROR_HEX_STRING_INVALID_CHECKSUM:
            default:
               ResetHexString();
               return;
         }
      }
      else if (smosService.smos_IsStartCode(c))
      {
         hexString[hexStringLength] = c;
         hexStringLength++;
      }
   }
}
