#include <iostream>
#include <unistd.h>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;

    int IDrange  = 254;         // search ID up to
    int targetID = -1;           // set to this ID, -1 to not set
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"scanUpdateID_zeroing(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }
    //////////////////////////////////////////////////////////////////////////
    int foundID = -1;
    std::cout<<"scanUpdateID_zeroing(): scanning for servo ID..."<<std::endl;
    for (int id = 0; id < IDrange; id++) {
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
    //////////////////////////////////////////////////////////////////////////
    if (targetID == foundID) targetID = -1;
    if ((foundID != -1) && (targetID != -1)) {
        std::cout << "scanUpdateID_zeroing(): changing ID "<<foundID<<" to "<<targetID<<"..."<<std::endl;
        
        sts_wb.unLockEprom(foundID);                // unlock EEPROM 
        sts_wb.writeByte(foundID, 5, targetID);     // register 5 for updating ID
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

    sts_wb.end();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

