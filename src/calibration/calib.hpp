#ifndef __CALIB_H__
#define __CALIB_H__

#include <util/crc16.h>
#include <avr/eeprom.h>
#include <string.h>

#define CALIB_VERSION   1
#define CALIB_START_ADDR    (0x0000u)

typedef struct t_calib_v1
{
    int16_t mag[3];
    int16_t acc[3];
} t_calib_v1;

t_calib_v1 getCalibData (void);
bool getCalibDataValid (void);
void setCalibData (const t_calib_v1 &calibData);

#endif // __CALIB_H__