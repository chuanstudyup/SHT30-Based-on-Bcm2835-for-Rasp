#ifndef SHT30_H
#define SHT30_H

#include <cstdio>
#include <cstdint>
#include <bcm2835.h>

#define SHT30_DEFAULT_ADDR 0x44

#define SHT3X_ACCURACY_HIGH 0x2400
#define SHT3X_ACCURACY_MEDIUM 0x240b
#define SHT3X_ACCURACY_LOW 0x2416

#define SHT3X_ACCURACY_HIGH_DURATION 15
#define SHT3X_ACCURACY_MEDIUM_DURATION 6
#define SHT3X_ACCURACY_LOW_DURATION 4

#define SHT3X_RESET_STATUS 0x3041
#define SHT3X_READ_STATUS 0xF32D

/**
* Accuracy setting of measurement.
* Not all sensors support changing the sampling accuracy.
*/
enum SHTAccuracy {
/** Highest repeatability at the cost of slower measurement */
SHT_ACCURACY_HIGH,
/** Balanced repeatability and speed of measurement */
SHT_ACCURACY_MEDIUM,
/** Fastest measurement but lowest repeatability */
SHT_ACCURACY_LOW
};

class SHT30{
public:	
	SHT30(uint8_t addr = SHT30_DEFAULT_ADDR,SHTAccuracy accuracy = SHT_ACCURACY_HIGH);
	bool init();
	bool setAccuracy(SHTAccuracy newAccuracy);
	
	/**
	 * @brief: Read data from SHT, update temp and humi. The function need time refer:
	 * 			ACCURACY_HIGH    -> 15ms
	 * 			ACCURACY_MEDIUM  -> 6ms
	 * 			ACCURACY_LOW     -> 4ms
	 * @return: false-update failed; true-update successfully.
	 */
	bool read();
	
	float getHumi() const {return _humi;}
	float getTemp() const {return _temp;}
	
private:
	uint8_t _addr;
	char sendBuf[2];
	char recvBuf[6];
	uint8_t errCode;
	
	float _humi;
	float _temp;
	
	uint16_t _i2cCommand;
	uint8_t _duration;
	
	/**
	 * @brief: crc8 x8+x5+x4+x1
	 * @param: Data Byte array to be checked
	 * @param: The length of the data byte array
	 */
	uint8_t crc8(const char *data, uint8_t len);
};

#endif
