
/*
 * parse-args.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "helpers.h"
#include "parse-args.h"

void pf_command_line(int   argc,
            char*          argv[],
            char*          logfile) {

    int i=0;

    if(argc==0) {
        printHelp(argv[0]);
        exit(EXIT_SUCCESS);
    }
    for(i=0; i< argc ; i++) {
        if( (strcmp( argv[i],"-h") == 0) || (strcmp ( argv[i], "--help")==0) ){
            printHelp( argv[0]);
            exit(EXIT_SUCCESS);
        }  else if(strcmp ( argv[i],"--logfile") == 0) {
            if(i< argc-1) {
                strncpy(logfile,argv[i+1],NAME_SIZE-1);
                logfile[NAME_SIZE-1] = '\0';
            } else {
                error("No log file name.");
                printHelp( argv[0]);
                exit(EXIT_FAILURE);
            }
            ++i;
       } else {
            if(i!=0){
                error("Unrecognized option:"); 
                fprintf(stderr,"\t%s <- unrecognized\n",  argv[i]); 
                printHelp( argv[0]);
                exit(EXIT_FAILURE);
            }
        }
    }
}

/*
 * printHelp
 */
void printHelp(const char* progname) {

    printf("\n%s is a program to read data from the usb port driven by a GFE sensor node.\n\
            \n\
            Usage:\n\
                --logfile string\n\
            \t [-h|--help]\n\
            \n\
            --logfile          the file with run statistics and messages. \n\n\
            -h | --help        help message.\n\n\
            \n\n"\
            ,progname);
}

//        } else if(strcmp ( argv[i],"--ctffile") == 0) {
//            if(i< argc-1) {
//                strncpy(ctffile,argv[i+1],NAME_SIZE-1);
//                ctffile[NAME_SIZE-1] = '\0';
//            } else {
//                error("No ctffile name.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--nohash") == 0) {
//            *bloomfilter = false;
//        } else if(strcmp ( argv[i],"--zgn_search") == 0) {
//            *zgn_search = true;
//        } else if(strcmp ( argv[i],"--seedpick_heur") == 0) {
//            if(i< argc-1) {
//                if (strcmp ( argv[i+1],"RANDOM_PICK") == 0)
//                    *seedpick_heur = RANDOM_PICK;
//                else if (strcmp ( argv[i+1],"MOST_CONNECTED_PICK") == 0)
//                    *seedpick_heur = MOST_CONNECTED_PICK;
//                else if (strcmp ( argv[i+1],"LEAST_CONNECTED_PICK") == 0)
//                    *seedpick_heur = LEAST_CONNECTED_PICK;
//                else if (strcmp ( argv[i+1],"EXTRAPOLATE_AVG_CONN") == 0)
//                    *seedpick_heur = EXTRAPOLATE_AVG_CONN;
//                else { error("Not a valid choice for seedpick heuristic."); exit(EXIT_FAILURE); }
//            } else {
//                error("No seedpick heuristic choice.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--search_alg") == 0) {
//            if(i< argc-1) {
//                if (strcmp ( argv[i+1],"LBFS") == 0)
//                    *search_alg = LBFS;
//                else if (strcmp ( argv[i+1],"LDFS") == 0)
//                    *search_alg = LDFS;
//                else { error("Not a valid choice for search algorithm."); exit(EXIT_FAILURE); }
//            } else {
//                error("No search algorithm choice.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--search_order") == 0) {
//            if(i< argc-1) {
//                if (strcmp ( argv[i+1],"NONE") == 0)
//                    *search_order = NONE;
//                else if (strcmp ( argv[i+1],"ORDERED_PLY_SEARCH") == 0)
//                    *search_order = ORDERED_PLY_SEARCH;
//                else { error("Not a valid choice for search order."); exit(EXIT_FAILURE); }
//            } else {
//                error("No search order choice.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--search_depth") == 0) {
//            if(i< argc-1) {
//                *search_depth = atoi( argv[i+1]);
//                if( *search_depth<=0 ) { error("Bad search depth value. Must be >0."); exit(EXIT_FAILURE);}
//            } else {
//                error("No search_depth value.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--numseeds") == 0) {
//            if(i< argc-1) {
//                *numseeds = atoi( argv[i+1]);
//                if( *numseeds<=0 ) { error("Bad numseeds value. Must be >0."); exit(EXIT_FAILURE);}
//            } else {
//                error("No numseeds value.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
//        } else if(strcmp ( argv[i],"--numagents") == 0) {
//            if(i< argc-1) {
//                *numagents = atoi( argv[i+1]);
//                if( *numagents<=0 ) { error("Bad numagents value. Must be >0."); exit(EXIT_FAILURE);}
//            } else {
//                error("No numagents value.");
//                printHelp( argv[0]);
//                exit(EXIT_FAILURE);
//            }
//            ++i;
 
