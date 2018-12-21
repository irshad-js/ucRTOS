BINPATH=/usr/bin

CC=$(BINPATH)/gcc
AR=$(BINPATH)/ar
LD=$(BINPATH)/ld
OBJCOPY=$(BINPATH)/objcopy
SIZE=$(BINPATH)/size

#vpath %.c .
#vpath %.c ../../../core
#vpath %.c ../../../core/screens
#vpath %.c ../../../lib/mystdio
#vpath %.c ../../../lib/AsciiLib
#vpath %.c ../../../lib/LockFreeFifo
#vpath %.c ../../../lib/eMIDI/src
#vpath %.c ../../../lib/eMIDI/src/hal
vpath %.c ../../../lib/FreeRTOS/Source
vpath %.c ../../../lib/FreeRTOS/Source/portable/MemMang
vpath %.c ../../../lib/FreeRTOS/Source/portable/GCC/Posix
vpath %.c src
#vpath %.c src/drivers
#vpath %.c ../lib/NRF24L01P/src
#vpath %.c ../lib/NRF24L01P/hal/stm32f4
#vpath %.a lib


CFLAGS  = -g -Wall -pthread
CFLAGS += -Ihdr # -Ilib -Ilib/inc
#CFLAGS += -Ilib/inc/core
#CFLAGS += -I../../../core
#CFLAGS += -I../../../core/screens
#CFLAGS += -I../../../lib/mystdio
#CFLAGS += -I../../../lib/AsciiLib
#CFLAGS += -I../../../lib/LockFreeFifo
CFLAGS += -I../../../lib/FreeRTOS/Source/include
CFLAGS += -I../../../lib/FreeRTOS/Source/portable/GCC/Posix
#CFLAGS += -I../lib/NRF24L01P/src
#CFLAGS += -I../lib/NRF24L01P/hal

###################################################

OBJDIR=obj
BINDIR=bin
MAIN_SRCS = main.c heap_3.c port.c tasks.c list.c timers.c queue.c croutine.c event_groups.c

#core_main.c hardware.c nesgamepad.c ssd1289.c stm32f4xx_it.c system_stm32f4xx.c syscalls.c \
#		port.c tasks.c list.c timers.c queue.c \
#		lib/cmsis_boot/startup_stm32f4xx.s hal_stm32f4.c # add startup file to build

#LIB_SRCS = misc.c stm32f4xx_dma.c stm32f4xx_rcc.c stm32f4xx_adc.c \
	stm32f4xx_exti.c stm32f4xx_rng.c stm32f4xx_can.c stm32f4xx_flash.c \
  stm32f4xx_rtc.c stm32f4xx_crc.c stm32f4xx_fsmc.c stm32f4xx_sdio.c \
	stm32f4xx_cryp_aes.c stm32f4xx_gpio.c stm32f4xx_spi.c \
	stm32f4xx_cryp.c stm32f4xx_hash.c stm32f4xx_syscfg.c \
	stm32f4xx_cryp_des.c stm32f4xx_hash_md5.c stm32f4xx_tim.c \
	stm32f4xx_cryp_tdes.c stm32f4xx_hash_sha1.c  stm32f4xx_usart.c \
  tm32f4xx_dac.c stm32f4xx_i2c.c stm32f4xx_wwdg.c \
	stm32f4xx_dbgmcu.c stm32f4xx_iwdg.c \
	stm32f4xx_dcmi.c stm32f4xx_pwr.c \

#UCRTOS_LIB_SRCS = myprintf.c display.c StackBasedFsm.c InputDevice.c core_main.c \
		  SlotBasedMenu.c AsciiLib.c LockFreeFifo.c _examplescreen.c mainmenuscreen.c aboutscreen.c imgtestscreen.c playermenuscreen.c playerscreen.c skateboardscreen.c floppyorgelscreen.c buttontestscreen.c livemodescreen.c images.c helpers.c midifile.c midiplayer.c lut.c emidi_stm32f4.c nrf24l01p.c

PROJ_NAME=ucrtos
OUTPATH=bin

###################################################

MAIN_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(MAIN_SRCS))
#LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(LIB_SRCS))
#UCRTOS_LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(UCRTOS_LIB_SRCS))

###################################################

.PHONY: proj flash clean

all: $(OUTPATH)/$(PROJ_NAME).elf

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -L$(OBJDIR)

#$(OBJDIR)/libucrtos.a: $(UCRTOS_LIB_OBJS)
#	$(AR) -r $@ $(UCRTOS_LIB_OBJS)

$(OUTPATH)/$(PROJ_NAME).elf: $(MAIN_OBJS)
	$(CC) $(CFLAGS) $^ -o $@ -L. -Lobj
	$(SIZE) $@

proj: $(OUTPATH)/$(PROJ_NAME).elf

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

