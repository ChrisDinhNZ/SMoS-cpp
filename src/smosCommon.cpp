/**
 * SMoS - Library for encoding and decoding of SMoS messages.
 *        Please refer to https://github.com/ChrisDinhNZ/SMoS for more details.
 * Created by Chris Dinh, 2020
 * Released under MIT license
 * 
 * The library was derived from LibGIS IHex implementation (https://github.com/vsergeev/libGIS)
 */

/* HEADER INCLUDES */
#include "smosCommon.h"

/* CONSTANT DECLARATIONS */
#define NUMBER_OF_FIELDS_IN_SMOS_PDU 12U

/* FUNCTION DECLARATIONS */
static uint8_t smos_PackFieldIntoByte(const SMoSObject_t *message, SMoSPduFields_e pduField);

/* VARIABLE DECLARATIONS */
static SMoSPduFields_e pduFields[NUMBER_OF_FIELDS_IN_SMOS_PDU] =
{
   SMOS_PDU_FIELD_IDENTIFIER_BYTE_COUNT,
   SMOS_PDU_FIELD_IDENTIFIER_VERSION,
   SMOS_PDU_FIELD_IDENTIFIER_CONTEXT_TYPE,
   SMOS_PDU_FIELD_IDENTIFIER_LAST_BLOCK_FLAG,
   SMOS_PDU_FIELD_IDENTIFIER_BLOCK_SEQUENCE_INDEX,
   SMOS_PDU_FIELD_IDENTIFIER_CODE_CLASS,
   SMOS_PDU_FIELD_IDENTIFIER_CODE_DETAIL,
   SMOS_PDU_FIELD_IDENTIFIER_MESSAGE_ID,
   SMOS_PDU_FIELD_IDENTIFIER_OBSERVE_FLAG,
   SMOS_PDU_FIELD_IDENTIFIER_OBSERVE_NOTIFICATION_INDEX,
   SMOS_PDU_FIELD_IDENTIFIER_RESOURCE_INDEX,
   SMOS_PDU_FIELD_IDENTIFIER_PAYLOAD
};

/* FUNCTION DEFINITIONS */
uint8_t smos_CreateChecksum(const SMoSObject_t *message)
{
   uint8_t checksum, i;

   checksum = 0;

   for (i = 0; i < NUMBER_OF_FIELDS_IN_SMOS_PDU; i++)
   {
      if (pduFields[i] == SMOS_PDU_FIELD_IDENTIFIER_PAYLOAD)
      {
         uint8_t j;

         for (j = 0; j < message->byteCount; j++)
         {
            checksum += message->payload[j];
         }
      }
      else
      {
         checksum += smos_PackFieldIntoByte(message, pduFields[i]);
      }
   }

   /* Two's complement on checksum */
	checksum = ~checksum + 1;

   return checksum;
}

bool smos_ValidateChecksum(const uint8_t checksum, const SMoSObject_t *message)
{
   return checksum == smos_CreateChecksum(message);
}

uint16_t smos_GetMinimumHexStringLength(void)
{
   return SMOS_HEX_STRING_MIN_LENGTH;
}

bool smos_IsStartCode(const char c);
{
   return c == SMOS_START_CODE;
}

bool smos_IsConfirmableRequest(const SMoSObject_t *message)
{
   return (message->contextType == SMOS_CONTEXT_TYPE_CON && message->codeClass == SMOS_CODE_CLASS_REQ);
}


static uint8_t smos_PackFieldIntoByte(const SMoSObject_t *message, SMoSPduFields_e pduField)
{
   uint8_t pduFieldPacked = 0;

   switch (pduField)
   {
      case SMOS_PDU_FIELD_IDENTIFIER_BYTE_COUNT:
         pduFieldPacked |=
             ((message->byteCount << SMOS_BYTE_COUNT_LSB_OFFSET) & SMOS_BYTE_COUNT_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_VERSION:
         pduFieldPacked |=
             ((message->version << SMOS_VERSION_LSB_OFFSET) & SMOS_VERSION_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_CONTEXT_TYPE:
         pduFieldPacked |=
             ((message->contextType << SMOS_CONTEXT_TYPE_LSB_OFFSET) & SMOS_CONTEXT_TYPE_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_LAST_BLOCK_FLAG:
         pduFieldPacked |=
             (((uint8_t)(message->lastBlockFlag) << SMOS_LAST_BLOCK_FLAG_LSB_OFFSET) & SMOS_LAST_BLOCK_FLAG_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_BLOCK_SEQUENCE_INDEX:
         pduFieldPacked |=
             ((message->blockSequenceIndex << SMOS_BLOCK_SEQUENCE_INDEX_LSB_OFFSET) & SMOS_BLOCK_SEQUENCE_INDEX_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_CODE_CLASS:
         pduFieldPacked |=
             ((message->codeClass << SMOS_CODE_CLASS_LSB_OFFSET) & SMOS_CODE_CLASS_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_CODE_DETAIL:
         /* We assume code class has been set. */
         if (message->codeClass == SMOS_CODE_CLASS_REQ)
         {
            pduFieldPacked |=
             ((message->codeDetailRequest << SMOS_CODE_DETAIL_LSB_OFFSET) & SMOS_CODE_DETAIL_BIT_MASK);
         }
         else
         {
            pduFieldPacked |=
             ((message->codeDetailResponse << SMOS_CODE_DETAIL_LSB_OFFSET) & SMOS_CODE_DETAIL_BIT_MASK);
         }
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_MESSAGE_ID:
         pduFieldPacked |=
             ((message->messageId << SMOS_MESSAGE_ID_LSB_OFFSET) & SMOS_MESSAGE_ID_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_OBSERVE_FLAG:
         pduFieldPacked |=
             (((uint8_t)(message->observeFlag) << SMOS_OBSERVE_FLAG_LSB_OFFSET) & SMOS_OBSERVE_FLAG_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_OBSERVE_NOTIFICATION_INDEX:
         pduFieldPacked |=
             ((message->observeNotificationIndex << SMOS_OBSERVE_NOTIFICATION_INDEX_LSB_OFFSET) & SMOS_OBSERVE_NOTIFICATION_INDEX_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_RESOURCE_INDEX:
         pduFieldPacked |=
             ((message->ResourceId << SMOS_RESOURCE_INDEX_LSB_OFFSET) & SMOS_RESOURCE_INDEX_BIT_MASK);
         break;

      case SMOS_PDU_FIELD_IDENTIFIER_PAYLOAD:
         /* We don't handle payload in here. */
         break;
   }

   return pduFieldPacked;
}