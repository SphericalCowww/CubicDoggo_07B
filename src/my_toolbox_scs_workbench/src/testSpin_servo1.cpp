#include <iostream>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int const MINIMUM_POSITION_VALUE = 0;
int const MAXIMUM_POSITION_VALUE = 4095;
int main()
{
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;
    int         servoID   = 1; 
    //////////////////////////////////////////////////////////////////////////
    u8  ID [1] = {servoID};
    s16 pos[1];
    u16 vel[1];                 // unit of 0.0146rpm
    u8  acc[1];                 // 0 for maximum possible acceleration
    int present_pos;
    int present_vel;
    int present_load;
    int moving_status;
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"testSpin_servo1(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
	while(1){
		pos[0] = MAXIMUM_POSITION_VALUE*3.0/4.0, vel[0] = 1000, acc[0] = 20;
		sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
        present_pos   = sts_wb.ReadPos(ID[0]);
        present_vel   = sts_wb.ReadSpeed(ID[0]);
        present_load  = sts_wb.ReadLoad(ID[0]);
        moving_status = sts_wb.ReadMove(ID[0]);
        std::cout<<"testSpin_servo1():\n"
                 <<"  ID:            "<<int(ID[0])<<"\n"
                 <<"  goal pos:      "<<int(pos[0])<<"\n"
                 <<"  present_pos:   "<<present_pos<<"\n"
                 <<"  present_vel:   "<<present_vel<<"\n"
                 <<"  present_load:  "<<present_load<<"\n"
                 <<"  moving_status: "<<moving_status<<std::endl;
        usleep(3000*1000);
  
		pos[0] = MAXIMUM_POSITION_VALUE*1.0/4.0, vel[0] = 2000, acc[0] = 50;
		sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
	    present_pos   = sts_wb.ReadPos(ID[0]);
        present_vel   = sts_wb.ReadSpeed(ID[0]);
        present_load  = sts_wb.ReadLoad(ID[0]);
        moving_status = sts_wb.ReadMove(ID[0]);
        std::cout<<"testSpin_servo1():\n"
                 <<"  ID:            "<<int(ID[0])<<"\n"
                 <<"  goal pos:      "<<int(pos[0])<<"\n"
                 <<"  present_pos:   "<<present_pos<<"\n"
                 <<"  present_vel:   "<<present_vel<<"\n"
                 <<"  present_load:  "<<present_load<<"\n"
                 <<"  moving_status: "<<moving_status<<std::endl;
        usleep(3000*1000);
    }
	sts_wb.end();
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



