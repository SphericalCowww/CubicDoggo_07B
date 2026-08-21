#include <iostream>
#include <unistd.h>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////
int const MINIMUM_POSITION_VALUE = 0;
int const MAXIMUM_POSITION_VALUE = 4095;
int const ID_RANGE               = 254;
int main() {
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;

    int targetID   = 31;            // set to this ID, -1 to not set
    int pos_offset = 20;            // offset to the default position, remember to recording it
    ///////////////////////////////////////////////////////////////// connect only 1 controller and 1 servo
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"scanUpdateID_zeroing(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
    ///////////////////////////////////////////////////////////////// scan ID
    int foundID = -1;
    std::cout<<"scanUpdateID_zeroing(): scanning for servo ID..."<<std::endl;
    for (int id = 0; id < ID_RANGE; id++) {
        if (sts_wb.Ping(id) != -1) {
            foundID = id;
            std::cout<<"scanUpdateID_zeroing(): found servo with ID: "<<foundID<<std::endl;
            break; 
        }
    }
    // if scanning fails, try broadcast ID (0xFE/254) 
    // NOTE: only use this if ONE servo is connected!
    if (foundID == -1) {
        std::cout<<"scanUpdateID_zeroing(): no specific ID found. Broadcasting ID (254)..."<<std::endl;
        foundID = 0xFE;     // 254 
    }
    ///////////////////////////////////////////////////////////////// change ID
    if (targetID == foundID) targetID = -1;
    if ((foundID != -1) && (targetID != -1)) {
        std::cout << "scanUpdateID_zeroing(): changing ID "<<foundID<<" to "<<targetID<<"..."<<std::endl;
        
        sts_wb.unLockEprom(foundID);                // unlock EEPROM 
        sts_wb.writeByte(foundID, 5, targetID);     // register 5 for updating ID
        // PID for walking robot
        sts_wb.writeByte(targetID, 21, 50);         // P
        sts_wb.writeByte(targetID, 23, 0);          // I
        sts_wb.writeByte(targetID, 22, 40);         // D
        // voltage limit
        sts_wb.writeByte(targetID, 14, 140);        // upper bound to 14V
        sts_wb.writeByte(targetID, 15, 100);        // lower bound to 10V
        sts_wb.LockEprom(targetID);                 // lock EEPROM
        usleep(1000000);                            // 1 second for update

        if (sts_wb.Ping(targetID) != -1) {
            std::cout<<"scanUpdateID_zeroing(): servo ID updated to: "<<targetID<<std::endl;
        } else {
            std::cout<<"scanUpdateID_zeroing(): servo ID update failed"<<std::endl;
        }
    } else if (foundID == -1) {
        std::cout<<"scanUpdateID_zeroing(): no servo detected on the bus"<<std::endl;
    }

    ///////////////////////////////////////////////////////////////// to default position
    if (targetID == -1) targetID = foundID;
    u8  ID [1] = {targetID};
    s16 pos[1];
    u16 vel[1];
    u8  acc[1];
    int present_pos;
    int present_vel;
    int present_load;
    int moving_status;   
 
    pos[0] = MAXIMUM_POSITION_VALUE/2 + 1 + pos_offset, vel[0] = 100, acc[0] = 10;
    sts_wb.SyncWritePosEx(ID, 1, pos, vel, acc);
	present_pos   = sts_wb.ReadPos(ID[0]);
    present_vel   = sts_wb.ReadSpeed(ID[0]);
    present_load  = sts_wb.ReadLoad(ID[0]);
    moving_status = sts_wb.ReadMove(ID[0]);
    std::cout<<"scanUpdateID_zeroing():\n"
                 <<"  ID:            "<<int(ID[0])<<"\n"
                 <<"  goal pos:      "<<int(pos[0])<<"\n"
                 <<"  present_pos:   "<<present_pos<<"\n"
                 <<"  present_vel:   "<<present_vel<<"\n"
                 <<"  present_load:  "<<present_load<<"\n"
                 <<"  moving_status: "<<moving_status<<std::endl;
    usleep(3000*1000);
    ///////////////////////////////////////////////////////////////// which error state?
    int errorState = sts_wb.readByte(targetID, 65);
    std::cout<<"scanUpdateID_zeroing(): error register: "<<errorState<<std::endl;

    sts_wb.end();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

