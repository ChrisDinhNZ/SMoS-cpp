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
   SMoSObject_t message;

   if (hexString == NULL)
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

   if (dataContent != NULL && byteCount > 0)
   {
      memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   }

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
   SMoSObject_t message;

   if (hexString == NULL)
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

   if (dataContent != NULL && byteCount > 0)
   {
      memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   }

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
   SMoSObject_t message;

   if (hexString == NULL)
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

   if (dataContent != NULL && byteCount > 0)
   {
      memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   }

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
   SMoSObject_t message;

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
   SMoSObject_t message;

   if (hexString == NULL)
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

   if (dataContent != NULL && byteCount > 0)
   {
      memcpy(message.dataContent, dataContent, sizeof(dataContent[0]) * byteCount);
   }

   message.checksum = CreateChecksum(&message);

   if (ConvertMessageToHexString(&message, hexString) == 0)
   {
      return SMOS_RESULT_ERROR_ENCODE_MESSAGE;
   }

   return SMOS_RESULT_SUCCESS;
}


