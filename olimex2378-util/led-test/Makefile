
NAME            := led-test
LPCLIBDIR       := ../../liblpc23xx
LIBDIR          := ../..


CROSS           := /opt/cross
GCC_VERSION     ?= 4.5.2

CC              := arm-elf-gcc
LD              := arm-elf-ld
AR              := arm-elf-ar rvs
AS              := arm-elf-as
CP              := arm-elf-objcopy
OD              := arm-elf-objdump

INCLUDE         := -I.\
                   -I./include\
                   -I$(LPCLIBDIR)/include\
                   -I$(LPCLIBDIR)/lpc23xx-uart/include\
                   -I$(LPCLIBDIR)/lpc23xx-pll/include\
                   -I$(LIBDIR)/olimex2378-util/include

TYPE            ?= lpc23xx

TARGET          ?=

DEBUG           ?=
#DEBUG           = -DDEBUG
                  
CFLAGS          = $(INCLUDE) $(DEBUG) $(TARGET) -c -Wall -Werror -flto -fno-common -O2 -g -mcpu=arm7tdmi-s

AFLAGS          = -g  -ahls -mapcs-32

ASFLAGS         = -S -c -g $(INCLUDE)

LDFLAGS          = -T $(TYPE).ld -nostartfiles -Map $(NAME).map

CPFLAGS         := -O binary
HEXFLAGS        := -O ihex
ODFLAGS         := -x --syms

ASRCS           := boot.s
                  
CSRCS           := $(NAME).c hwsys.c

COBJS           = $(CSRCS:.c=.o)

AOBJS           = $(ASRCS:.s=.o)

EXLIBS          = $(LPCLIBDIR)/liblpc23xx.a $(LIBDIR)/libolimex2378.a

PROGS           = $(NAME).out

.PHONY: clean

.SUFFIXES : .c .cpp .s

.c.o :
	@echo "======== COMPILING $@ ========================"
	$(CC) $(CFLAGS) -c $<

.s.o :
	@echo "======== COMPILING $@ ========================"
	$(AS) $(AFLAGS) -o $@ $< > $*.lst
        
all:  $(PROGS) $(EXLIBS) $(NAME).hex Makefile $(NAME).s

$(COBJS): include/*.h

$(EXLIBS):
	@echo "========= Recursive make: $(@D)    ========================"
	$(MAKE) TARGET=$(TARGET) -s -C $(@D) $(@F)

$(PROGS): $(AOBJS) $(COBJS) $(EXLIBS)
	@echo "========= LINKING $@ ========================"
	$(LD) $(LDFLAGS) -o $@ $(AOBJS) $(COBJS) $(EXLIBS) -L$(CROSS)/arm-elf/lib -lc -L$(CROSS)/lib/gcc/arm-elf/$(GCC_VERSION) -lgcc

$(NAME).s: $(NAME).c
	@echo "========= Combined Assembler and Source for $@ =================="
	$(CC) $(ASFLAGS) -o $@ $(NAME).c

$(NAME).hex: $(NAME).out
	@echo "========= hex file for $@ =================="
	$(CP) $(HEXFLAGS) $(NAME).out $(NAME).hex
		
$(NAME).bin: $(NAME).out
	@echo "========= bin file for $@ =================="
	$(CP) $(CPFLAGS) $(NAME).out $(NAME).bin
	$(OD) $(ODFLAGS) $(NAME).out > $(NAME).dmp

clean:
	$(RM) $(EXLIBS) $(PROGS) $(AOBJS) $(COBJS) \
	*.map *.hex *.bin *.lst *~ ./include/*~ a.out 

allclean: clean
	$(MAKE)  -s -C $(LIBDIR) allclean

rebuild: allclean
	$(MAKE)  -s -C $(LPCLIBDIR) allclean
	$(MAKE)

