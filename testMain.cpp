#include "SHT30.h"

int main()
{
	SHT30 sht;
	bcm2835_init();
	if(!sht.init())
	{
		printf("SHT init failed\n");
		return 0;
	}
	else
		printf("SHT init successfully\n");
		
	while(1)
	{
		if(sht.read())
		{
			printf("Temp = %.2f, Humi = %.2f \n",sht.getTemp(),sht.getHumi());
		}else
			printf("SHT read failed\n");
		bcm2835_delay(2000);
	}
	return 0;
}
// g++ -Wall -o "%e" "%f" SHT30.cpp -l bcm2835
