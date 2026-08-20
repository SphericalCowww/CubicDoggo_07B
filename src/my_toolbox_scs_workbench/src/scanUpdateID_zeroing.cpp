#include <iostream>
#include <unistd.h>
#include "SCServo.h"
////////////////////////////////////////////////////////////////////////////////////////////////////////////

int main() {
    const char* port_name = "/dev/ttyACM0";
    int         baud_rate = 1000000;

    int IDrange  = 254;         // search ID up to
    int targetID = 1;           // set to this ID
    //////////////////////////////////////////////////////////////////////////
    SMS_STS sts_wb;
    if(!sts_wb.begin(baud_rate, port_name)){
        std::cout<<"scanUpdateID_zeroing(): failed to initialize SMS_STS servo!"<<std::endl;
        return 0;
    }

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
        foundID = 0xFE; 
    }

    /*
    if (foundID != -1) {
        std::cout << "Changing ID " << foundID << " to " << targetID << "..." << std::endl;
        
        // Register 5 (SCSCL_ID) is the ID register. 
        // We unlock the EPROM first (some versions require this, others don't)
        sts_wb.writeByte(foundID, SCSCL_LOCK, 0); 
        
        // Write new ID to register 5
        sts_wb.writeByte(foundID, SCSCL_ID, targetID);
        
        // Lock EPROM again
        sts_wb.writeByte(targetID, SCSCL_LOCK, 1);

        std::cout << "ID change command sent. Testing new ID..." << std::endl;
        usleep(200000); // Wait for EEPROM write

        if (sts_wb.ping(targetID) != -1) {
            std::cout << "scanUpdateID_zeroing(): Success! Servo is now ID: " << targetID << std::endl;
        } else {
            std::cout << "scanUpdateID_zeroing(): Verification failed. Check wiring or power." << std::endl;
        }
    } else {
        std::cout << "No servo detected on the bus." << std::endl;
    }
    */

    sts_wb.end();
    return 0;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////

