/**
 * SMoS - Library for encoding and decoding of SMoS messages.
 *        Please refer to https://github.com/ChrisDinhNZ/SMoS for more details.
 * Created by Chris Dinh, 2020
 * Released under MIT license
 * 
 * The library was derived from LibGIS IHex implementation (https://github.com/vsergeev/libGIS)
 */

#ifndef SMOS_H
#define SMOS_H

#include <stdint.h>

typedef enum SMoSDefinitions_e
{
   /* Context type */
   SMOS_HEX_STR_CONTEXT_TYPE_OFFSET = 3,
   SMOS_CONTEXT_TYPE_LSB_OFFSET = 6,
   SMOS_CONTEXT_TYPE_BIT_MASK = 0xC0,

   /* Content type */
   SMOS_HEX_STR_CONTENT_TYPE_OFFSET = 3,
   SMOS_CONTENT_TYPE_LSB_OFFSET = 4,
   SMOS_CONTENT_TYPE_BIT_MASK = 0x30,

   /* Content type */
   SMOS_HEX_STR_CONTENT_TYPE_OPTIONS_OFFSET = 3,
   SMOS_CONTENT_TYPE_OPTIONS_LSB_OFFSET = 0,
   SMOS_CONTENT_TYPE_OPTIONS_BIT_MASK = 0x0F,

   /* Code class */
   SMOS_HEX_STR_CODE_CLASS_OFFSET = 5,
   SMOS_CODE_CLASS_LSB_OFFSET = 5,
   SMOS_CODE_CLASS_BIT_MASK = 0xE0,

   /* Code detail */
   SMOS_HEX_STR_CODE_DETAIL_OFFSET = 5,
   SMOS_CODE_DETAIL_LSB_OFFSET = 0,
   SMOS_CODE_DETAIL_BIT_MASK = 0x1F,

   /* Message Id */
   SMOS_HEX_STR_MESSAGE_ID_OFFSET = 7,
   SMOS_MESSAGE_ID_LSB_OFFSET = 4,
   SMOS_MESSAGE_ID_BIT_MASK = 0xF0,

   /* Token Id */
   SMOS_HEX_STR_TOKEN_ID_OFFSET = 7,
   SMOS_TOKEN_ID_LSB_OFFSET = 0,
   SMOS_TOKEN_ID_BIT_MASK = 0x0F,

   /* Data content */
   SMOS_HEX_STR_DATA_OFFSET = 9,
   SMOS_MAX_DATA_BYTE_LEN = 255,

   /* SMoS minimum Hex string length (i.e. when byte count is 0) */
   SMOS_HEX_STRING_MIN_LENGTH = 11,

   /* SMoS maximum Hex string length (i.e. when byte count is 255) */
   SMOS_HEX_STRING_MAX_LENGTH = 521,

   SMOS_START_CODE = 0x3A,

   HEX_STR_LENGTH_PER_BYTE = 2
};

typedef enum SMoSContextType_e
{
   SMOS_CONTEXT_TYPE_CON = 0x00,
   SMOS_CONTEXT_TYPE_NON = 0x01,
   SMOS_CONTEXT_TYPE_ACK = 0x02,
   SMOS_CONTEXT_TYPE_RST = 0x03,
};

typedef enum SMoSContentType_e
{
   SMOS_CONTENT_TYPE_GENERIC = 0x00,
   SMOS_CONTENT_TYPE_GATT = 0x01,
};

typedef enum SMoSCodeClass_e
{
   SMOS_CODE_CLASS_REQ = 0x00,
   SMOS_CODE_CLASS_RESP_SUCCESS = 0x02,
   SMOS_CODE_CLASS_RESP_CLIENT_ERROR = 0x04,
   SMOS_CODE_CLASS_RESP_SERVER_ERROR = 0x05,
};

typedef enum SMoSCodeDetailRequest_e
{
   SMOS_CODE_DETAIL_GET = 0x01,
   SMOS_CODE_DETAIL_OBSERVE = SMOS_CODE_DETAIL_GET,
   SMOS_CODE_DETAIL_POST = 0x02,
   SMOS_CODE_DETAIL_PUT = 0x03,
   SMOS_CODE_DETAIL_DELETE = 0x04,
};

