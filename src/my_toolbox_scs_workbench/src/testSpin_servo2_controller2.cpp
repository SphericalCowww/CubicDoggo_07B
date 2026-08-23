#include <iostream>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int const MINIMUM_POSITION_VALUE = 0;
int const MAXIMUM_POSITION_VALUE = 4095;
int main()
{
    const char* port_name[2] = {"/dev/ttyACM0", "/dev/ttyACM1"};
    int         baud_rate    = 1000000;
    int         servoID[2]   = {1, 1}; 
    //////////////////////////////////////////////////////////////////////////
    u8  ID [2] = {servoID[0], servoID[1]};
    s16 pos[2];
    u16 vel[2];                 // unit of 1/4096 rpm
    u8  acc[2];                 // 0 for maximum possible acceleration
    int present_pos[2];
    int present_vel[2];
    int present_load[2];
    int moving_status[2];
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb[2];
    for (std::size_t ctrlIdx = 0; ctrlIdx < std::size(sts_wb); ctrlIdx++) {
        if(!sts_wb[ctrlIdx].begin(baud_rate, port_name[ctrlIdx])) {
            std::cout<<"testSpin_servo2_controller2(): failed to initialize SMS_STS servo at port: "
                     <<port_name[ctrlIdx]<<std::endl;
            return 0;
        }
    }
	while(1){
		for (std::size_t ctrlIdx = 0; ctrlIdx < std::size(sts_wb); ctrlIdx++) {
            pos[ctrlIdx] = MAXIMUM_POSITION_VALUE*3.0/4.0, vel[ctrlIdx] = 1000, acc[ctrlIdx] = 20;
	    	sts_wb[ctrlIdx].SyncWritePosEx(&ID[ctrlIdx], 1, &pos[ctrlIdx], &vel[ctrlIdx], &acc[ctrlIdx]);
		    std::cout<<"goal pos = "<<int(pos[ctrlIdx])<<std::endl;
            present_pos[ctrlIdx]   = sts_wb[ctrlIdx].ReadPos(ID[ctrlIdx]);
            present_vel[ctrlIdx]   = sts_wb[ctrlIdx].ReadSpeed(ID[ctrlIdx]);
            present_load[ctrlIdx]  = sts_wb[ctrlIdx].ReadLoad(ID[ctrlIdx]);
            moving_status[ctrlIdx] = sts_wb[ctrlIdx].ReadMove(ID[ctrlIdx]);
            std::cout<<"testSpin_servo2_controller2():\n"
                     <<"  port:          "<<port_name[ctrlIdx]<<"\n"
                     <<"  ID:            "<<int(ID[ctrlIdx])<<"\n"
                     <<"  goal pos:      "<<int(pos[ctrlIdx])<<"\n"
                     <<"  present_pos:   "<<present_pos[ctrlIdx]<<"\n"
                     <<"  present_vel:   "<<present_vel[ctrlIdx]<<"\n"
                     <<"  present_load:  "<<present_load[ctrlIdx]<<"\n"
                     <<"  moving_status: "<<moving_status[ctrlIdx]<<std::endl;
        }
        usleep(3000*1000);

        for (std::size_t ctrlIdx = 0; ctrlIdx < std::size(sts_wb); ctrlIdx++) {
            pos[ctrlIdx] = MAXIMUM_POSITION_VALUE*1.0/4.0, vel[ctrlIdx] = 2000, acc[ctrlIdx] = 50;
		    sts_wb[ctrlIdx].SyncWritePosEx(&ID[ctrlIdx], 1, &pos[ctrlIdx], &vel[ctrlIdx], &acc[ctrlIdx]);
            std::cout<<"goal pos = "<<int(pos[ctrlIdx])<<std::endl;
            present_pos[ctrlIdx]   = sts_wb[ctrlIdx].ReadPos(ID[ctrlIdx]);
            present_vel[ctrlIdx]   = sts_wb[ctrlIdx].ReadSpeed(ID[ctrlIdx]);
            present_load[ctrlIdx]  = sts_wb[ctrlIdx].ReadLoad(ID[ctrlIdx]);
            moving_status[ctrlIdx] = sts_wb[ctrlIdx].ReadMove(ID[ctrlIdx]);
            std::cout<<"testSpin_servo2_controller2():\n"
                     <<"  port:          "<<port_name[ctrlIdx]<<"\n"
                     <<"  ID:            "<<int(ID[ctrlIdx])<<"\n"
                     <<"  goal pos:      "<<int(pos[ctrlIdx])<<"\n" 
                     <<"  present_pos:   "<<present_pos[ctrlIdx]<<"\n"
                     <<"  present_vel:   "<<present_vel[ctrlIdx]<<"\n"
                     <<"  present_load:  "<<present_load[ctrlIdx]<<"\n"
                     <<"  moving_status: "<<moving_status[ctrlIdx]<<std::endl;
        }
        usleep(3000*1000);
    }
    for (std::size_t ctrlIdx = 0; ctrlIdx < std::size(sts_wb); ctrlIdx++) sts_wb[ctrlIdx].end();
	return 1;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////



