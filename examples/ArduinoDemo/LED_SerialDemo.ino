/*
 * ArduinoDemo:
 *
 * Copyright Chris Dinh 2020
 */

#include "Arduino.h"
#include "smosServer.h"

#define RESOURCE_ID_FOR_SWITCH 0x01

static SMoSObject_t smosObject;
static char hexString[SMOS_HEX_STRING_MAX_LENGTH + 1]; /* NULL terminated string */
static uint16_t hexStringLength;
static bool switchIsOn = false;

static void ResetBuiltInLedResource(void)
{
   switchIsOn = false;
   pinMode(LED_BUILTIN, OUTPUT);
   digitalWrite(LED_BUILTIN, LOW);
}

static void ResetHexString(void)
{
   memset(hexString, 0, sizeof(hexString[0]) * (SMOS_HEX_STRING_MAX_LENGTH + 1));
   hexStringLength = 0;
}

static void SetBuiltInLedState(bool on)
{
   if (on)
   {
      switchIsOn = true;
      digitalWrite(LED_BUILTIN, HIGH);
   }
   else
   {
      switchIsOn = false;
      digitalWrite(LED_BUILTIN, LOW);
   }
}

static bool IsBuiltInLedOn(void)
{
   return switchIsOn;
}

static void ProcessConfirmableRequest(SMoSObject_t const * const reqMessage)
{
   char respHexString[SMOS_HEX_STRING_MAX_LENGTH + 1]; /* NULL terminated string */
   SMoSObject_t respMessage;

   memset(respHexString, 0, sizeof(respHexString[0]) * (SMOS_HEX_STRING_MAX_LENGTH + 1));
   memset(&respMessage, 0, sizeof(respMessage));

   /* Populate response fields that are common. */
      respMessage.version = SMOS_VERSION_CURRENT;
      respMessage.lastBlockFlag = true;
      respMessage.messageId = reqMessage->messageId;
      respMessage.resourceIndex = reqMessage->resourceIndex;
      respMessage.contextType = SMOS_CONTEXT_TYPE_ACK;

   /* The only resource is the switch. Anything else, send a resource not found. */
   if (reqMessage->resourceIndex != RESOURCE_ID_FOR_SWITCH)
   {
      respMessage.codeClass = SMOS_CODE_CLASS_RESP_CLIENT_ERROR;
      respMessage.codeDetailResponse = SMOS_CODE_DETAIL_CLIENT_ERROR_NOT_FOUND;

      if (smos_EncodeToHexString(&respMessage, respHexString) == SMOS_RESULT_SUCCESS)
      {
         Serial.println(respHexString);
      }
      else
      {
         Serial.println("Failed to encode hex string");
      }

      return;
   }

   switch (reqMessage->codeDetailRequest)
   {
      case SMOS_CODE_DETAIL_GET:
         /* Note that we are ignoring observe request for now. Just return direct queries response. */
         respMessage.byteCount = 0x01;
         respMessage.codeClass = SMOS_CODE_CLASS_RESP_SUCCESS;
         respMessage.codeDetailResponse = SMOS_CODE_DETAIL_SUCCESS_CONTENT;
         respMessage.payload[0] = (uint8_t)IsBuiltInLedOn();

         if (smos_EncodeToHexString(&respMessage, respHexString) == SMOS_RESULT_SUCCESS)
         {
            Serial.println(respHexString);
         }
         else
         {
            Serial.println("Failed to encode hex string");
         }
         break;

      case SMOS_CODE_DETAIL_POST:
      case SMOS_CODE_DETAIL_DELETE:
         respMessage.codeClass = SMOS_CODE_CLASS_RESP_CLIENT_ERROR;
         respMessage.codeDetailResponse = SMOS_CODE_DETAIL_CLIENT_ERROR_METHOD_NOT_ALLOWED;

         if (smos_EncodeToHexString(&respMessage, respHexString) == SMOS_RESULT_SUCCESS)
         {
            Serial.println(respHexString);
         }
         else
         {
            Serial.println("Failed to encode hex string");
         }
         break;

      case SMOS_CODE_DETAIL_PUT:
         /* Update the built-in LED based on the request. */
         if (reqMessage->byteCount == 0)
         {
            respMessage.codeClass = SMOS_CODE_CLASS_RESP_CLIENT_ERROR;
            respMessage.codeDetailResponse = SMOS_CODE_DETAIL_CLIENT_ERROR_BAD_REQUEST;

            if (smos_EncodeToHexString(&respMessage, respHexString) == SMOS_RESULT_SUCCESS)
            {
               Serial.println(respHexString);
            }
            else
            {
               Serial.println("Failed to encode hex string");
            }

            return;
         }

         SetBuiltInLedState((bool)(reqMessage->payload[0]));

         respMessage.codeClass = SMOS_CODE_CLASS_RESP_SUCCESS;
         respMessage.codeDetailResponse = SMOS_CODE_DETAIL_SUCCESS_CHANGED;

         if (smos_EncodeToHexString(&respMessage, respHexString) == SMOS_RESULT_SUCCESS)
         {
            Serial.println(respHexString);
         }
         else
         {
            Serial.println("Failed to encode hex string");
         }

         break;
   }
}

static void ProcessSMoSMessage(SMoSObject_t const * const message)
{
   /* Currently we only cared about confirmable requests (i.e expects a
      response from the Arduino). */
   if (smos_IsConfirmableRequest(message))
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
      if (smos_IsStartCode(c))
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
         if (hexStringLength < smos_GetMinimumHexStringLength() ||
             smos_GetExpectedHexStringLength(hexString, hexStringLength, &expectedHexStringLength) != SMOS_RESULT_SUCCESS ||
             hexStringLength < expectedHexStringLength)
         {
            return;
         }

         result = smos_DecodeFromHexString(hexString, hexStringLength, &smosObject);

         switch (result)
         {
            case SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING:
            case SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE:
               /* String not long enough, do nothing for now. */
               Serial.println("Incomplete hex string");
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
               Serial.println("Failed to decode hex string");
               break;
         }
      }
   }
}
