/**
 * SMoS - Library for encoding and decoding of SMoS messages.
 *        Please refer to https://github.com/ChrisDinhNZ/SMoS for more details.
 * Created by Chris Dinh, 2020
 * Released under MIT license
 * 
 * The library was derived from LibGIS IHex implementation (https://github.com/vsergeev/libGIS)
 */

/* HEADER INCLUDES */
#include "smosDecoder.h"

/* CONSTANT DECLARATIONS */

/* FUNCTION DECLARATIONS */

/* VARIABLE DECLARATIONS */

/* FUNCTION DEFINITIONS */

SMoSResult_e smos_DecodeFromHexString(const char *hexString,
                                      const uint16_t hexStringLength,
                                      SMoSObject_t *message)
{
   uint8_t currentByte, i;
   char hexBuff[HEX_STR_LENGTH_PER_BYTE + 1]; /* Null terminated */

   if (hexString == NULL || message == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (hexStringLength < SMOS_HEX_STRING_MIN_LENGTH)
   {
      return SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING;
   }

   strncpy(hexBuff, hexString + SMOS_BYTE_COUNT_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   /* Check Hex string length. Note that actual Hex string length will be truncated (ignored)
      if longer than expected Hex string length. */
   if (hexStringLength < SMOS_HEX_STRING_MIN_LENGTH + currentByte * HEX_STR_LENGTH_PER_BYTE)
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE;
   }

   if (hexString[0] != SMOS_START_CODE_VALUE)
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE;
   }

   message->byteCount = currentByte;

   strncpy(hexBuff, hexString + SMOS_CONTEXT_TYPE_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->version = (currentByte & SMOS_VERSION_BIT_MASK) >> SMOS_VERSION_LSB_OFFSET;
   message->contextType = (SMoSContextType_e)((currentByte & SMOS_CONTEXT_TYPE_BIT_MASK) >> SMOS_CONTEXT_TYPE_LSB_OFFSET);
   message->lastBlockFlag = (bool)((currentByte & SMOS_LAST_BLOCK_FLAG_BIT_MASK) >> SMOS_LAST_BLOCK_FLAG_LSB_OFFSET);
   message->blockSequenceIndex = (currentByte & SMOS_BLOCK_SEQUENCE_INDEX_BIT_MASK) >> SMOS_BLOCK_SEQUENCE_INDEX_LSB_OFFSET;

   strncpy(hexBuff, hexString + SMOS_CODE_CLASS_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->codeClass = (SMoSCodeClass_e)((currentByte & SMOS_CODE_CLASS_BIT_MASK) >> SMOS_CODE_CLASS_LSB_OFFSET);

   if (message->codeClass == SMOS_CODE_CLASS_REQ)
   {
      message->codeDetailRequest = (SMoSCodeDetailRequest_e)((currentByte & SMOS_CODE_DETAIL_BIT_MASK) >> SMOS_CODE_DETAIL_LSB_OFFSET);
   }
   else
   {
      message->codeDetailResponse = (SMoSCodeDetailResponse_e)((currentByte & SMOS_CODE_DETAIL_BIT_MASK) >> SMOS_CODE_DETAIL_LSB_OFFSET);
   }

   strncpy(hexBuff, hexString + SMOS_MESSAGE_ID_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->messageId = currentByte;

   strncpy(hexBuff, hexString + SMOS_OBSERVE_FLAG_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->observeFlag = (bool)((currentByte & SMOS_OBSERVE_FLAG_BIT_MASK) >> SMOS_OBSERVE_FLAG_LSB_OFFSET);
   message->observeNotificationIndex = (currentByte & SMOS_OBSERVE_NOTIFICATION_INDEX_BIT_MASK) >> SMOS_OBSERVE_NOTIFICATION_INDEX_LSB_OFFSET;

   strncpy(hexBuff, hexString + SMOS_RESOURCE_INDEX_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->resourceIndex = currentByte;

   /* Decode payload. */
   for (i = 0; i < message->byteCount; i++)
   {
      strncpy(hexBuff,
              hexString + SMOS_PAYLOAD_HEX_STR_OFFSET + HEX_STR_LENGTH_PER_BYTE * i,
              HEX_STR_LENGTH_PER_BYTE);

      hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
      message->payload[i] = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);
   }

   strncpy(hexBuff, hexString + SMOS_PAYLOAD_HEX_STR_OFFSET + message->byteCount * HEX_STR_LENGTH_PER_BYTE, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   if (!smos_ValidateChecksum(currentByte, message))
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INVALID_CHECKSUM;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e smos_GetExpectedHexStringLength(const char *hexString,
                                             const uint16_t hexStringLength,
                                             uint16_t *expectedHexStringLength)
{
   /* As bytes are being sent across the wire, it would be nice to know how many bytes
   we need to make up a message. */

   char hexBuff[HEX_STR_LENGTH_PER_BYTE + 1]; /* Null terminated */
   uint8_t byteCount;

   if (hexString == NULL || expectedHexStringLength == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (hexStringLength < SMOS_HEX_STRING_MIN_LENGTH)
   {
      return SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING;
   }

   strncpy(hexBuff, hexString + SMOS_BYTE_COUNT_HEX_STR_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;

   byteCount = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   *expectedHexStringLength = SMOS_HEX_STRING_MIN_LENGTH + byteCount * HEX_STR_LENGTH_PER_BYTE;

   return SMOS_RESULT_SUCCESS;
}