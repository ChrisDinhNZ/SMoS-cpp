/**
 * SMoS - Library for encoding and decoding of SMoS messages.
 *        Please refer to https://github.com/ChrisDinhNZ/SMoS for more details.
 * Created by Chris Dinh, 2020
 * Released under MIT license
 * 
 * The library was derived from LibGIS IHex implementation (https://github.com/vsergeev/libGIS)
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "smos.h"


/***********************
 *** PRIVATE MEMBERS ***
 ***********************/

uint8_t SMoS::CreateChecksum(
   const SMoSObject *message)
{
   uint8_t checksum, i;

   /* Note that checksum does not include the start code ':' */
   checksum = message->byteCount;
   checksum += CalculateContextByteInfo(message, 0);
   checksum += CalculateContextByteInfo(message, 1);
   checksum += CalculateContextByteInfo(message, 2);

   for (i = 0; i < message->byteCount; i++)
   {
      checksum += message->dataContent[i];
   }

   /* Two's complement on checksum */
	checksum = ~checksum + 1;

   return checksum;
}

bool SMoS::ValidateChecksum(
    const uint8_t checksum,
    const SMoSObject *message)
{
   return checksum == CreateChecksum(message);
}

uint16_t SMoS::ConvertMessageToHexString(
   const SMoSObject *message,
   char *hexString)
{
   uint8_t i;
   uint16_t hexStringLength = 0;

   if (message == NULL ||
       hexString == NULL ||
       message->byteCount > SMOS_MAX_DATA_BYTE_LEN)
   {
      return 0;
   }

   hexString += sprintf(hexString, "%c", message->startCode);
   hexStringLength += 1;
   hexString += sprintf(hexString, "%02X", message->byteCount);
   hexStringLength += 2;
   hexString += sprintf(hexString, "%02X", CalculateContextByteInfo(message, 0));
   hexStringLength += 2;
   hexString += sprintf(hexString, "%02X", CalculateContextByteInfo(message, 1));
   hexStringLength += 2;
   hexString += sprintf(hexString, "%02X", CalculateContextByteInfo(message, 2));
   hexStringLength += 2;

   for (i = 0; i < message->byteCount; i++)
   {
      hexString += sprintf(hexString, "%02X", message->dataContent[i]);
      hexStringLength += 2;
   }

   hexString += sprintf(hexString, "%02X", message->checksum);
   hexStringLength += 2;

   return hexStringLength;
}

uint8_t SMoS::CalculateContextByteInfo(
    const SMoSObject *message,
    uint8_t contextByteIndex)
{
   uint8_t tempByte = 0;

   switch (contextByteIndex)
   {
      case 0:
         tempByte |= ((message->contextType << SMOS_CONTEXT_TYPE_LSB_OFFSET) & SMOS_CONTEXT_TYPE_BIT_MASK);
         tempByte |= ((message->contentType << SMOS_CONTENT_TYPE_LSB_OFFSET) & SMOS_CONTENT_TYPE_BIT_MASK);
         tempByte |= ((message->contentTypeOptions << SMOS_CONTENT_TYPE_OPTIONS_LSB_OFFSET) & SMOS_CONTENT_TYPE_OPTIONS_BIT_MASK);
         break;

      case 1:
         tempByte |= ((message->codeClass << SMOS_CODE_CLASS_LSB_OFFSET) & SMOS_CODE_CLASS_BIT_MASK);
         tempByte |= ((message->codeDetail << SMOS_CODE_DETAIL_LSB_OFFSET) & SMOS_CODE_DETAIL_BIT_MASK);
         break;

      case 2:
         tempByte |= ((message->messageId << SMOS_MESSAGE_ID_LSB_OFFSET) & SMOS_MESSAGE_ID_BIT_MASK);
         tempByte |= ((message->tokenId << SMOS_TOKEN_ID_LSB_OFFSET) & SMOS_TOKEN_ID_BIT_MASK);
         break;

      default:
         break;
   }

   return tempByte;
}


/**********************
 *** PUBLIC MEMBERS ***
 **********************/

