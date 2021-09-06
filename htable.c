#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "htable.h"
#include "mylib.h"

/*!
 * @struct htablerec
 * @abstract Contains all of the hash table's properties.
 *
 * @field capacity Integer value of the size of the hash table.
 * @field num_keys Integer value of the number of keys in the hash table.
 * @field *frequencies Pointer to Integer value of the frequency of each key.
 * @field **keys Pointer to String keys.
 * @field *stats Pointer to Integers of the statistics of collisions.
 * @field method Type hashing_t either Linear Probing  or Double Hashing method. 
*/

struct htablerec{
    int capacity;
    int num_keys;
    int *frequencies;
    char **keys; 
    int *stats;
    hashing_t method;
    
};


/*!
 * @function htable_new
 * @abstract Creates a new hash table with a given size and method of hashing.
 *
 * @param capacity The capacity of the hash table.
 * @param method The method of hashing: either linear probing or double hashing.
 *
 * @result The new table created. 
*/
htable htable_new(int capacity, hashing_t method){
    int i =0;
    htable table = emalloc(sizeof *table);
    table -> capacity = capacity;
    table -> num_keys = 0;
    table -> method = method;
    table -> frequencies = emalloc(table -> capacity * sizeof table->frequencies[0]);
    table -> keys = emalloc(table -> capacity * sizeof table -> keys[0]);
    table -> stats = emalloc(table -> capacity * sizeof table -> stats[0]);

    /*creating the table */
    for(i = 0; i < capacity; i++){
        table -> frequencies[i] = 0;
        table -> keys[i] = NULL;
        table -> stats[i] = 0;
    }
    return table;
}


/*!
 * @function htable_word_to_int
 * @abstract Turns the given String into an unsigned int used for indexing.
 *
 * @param *word The String to change into an unsigned int. 
 *
 * @result The unsigned int transformation of the given String. 
*/
static unsigned int htable_word_to_int(char *word){
    unsigned int result =0;

    while(*word != '\0'){
        result = (*word++ + 31 *result);
    }
    return result;
}


/*!
 * @function htable_step
 * @abstract Adds a step to the hash table for double hashing. 
 *
 * @param h The hash table.
 * @param i_key The original value to add the step to. 
 *
 * @result The stepped unsigned int. 
*/
static unsigned int htable_step(htable h, unsigned int i_key){
    return 1 + (i_key % (h->capacity -1));
}


/*!
 * @function htable_insert
 * @abstract Inserts Strings into the hash table.
 *
 * @param h The hash table.
 * @param *str The String to insert.
 *
 * @result Return 0 when the hash table is full. 
*/
int htable_insert(htable h, char *str){
    int i=0;
    unsigned int result = htable_word_to_int(str);
    int collisions = 0;
    int step;
    

    /* Adding a step before begining to search */
    /*i = result%h->capacity;*/
    step = htable_step(h, result);
    i = result%h->capacity;
    
    /*inserting value into the hash table */
    while(collisions<h->capacity){
      if(h-> keys[i] == NULL){
        h -> keys[i] = emalloc((strlen(str)+1) * sizeof(char));
        strcpy(h-> keys[i],str);

        /*record num collisions in *stats. */
        h->stats[h->num_keys] = collisions;

        h -> num_keys++;
        h -> frequencies[i]++;

        return 1;

      }else if(strcmp(h -> keys[i], str) == 0){
        h-> frequencies[i]++;
        return h->frequencies[i];

      }
      
	if (h->method == DOUBLE_H){
	  i = (i + step) % h->capacity;
	} else {
	  i = (i + 1) % h->capacity;
	}
	collisions++;

	/*record num collisions in *stats. */
	h->stats[h->num_keys] = collisions;
    }
        /* hash table is full so return 0. */
        return 0;
}



/*!
 * @function htable_print
 * @abstract Prints the frequencies and keys of the hash table.
 *
 * @param h The hash table.
 * @param f() A print info function in the main method to be passed in. 
*/
void htable_print(htable h, void f()){
    int pos = 0;
    for(pos = 0; pos < h->capacity; pos++){
        if(h->keys[pos] != NULL){
	    f(h->frequencies[pos], h->keys[pos]);
        }
    }
}


