# Hey Emacs, this is a -*- Makefile -*-

# Makefile
# All paths are relative to Makefile location. Possible make targets:
#  all    build firmware .elf, .hex, and .bin binaries
#  swd    attempt to flash firmware via ST-Link (linux only)
#  sfl    attempt to flash firmware via serial bootloader
#  clean  delete all built objects (not binaries or archives)

PROJ:=load
TARGET:=$(PROJ)

TOOLCHAIN_PREFIX:=arm-none-eabi

CC=$(TOOLCHAIN_PREFIX)-gcc
LD=$(TOOLCHAIN_PREFIX)-gcc
OBJCOPY=$(TOOLCHAIN_PREFIX)-objcopy
AS=$(TOOLCHAIN_PREFIX)-as
AR=$(TOOLCHAIN_PREFIX)-ar
GDB=$(TOOLCHAIN_PREFIX)-gdb

PROJ_ROOT=$(CURDIR)
BUILD_TYPE?=Release
SERIAL_BL?=/dev/ttyUSB1

ifeq ($(findstring Debug, $(BUILD_TYPE)), Debug)
BUILD_TYPE=Debug
BUILD_DIR=$(PROJ_ROOT)/build/Debug/obj
BIN_DIR=$(PROJ_ROOT)/build/Debug
OPTLVL=-O0
DBG=-g3
else
BUILD_DIR=$(PROJ_ROOT)/build/Release/obj
BIN_DIR=$(PROJ_ROOT)/build/Release
OPTLVL=-O2
DBG=
endif

STARTUP:=$(PROJ_ROOT)/src
LINKER_SCRIPT:=$(PROJ_ROOT)/STM32F103RCTx_FLASH.ld

INCLUDE=-I$(PROJ_ROOT)/src
INCLUDE+=-I$(PROJ_ROOT)/src/analog
INCLUDE+=-I$(PROJ_ROOT)/src/ui
INCLUDE+=-I$(PROJ_ROOT)/src/utils
INCLUDE+=-I$(PROJ_ROOT)/lib/STM32F10x_StdPeriph_Driver/inc
INCLUDE+=-I$(PROJ_ROOT)/lib/CMSIS

SRC_DIR=$(PROJ_ROOT)/src
SRC_DIR+=$(PROJ_ROOT)/src/analog
SRC_DIR+=$(PROJ_ROOT)/src/ui
SRC_DIR+=$(PROJ_ROOT)/src/utils
SRC_DIR+=$(PROJ_ROOT)/lib/STM32F10x_StdPeriph_Driver/src

vpath %.c $(SRC_DIR)
vpath %.s $(STARTUP)

# Startup file
ASRC=startup_stm32f103.s

# Project Source Files
SRC=main.c
SRC+=ILI9163C.c
SRC+=adc.c
SRC+=dac.c
SRC+=delay.c
SRC+=encoder.c
SRC+=flash.c
SRC+=spi.c
SRC+=dummyload.c
SRC+=timer.c

SRC+=system_stm32f10x.c
SRC+=syscalls.c

# Standard Peripheral Source Files
SRC+=misc.c
SRC+=stm32f10x_tim.c
SRC+=stm32f10x_exti.c
SRC+=stm32f10x_gpio.c
SRC+=stm32f10x_pwr.c
SRC+=stm32f10x_dma.c
SRC+=stm32f10x_usart.c
SRC+=stm32f10x_rcc.c
SRC+=stm32f10x_spi.c
SRC+=stm32f10x_flash.c
SRC+=stm32f10x_dac.c
SRC+=stm32f10x_adc.c
#SRC+=stm32f10x_wwdg.c
#SRC+=stm32f10x_bkp.c
#SRC+=stm32f10x_crc.c
#SRC+=stm32f10x_sdio.c
#SRC+=stm32f10x_cec.c
#SRC+=stm32f10x_dbgmcu.c
#SRC+=stm32f10x_i2c.c
#SRC+=stm32f10x_can.c
#SRC+=stm32f10x_fsmc.c
#SRC+=stm32f10x_iwdg.c
#SRC+=stm32f10x_rtc.c

CDEFS=-DUSE_STDPERIPH_DRIVER
CDEFS+=-DSTM32F10X_MD
CDEFS+=-DHSE_VALUE=8000000

MCUFLAGS=-mcpu=cortex-m3 -mthumb -mfloat-abi=soft
COMMONFLAGS=$(OPTLVL) $(DBG) -Wall -ffunction-sections -fdata-sections -fsingle-precision-constant -finline-functions -Wdouble-promotion -std=gnu99
CFLAGS=$(COMMONFLAGS) $(MCUFLAGS) $(INCLUDE) $(CDEFS)

LDLIBS=-lm -lc -lgcc
LDFLAGS=$(MCUFLAGS) -u -fno-exceptions -Wl,--gc-sections,-T$(LINKER_SCRIPT)

OBJ=$(ASRC:%.s=$(BUILD_DIR)/%.o)
OBJ+=$(SRC:%.c=$(BUILD_DIR)/%.o)
DEP:=$(OBJ:.o=.d)

.PHONY: clean all elf bin

all: elf bin

ifeq ($(findstring clean, $(MAKECMDGOALS)),)
-include $(DEP)
endif

$(BUILD_DIR)/%.o: %.s
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@echo [AS] $(notdir $(STARTUP)/$(ASRC))
	@$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $(@) -c -o $@ $<

$(BUILD_DIR)/%.o: %.c
	@echo [CC] $(notdir $<)
	@$(CC) $(CFLAGS) -MMD -MP -MF $(@:%.o=%.d) -MT $(@) -c -o $@ $<

elf: $(OBJ)
	@echo [LD] $(notdir $(BIN_DIR)/$(TARGET).elf)
	@$(LD) $(LDFLAGS) -o $(BIN_DIR)/$(TARGET).elf $(OBJ) $(LDLIBS)

bin: elf
	@echo [HEX] $(notdir $(BIN_DIR)/$(TARGET).hex)
	@$(OBJCOPY) -O ihex $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).hex
	@echo [BIN] $(notdir $(BIN_DIR)/$(TARGET).bin)
	@$(OBJCOPY) -O binary $(BIN_DIR)/$(TARGET).elf $(BIN_DIR)/$(TARGET).bin

clean:
	@echo [RM] Objects
	@rm -f $(OBJ)
	@rm -f $(DEP)
	@rm -f $(ASRC:%.s=$(BUILD_DIR)/%.o)
	@echo [RM] Binaries
	@rm -f $(BIN_DIR)/$(TARGET).elf
	@rm -f $(BIN_DIR)/$(TARGET).hex
	@rm -f $(BIN_DIR)/$(TARGET).bin

# Flash firmware via ST-Link
# more at https://github.com/texane/stlink
swd: all
	@st-flash --reset write $(BIN_DIR)/$(TARGET).bin 0x8000000

# Flash firmware via USART1, neet to put stm32 into bootloader mode
# more at https://sourceforge.net/p/stm32flash/wiki/Home/
sfl: all
	@stm32flash -b 115200 -w $(BIN_DIR)/$(TARGET).bin -s 0x08000000 -v $(SERIAL_BL)
