#include <iostream>
#include <unistd.h>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int const MINIMUM_POSITION_VALUE = 0;
int const MAXIMUM_POSITION_VALUE = 4095;
int const ID_RANGE               = 254;
int main() {
    int baud_rate = 1000000;

    const char* port_name = "/dev/ttyACM0";
    u8  servoIDs   [3] = {11, 12, 13};
    s16 pos_offsets[3] = {40, 20, 50};
    /////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"scanUpdateAll_zeroing(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
    /////////////////////////////////////////////////////////////////
    for (std::size_t IDidx = 0; IDidx < std::size(servoIDs); IDidx++) {
        u8 servoID = servoIDs[IDidx];
        std::cout<<"scanUpdateAll_zeroing(): loading EEPROM for servo ID "<<int(servoID)<<std::endl;
        sts_wb.unLockEprom(servoID);                // unlock EEPROM 
        std::cout<<"scanUpdateAll_zeroing(): updating PID, voltage/current limit"<<std::endl;
        // PID for walking robot
        sts_wb.writeByte(servoID, 21, 50);         // P
        sts_wb.writeByte(servoID, 23, 0);          // I
        sts_wb.writeByte(servoID, 22, 40);         // D
        // voltage limit
        sts_wb.writeByte(servoID, 14, 140);        // upper bound to 14V
        sts_wb.writeByte(servoID, 15, 100);        // lower bound to 10V
        // current limit: 6.5 mA per unit => 2000mA/6.5 ~ 308
        sts_wb.writeWord(servoID, 28, 300);
        std::cout<<"scanUpdateAll_zeroing(): updating position mode, offset"<<std::endl;
        // mode: position mode at mode 0
        sts_wb.writeByte(servoID, 33, 0);
        // offset
        sts_wb.writeWord(servoID, 31, pos_offsets[IDidx]);
        sts_wb.LockEprom(servoID);                 // lock EEPROM
        usleep(1000*1000);                           // 1 second for update

        ///////////////////////////////////////////////////////////////// to default position
        u8  ID [1] = {servoID};
        s16 pos[1] = {MAXIMUM_POSITION_VALUE/2 + 1};
        u16 vel[1] = {100};
        u8  acc[1] = {10};
        int present_pos;
        int present_vel;
        int present_load;
        int moving_status;   
 
        sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
	    present_pos   = sts_wb.ReadPos(servoID);
        present_vel   = sts_wb.ReadSpeed(servoID);
        present_load  = sts_wb.ReadLoad(servoID);
        moving_status = sts_wb.ReadMove(servoID);
        std::cout<<"scanUpdateAll_zeroing():\n"
                     <<"  ID:            "<<int(servoID)<<"\n"
                     <<"  goal pos:      "<<int(pos[0])<<"\n"
                     <<"  present_pos:   "<<present_pos<<"\n"
                     <<"  present_vel:   "<<present_vel<<"\n"
                     <<"  present_load:  "<<present_load<<"\n"
                     <<"  moving_status: "<<moving_status<<std::endl;
        usleep(3000*1000);
        ///////////////////////////////////////////////////////////////// which error state?
        int errorState = sts_wb.readByte(servoID, 65);
        std::cout<<"scanUpdateID_zeroing(): error register: "<<errorState<<std::endl<<std::endl;
    }
    sts_wb.end();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

