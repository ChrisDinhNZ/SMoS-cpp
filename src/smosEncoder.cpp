/**
 * SMoS - Library for encoding and decoding of SMoS messages.
 *        Please refer to https://github.com/ChrisDinhNZ/SMoS for more details.
 * Created by Chris Dinh, 2020
 * Released under MIT license
 * 
 * The library was derived from LibGIS IHex implementation (https://github.com/vsergeev/libGIS)
 */

/* HEADER INCLUDES */
#include "smosEncoder.h"

/* CONSTANT DECLARATIONS */

/* FUNCTION DECLARATIONS */

/* VARIABLE DECLARATIONS */

/* FUNCTION DEFINITIONS */

SMoSResult_e smos_EncodeToHexString(const SMoSObject_t *message, char *hexString)
{
   uint8_t currentByte, i;

   if (message == NULL || hexString == NULL)
   {
      return SMOS_RESULT_ERROR_NULL_POINTER;
   }

   if (message->byteCount > SMOS_PAYLOAD_MAX_BYTE_COUNT)
   {
      return SMOS_RESULT_ERROR_EXCEED_MAX_DATA_SIZE;
   }

   /* Encode Start Code */
   hexString += sprintf(hexString, "%c", SMOS_START_CODE_VALUE);

   /* Encode Byte Count */
   hexString += sprintf(hexString, "%02X", message->byteCount);

   /* Encode Version, Context Type, Last Block Flag, Block Sequence Index */
   currentByte = ((message->version << SMOS_VERSION_LSB_OFFSET) & SMOS_VERSION_BIT_MASK);
   currentByte |= ((message->contextType << SMOS_CONTEXT_TYPE_LSB_OFFSET) & SMOS_CONTEXT_TYPE_BIT_MASK);
   currentByte |= (((uint8_t)(message->lastBlockFlag) << SMOS_LAST_BLOCK_FLAG_LSB_OFFSET) & SMOS_LAST_BLOCK_FLAG_BIT_MASK);
   currentByte |= ((message->blockSequenceIndex << SMOS_BLOCK_SEQUENCE_INDEX_LSB_OFFSET) & SMOS_BLOCK_SEQUENCE_INDEX_BIT_MASK);
   hexString += sprintf(hexString, "%02X", currentByte);

   /* Encode Code Class, Code Detail */
   currentByte = ((message->codeClass << SMOS_CODE_CLASS_LSB_OFFSET) & SMOS_CODE_CLASS_BIT_MASK);
   if (message->codeClass == SMOS_CODE_CLASS_REQ)
   {
      currentByte |= ((message->codeDetailRequest << SMOS_CODE_DETAIL_LSB_OFFSET) & SMOS_CODE_DETAIL_BIT_MASK);
   }
   else
   {
      currentByte |= ((message->codeDetailResponse << SMOS_CODE_DETAIL_LSB_OFFSET) & SMOS_CODE_DETAIL_BIT_MASK);
   }
   hexString += sprintf(hexString, "%02X", currentByte);

   /* Encode Message ID */
   hexString += sprintf(hexString, "%02X", message->messageId);

   /* Encode Observe Flag, Observe Notification Index */
   currentByte = (((uint8_t)(message->observeFlag) << SMOS_OBSERVE_FLAG_LSB_OFFSET) & SMOS_OBSERVE_FLAG_BIT_MASK);
   currentByte |= ((message->observeNotificationIndex << SMOS_OBSERVE_NOTIFICATION_INDEX_LSB_OFFSET) & SMOS_OBSERVE_NOTIFICATION_INDEX_BIT_MASK);
   hexString += sprintf(hexString, "%02X", currentByte);

   /* Encode Resource Index */
   hexString += sprintf(hexString, "%02X", message->resourceIndex);

   /* Encode Payload */
   for (i = 0; i < message->byteCount; i++)
   {
      hexString += sprintf(hexString, "%02X", message->payload[i]);
   }

   /* Encode Checksum */
   hexString += sprintf(hexString, "%02X", smos_CreateChecksum(message));

   return SMOS_RESULT_SUCCESS;
}
