
#
# Makefile for serial-test
#

NAME            := datapath-test
LPCLIBDIR       := ../../liblpc23xx
LIBDIR          := ../..

INCLUDE         := -I.\
                   -I./include\
                   -I$(LPCLIBDIR)/include\
                   -I$(LPCLIBDIR)/lpc23xx-uart/include\
                   -I$(LPCLIBDIR)/lpc23xx-usb/include\
                   -I$(LPCLIBDIR)/lpc23xx-binsem/include\
                   -I$(LPCLIBDIR)/lpc23xx-i2c/include\
                   -I$(LPCLIBDIR)/lpc23xx-util/include\
                   -I$(LPCLIBDIR)/lpc23xx-pll/include\
                   -I$(LPCLIBDIR)/lpc23xx-mam/include\
                   -I$(LPCLIBDIR)/lpc23xx-vic/include\
                   -I$(LIBDIR)/olimex2378-util/include

#CSRCS            = serial-fifo.c
DEBUG           := -DDEBUG_USB
include $(LIBDIR)/include/common-test-make.mk


