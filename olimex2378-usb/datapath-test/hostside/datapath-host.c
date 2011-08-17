

/*
 * datapath-host.c
 */


#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <termios.h>    // terminal
#include <unistd.h>     // read, write, close

#include "serial-port.h"
#include "parse-args.h"
#include "datapath-host.h"


/*
 * init_stdin
 */
int init_stdin(struct termios* orig_stdin_tios) {

    struct termios      tios;

    // Get tios 
    if (tcgetattr(0, &tios)){
        printf("init_stdin: Error getting current terminal settings\n");
        return (-1);
    } else {
        tcgetattr(0, orig_stdin_tios);
    }

    tios.c_lflag        &= ~ICANON;
    tios.c_lflag        |= ECHO;

    tios.c_cc[VMIN]     = VMIN_VALUE;
    tios.c_cc[VTIME]    = VTIME_VALUE;

    if (tcsetattr(0, TCSAFLUSH, &tios)){
        printf("init_stdin: Error applying terminal settings\n");
        return (-1);
    }

    return(0);
}

/*
 * reset_stdin
 */
int reset_stdin(struct termios* orig_stdin_tios) {

    if(tcsetattr(0, TCSAFLUSH, orig_stdin_tios)) {
            syslog(LOG_CRIT, "reset_stdin: failed to reset attributes on stdin.\n");
            return(-1);
    };

    return(0);
}


/*
 * datapath_task
 */
void datapath_task(const char* portname, const char* logfile) {
    int                 fd, closed; 
    int                 bytes_stdin; 
    char                value_stdin; 

    struct termios      orig_tios;
    struct termios      orig_stdin_tios;

    printf("Starting datapath test.\n\n");
    printf("Opening serial port.\n");

    fd = init_port_raw(portname, 115200, &orig_tios) ;
    if(fd < 0) {
        fprintf(stderr, "Failed to open %s\n", portname);
        exit(EXIT_FAILURE);
    }

    // read value from stdin
    if(init_stdin(&orig_stdin_tios) != 0) {
        fprintf(stderr, "datapath_task: Failed to init stdin.\n");
        exit(EXIT_FAILURE);
    };

    // Tue 16 August 2011 14:56:44 (PDT)
    // next, read structured binary data from lpc, and decode.

    while(1) {
        bytes_stdin = read(0, &value_stdin, 1);
        if(bytes_stdin < 0) {
            fprintf(stderr, "datapath_task: stdin read error.\n");
        } else if (bytes_stdin > 0) {
            printf("\nYou typed: %c\n", value_stdin);
        } else {}

        if(value_stdin == 'q') {
            printf("You typed quit.\n");
            break;
        }

        write(fd, &value_stdin, 1);

    }

    printf("Closing serial port.\n");
    closed = close_port(fd, &orig_tios);
    if(closed != 0) {
        fprintf(stderr, "Unable to close fd: %i\n", fd);
        exit(EXIT_FAILURE);
    }

    printf("Reset stdin.\n");
    reset_stdin(&orig_stdin_tios);

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

