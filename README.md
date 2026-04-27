# IF YALL USE WINDOWS & VSCODE + PLATFORM.IO AS I DO, DO AS FOLLOWS:

##### Execute:
cd /kinematics
pip install -e .

##### Install STM32 DFU Driver from:
https://www.st.com/en/development-tools/stm32cubeprog.html

##### Hardware
- plug STM32 to laptop
- hold B0 button, press NR and release, wait for 0.5 sec then release the B0 button
- Open device manager, thou shalt see "STM32 BOOTLOADER" in "Universal Serial Bus Devices"
- Upload src/main.cpp to STM32 through platform.io
- Press NR button once
- Take a look at device manager, under "Ports (COM & LPT)", copy the COM number

##### Running test_motor.py
- In host/test_motor.py, change the COM_PORT to whatever the COM was in ur device manager
- run test_motor.py