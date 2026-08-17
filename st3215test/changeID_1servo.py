import sys, os
from scservo_sdk import *



OLD_ID = 1              
NEW_ID = 11


















################################################################################################################

BAUDRATE   = 1000000       
DEVICENAME = '/dev/ttyACM0'  # Update to your port (e.g., "COM3" on Windows)

# Initialize PortHandler and PacketHandler
portHandler = PortHandler(DEVICENAME)
packetHandler = sms_sts(portHandler)

# Open port
if not portHandler.openPort():
    print("Failed to open the port")
    sys.exit()

# Set baudrate
if not portHandler.setBaudRate(BAUDRATE):
    print("Failed to change the baudrate")
    sys.exit()

# --- Changing the ID ---
# Note: EEPROM writes require unlocking or writing directly to the EEPROM address.
# For SMS/STS servos, packetHandler.write1ByteTxRx writes to a control table address.
# Address 5 is typically the ID register for SMS/STS series servos.

ADDR_STS_ID = 5

scs_comm_result, scs_error = packetHandler.write1ByteTxRx(OLD_ID, ADDR_STS_ID, NEW_ID)

if scs_comm_result != COMM_SUCCESS:
    print(f"Communication Error: {packetHandler.getTxRxResult(scs_comm_result)}")
elif scs_error != 0:
    print(f"Servo Error: {packetHandler.getRxPacketError(scs_error)}")
else:
    print(f"Success! Servo ID has been changed from {OLD_ID} to {NEW_ID}.")

# Close port
portHandler.closePort()



