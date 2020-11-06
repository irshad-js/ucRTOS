BINPATH=/usr/bin

CC=$(BINPATH)/gcc
CXX=$(BINPATH)/g++
AR=$(BINPATH)/ar
LD=$(BINPATH)/ld
OBJCOPY=$(BINPATH)/objcopy
SIZE=$(BINPATH)/size

vpath %.c ../../../core
vpath %.c ../../../core/screens
vpath %.c ../../../lib/mystdio
vpath %.c ../../../lib/AsciiLib
vpath %.c ../../../lib/LockFreeFifo
vpath %.c ../../../lib/eMIDI/src
vpath %.c ../../../lib/eMIDI/src/hal
vpath %.c ../../../lib/FreeRTOS/Source
vpath %.c ../../../lib/FreeRTOS/Source/portable/MemMang
vpath %.c ../../../lib/FreeRTOS/Source/portable/GCC/Posix
vpath %.c src
vpath %.c src/drivers
vpath %.c ../../simulators/lib

CFLAGS  = -g -Wall -pthread -m64 # TODO: use -m32
CFLAGS += -Ihdr
CFLAGS += -I../../../core
CFLAGS += -I../../../core/screens
CFLAGS += -I../lib/wxWidgets/include
CFLAGS += -I../../../lib/mystdio
CFLAGS += -I../../../lib/AsciiLib
CFLAGS += -I../../../lib/LockFreeFifo
CFLAGS += -I../../../lib/FreeRTOS/Source/include
CFLAGS += -I../../../lib/FreeRTOS/Source/portable/GCC/Posix
CFLAGS += -I../../embedded/lib/NRF24L01P/src
CFLAGS += -I../../embedded/lib/NRF24L01P/hal

###################################################

OBJDIR=obj
BINDIR=bin
MAIN_SRCS = heap_3.c port.c tasks.c list.c timers.c queue.c croutine.c event_groups.c \
            core_main.c hardware.c

#LIB_SRCS = <TODO: add files if needed>

UCRTOS_LIB_SRCS = myprintf.c display.c StackBasedFsm.c InputDevice.c core_main.c \
		              SlotBasedMenu.c AsciiLib.c LockFreeFifo.c _examplescreen.c mainmenuscreen.c \
                  aboutscreen.c imgtestscreen.c playermenuscreen.c playerscreen.c \
                  floppyorgelscreen.c buttontestscreen.c livemodescreen.c sdcardscreen.c \
                  images.c helpers.c midifile.c midiplayer.c lut.c emidi_linux.c \
                  skateboardscreen.c magiclanternscreen.c nrf24l01p_dummy.c

PROJ_NAME=ucrtos
OUTPATH=bin

###################################################

MAIN_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(MAIN_SRCS))
#LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(LIB_SRCS))
UCRTOS_LIB_OBJS=$(patsubst %.c,$(OBJDIR)/%.o,$(UCRTOS_LIB_SRCS))

###################################################

.PHONY: proj flash clean

all: $(OUTPATH)/$(PROJ_NAME).elf

$(OBJDIR):
	mkdir -p $(OBJDIR)
	mkdir -p $(BINDIR)

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@ -L$(OBJDIR)

$(OBJDIR)/libucrtos.a: $(UCRTOS_LIB_OBJS)
	$(AR) -r $@ $(UCRTOS_LIB_OBJS)

$(OBJDIR)/main.o: src/main.cpp
	$(CXX) $(CFLAGS) -c $< -o $@ -L$(OBJDIR) `wx-config --cxxflags --libs`

$(OUTPATH)/$(PROJ_NAME).elf: $(OBJDIR)/libucrtos.a $(MAIN_OBJS) $(OBJDIR)/main.o
	$(CXX) $(CFLAGS) $^ -o $@ -L. -Lobj -lucrtos `wx-config --cxxflags --libs`
	$(SIZE) $@

proj: $(OUTPATH)/$(PROJ_NAME).elf

clean:
	rm -rf $(OBJDIR)
	rm -rf $(BINDIR)

