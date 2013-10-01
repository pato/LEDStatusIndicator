DEVICE  = attiny25
F_CPU   = 16000 #1000000
AVRDUDE = avrdude -v -c usbtiny -p $(DEVICE)  # edit this line for your programmer. For slow devices: #64

CFLAGS  = -I. -DDEBUG_LEVEL=0
OBJECTS = main.o

COMPILE = avr-gcc -Wall -Ofast -DF_CPU=$F_CPU $(CFLAGS) -mmcu=$(DEVICE)
COMPILEPP = avr-g++ -Wall -Ofast -DF_CPU=$(F_CPU) $(CFLAGS) -mmcu=$(DEVICE)

#To build: (Make sure each step succeeds, they may fail when the micro is at low speeds)
#$make hex #compile
#$make fast #Speed up clock
#$make flash #put program on device
#$make slow #Bring device clock way down


# symbolic targets:
help:
	@echo "This Makefile has no default rule. Use one of the following:"
	@echo "make hex ....... to build main.hex"
	@echo "make program ... to flash fuses and firmware"
	@echo "make fuse ...... to flash the fuses"
	@echo "make flash ..... to flash the firmware"
	@echo "make clean ..... to delete objects and hex file"

hex: main.hex

program: flash fuse

# rule for programming fuse bits:
fuse:
	#62 is full speed
	#64 is 16khz
	$(AVRDUDE) -B 1000 -U hfuse:w:0xDF:m -U lfuse:w:0x64:m

# Minimize clock speed
slow:
	#62 is full speed
	#64 is 16khz
	$(AVRDUDE) -B 1000 -U hfuse:w:0xDF:m -U lfuse:w:0x64:m

# Bring clock speed back up
fast:
	#62 is full speed
	#64 is 16khz
	$(AVRDUDE) -B 1000 -U hfuse:w:0xDF:m -U lfuse:w:0x62:m

# rule for uploading firmware:
flash: main.hex
	$(AVRDUDE) -U flash:w:main.hex:i

# rule for deleting dependent files (those which can be built by Make):
clean:
	rm -f main.hex main.lst main.obj main.cof main.list main.map main.eep.hex main.elf *.o  main.s
.cpp.o:
	$(COMPILEPP) -c $< -o $@


# Generic rule for compiling C files:
.c.o:
	$(COMPILE) -c $< -o $@

# Generic rule for assembling Assembler source files:
.S.o:
	$(COMPILE) -x assembler-with-cpp -c $< -o $@
# "-x assembler-with-cpp" should not be necessary since this is the default
# file type for the .S (with capital S) extension. However, upper case
# characters are not always preserved on Windows. To ensure WinAVR
# compatibility define the file type manually.

# Generic rule for compiling C to assembler, used for debugging only.
.c.s:
	$(COMPILE) -S $< -o $@

# file targets:

main.elf: $(OBJECTS)
	$(COMPILE) -o main.elf $(OBJECTS)

main.hex: main.elf
	rm -f main.hex main.eep.hex
	avr-objcopy -j .text -j .data -O ihex main.elf main.hex
	avr-size main.hex

# debugging targets:

disasm:	main.elf
	avr-objdump -d main.elf

cpp:
	$(COMPILE) -E main.cpp
