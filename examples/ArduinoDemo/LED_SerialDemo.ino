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
   /* Alarm resource not implemented */
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
static SMoSObject_t smosObject;
static char hexString[SMOS_HEX_STRING_MAX_LENGTH + 1]; /* NULL terminated string */
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
   memset(hexString, 0, sizeof(hexString[0]) * (SMOS_HEX_STRING_MAX_LENGTH + 1));
   hexStringLength = 0;
}

static void SetBuiltInLedState(uint8_t data)
{
   if (data == 0u)
   {
      builtInLedResource.state = RESOURCE_STATE_OFF;
      digitalWrite(LED_BUILTIN, LOW);
   }
   else
   {
      builtInLedResource.state = RESOURCE_STATE_ON;
      digitalWrite(LED_BUILTIN, HIGH);
   }
}

static ResourceState_e GetBuiltInLedState(void)
{
   return builtInLedResource.state;
}

static void ProcessConfirmableRequest(SMoSObject_t const * const message)
{
   char respHexString[SMOS_HEX_STRING_MAX_LENGTH + 1]; /* NULL terminated string */
   uint8_t respData[DATA_ARRAY_MAX_SIZE];

   memset(respHexString, 0, sizeof(respHexString[0]) * (SMOS_HEX_STRING_MAX_LENGTH + 1));
   memset(respData, 0, sizeof(respData[0]) * (DATA_ARRAY_MAX_SIZE));

   if (message->dataContent[DATA_ARRAY_INDEX_INFO_RESOURCE] == RESOURCE_TYPE_ALARM)
   {
      /* We haven't implemented support for the alarm reousrce so send an ACK with a 404.
         Note that with a piggy-back acknowledgement response, we can omit some details
         in the reponse because we are responding to a specific request (using the request message Id). */
      if (smosService.smos_EncodePiggyBackAckMessage(
              0,
              SMOS_CONTENT_TYPE_GENERIC,
              0,
              SMOS_CODE_CLASS_RESP_CLIENT_ERROR,
              SMOS_CODE_DETAIL_CLIENT_ERROR_NOT_FOUND,
              message->messageId,
              NULL,
              respHexString) == SMOS_RESULT_SUCCESS)
      {
         Serial.println(respHexString);
      }

      return;
   }

   /* Request is about the Switch resource from here on in. */
   respData[DATA_ARRAY_INDEX_INFO_RESOURCE] = RESOURCE_TYPE_SWITCH;

   switch (message->codeDetail)
   {
      case SMOS_CODE_DETAIL_GET:
         if (message->tokenId != 0)
         {
            /* There is a token ID so this is an observe request. We send an ACK with a METHOD_NOT_ALLOWED. */
            if (smosService.smos_EncodePiggyBackAckMessage(
                    0,
                    SMOS_CONTENT_TYPE_GENERIC,
                    0,
                    SMOS_CODE_CLASS_RESP_CLIENT_ERROR,
                    SMOS_CODE_DETAIL_CLIENT_ERROR_METHOD_NOT_ALLOWED,
                    message->messageId,
                    NULL,
                    respHexString) == SMOS_RESULT_SUCCESS)
            {
               Serial.println(respHexString);
            }
         }
         else
         {
            /* Send an ACK with state of LED. */
            respData[DATA_ARRAY_INDEX_INFO_RESOURCE_INFO] = GetBuiltInLedState();

            if (smosService.smos_EncodePiggyBackAckMessage(
                    2,
                    SMOS_CONTENT_TYPE_GENERIC,
                    0,
                    SMOS_CODE_CLASS_RESP_SUCCESS,
                    SMOS_CODE_DETAIL_SUCCESS_CONTENT,
                    message->messageId,
                    respData,
                    respHexString) == SMOS_RESULT_SUCCESS)
            {
               Serial.println(respHexString);
            }
         }
         break;

      case SMOS_CODE_DETAIL_POST:
      case SMOS_CODE_DETAIL_DELETE:
         /* Since the switch doesn't support POST and DELETE, we send an ACK with a METHOD_NOT_ALLOWED. */
         if (smosService.smos_EncodePiggyBackAckMessage(
                 0,
                 SMOS_CONTENT_TYPE_GENERIC,
                 0,
                 SMOS_CODE_CLASS_RESP_CLIENT_ERROR,
                 SMOS_CODE_DETAIL_CLIENT_ERROR_METHOD_NOT_ALLOWED,
                 message->messageId,
                 NULL,
                 respHexString) == SMOS_RESULT_SUCCESS)
         {
            Serial.println(respHexString);
         }
         break;

      case SMOS_CODE_DETAIL_PUT:
         /* Update the built-in LED based on the request. */
         SetBuiltInLedState(message->dataContent[DATA_ARRAY_INDEX_INFO_RESOURCE_INFO]);

         /* Send an ACK with a CHANGED status. */
         if (smosService.smos_EncodePiggyBackAckMessage(
                 0,
                 SMOS_CONTENT_TYPE_GENERIC,
                 0,
                 SMOS_CODE_CLASS_RESP_SUCCESS,
                 SMOS_CODE_DETAIL_SUCCESS_CHANGED,
                 message->messageId,
                 NULL,
                 respHexString) == SMOS_RESULT_SUCCESS)
         {
            Serial.println(respHexString);
         }
         break;
   }
}

static void ProcessSMoSMessage(SMoSObject_t const * const message)
{
   /* Currently we only cared about confirmable requests (i.e expects a
      response from the Arduino). */
   if (smosService.smos_IsConfirmableRequest(message))
   {
       ProcessConfirmableRequest(message);
       return;
   }
}


/******************************
 * Arduino Setup() and Loop() *
 ******************************/

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

      /* If we get a start code, we will reset the hex string and start over. */
      if (smosService.smos_IsStartCode(c))
      {
         Serial.println("");
         ResetHexString();
         hexString[hexStringLength] = c;
         hexStringLength++;
         hexString[hexStringLength] = 0;

         return;
      }

      /* We have already started processing a hex string so carry on processing it. */
      if (hexStringLength != 0)
      {
         SMoSResult_e result;
         uint16_t expectedHexStringLength = 0;

         hexString[hexStringLength] = c;
         hexStringLength++;
         hexString[hexStringLength] = 0;

         /* We are reading data over the serial link, char by char. So make sure we read
            the whole string before processing it. */
         if (hexStringLength < smosService.smos_GetMinimumHexStringLength() ||
             smosService.smos_GetExpectedHexStringLength(hexString, hexStringLength, &expectedHexStringLength) != SMOS_RESULT_SUCCESS ||
             hexStringLength < expectedHexStringLength)
         {
            return;
         }

         result = smosService.smos_DecodeHexString(hexString, hexStringLength, &smosObject);

         switch (result)
         {
            case SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING:
            case SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE:
               /* String not long enough, do nothing for now. */
               break;

            case SMOS_RESULT_SUCCESS:
               ProcessSMoSMessage(&smosObject);
               ResetHexString();
               break;

            case SMOS_RESULT_ERROR_NULL_POINTER:
            case SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE:
            case SMOS_RESULT_ERROR_HEX_STRING_INVALID_CHECKSUM:
            default:
               /* We received a bad hex string. There's nothing we can do about it. */
               ResetHexString();
               break;
         }
      }
   }
}
