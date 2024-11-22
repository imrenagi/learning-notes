CC=arm-none-eabi-gcc
MACH=cortex-m7
# -std=gnu11: Use the GNU11 standard
# -O0: No optimization
# -Wall: Enable all warnings
CFLAGS=-c -mcpu=$(MACH) -mthumb -std=gnu11 -O0 -Wall
# -nostdlib: Do not use the standard library (this is just temporary)
# -Wl,-Map=final.map: Generate a map file. -Wl is used to pass flags to the linker
LDFLAGS=-T STM32H7S3L8_default_ls.ld -nostdlib -Wl,-Map=final.map

all: main.o led.o STM32H7S3L8_startup.o final.elf
# $^: denotes dependencies
# $@: denotes target
main.o: main.c
	$(CC) $(CFLAGS) -o $@ $^ 
	 
led.o: led.c
	$(CC) $(CFLAGS) -o $@ $^

STM32H7S3L8_startup.o: STM32H7S3L8_startup.c
	$(CC) $(CFLAGS) -o $@ $^

final.elf: main.o led.o STM32H7S3L8_startup.o
	$(CC) $(LDFLAGS) -o $@ $^

symbol: final.elf
	arm-none-eabi-nm -n final.elf

load: 
	cd /opt/xpack-openocd/openocd/scripts/board
	openocd -f st_nucleo_h743zi.cfg

	

clean:
	rm -rf *.o *.elf