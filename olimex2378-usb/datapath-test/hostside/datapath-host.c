

/*
 * datapath-host.c
 */


#include <stdio.h>

#include "parse-args.h"
#include "datapath-host.h"



int main(int argc, char* argv[]) {

    char        logfile[NAME_SIZE];


    pf_command_line( argc,
                     argv,
                     logfile);

    printf("Logfile is: %s\n", logfile);

    printf("hello world\n");




    return(0);

}
