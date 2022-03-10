# Swalkit

A generic augmented walker kit to provide haptic feedback navigation assistance to people with both visual and motor impairments

<p align="center"> <img src="https://user-images.githubusercontent.com/100538879/157646210-4b248532-0007-4f7d-b5d4-7d8bc51b2a63.png" alt="walker" height="400"/>

https://user-images.githubusercontent.com/100538879/157646195-c86fd216-0dd4-412f-9931-ac7addbbb123.mp4


# Overview
	
<p align="center"> <img src="https://user-images.githubusercontent.com/100538879/157646392-679fcf65-dfe2-4fdc-a37a-ba51c5b768a8.jpg" alt="overview" height="300"/>

	
# Loading the program to the M5Stacks
For loading a program to the M5Stacks you will need a corresponding program like the ESP Flash Tools from Espressif or Visual Studio Code. 
The second option is only necessary if you want to change the code. You will find the ESP Flash Tools here: Tools | Espressif Systems. Download the Flash Download Tools. You will download a Zip file containing an exe-file: flash_download_tool_x where x is the version number. Double-click this file to execute it and install your program. A window will open. Select ESP32 as chipType and develop as workMode. With the “…”-button you can choose the program you want to load on your M5Stack. Change the COM-Port according to your device…
To install Visual Studio Code go to https://code.visualstudio.com/download and chose the version according to your computer. Download and execute. Next you will need PlatformIO, you will find the download files here: PlatformIO IDE - Visual Studio Marketplace. Once set up you can open VSCode. Click on “File” and “open Folder…” to select the program folder for the M5Stack from your file system. Now you can plug in your M5Stack. In VSCode you will find a button that shows a flash and says “PlatformIO: Upload” when you hover over it. Click on it to program your device.

# Installing the Android application
To install the application on your device you will need the APK-file. Download the file in a folder on your device and try to open it. Click on install, the application will be available on the device. For some devices it might be necessary to allow the installation of APKs from unknown sources. If problems occur in this step please check that for your specific device.

# Printing the 3D-objects
For the Smart Walker you will need to print the following parts:
Sensor Case - Body (2 x mirrored)
	Sensor Case - Top (2 x mirrored)
Sensor Case – Support (2 x mirrored)
	Motor Case (2 x mirrored) 
	Motor Case Top
For the wheelchair you will need to print the following parts:
Sensor Case Big – Body	
	Sensor Case Big – Top	
Motor Case Big 1 (2 x mirrored) 
Motor Case Big 2 (2 x mirrored) 
	Motor Case Top
	Tablet M5 (has to be mirrored for left-handers)

# Assembly
The sensors have to be positioned in the sensor cases and fixed with M2 screws. Then the sensors are going to be connected with the I2C Hub following the scheme:
	--einfuegen!
Note, that for the walker you will have to change the address of one of the hubs by soldering the following connection:
--einfuegen!
Then the lid can be screwed to the bottom to close the case.
The motors will be glued inside their cases, then the cases can be screwed to the walker.
--bild einfuegen
For the wheelchair the tablet will be screwed to the armrest (on the left for right-handers and on the right for left-handers). The M5Stack can be glued to the tablet. If you wish to install the LED strip as well cut it to a length of 5 LEDs and glue them on the curve of the tablet. The gab on the inner side of the tablet is meant for the H-Bridge.
--Bild einfuegen
Next you will have to connect everything with cables.
--bild einfuegen

# Using the Application:
Before the first connection you have to couple your devices:
	- turn on Bluetooth on your device
	- press Button A on your M5Stack to start Bluetooth
- Your Android device should find a Bluetooth device “Fauteuil IRISA”, chose it for connecting and wait for the coupling process to finish
For every connection to your M5 do the following:
	- turn on Bluetooth on your Android device
	- turn on Bluetooth on your M5Stack by pressing Button A
- open the Application and choose your M5Stack from the device list, after a few seconds you should connect automatically
Once connected you will start on the profile page showing you your current profile. If you already have profiles saved on your Android device you can simply change devices with the corresponding button
--bild
By changing the name of the profile and hitting safe profile you will create a new profile that is going to be saved on your android device.
Swipe to access the profile settings. You will find a table containing the following settings:
Frequency: the vibration frequency for a critical distance value
Pulse: the pulse length for a critical distance value
Distance: the critical distance values
These settings can be changed for 4 danger levels:
Front: an obstacle is detected on front of the walker/ right behind the wheelchair. It is recommended to set this value higher than the others to detect frontal obstacles earlier.
Danger; Near; Far: Three distance levels for lateral obstacles.
For all cases vibration and pulses can be turned off.
The M5Stack can hold two profiles at once. To change between the profiles you have to press Button C. You can change the profile every time, also when connected to your Android device to change the second profile respectively.
If you swipe further you will have an overview of the current sensor data. This diagram can help to find malfunctioning sensors.
Troubleshooting:
If you cannot find your M5Stack in the device list make sure both sides have Bluetooth turned on and that your Android device already knows your M5Stack (see first connection). You will know the Bluetooth on the M5Stack is turned on because a blue “B”-sign will appear.
If you have already connected to another “Fauteuil IRISA”-device and now can’t connect, erase the old device from your Bluetooth list before restarting with the steps for a first connection.
