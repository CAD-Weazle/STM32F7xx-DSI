### project name -- edit this
TARGET = h7-test

### sources -- edit this
CSRC += ./core/src/system_stm32f7xx.c
CSRC += main.c 
CSRC += pll.c 
CSRC += ports.c 
CSRC += ticker.c 
CSRC += uart6.c 
CSRC += i2c.c 
CSRC += fmc.c 
CSRC += dsi.c 
CSRC += otm8009a.c 
CSRC += ft6336.c 
CSRC += printf.c 
CSRC += dump.c 

### add startup file to build -- edit this
ASRC += ./core/src/startup_stm32f769xx.S    # STM32F769NIH
										    
### linker-loader script -- edit this       
LDSCRIPT = STM32F769NIHx_FLASH.ld           # STM32F769NIH    FLASH 2048K RAM 1024K

# GCC compiler
CCPATH  = d:/gcc/bin/
CC      = $(CCPATH)arm-none-eabi-gcc
OBJCOPY = $(CCPATH)arm-none-eabi-objcopy
OBJDUMP = $(CCPATH)arm-none-eabi-objdump
SIZE    = $(CCPATH)arm-none-eabi-size

CFLAGS  = -mcpu=cortex-m7                   # -- edit his
CFLAGS += -DSTM32F769xx                     # -- edit his
CFLAGS += -T$(LDSCRIPT)                     
CFLAGS += -std=gnu99 -g -O2 -Wall           
CFLAGS += -mlittle-endian -mthumb           
CFLAGS += -mfpu=fpv5-sp-d16                 # single precission floating point
CFLAGS += -mfloat-abi=hard                  # hardware floating point
CFLAGS += -specs=nano.specs -specs=nosys.specs

### own flags -- edit this
CFLAGS += -Icore/src -Icore/inc -Icore/inc_hal -Iown_inc

AFLAGS = -Iinc

# dependencies
GENDEPFLAGS = -MD -MP -MF .dep/$(@F).d

# rules
.PHONY: lib proj

HEX = $(TARGET).hex
ELF = $(TARGET).elf
DIS = $(TARGET).dis
OBJS = $(CSRC:.c=.o) $(ASRC:.S=.o)

all: proj

#proj: $(ELF) $(DIS) $(HEX)
proj: $(ELF) $(HEX)

$(DIS): $(ELF)
	$(OBJDUMP) -d $(ELF) > $(DIS)
#	$(OBJDUMP) -D $(ELF) > $(DIS).all

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex --set-start=0 $(ELF) $(HEX)

$(ELF): $(OBJS) 
	$(CC) $(GENDEPFLAGS) $(CFLAGS) $(OBJS) -o $@ -lm -Wl,-Map=$(TARGET).map

%.o: %.c
	@echo $(CC) $<
	@$(CC) $(GENDEPFLAGS) $(CFLAGS) -c $< -o $@ 

%.o: %.S
	@echo $(CC) $<
	@$(CC) $(GENDEPFLAGS) $(AFLAGS) -c $< -o $@ 

clean:
	rm -f $(wildcard *.o)
	rm -f $(wildcard core/src/*.o)
	rm -f $(ELF) 
	rm -f $(DIS)
	rm -f $(TARGET).bin
	rm -f $(TARGET).map
	rm -f $(TARGET).hex
	rm -f $(wildcard .dep/*)

list:
	@echo
	@echo C sources:
	@echo $(CSRC)
	@echo
	@echo ASM sources:
	@echo $(ASRC)
	@echo
	@echo objects:
	@echo $(OBJS)

include $(wildcard .dep/*)
