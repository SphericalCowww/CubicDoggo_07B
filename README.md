# CubicDoggo 07B: Homemade 12-DOF 4-Legged Robot Recipe

Cubic Doggo 07B Wouf is upgraded from the base <a href="https://github.com/SphericalCowww/CubicDoggo">Cubic Doggo</a>. The goal is to incorporate a stronger servo and improve the structural support that can handle such a change.


## Ingredients

### Hardware 

| device | models | count | specification |
| - | - | - | - |
| servo motor | <a href="https://eckstein-shop.de/feetech-st-3215-c018-servo-en">ST-3215-C018</a> | 12 | Max stall torque: 3.0 N*m (at 12.0V). Need also corresponding signal wires of various lengths |
| driver board | <a href="https://eckstein-shop.de/WaveShare-Serial-Bus-Servo-Driver-Board-for-ST-SC-Serial-Bus-Servos-EN">servo driver</a> | 2 | Ccontrol and power the servos in daisy chain |

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

### Testing with ROS

Download ``ST/SC serial bus servo control library (Linux)`` from <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">link</a>, expand it under ``CubicDoggo_07B/src/my_toolbox_scs_workbench/`` and replace ``src/my_toolbox_scs_workbench/SCServo_Linux/SCServo_Linux_220329/SCServo_Linux/CMakeLists.txt``.

Then remember to change ID under ``CubicDoggo_07B/src/my_toolbox_scs_workbench/src/testSpin_servo1.cpp``, run,

    colcon build --packages-select my_toolbox_scs_workbench --cmake-clean-first
    ros2 run my_toolbox_scs_workbench testSpin_servo1

### Initializing the servos

To change the servo ID and initialize the servo, connect only 1 controller and 1 servo,

    vim CubicDoggo_07B/src/my_toolbox_scs_workbench/src/scanUpdateID_zeroing.cpp
    # Modify targetID and pos_offset
    ros2 run my_toolbox_scs_workbench scanUpdateID_zeroing

**NOTE:** Record the ``pos_offset`` such that the servo's default position is finely aligned.

**NOTE:** ``sts_wb.writeByte``requires unlocking the EEPROM; do NOT do this firmware change too frequently, otherwise it can cause corruption.

## References:

- Waveshare, <a href="https://www.waveshare.com/wiki/ST3215_Servo">ST3215 Servo</a>, <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">Bus Servo Adapter</a>
- Kitajima, How to Use the ST3215 Servo Motor (<a href="https://www.youtube.com/watch?v=T5T7qCg4pGE">YouTube</a>)
- Kevin McAleer, Why are bus servos better? ([YouTube](https://www.youtube.com/watch?v=nzBZOTdEdtE))

## Acknowledgements

- Libraries and some examples taken from: https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A). The license is not defined.
