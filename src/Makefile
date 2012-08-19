
CROSS_COMPILE = arm-elf-

CC = $(CROSS_COMPILE)gcc
SIZE = $(CROSS_COMPILE)size
STRIP = $(CROSS_COMPILE)strip
OBJCOPY = $(CROSS_COMPILE)objcopy

#LDS = cc32/CC32RS512.lds
LDS = cc32/CC32RS512_ramonly.lds

OPTIMIZATION = -Os

ASM_OBJECTS = cc32/start.o

CC32_OBJECTS = cc32/cc32_flcon.o cc32/cc32_irq.o cc32/cc32_gpio.o \
	       cc32/cc32_spi.o cc32/iso7816_slave.o \
	       cc32/uart_sc16is740.o

C_OBJECTS = $(CC32_OBJECTS) main.o


CFLAGS = -Wall -ffunction-sections -fno-exceptions -nostartfiles -nostdlib #-mthumb
CFLAGS += -I../include
CFLAGS += -g $(OPTIMIZATION)

LDFLAGS = -g $(OPTIMIZATION) -nostartfiles -Wl,--gc-sections

all: target target.bin

target: $(ASM_OBJECTS) $(C_OBJECTS)
	$(CC) $(LDFLAGS) -T$(LDS) -o $@.elf $^

%.bin: %.elf
	$(OBJCOPY) --gap-fill=0xff -O binary $^ $@

$(ASM_OBJECTS): %.o: %.s Makefile
	$(CC) $(ASFLAGS) -c -o $@ $<

$(C_OBJECTS): %.o: %.c Makefile
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@rm $(ASM_OBJECTS) $(C_OBJECTS) target.elf
