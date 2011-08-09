
#
# Makefile for olimex status led test
#

NAME            := scp1000-test
LPCLIBDIR       := ../../liblpc23xx
LIBDIR          := ../..

INCLUDE         := -I.\
                   -I./include\
                   -I$(LPCLIBDIR)/include\
                   -I$(LPCLIBDIR)/lpc23xx-uart/include\
                   -I$(LPCLIBDIR)/lpc23xx-spi/include\
                   -I$(LPCLIBDIR)/lpc23xx-util/include\
                   -I$(LPCLIBDIR)/lpc23xx-pll/include\
                   -I$(LIBDIR)/olimex2378-util/include

DEBUG           := -DDEBUG_SPI
include $(LIBDIR)/include/common-test-make.mk


