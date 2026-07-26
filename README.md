# CubicDoggo 07B: Homemade 12-DOF 4-Legged Robot Recipe

Cubic Doggo 07B Wouf is upgraded from the base <a href="https://github.com/SphericalCowww/CubicDoggo">Cubic Doggo</a>. The goal is to incorporate a stronger servo and improve the structural support that can handle such a change.


## Ingredients

### Hardware 

| device | models | count | specification |
| - | - | - | - |
| servo motor | <a href="https://eckstein-shop.de/feetech-st-3215-c018-servo-en">ST-3215-C018</a> | 12 | Max stall torque: 3.0 N*m (at 12.0V). Need also corresponding signal wires of various lengths |
| driver board | <a href="https://eckstein-shop.de/WaveShare-Serial-Bus-Servo-Driver-Board-for-ST-SC-Serial-Bus-Servos-EN">servo driver</a> | 2 | Ccontrol and power the servos in daisy chain |

## Testing ST-3215 

Plug in the device:

    dmesg | tail -30
    ls -l /dev/ttyACM*                      # or whatever connection is found
    sudo usermod -aG dialout $USER
    sudo reboot

Download ``ST/SC serial bus servo control library (Python)`` from <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">link</a>, put it under ``CubicDoggo_07B/st3215test/``.

    cd CubicDoggo_07B/st3215test/
    python3 -m venv st3215env
    source st3215env/bin/activate
    unzip STServo_Python.zip
    pip install -r STServo_Python/requirements.txt
    cp -r STServo_Python/stservo-env/scservo_sdk .
    cp STServo_Python/stservo-env/sms_sts/read_write.py test.py    
    vim test.py
    # DEVICENAME                  = '/dev/ttyUSB0'
    python3 test.py 

## References:

- Waveshare, <a href="https://www.waveshare.com/wiki/ST3215_Servo">ST3215 Servo</a>, <a href="https://www.waveshare.com/wiki/Bus_Servo_Adapter_(A)">Bus Servo Adapter</a>
- Kitajima, How to Use the ST3215 Servo Motor (<a href="https://www.youtube.com/watch?v=T5T7qCg4pGE">YouTube</a>) 

