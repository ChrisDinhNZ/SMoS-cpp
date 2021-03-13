#ifndef SMOS_COMMON_H
#define SMOS_COMMON_H

/* HEADER INCLUDES */
#include "smosDefinitions.h"

/* FUNCTION DECLARATIONS */
uint8_t smos_CreateChecksum(const SMoSObject_t *message);
bool smos_ValidateChecksum(const uint8_t checksum, const SMoSObject_t *message);

uint16_t smos_GetMinimumHexStringLength(void);
bool smos_IsStartCode(const char c);

bool smos_IsConfirmableRequest(const SMoSObject_t *message);

#endif /* #define SMOS_COMMON_H */