/*!
 * @function htable_search
 * @abstract Searches the hash table for a given String.
 *
 * @param h The hash table.
 * @param *str The String to search for in the hash table.
 *
 * @result If not found return 0. If found return the frequency at the position.  
*/
int htable_search(htable h, char *str){
    /*! @var collisions The number of collisions while searching. */
    int collisions = 0;
    /*! @var pos The current position searched. */
    int pos;
    /*! @var word The transformed String to an unsigned int for indexing. */
    unsigned int word;
    word = htable_word_to_int(str);
    pos = word % h->capacity;

    /* Adding a step before the search */
    if (h->method == DOUBLE_H){
      pos = htable_step(h, word);
    }
    
    while(strcmp(h->keys[pos],str) != 0 && h->keys[pos] != NULL && collisions <= h->capacity){
      if (h->method == DOUBLE_H){
        pos = htable_step(h, pos);
      } else {
	    pos = (pos + 1)%h->capacity;
      }
        collisions++;
    }
    if(collisions == h->capacity){
        return 0;
    }else{
        return h->frequencies[pos];
    }
}

/*!
 * @function htable_print_entire_table
 * @abstract Prints the entire hash table.
 *
 * @param h The hash table.
 * @param *stream The File stream for printing to. 
*/
void htable_print_entire_table(htable h, FILE *stream){
    int pos = 0;
    fprintf(stream, "%5s %5s  %s  %s\n","Pos","Freq","Stats","Word");
    fprintf(stream, "----------------------------------------\n");
    while(pos < h-> capacity){
        if(h->keys[pos] != NULL){
	  fprintf(stream, "%5d %5d %5d   %s\n", pos, h->frequencies[pos], h->stats[pos], h->keys[pos]);
                }
        pos++;
    }
}


/*!
 * @function htable_free
 * @abstract Frees the memory associated with the hash table. 
 *
 * @param h The hash table.
*/
void htable_free(htable h){
    int i;
    for(i = 0; i < h->capacity; i++){
        if(h->keys[i] != NULL){
            free(h->keys[i]);
        }
    }
    free(h->keys);
    free(h->frequencies);
    free(h);
}

/**                                                                                                 
 * Prints out a table showing what the following attributes were like                               
 * at regular intervals (as determined by num_stats) while the                                      
 * hashtable was being built.                                                                       
 *                                                                                                  
 * @li Percent At Home - how many keys were placed without a collision                              
 * occurring.                                                                                       
 * @li Average Collisions - how many collisions have occurred on                                    
 *  average while placing all of the keys so far.                                                   
 * @li Maximum Collisions - the most collisions that have occurred                                  
 * while placing a key.                                                                             
 *                                                                                                  
 * @param h the hashtable to print statistics summary from.                                         
 * @param stream the stream to send output to.                                                      
 * @param num_stats the maximum number of statistical snapshots to print.                           
 */                                                                                                 
void htable_print_stats(htable h, FILE *stream, int num_stats) {                                    
    int i;                                                                                          
                                                                                                    
    fprintf(stream, "\n%s\n\n",                                                                    
            h->method == LINEAR_P ? "Linear Probing" : "Double Hashing");                           
    fprintf(stream, "Percent   Current    Percent    Average      Maximum\n");                      
    fprintf(stream, " Full     Entries    At Home   Collisions   Collisions\n");                   
    fprintf(stream, "------------------------------------------------------\n");                   
    for (i = 1; i <= num_stats; i++) {                                                           
        print_stats_line(h, stream, 100 * i / num_stats);                                
    }                                                                                              
    fprintf(stream, "------------------------------------------------------\n\n");                  
}                  

/**                                                                                                 
 * Prints out a line of data from the hash table to reflect the state                               
 * the table was in when it was a certain percentage full.                                          
 * Note: If the hashtable is less full than percent_full then no data                               
 * will be printed.                                                                                 
 *                                                                                                  
 * @param h - the hash table.                                                                       
 * @param stream - a stream to print the data to.                                                   
 * @param percent_full - the point at which to show the data from.                                  
 */                                                                                                 
void print_stats_line(htable h, FILE *stream, int percent_full) {                            
    int current_entries = h->capacity * percent_full / 100;                                         
    double average_collisions = 0.0;                                                                
    int at_home = 0;                                                                                
    int max_collisions = 0;                                                                         
    int i = 0;
    if (current_entries > 0 && current_entries <= h->num_keys) {                                   
        for (i = 0; i < current_entries; i++) {                                                 
            if (h->stats[i] == 0) {                                                                
                at_home++;                                                                        
            }                                                                                       
            if (h->stats[i] > max_collisions) {                                                    
                max_collisions = h->stats[i];                                                       
            }                                                                                       
            average_collisions += h->stats[i];                                                      
        }                                                                                           
                                                                                                    
        fprintf(stream, "%4d %10d %11.1f %10.2f %11d\n", percent_full,                              
                current_entries, at_home * 100.0 / current_entries,                                
                average_collisions / current_entries, max_collisions);                             
    }                                                                                               
}
 
