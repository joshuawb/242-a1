#include <stdio.h>                                                                     
#include <stdlib.h>
#include <getopt.h>
#include "mylib.h"                                                                     
#include "htable.h"


/* setting the default hash table size to 113 */
#define HASHSIZE 113

/*!
 * @function print_info
 * @abstract Prints frequencies and words given. 
 *
 * @param freq The frequency.
 * @param *word The word.
 */
static void print_info(int freq, char *word) {
    printf("%-4d %s\n", freq, word);
}

/*!
 * @function next_prime
 * @abstract Find the next prime larger than a given value.
 *
 * @param input The number that a prime larger than needs to be found.
 *
 * @result The next prime larger than given input unless input is prime then input is returned. 
 */
static int next_prime(int input){                                                             
    int i;
    if (input <= 1){
      return next_prime(input++);
    }
    
    if(input <= 3){                                                                
        return input;                                                                           
    }
    
    if (input % 2 == 0 || input % 3 == 0){
      return next_prime(input++);
    }
    
    for(i = 5; i * i <= input; i=i+6){                                                           
      if(input % i == 0 || input % (i + 2) == 0){                                                
	return next_prime(input++);                                                       
      }
    }
    
    return input;
}

/*!
 * @function help_messages
 * @abstract Sends help messages to stderr if command line input is invalid.
 *
 * @param *program The invalid argument given. 
 */
static void help_messages(char *program){
    fprintf(stderr, "Invalid input to: %s\n\n%s", program,
            "-d  Sets collision resolution strategy to double hashing\n"
	    "    rather than linear probing. \n\n");

    fprintf(stderr,
            "-p  Should use the function in print-stats.txt instead\n"
            "    of trying to print the words and frequencies. \n\n"
            "-e  Display full hash table contents in a string format. \n\n");

    fprintf(stderr,
            "-h  Print a help message. \n\n" 
            "-s  Snapshot should be used to display hashtable\n"
            "    statistic. \n\n"
            "-t  Use the first prime number which is less than or equal\n"
            "    to the size of the table. Assuming all tables are\n"
            "    greater than size 0. ");
}

/*!
 * @function main
 * @abstract The main method for running the hash table program. 
 *
 * @param argc An integer argument for the optstring.
 * @param **argv A String argument for the optstring.
 *
 * @result EXIT_SUCCESS if successful. 
 */
int main(int argc, char **argv) {
    /*! @var capacity Default size of 113. */
    int capacity = 113;
    /*! @var method Default method of Linear Probing. */
    hashing_t method = LINEAR_P;
    /*! @var input Instantiated for use for arguments later. */ 
    /*int input = 0;*/
    /*! @var printstats Initialized at 0 but changes given different command line args. */
    int printstats = 0;
    /*! @var h The new hash table with given capacity and method. */
    htable h;
    /*! @var word[] Array of chars set to 256 length. */
    char word[256];

    int snapshots = 10;
    
    /*! @var *optstring The String of valid command line arguments. */
    const char *optstring = "deps:t:h";
    /*! @var option Will be set to getopt(argc, argv, optstring). */
    char option;



    
    while ((option = getopt(argc, argv, optstring)) != EOF) {
        switch (option) {
            case 'd':
                /* double hashing */
                method = DOUBLE_H;
                break;
            case 'e':
                /* display contents of htable on stderr using format string in asgn */
                printstats = 3;
                break;
            case 'p':
                /* print stats info using functions in print-stats.txt  */
                printstats = 2;
                break;
            case 's':
                /* display up to the given number of stats snapshots when given -p as arg. */
	        snapshots = atoi(optarg);
                break;
            case 't':
                /* first prime bigger then tablesize as size of htable assuming > 0 */
                capacity = next_prime(atoi(optarg));
                break;
            case 'h':
                /* print help message describing how to use program */
                help_messages(argv[optind]);
                exit(EXIT_SUCCESS);
        }
    }  

    h = htable_new(capacity, method);
                                                                                       
    /* Getting word and inserting it into the htable. */                                           
    while(getword(word, sizeof word, stdin) != EOF){
        htable_insert(h, word);
    }
    
    if (printstats == 3) {
        htable_print_entire_table(h, stderr);
	htable_print(h, print_info);
    } else if (printstats == 2) {
        htable_print_stats(h, stdout, snapshots);
    } else {
        htable_print(h, print_info);
    }

                                                                                   
    htable_free(h);                                                   
    return EXIT_SUCCESS;
}

