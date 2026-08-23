# CubicDoggo 07B: Homemade 12-DOF 4-Legged Robot Recipe

Cubic Doggo 07B Wouf is upgraded from the base <a href="https://github.com/SphericalCowww/CubicDoggo">Cubic Doggo</a>. The goal is to use a stronger servo and improve the structural support to handle the change.


## Ingredients

### Hardware 

| device | models | count | specification |
| - | - | - | - |
| servo motor | <a href="https://eckstein-shop.de/feetech-st-3215-c018-servo-en">ST-3215-C018</a> | 12 | Max stall torque: 3.0 N*m (at 12.0V, 2.7A). Weight of 55g. Need also corresponding signal wires of various lengths |
| driver board | <a href="https://eckstein-shop.de/WaveShare-Serial-Bus-Servo-Driver-Board-for-ST-SC-Serial-Bus-Servos-EN">servo driver</a> | 2 | Ccontrol and power the servos in daisy chain |
| onboard computer | Raspberry Pi 5 | 1 | For running just IK, a small RAM is sufficient; Pi 4 could be good enough as long as ROS2 Jazzy can be installed | 
| DC-DC step-down converter | Hailege <a href="https://www.amazon.de/Hailege-Module-Step-Down-Supply-Converter/dp/B07XFMMY1F">24V/12V to 5V/5A</a> | 1 | USB Port port to RaspPi,  DC 5.5mm x 2.5mm Male to battery | 
| battery | ZYGY <a href="https://www.amazon.de/dp/B0BB6RMM5Q">11.1V 2000mAh</a> | 2 | They already include protection. Need Charger. Need adapters for: T-plug => XT60 Male => DC 5.5mm x 2.5mm Male | 
| capacitor | 1000uF | 2 | rating 25V or higher | 
| bearings | M3 bearing+<a href="https://www.amazon.de/dp/B01M2ZCLKX">spacer</a>, threaded rod, rod-end bearing | 8, 4, 4, 4 | rod length of 60mm to match the leg length; other dimensions can be accomodated by modifying the CAD |
| bolts and nuts | | | M3 screws are used throughout, except where required to accommodate the servos and electronic boards; use locknuts |
| locking washers | [M3](https://www.amazon.de/dp/B09SCQV81D) | | use in case when locknuts are not available; use it along side regular washer: 3D-printed-surface => regular washer => locking washer (bulge faces away from surface) => bolt |

Other than the M3 screw, one specific requirement is the M2 self-tapping screws, specific to 1cm length given 6mm padding, for the servos. Note that the length matters a lot, screw in 1 mm too much can already damage the servo!

**Soldering requirement:** solder the 1000uF capacitor to the T-plug to screw terminal adapter (for the servo controllers). Watch out for the polarity.

### Power system

  * Daisy chain no more than 3 servos to avoid delay
  * Power the servo controller with the screw terminal to handle ~16 amp current draw. Insert a 1000uF capacitor between rail and ground with correct polarity
  * The ground between the 2 controllers should be shared when both are connecting the Rasp Pi with USB
  * Power the RaspPi via a ~12V-to-5V DC-DC converter. Don't forget to put a kapton tape to insulate the two boards

<img src="https://github.com/SphericalCowww/CubicDoggo_07B/blob/main/powerSystem.png" height="400"> 

## Running a Single Servo on ROS2

<img src="https://github.com/SphericalCowww/CubicDoggo_07B/blob/main/fig_servo1.png" height="200">

### Testing with Python library

Plug in the device:

    dmesg | tail -30
    ls -l /dev/ttyACM*                      # or whatever connection is found
    sudo usermod -aG dialout $USER
    sudo reboot

Download ``ST/SC serial bus servo control library (Python)`` from <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">link</a>, put it under ``CubicDoggo_07B/st3215test/``.

    cd CubicDoggo_07B/
    cd ..
    python3 -m venv CubicDoggo_07B_env
    source CubicDoggo_07B_env/bin/activate
    cd CubicDoggo_07B/st3215test/
    unzip STServo_Python.zip
    pip install -r STServo_Python/requirements.txt
    cp -r STServo_Python/stservo-env/scservo_sdk .
    cp STServo_Python/stservo-env/sms_sts/read_write.py test.py    
    vim test.py
    # DEVICENAME = '/dev/ttyUSB0'    # change to the port for the controller
    python3 test.py                  # or spinTest_1servo.py in the code

### Initializing and testing the servo  with ROS

Download ``ST/SC serial bus servo control library (Linux)`` from <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">link</a>, expand it under ``CubicDoggo_07B/src/my_toolbox_scs_workbench/`` and replace ``src/my_toolbox_scs_workbench/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux/CMakeLists.txt``.

    colcon build --packages-select my_toolbox_scs_workbench --cmake-clean-first

To change the servo ID and initialize the servo, connect only 1 controller and 1 servo,

    colcon build --packages-select my_toolbox_scs_workbench --cmake-clean-first
    vim CubicDoggo_07B/src/my_toolbox_scs_workbench/src/scanUpdateID_zeroing.cpp
    # Modify targetID and pos_offset
    ros2 run my_toolbox_scs_workbench scanUpdateID_zeroing

Remember to tune and record the pos_offset to set the servo at the correct default position (halfway between minimal and maximal ticks).

To test 1 servo again, do,

    ros2 run my_toolbox_scs_workbench testSpin_servo1

To test 2 servos with 2 controllers, or 3 servos daisy chained, redo the connection accordingly and do,

    ros2 run my_toolbox_scs_workbench testSpin_servo2controller2
    ros2 run my_toolbox_scs_workbench testSpin_servo3

<img src="https://github.com/SphericalCowww/CubicDoggo_07B/blob/main/fig_servo2controller2.png" height="100"><img src="https://github.com/SphericalCowww/CubicDoggo_07B/blob/main/fig_servo3.png" height="100">


**NOTE:** Record the ``pos_offset`` for each servo independently such that each of their default position is finely aligned.

**NOTE:** ``sts_wb.writeByte``requires unlocking the EEPROM; do NOT do this firmware change too frequently, otherwise it can cause corruption.

## References:

- Waveshare, <a href="https://www.waveshare.com/wiki/ST3215_Servo">ST3215 Servo</a>, <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">Bus Servo Adapter</a>
- Kitajima, How to Use the ST3215 Servo Motor (<a href="https://www.youtube.com/watch?v=T5T7qCg4pGE">YouTube</a>)
- Kevin McAleer, Why are bus servos better? ([YouTube](https://www.youtube.com/watch?v=nzBZOTdEdtE))

## Acknowledgements

- Libraries and some examples taken from: https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A). The license is not defined.
