
CROSS_COMPILE = arm-elf-

CC = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy

OPTIMIZATION = -Os

ASM_OBJECTS = cc32/start.o

C_OBJECTS = main.o cc32/cc32_flcon.o cc32/cc32_irq.o cc32/iso7816_slave.o cc32/cc32_spi.o


CFLAGS = -Wall -ffunction-sections -fno-exceptions -nostartfiles -nostdlib #-mthumb
CFLAGS += -I../include
CFLAGS += -g $(OPTIMIZATION)

LDFLAGS = -g $(OPTIMIZATION) -nostartfiles -Wl,--gc-sections

target: $(ASM_OBJECTS) $(C_OBJECTS)
	$(CC) $(LDFLAGS) -T"cc32/CC32RS512.lds" -o $@.elf $^

$(ASM_OBJECTS): %.o: %.s Makefile
	$(CC) $(ASFLAGS) -c -o $@ $<

$(C_OBJECTS): %.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm $(ASM_OBJECTS) $(C_OBJECTS) target.elf
