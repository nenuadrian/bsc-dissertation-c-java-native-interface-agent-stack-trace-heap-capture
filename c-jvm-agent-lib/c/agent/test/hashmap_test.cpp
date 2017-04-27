#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "logging.h"
#include "hashmap.h"
#define KEY_MAX_LENGTH (256)
#define KEY_PREFIX ("somekey")
#define KEY_COUNT (20)

using namespace std;
int VERBOSE;

typedef struct data_struct_s
{
    char key_string[KEY_MAX_LENGTH];
    int number;
} data_struct_t;

int hashmap_iterator(char * key, any_t data, any_t item) {
  debug("key: %s", key);
  return 0;
}

struct MapHolder {
  map_t mymap;
};

void testPassing2(MapHolder * mapHolder){
  /* First, populate the hash map with ascending values */
    for (int index=434333434; index<434333434 + KEY_COUNT; index+=1)
    {
        /* Store the key string along side the numerical value so we can free it later */
        char *key = (char*)malloc( sizeof(char) * ( 40 + 1 ) );
        sprintf(key, "%s_%d", KEY_PREFIX, index);
       

        int error = hashmap_put(mapHolder->mymap, key, key);
      //  assert(error==MAP_OK);
    }

}
void testPassing(MapHolder * mapHolder) {
  testPassing2(mapHolder);
}

int main(int argc, char** argv)
{
    VERBOSE = 3;
    MapHolder mapHolder;
    
    mapHolder.mymap = hashmap_new();

    

    testPassing(&mapHolder);
      hashmap_iterate(mapHolder.mymap, &hashmap_iterator, NULL);

    /* Now, destroy the map */
    hashmap_free(mapHolder.mymap);

    return 1;
}