SMoSResult_e SMoS::smos_EncodeGetMessage(
    uint8_t byteCount,
    SMoSContentType_e contentType,
    uint8_t contentTypeOptions,
    uint8_t messageId,
    const uint8_t *dataContent,
    char *hexString)
{
   SMoSObject message;

   if (dataContent == NULL || hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (byteCount > SMOS_MAX_DATA_BYTE_LEN)
   {
      return SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE;
   }

   memset(&message, 0, sizeof(message));
   hexString[0] = 0;

   message.startCode = SMOS_START_CODE;
   message.byteCount = byteCount;
   message.contextType = SMOS_CONTEXT_TYPE_CON;
   message.contentType = contentType;
   message.contentTypeOptions = contentTypeOptions;
   message.codeClass = SMOS_CODE_CLASS_REQ;
   message.codeDetail = SMOS_CODE_DETAIL_GET;
   message.messageId = messageId;
   message.tokenId = 0;
   memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e SMoS::smos_EncodePutMessage(
    uint8_t byteCount,
    SMoSContentType_e contentType,
    uint8_t contentTypeOptions,
    uint8_t messageId,
    const uint8_t *dataContent,
    char *hexString)
{
   SMoSObject message;

   if (dataContent == NULL || hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (byteCount > SMOS_MAX_DATA_BYTE_LEN)
   {
      return SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE;
   }

   memset(&message, 0, sizeof(message));
   hexString[0] = 0;

   message.startCode = SMOS_START_CODE;
   message.byteCount = byteCount;
   message.contextType = SMOS_CONTEXT_TYPE_CON;
   message.contentType = contentType;
   message.contentTypeOptions = contentTypeOptions;
   message.codeClass = SMOS_CODE_CLASS_REQ;
   message.codeDetail = SMOS_CODE_DETAIL_PUT;
   message.messageId = messageId;
   message.tokenId = 0;
   memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e SMoS::smos_EncodePiggyBackAckMessage(
    uint8_t byteCount,
    SMoSContentType_e contentType,
    uint8_t contentTypeOptions,
    SMoSCodeClass_e codeClass,
    SMoSCodeDetailResponse_e codeDetailResponse,
    uint8_t messageId,
    const uint8_t *dataContent,
    char *hexString)
{
   SMoSObject message;

   if (dataContent == NULL || hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (byteCount > SMOS_MAX_DATA_BYTE_LEN)
   {
      return SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE;
   }

   memset(&message, 0, sizeof(message));
   hexString[0] = 0;

   message.startCode = SMOS_START_CODE;
   message.byteCount = byteCount;
   message.contextType = SMOS_CONTEXT_TYPE_ACK;
   message.contentType = contentType;
   message.contentTypeOptions = contentTypeOptions;
   message.codeClass = codeClass;
   message.codeDetail = codeDetailResponse;
   message.messageId = messageId;
   message.tokenId = 0;
   memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e SMoS::smos_EncodeEmptyAckMessage(
         uint8_t messageId,
         char *hexString)
{
   SMoSObject message;

   if (hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   memset(&message, 0, sizeof(message));
   hexString[0] = 0;

   message.startCode = SMOS_START_CODE;
   message.contextType = SMOS_CONTEXT_TYPE_ACK;
   message.messageId = messageId;
   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e SMoS::smos_EncodeNonConfirmableResponseMessage(
    uint8_t byteCount,
    SMoSContentType_e contentType,
    uint8_t contentTypeOptions,
    SMoSCodeClass_e codeClass,
    SMoSCodeDetailResponse_e codeDetailResponse,
    uint8_t messageId,
    const uint8_t *dataContent,
    char *hexString)
{
   SMoSObject message;

   if (dataContent == NULL || hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (byteCount > SMOS_MAX_DATA_BYTE_LEN)
   {
      return SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE;
   }

   memset(&message, 0, sizeof(message));
   hexString[0] = 0;

   message.startCode = SMOS_START_CODE;
   message.byteCount = byteCount;
   message.contextType = SMOS_CONTEXT_TYPE_NON;
   message.contentType = contentType;
   message.contentTypeOptions = contentTypeOptions;
   message.codeClass = codeClass;
   message.codeDetail = codeDetailResponse;
   message.messageId = messageId;
   message.tokenId = 0;
   memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}

SMoSResult_e SMoS::smos_DecodeHexString(
    const char *hexString,
    const uint16_t hexStringLength,
    SMoSObject *message)
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

   strncpy(hexBuff, hexString + SMOS_HEX_STR_BYTECOUNT_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   /* Check Hex string length. Note that actual Hex string length will be truncated (ignored)
      if longer than expected Hex string length. */
   if (hexStringLength < SMOS_HEX_STRING_MIN_LENGTH + currentByte)
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE;
   }

   if (hexString[0] != SMOS_START_CODE)
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE;
   }

   message->startCode = hexString[0];
   message->byteCount = currentByte;

   strncpy(hexBuff, hexString + SMOS_HEX_STR_CONTEXT_TYPE_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->contextType = (currentByte & SMOS_CONTEXT_TYPE_BIT_MASK ) >> SMOS_CONTEXT_TYPE_LSB_OFFSET;
   message->contentType = (currentByte & SMOS_CONTENT_TYPE_BIT_MASK ) >> SMOS_CONTENT_TYPE_LSB_OFFSET;
   message->contentTypeOptions = (currentByte & SMOS_CONTENT_TYPE_OPTIONS_BIT_MASK ) >> SMOS_CONTENT_TYPE_OPTIONS_LSB_OFFSET;

   strncpy(hexBuff, hexString + SMOS_HEX_STR_CODE_CLASS_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->codeClass = (currentByte & SMOS_CODE_CLASS_BIT_MASK ) >> SMOS_CODE_CLASS_LSB_OFFSET;
   message->codeDetail = (currentByte & SMOS_CODE_DETAIL_BIT_MASK ) >> SMOS_CODE_DETAIL_LSB_OFFSET;

   strncpy(hexBuff, hexString + SMOS_HEX_STR_MESSAGE_ID_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->messageId = (currentByte & SMOS_MESSAGE_ID_BIT_MASK ) >> SMOS_MESSAGE_ID_LSB_OFFSET;
   message->tokenId = (currentByte & SMOS_TOKEN_ID_BIT_MASK ) >> SMOS_TOKEN_ID_LSB_OFFSET;

   /* Decode data content. */
   for (i = 0; i < message->byteCount; i++)
   {
      /* Times two i because every byte is represented by two ASCII hex characters */
      strncpy(hexBuff, hexString + SMOS_HEX_STR_DATA_OFFSET + 2 * i, HEX_STR_LENGTH_PER_BYTE);
      hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
      message->dataContent[i] = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);
   }

   strncpy(hexBuff, hexString + SMOS_HEX_STR_DATA_OFFSET + message->byteCount * 2, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   currentByte = (uint8_t)strtoul(hexBuff, (char **)NULL, 16);

   message->checksum = currentByte;

   if (!ValidateChecksum(message->checksum, message))
   {
      return SMOS_RESULT_ERROR_HEX_STRING_INVALID_CHECKSUM;
   }

   return SMOS_RESULT_SUCCESS;
}

/* As bytes are being sent across the wire, it would be nice to know how many bytes
   we need to make up a message. */
SMoSResult_e SMoS::smos_GetExpectedHexStringLength(
    const char *hexString,
    const uint16_t hexStringLength,
    uint8_t *expectedHexStringLength)
{
   char hexBuff[HEX_STR_LENGTH_PER_BYTE + 1]; /* Null terminated */

   if (hexString == NULL || expectedHexStringLength == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (hexStringLength < SMOS_HEX_STRING_MIN_LENGTH)
   {
      return SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING;
   }

   strncpy(hexBuff, hexString + SMOS_HEX_STR_BYTECOUNT_OFFSET, HEX_STR_LENGTH_PER_BYTE);
   hexBuff[HEX_STR_LENGTH_PER_BYTE] = 0;
   
   return (uint8_t)strtoul(hexBuff, (char **)NULL, 16);
}

uint16_t SMoS::smos_GetMinimumHexStringLength(void)
{
   return SMOS_HEX_STRING_MIN_LENGTH;
}

bool SMoS::smos_IsStartCode(const char c)
{
   return c == SMOS_START_CODE;
}

bool SMoS::smos_IsNonConfirmableRequest(const SMoSObject *message)
{
   return (message->contextType == SMOS_CONTEXT_TYPE_NON && message->codeClass == SMOS_CODE_CLASS_REQ);
}

bool SMoS::smos_IsConfirmableRequest(const SMoSObject *message)
{
   return (message->contextType == SMOS_CONTEXT_TYPE_CON && message->codeClass == SMOS_CODE_CLASS_REQ);
}
