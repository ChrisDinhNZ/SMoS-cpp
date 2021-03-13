#ifndef SMOS_DECODER_H
#define SMOS_DECODER_H

#include "smosDefinitions.h"

SMoSResult_e smos_DecodeFromHexString(const char *hexString,
                                      const uint16_t hexStringLength,
                                      SMoSObject_t *message);

SMoSResult_e smos_GetExpectedHexStringLength(const char *hexString,
                                             const uint16_t hexStringLength,
                                             uint16_t *expectedHexStringLength);

#endif /* #define SMOS_DECODER_H */
