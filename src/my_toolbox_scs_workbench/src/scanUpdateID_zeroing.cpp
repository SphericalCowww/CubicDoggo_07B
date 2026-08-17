#include <iostream>
#include <unistd.h>
#include "SCServo.h"

SCSCL scs_wb;

int main() {
    const char* port_name = "/dev/ttyACM0";
    int baud_rate = 1000000;
    int targetID = 1; // The ID you want to set
    int foundID = -1;

    if(!scs_wb.begin(baud_rate, port_name)){
        std::cout << "Failed to init scscl motor!" << std::endl;
        return 0;
    }

    std::cout << "Scanning for servo..." << std::endl;

    // Method 1: Scan all IDs (0-253)
    for (int i = 0; i < 254; i++) {
        if (scs_wb.ping(i) != -1) {
            foundID = i;
            std::cout << "Found servo with ID: " << foundID << std::endl;
            break; 
        }
    }

    // Method 2: If scanning fails, try broadcast ID (0xFE / 254) 
    // Only use this if ONE servo is connected!
    if (foundID == -1) {
        std::cout << "No specific ID found. Attempting broadcast ID (254)..." << std::endl;
        foundID = 0xFE; 
    }

    if (foundID != -1) {
        std::cout << "Changing ID " << foundID << " to " << targetID << "..." << std::endl;
        
        // Register 5 (SCSCL_ID) is the ID register. 
        // We unlock the EPROM first (some versions require this, others don't)
        scs_wb.writeByte(foundID, SCSCL_LOCK, 0); 
        
        // Write new ID to register 5
        scs_wb.writeByte(foundID, SCSCL_ID, targetID);
        
        // Lock EPROM again
        scs_wb.writeByte(targetID, SCSCL_LOCK, 1);

        std::cout << "ID change command sent. Testing new ID..." << std::endl;
        usleep(200000); // Wait for EEPROM write

        if (scs_wb.ping(targetID) != -1) {
            std::cout << "Success! Servo is now ID: " << targetID << std::endl;
        } else {
            std::cout << "Verification failed. Check wiring or power." << std::endl;
        }
    } else {
        std::cout << "No servo detected on the bus." << std::endl;
    }

    scs_wb.end();
    return 0;
}
