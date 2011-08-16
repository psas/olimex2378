

/*
 * datapath-host.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <termios.h>    // terminal
#include <unistd.h>     // read, write, close

#include "serial-port.h"
#include "parse-args.h"
#include "datapath-host.h"

/*
 * datapath_task
 */
void datapath_task(const char* portname, const char* logfile) {
    int                 fd, closed; 

    struct termios      orig_tios;
    char                stop;

    printf("Starting datapath test.\n\n");
    printf("Opening serial port.\n");

    fd = init_port_raw(portname, 115200, &orig_tios) ;
    if(fd < 0) {
        fprintf(stderr, "Failed to open %s\n", portname);
        exit(EXIT_FAILURE);
    }

    printf("g is: %i\n", 'g');

    stop = 'g';
    write(fd, &stop, 1);

    printf("Closing serial port.\n");
    closed = close_port(fd, &orig_tios);
    if(closed != 0) {
        fprintf(stderr, "Unable to close fd: %i\n", fd);
        exit(EXIT_FAILURE);
    }

}

int main(int argc, char* argv[]) {

    char                portname[NAME_SIZE] = "/dev/ttyACM0";

    char                logfile[NAME_SIZE]  = "./datapath-host.log";

    pf_command_line( argc,
                     argv,
                     logfile,
                     portname);

    printf("\nSettings: %s --logfile %s --portname %s \n", argv[0], logfile, portname);

    datapath_task(portname, logfile);

    printf("\n** %s Done. **\n\n", argv[0]);
    return(0);

}

