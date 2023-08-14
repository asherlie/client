#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

struct client{
    char* name, * notes;

    struct client* next;
};

struct client_book{
    struct client** c_buckets;
};

void init_cb(struct client_book* cb){
    cb->c_buckets = calloc(sizeof(struct client*), UINT8_MAX);
}

/* TODO: alphebatize */
void insert_cb(struct client_book* cb, char* name, char* notes){
    struct client* c = cb->c_buckets[(uint8_t)*name], * e = malloc(sizeof(struct client));

    e->name = name;
    e->notes = notes;
    e->next = c;
    cb->c_buckets[(uint8_t)*name] = e;

}

// if no matches are found, do a O(n) search through all elements

int main(){
    struct client_book cb;

    init_cb(&cb);
}
