#ifndef SMOS_ENCODER_H
#define SMOS_ENCODER_H

#include "smosDefinitions.h"

SMoSResult_e smos_EncodeToHexString(const SMoSObject_t *message, char *hexString, uint16_t *hexStringLength);

#endif /* #define SMOS_ENCODER_H */
