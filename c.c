#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct client{
    char* name, * notes;

    struct client* next;
};

struct client_book{
    struct client** c_buckets;
    int n_clients;
};

void init_cb(struct client_book* cb){
    cb->c_buckets = calloc(sizeof(struct client*), UINT8_MAX);
    cb->n_clients = 0;
}

/* TODO: alphebatize */
void insert_cb(struct client_book* cb, char* name, char* notes){
    struct client* c = cb->c_buckets[(uint8_t)*name], * e = malloc(sizeof(struct client));

    e->name = name;
    e->notes = notes;
    e->next = c;
    cb->c_buckets[(uint8_t)*name] = e;

    ++cb->n_clients;
}

/* prints a given bucket with an optional search term */
void print_bucket(struct client* c, char* term, FILE* fp){
    for(struct client* cp = c; cp; cp = cp->next){
        if(!term || strstr(cp->name, term))
            fprintf(fp, "%s: \"%s\"\n", cp->name, cp->notes);
    }
}

void print_clients(struct client_book* cb, char** terms, int n_terms, FILE* fp){
    if(!n_terms || !terms){
        for(int i = 0; i < UINT8_MAX; ++i){
            print_bucket(cb->c_buckets[i], NULL, fp);
        }
        return;
    }
    for(int i = 0; i < n_terms; ++i){
        print_bucket(cb->c_buckets[(uint8_t)*terms[i]], terms[i], fp);
    }
}

// if no matches are found, do a O(n) search through all elements

int main(int argc, char** argv){
    struct client_book cb;

    init_cb(&cb);
    insert_cb(&cb, "zack", "late");
    insert_cb(&cb, "asher lieber", "good man");
    insert_cb(&cb, "another name", "pretty good man");

    print_clients(&cb, argv+1, argc-1, stdout);
}
