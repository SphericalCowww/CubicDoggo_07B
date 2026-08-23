#include <iostream>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int const MINIMUM_POSITION_VALUE = 0;
int const MAXIMUM_POSITION_VALUE = 4095;
int main()
{
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;
    u8          ID[3]     = {11, 12, 13};
    //////////////////////////////////////////////////////////////////////////
    s16 pos[3];
    u16 vel[3];                 // unit of 1/4096 rpm
    u8  acc[3];                 // 0 for maximum possible acceleration
    int present_pos[3];
    int present_vel[3];
    int present_load[3];
    int moving_status[3];
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"testSpin_servo1(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
	while(1){
    
        for (std::size_t servoIdx = 0; servoIdx < std::size(ID); servoIdx++) {
    		pos[servoIdx] = MAXIMUM_POSITION_VALUE*3.0/4.0, vel[servoIdx] = 1000, acc[servoIdx] = 20;
        }
        sts_wb.SyncWritePosEx(ID, 3, pos, vel, acc);
        for (std::size_t servoIdx = 0; servoIdx < std::size(ID); servoIdx++) {
            present_pos[servoIdx]   = sts_wb.ReadPos(ID[servoIdx]);
            present_vel[servoIdx]   = sts_wb.ReadSpeed(ID[servoIdx]);
            present_load[servoIdx]  = sts_wb.ReadLoad(ID[servoIdx]);
            moving_status[servoIdx] = sts_wb.ReadMove(ID[servoIdx]);
            std::cout<<"testSpin_servo1():\n"
                     <<"  ID:            "<<int(ID[servoIdx])<<"\n"
                     <<"  goal pos:      "<<int(pos[servoIdx])<<"\n"
                     <<"  present_pos:   "<<present_pos[servoIdx]<<"\n"
                     <<"  present_vel:   "<<present_vel[servoIdx]<<"\n"
                     <<"  present_load:  "<<present_load[servoIdx]<<"\n"
                     <<"  moving_status: "<<moving_status[servoIdx]<<std::endl;
        }
        usleep(3000*1000);

        for (std::size_t servoIdx = 0; servoIdx < std::size(ID); servoIdx++) {
    		pos[servoIdx] = MAXIMUM_POSITION_VALUE*1.0/4.0, vel[servoIdx] = 2000, acc[servoIdx] = 50;
	    }
        sts_wb.SyncWritePosEx(ID, 3, pos, vel, acc);
        for (std::size_t servoIdx = 0; servoIdx < std::size(ID); servoIdx++) {
            present_pos[servoIdx]   = sts_wb.ReadPos(ID[servoIdx]);
            present_vel[servoIdx]   = sts_wb.ReadSpeed(ID[servoIdx]);
            present_load[servoIdx]  = sts_wb.ReadLoad(ID[servoIdx]);
            moving_status[servoIdx] = sts_wb.ReadMove(ID[servoIdx]);
            std::cout<<"testSpin_servo1():\n"
                     <<"  ID:            "<<int(ID[servoIdx])<<"\n"
                     <<"  goal pos:      "<<int(pos[servoIdx])<<"\n"
                     <<"  present_pos:   "<<present_pos[servoIdx]<<"\n"
                     <<"  present_vel:   "<<present_vel[servoIdx]<<"\n"
                     <<"  present_load:  "<<present_load[servoIdx]<<"\n"
                     <<"  moving_status: "<<moving_status[servoIdx]<<std::endl;
        }
        usleep(3000*1000);
    }
	sts_wb.end();
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



