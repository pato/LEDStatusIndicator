led.hex: led.c
	avr-gcc -g -Os -mmcu=atmega168a -c led.c
	avr-gcc -g -mmcu=atmega168a -o led.elf led.o
	avr-objcopy -j .text -j .data -O ihex led.elf led.hex
flash:
	sudo avrdude -c usbasp -p m168 -u -U flash:w:led.hex
clean:
	rm led.o
	rm led.elf
