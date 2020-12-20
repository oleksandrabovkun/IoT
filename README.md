# Readme 

## Installation
1. Urls in the  `arduino.additionalUrls` settings for Tools are broken, that's why we updated them and specified path from this repo
2. Install board with Board Manager
3. For compiler `arm-none-eabi-gcc` it worked well, but for openocd not really. Openocd that we downloaded is the source code, not the binary. 
3.1 To build Openocd on mac you need to install extra libraries with brew `brew install automake libtool libusb gcc@4.9 pkg-config`. Make sure you're in `~/Library/Arduino15/packages/AZ3166/tools/openocd/0.10.0`. Then you might need to change the Makefile to ignore warnings (for every warning open the Makefile and add it to GCC_WARNINGS -Wno-<name of error>), and then run `./configure --prefix=macosx && make && make install `. Then `cp -r macosx/share/openocd/scripts macosx` because VSCode Azure IoT extention expects different folder stucture. 
4. Now you should be able to compile this project and push it to the device.

## Code 
Checks three sensors (temperature, humidity, and pressure) and sends it to Azure IoT. It also reacts on pressing button B. 


