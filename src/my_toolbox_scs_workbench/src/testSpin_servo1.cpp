#include <iostream>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main()
{
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;
 
    u8  ID [1] = {1};
    s16 pos[1];
    u16 vel[1];
    u8  acc[1] = {0};
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"testSpin_servo1(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
	while(1){
		pos[0] = 1000;
		vel[0] = 1500;                                          // unit of 0.0146rpm
		sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
		std::cout<<"pos = "<<1000<<std::endl;
		usleep(1000*1000);
  
		pos[0] = 20;
		vel[0] = 1500;
		sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
		std::cout<<"pos = "<<20<<std::endl;
		usleep(1000*1000);
	}
	sts_wb.end();
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



