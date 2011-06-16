
#
# Makefile for libolimex2378 
#

NAME            := libolimex2378
LPCLIBDIR       := ./liblpc23xx

CC              := arm-elf-gcc
LD              := arm-elf-ld
AR              := arm-elf-ar
AS              := arm-elf-as
CP              := arm-elf-objcopy
OD              := arm-elf-objdump

TYPE            ?= lpc23xx

TARGET          ?=

DEBUG           ?=
#DEBUG           = -DDEBUG
 
INCLUDE         := -I$(LPCLIBDIR)/include\
		   -I$(LPCLIBDIR)/lpc23xx-pll/include\
		   -I$(LPCLIBDIR)/lpc23xx-uart/include\
	           -I./olimex2378-util/include\
	           -I./olimex2378-util/led-test/include

HS              :=  $(wildcard ./include/*.h)\
                    $(wildcard ./olimex2378-util/include/*.h)
                    $(wildcard ./olimex2378-util/test/include/*.h)

EXLIBS          = ./liblpc23xx/liblpc23xx.a

LIBS            = $(NAME).a

TESTS           = ./olimex2378-util/led-test/led-test.hex

ASRCS           := $(wildcard olimex2378-util/*.s)

CSRCS           := $(wildcard olimex2378-util/*.c)

COBJS           = $(CSRCS:.c=.o)

AOBJS           = $(ASRCS:.s=.o)
                  
#CFLAGS          = $(INCLUDE) $(DEBUG) -fwhopr -flto -c -Wall -fno-common -O0 -g -mcpu=arm7tdmi-s
CFLAGS          = $(INCLUDE) $(DEBUG) -g -c -Wall -Werror -fno-common -O2 -mcpu=arm7tdmi-s

ARCHIVEFLAGS    = rvs

ASFLAGS         = -g -ahls -mfloat-abi=softfp $(INCLUDE) 
 
.PHONY: clean allclean rebuild

.SUFFIXES : .c .cpp .s

.c.o :
	@echo "======== COMPILING $@ ========================"
	$(CC) $(CFLAGS) -o $(<:.c=.o) -c $<

.s.o :
	@echo "======== COMPILING $@ ========================"
	$(AS) $(ASFLAGS) -o $@ $< > $*.lst
        
all: $(LIBS) $(EXLIBS) $(TESTS) Makefile

$(COBJS): $(HS)

$(EXLIBS): 
	@echo "========= Recursive make: $(@D)    ========================"
	$(MAKE) TARGET=$(TARGET) -s -C $(@D) $(@F)

$(LIBS): $(AOBJS) $(COBJS) $(EXLIBS)
	@echo "========= Making Library $@ ========================"
	$(AR) $(ARCHIVEFLAGS) $@ $(AOBJS) $(COBJS)

$(TESTS): $(LIBS)
	@echo "========= Recursive make: $(@D) ========================"
	$(MAKE) -s -C $(@D) $(@F)

clean:
	$(RM)  $(LIBS) $(AOBJS) $(COBJS) $(COBJS) \
	*.map *.hex *.bin *.lst *~ ./include/*~ a.out 
	$(MAKE) -s -C olimex2378-util/led-test clean

allclean: clean
	$(MAKE) -s -C liblpc23xx clean
	
rebuild: allclean
	$(MAKE)

