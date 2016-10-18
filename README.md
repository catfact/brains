# what

voltage sensor with bluetooth, on bare metal (teensy 3.2) [https://www.pjrc.com/store/teensy32.html]

code is arduino-compatible, but this project does not require arduino to be installed. (it includes its own copies of the teensy core headers.)

# hardware 

this project uses the MCP3903 ADC from Microchip. it uses a PWM line to provide the sampling clock, but a crystal would be better. 

connect the appropriate pin numbers from the teensy to the mcp3903 as seen in `arc/adc.h`.

# host setup

built on arch linux. 

build and install (teensy loader tool)[https://github.com/PaulStoffregen/teensy_loader_cli]

build and install `arm-none-eabi-gcc` toolchain package using `pacman` or as you like.

run `make` to build and `make upload` to flash the teensy.
