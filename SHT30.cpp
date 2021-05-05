#include "SHT30.h"

SHT30::SHT30(uint8_t addr, SHTAccuracy accuracy)
{
	_addr = addr;
	if(accuracy == SHT_ACCURACY_HIGH)
	{
		_i2cCommand = SHT3X_ACCURACY_HIGH;
		_duration = SHT3X_ACCURACY_HIGH_DURATION;
	}else if(accuracy == SHT_ACCURACY_MEDIUM)
	{
		_i2cCommand = SHT3X_ACCURACY_MEDIUM;
		_duration = SHT3X_ACCURACY_MEDIUM_DURATION;
	}else if(accuracy == SHT_ACCURACY_LOW)
	{
		_i2cCommand = SHT3X_ACCURACY_LOW;
		_duration = SHT3X_ACCURACY_LOW_DURATION;
	}
}

bool SHT30::init()
{
	bcm2835_i2c_setSlaveAddress(_addr);
	bcm2835_i2c_set_baudrate(100000);
	
	sendBuf[0] = SHT3X_RESET_STATUS >> 8;
	sendBuf[1] = SHT3X_RESET_STATUS & 0xFF;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s:%d, errCode = 0x%x\n", __FILE__, __LINE__ ,errCode);
		return false;
	}
	
	bcm2835_delay(50);
	
	sendBuf[0] = SHT3X_READ_STATUS >> 8;
	sendBuf[1] = SHT3X_READ_STATUS & 0xFF;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s:%d, errCode = 0x%x\n", __FILE__, __LINE__ ,errCode);
		return false;
	}
	bcm2835_delay(2);
	if((errCode = bcm2835_i2c_read(recvBuf,3)))
	{
		printf("bcm2835_i2c_read failed at %s:%d, errCode = 0x%x\n", __FILE__, __LINE__ ,errCode);
		return false;
	}
	if (crc8(&recvBuf[0], 2) != recvBuf[2]) {
		printf("STH30 Bad CRC at %s:%d\n",__FILE__,__LINE__);
		return false;
	}
	printf("%x %x\n",recvBuf[0],recvBuf[1]);
	if(recvBuf[0]&0x80)
	{
		printf("There was at least one pending alert in SHT\n");
		return false;
	}
	return true;
}

bool SHT30::setAccuracy(SHTAccuracy newAccuracy)
{
	switch (newAccuracy) {
		case SHT_ACCURACY_HIGH:
			_i2cCommand = SHT3X_ACCURACY_HIGH;
			_duration = SHT3X_ACCURACY_HIGH_DURATION;
		break;
		case SHT_ACCURACY_MEDIUM:
			_i2cCommand = SHT3X_ACCURACY_MEDIUM;
			_duration = SHT3X_ACCURACY_MEDIUM_DURATION;
		break;
		case SHT_ACCURACY_LOW:
			_i2cCommand = SHT3X_ACCURACY_LOW;
			_duration = SHT3X_ACCURACY_LOW_DURATION;
		break;
		default:
		return false;
	}
	return true;
}

bool SHT30::read()
{
	bcm2835_i2c_setSlaveAddress(_addr);
	sendBuf[0] = _i2cCommand >> 8;
	sendBuf[1] = _i2cCommand & 0xff;
	if((errCode = bcm2835_i2c_write(sendBuf,2)))
	{
		printf("bcm2835_i2c_write failed at %s:%d, errCode = 0x%x\n", __FILE__, __LINE__ ,errCode);
		return false;
	}
	bcm2835_delay(_duration);
	if((errCode = bcm2835_i2c_read(recvBuf,6)))
	{
		printf("bcm2835_i2c_read failed at %s:%d, errCode = 0x%x\n", __FILE__, __LINE__ ,errCode);
		return false;
	}
	
	// -- Important: assuming each 2 byte of data is followed by 1 byte of CRC
	// check CRC for both RH and T
	if (crc8(&recvBuf[0], 2) != recvBuf[2] || crc8(&recvBuf[3], 2) != recvBuf[5]) {
		printf("STH30 Bad CRC at %s:%d",__FILE__,__LINE__);
		return false;
	}
	
	// convert to Temperature/Humidity
	uint16_t val;
	val = (recvBuf[0] << 8) + recvBuf[1];
	_temp = -45.f +  val / 374.485714f;

	val = (recvBuf[3] << 8) + recvBuf[4];
	_humi = val / 655.35f;
	
	return true;
}

uint8_t SHT30::crc8(const char *data, uint8_t len)
{
  // adapted from SHT21 sample code from
  // http://www.sensirion.com/en/products/humidity-temperature/download-center/

  uint8_t crc = 0xff;
  uint8_t byteCtr;
  for (byteCtr = 0; byteCtr < len; ++byteCtr) {
    crc ^= data[byteCtr];
    for (uint8_t bit = 8; bit > 0; --bit) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x31;
      } else {
        crc = (crc << 1);
      }
    }
  }
  return crc;
}
