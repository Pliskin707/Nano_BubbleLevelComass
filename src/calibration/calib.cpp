#include "calib.hpp"

static struct
{
    t_calib_v1 data;
    bool read;
    bool valid;
} _calib = {{0}, false, false};

static uint16_t _calcCalibCrc (const t_calib_v1& calibData)
{
    uint16_t
        crc_calc = 0xFF1D;

    uint_fast8_t
        ii;

    const uint8_t
        * const data = (uint8_t *) &calibData;

    for (ii = 0; ii < sizeof(calibData); ii++)
        crc_calc = _crc16_update(crc_calc, data[ii]);

    return crc_calc;
}

t_calib_v1 getCalibData(void)
{
    if (!_calib.read)
    {
        _calib.read = true;

        uint16_t 
            crc_calc = 0xFF1D,
            crc_read = 0;

        // read from eeprom
        eeprom_read_block(&_calib.data, CALIB_START_ADDR, sizeof(_calib.data));

        // compare checksum
        crc_read = eeprom_read_word((const uint16_t *) (CALIB_START_ADDR + sizeof(_calib.data)));
        crc_calc = _calcCalibCrc(_calib.data);
            
        _calib.valid = (crc_calc == crc_read);
        if (!_calib.valid)
            memset(&_calib.data, 0, sizeof(_calib.data));
    }

    return _calib.data;
}

bool getCalibDataValid(void)
{
    if (!_calib.read)
        getCalibData(); // checks the crc and updates the "valid" flag

    return _calib.valid;
}

void setCalibData(const t_calib_v1 &calibData)
{
    uint16_t
        crc_calc = _calcCalibCrc(calibData);

    eeprom_write_block(&calibData, CALIB_START_ADDR, sizeof(calibData));
    eeprom_write_word((uint16_t *) (CALIB_START_ADDR + sizeof(calibData)), crc_calc);
    _calib.read = false;
}
