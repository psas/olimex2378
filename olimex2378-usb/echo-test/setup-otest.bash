#!/bin/bash

ln -s ../../liblpc23xx/startup/openocd_lpc2378.cfg .
ln -s ../../liblpc23xx/startup/hwsys.c .
ln -s ../../liblpc23xx/startup/boot.s .
ln -s ../../liblpc23xx/startup/lpc23xx.ld .

cp  ../../liblpc23xx/startup/oocd_flash_lpc2378.script .

cd include
ln -s ../../../liblpc23xx/startup/include/hwsys.h .

echo DONE