typedef enum SMoSCodeDetailResponse_e
{
   SMOS_CODE_DETAIL_SUCCESS_CREATED = 0x01,
   SMOS_CODE_DETAIL_SUCCESS_DELETED = 0x02,
   SMOS_CODE_DETAIL_SUCCESS_VALID = 0x03,
   SMOS_CODE_DETAIL_SUCCESS_CHANGED = 0x04,
   SMOS_CODE_DETAIL_SUCCESS_CONTENT = 0x05,

   SMOS_CODE_DETAIL_CLIENT_ERROR_BAD_REQUEST = 0x00,
   SMOS_CODE_DETAIL_CLIENT_ERROR_UNAUTHORIZED = 0x01,
   SMOS_CODE_DETAIL_CLIENT_ERROR_BAD_OPTION = 0x02,
   SMOS_CODE_DETAIL_CLIENT_ERROR_FORBIDDEN = 0x03,
   SMOS_CODE_DETAIL_CLIENT_ERROR_NOT_FOUND = 0x04,
   SMOS_CODE_DETAIL_CLIENT_ERROR_METHOD_NOT_ALLOWED = 0x05,
   SMOS_CODE_DETAIL_CLIENT_ERROR_NOT_ACCEPTABLE = 0x06,
   SMOS_CODE_DETAIL_CLIENT_ERROR_PRECONDITION_FAILED = 0x0C,
   SMOS_CODE_DETAIL_CLIENT_ERROR_REQUEST_ENTITY_TOO_LARGE = 0x0D,
   SMOS_CODE_DETAIL_CLIENT_ERROR_UNSUPPORTED_CONTENT_FORMAT = 0x0F,

   SMOS_CODE_DETAIL_SERVER_ERROR_INTERNAL_SERVER_ERROR = 0x00,
   SMOS_CODE_DETAIL_SERVER_ERROR_NOT_IMPLEMENTED = 0x01,
   SMOS_CODE_DETAIL_SERVER_ERROR_BAD_GATEWAY = 0x02,
   SMOS_CODE_DETAIL_SERVER_ERROR_SERVICE_UNAVAILABLE = 0x03,
   SMOS_CODE_DETAIL_SERVER_ERROR_GATEWAY_TIMEOUT = 0x04,
   SMOS_CODE_DETAIL_SERVER_ERROR_PROXYING_NOT_SUPPORTED = 0x05
};

typedef enum SMoSResult_e
{
   SMOS_RESULT_UNKNOWN,
   SMOS_RESULT_SUCCESS,
   SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE,
   SMOS_RESULT_ERROR_NULL_POINTER,
   SMOS_RESULT_ERROR_ENCODE_MESSAGE,
   SMOS_RESULT_ERROR_NOT_MIN_LENGTH_HEX_STRING,
   SMOS_RESULT_ERROR_HEX_STRING_INCOMPLETE,
   SMOS_RESULT_ERROR_HEX_STRING_INVALID_STARTCODE,
   SMOS_RESULT_ERROR_HEX_STRING_INVALID_CHECKSUM
};

/**
 * Structure to hold the fields of an SMoS message.
 */
typedef struct SMoSObject
{
   uint8_t startCode;
   uint8_t byteCount;
   uint8_t contextType;
   uint8_t contentType;
   uint8_t contentTypeOptions;
   uint8_t codeClass;
   uint8_t codeDetail;
   uint8_t messageId;
   uint8_t tokenId;
   uint8_t dataContent[SMOS_MAX_DATA_BYTE_LEN];
   uint8_t checksum;
};

class SMoS
{
   private:

      uint8_t CreateChecksum(
         const SMoSObject *message);

      bool ValidateChecksum(
         const uint8_t checksum,
         const SMoSObject *message);

      uint16_t ConvertMessageToHexString(
         const SMoSObject *message,
         char *hexString);

      uint8_t CalculateContextByteInfo(
         const SMoSObject *message,
         uint8_t contextByteIndex);

   public:

      SMoSResult_e smos_EncodeGetMessage(
         uint8_t byteCount,
         SMoSContentType_e contentType,
         uint8_t contentTypeOptions,
         uint8_t messageId,
         const uint8_t *dataContent,
         char *hexString);

      SMoSResult_e smos_EncodePutMessage(
         uint8_t byteCount,
         SMoSContentType_e contentType,
         uint8_t contentTypeOptions,
         uint8_t messageId,
         const uint8_t *dataContent,
         char *hexString);

      SMoSResult_e smos_EncodePiggyBackAckMessage(
         uint8_t byteCount,
         SMoSContentType_e contentType,
         uint8_t contentTypeOptions,
         SMoSCodeClass_e codeClass,
         SMoSCodeDetailResponse_e codeDetailResponse,
         uint8_t messageId,
         const uint8_t *dataContent,
         char *hexString);

      SMoSResult_e smos_EncodeEmptyAckMessage(
         uint8_t messageId,
         char *hexString);

      SMoSResult_e smos_EncodeNonConfirmableResponseMessage(
         uint8_t byteCount,
         SMoSContentType_e contentType,
         uint8_t contentTypeOptions,
         SMoSCodeClass_e codeClass,
         SMoSCodeDetailResponse_e codeDetailResponse,
         uint8_t messageId,
         const uint8_t *dataContent,
         char *hexString);

      SMoSResult_e smos_DecodeHexString(
         const char *hexString,
         const uint16_t hexStringLength,
         SMoSObject *message);

      SMoSResult_e smos_GetExpectedHexStringLength(
         const char *hexString,
         const uint16_t hexStringLength,
         uint16_t *expectedHexStringLength);

      /* Returns the value of SMOS_HEX_STRING_MIN_LENGTH, this can be useful
         when receiving data across the serial link one char at a time. */
      uint16_t smos_GetMinimumHexStringLength(void);

      bool smos_IsStartCode(const char c);

      bool smos_IsNonConfirmableRequest(const SMoSObject *message);
      bool smos_IsConfirmableRequest(const SMoSObject *message);
      bool smos_IsNonConfirmableResponse(const SMoSObject *message);
      bool smos_IsConfirmableResponse(const SMoSObject *message);
      bool smos_IsResetMessage(const SMoSObject *message);
      bool smos_IsEmptyAcknowledgement(const SMoSObject *message);
      bool smos_IsPiggybackAcknowledgement(const SMoSObject *message);
};

#endif
