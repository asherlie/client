#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

struct client{
    char* name, * notes;

    struct client* prev, * next;
};

struct client_book{
    struct client** c_buckets;
    int n_clients;
};

void init_cb(struct client_book* cb){
    cb->c_buckets = calloc(sizeof(struct client*), UINT8_MAX);
    cb->n_clients = 0;
}

void free_cb(struct client_book* cb){
    struct client* prev_c = NULL;
    for(int i = 0; i < UINT8_MAX; ++i){
        for(struct client* c = cb->c_buckets[i]; c; c = c->next){
            free(c->name);
            free(c->notes);
            if(prev_c)free(prev_c);
            prev_c = c;
        }
        if(prev_c){
            free(prev_c);
            prev_c = NULL;
        }
    }
    free(cb->c_buckets);
}

/* TODO: alphebatize */
void insert_cb(struct client_book* cb, char* name, char* notes, _Bool cap){
    struct client* c, * e = calloc(sizeof(struct client), 1);
    struct client* clast = NULL;

    /*insert before the first client that's > e->name*/
    /*update this to set prev*/
    /*update this to optionally alphebatize using strcmp*/
    e->name = strdup(name);
    e->notes = strdup(notes);
    ++cb->n_clients;

    if(cap){
        for(char* i = e->name; *i; ++i){
            *i = toupper(*i);
        }
    }

    c = cb->c_buckets[(uint8_t)*e->name];
    /*printf("%p\n", c);*/

    /*
     * if(!c){
     *     cb->c_buckets[(uint8_t)*name] = e;
     *     return;
     * }
    */

    /*prev -> cname -> next*/
    /*prev -> e -> cname -> next*/
    /*cname*/
    /*e -> cname*/
    /*printf("%p and %p\n", c);*/
    for(; c; c = c->next){
        // name < cname, insert before cname
        // c is invalid mem
        // why wouldn't it be zeroed if invalid
        if(strcmp(e->name, c->name) < 0){
            e->next = c;
            e->prev = c->prev;
            //
            if(c->prev){
                c->prev->next = e;
                /*return;*/
            }
            else{
                cb->c_buckets[(uint8_t)*e->name] = e;
                return;
            }
            c->prev = e;
            break;
        }
        /*return;*/
        clast = c;
    }

    /*if we got here, place at end of list*/
    /*e->next = c;*/

    /*handle inserting at idx 0 here!*/
    if(clast){
        clast->next = e;
        e->prev = clast;
        return;
    }

    cb->c_buckets[(uint8_t)*e->name] = e;


/*

    e->next = c;
    e->prev = NULL;
    cb->c_buckets[(uint8_t)*name] = e;

*/
    /*++cb->n_clients;*/
}

void print_client(const struct client* c, FILE* fp){
    fprintf(fp, "%s\n  %s\n\n", c->name, c->notes);
}

_Bool read_client(struct client_book* cb, FILE* fp){
    char c;
    /*char name_buf[1026];*/
    /*char note_buf[1026];*/
    char buf[2][1026];
    _Bool nl = 0;
    int idx = 0;

    while((c = fgetc(fp)) != EOF){
        buf[nl][idx++] = c;
        if(c == '\n'){
            buf[nl][idx-1] = 0;
            if(nl){
                /* discared \n */
                c = fgetc(fp);
                break;
            }
            idx = 0;
            nl = 1;
        }
    }
    if(c == EOF)
        return 0;

    /*strcmp();*/
    /*printf();*/
    insert_cb(cb, buf[0], buf[1], 1);

    return 1;
}

/* get rid of fallback in favor of a search_all option */
int print_clients(const struct client_book* cb, char** terms, int n_terms, FILE* fp, _Bool search_all){
    int ret = 0;
    struct client* cp;

    if(!search_all && n_terms && terms){
        for(int i = 0; i < n_terms; ++i){
            for(cp = cb->c_buckets[toupper((uint8_t)*terms[i])]; cp; cp = cp->next){
                if(strcasestr(cp->name, terms[i])){
                    print_client(cp, fp);
                    ++ret;
                }
            }
        }
    }
    else{
        for(int i = 0; i < UINT8_MAX; ++i){
            for(cp = cb->c_buckets[i]; cp; cp = cp->next){
                if(terms && n_terms){
                    for(int j = 0; j < n_terms; ++j){
                        if(strcasestr(cp->name, terms[j])){
                            print_client(cp, fp);
                            /* if this client matches one term, no need to check others */
                            break;
                        }
                    }
                }
                else{
                    print_client(cp, fp);
                }
            }
        }
    }

    return ret;
}

void test(struct client_book* cb, FILE* fp){
    while(read_client(cb, fp));
    /*for(int i = 0; i < 6; ++i){*/
        /*read_client(cb, fp);*/
    /*}*/

    /*print_clients(cb, NULL, 0, stdout, 0);*/
}

/*
 * can just have -i and -o flags
 * everything else is considered a filter
 *
 * output will be written to ofile
 * input read from ifile
 * 
 * -i inserts data from -i file
 *
 * -o outputs data to -o file
 *
 *  without any flags data will be printed to stdout
 *  
 */

int main(int argc, char** argv){
    struct client_book cb;
    FILE* fp = stdout;

    if(argc > 2 && *argv[argc-2] == '-' && argv[argc-2][1] == 'o'){
        // TODO: should just be w
        fp = fopen(argv[argc-1], "w+");
    }

    init_cb(&cb);
    insert_cb(&cb, "zack", "late", 1);
    insert_cb(&cb, "asher lieber", "good man", 1);
    insert_cb(&cb, "chrib", "bla bla", 1);
    insert_cb(&cb, "chria", "bla bla", 1);
    insert_cb(&cb, "asher liebeq", "good man", 1);
    insert_cb(&cb, "chric", "bla bla", 1);
    insert_cb(&cb, "chrac", "bla bla", 1);
    insert_cb(&cb, "asher liebea", "good man", 1);
    insert_cb(&cb, "zacj", "good man", 1);
    insert_cb(&cb, "chris", "bla bla", 1);
    insert_cb(&cb, "max", "xxx", 1);
    insert_cb(&cb, "moshe", "xxx", 1);

    print_clients(&cb, argv+1, argc-1 - (fp == stdout ? 0 : 2), fp, 0);


    if(fp != stdout){
        fseek(fp, 0, SEEK_SET);
        test(&cb, fp);
        printf("%i elements post test\n", cb.n_clients);
        fclose(fp);
    }

    free_cb(&cb);
}
