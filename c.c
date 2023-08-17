// GOAL: 556 fields
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

void print_client(const struct client* c, FILE* fp, _Bool name_only){
    if(name_only)fprintf(fp, "%s\n", c->name);
    else fprintf(fp, "%s\n  %s\n\n", c->name, c->notes);
}

/*
 * ignore \nline\n
 * ignore leading non-ascii chars
 * separate by multi-\n
 * first (two?) ascii words
*/

/*need to update prev_br so it's not ignored!*/
char* parse_client(struct client_book* cb, char* initial_prev, int* initial_prev_br, FILE* fp){
    char* prev_ln = initial_prev;
    char* ln = NULL;
    int consec_nl = 0;
    size_t sz = 0;
    ssize_t br, prev_br = *initial_prev_br, prev_prev_br = 0;

    /*
     * if(1 || prev_br){
     *     printf("iniital prev br: %li\n", prev_br);
     * }
    */
    int idx = 0;
    char name[80] = {0};
    char* note = calloc(1000000, 1);

    // need to:
    //  consider first two words a name
    //  ignore leading whitespace before a name
    //  ignore lone lines
    //
    //  stop parse-ing after multiple \n\n are found
    //      IF not a lone line AND 
    //
    //finished abby - 
    //  read first \n
    //  read 3 \ns
    //
    //  then read adam's first line before we got to 4 \n
    //  prev_br gets us over the edge to 4 \n
    //  but adam's name is stored in ln, not prev_ln
    
    _Bool zombie;
    (void)zombie;
    _Bool name_phase = 1;
    while((br = getline(&ln, &sz, fp)) != -1){
        zombie = 0;
        for(int i = 0; i < 2; ++i){
            if(ln[br-1] == '\r' || ln[br-1] == '\n'){
                ln[--br] = 0;
            }
        }
        if(name_phase && idx == 0){
            /*
             * while(!isalnum(*ln)){
             *     ++ln;
             *     --br;
             * }
            */
            for(int i = 0; i < br; ++i){
                if(isalnum(ln[i]))
                    break;
                ++ln;
                --br;
                --i;
            }
            /*for(char* i = ln; */
        }
        // is this proper? sometimes we have p
        if(!prev_br)++consec_nl;
        else consec_nl = 0;
        /*puts(ln);*/

/*
 * don't insert until
 * we only append/insert prev_ln when we recv a new one so we know we're not reading a zombie
*/
        // skip leading non-alpha

        /*H*/
        // ignore zombie line
        if(!prev_prev_br && !br){
            /*printf("found zombie \"%s\"\n", prev_ln);*/
            // lines should maybe not be zombs if middle is also \n
            // maybe just increment consec_nl and do conside ra zombie
            // oh wait that's what we do
            /*++consec_nl;*/
            zombie = 1;
            goto CONT;
            /*printf("%i\n", prev_br);*/
        }
        // ignore empty line
        if(!prev_br){
            goto CONT;
        }

        // take two first words as a name
        if(name_phase){
        /*we must stay in name phase */
            /*if(initial_prev){*/
                /*printf("in name phase with \"%s\"\n", initial_prev);*/
            /*}*/
            if(strcasestr(prev_ln, "full name")){
                /*puts("found fn");*/
                prev_ln += 10;
                prev_br -= 10;
            }
            for(int i = 0; i < prev_br; ++i){
                if(isalpha(prev_ln[i]))
                    break;
                ++prev_ln;
                --br;
                --i;
            }
            /*while(isa)*/
            /*skip non-alnum*/
            /*printf("in name phase with \"%s\"\n", prev_ln);*/
            /*need to skip over "FULL NAME"*/
            char* sp = strchr(prev_ln, ' '), * sec_sp;
            if(!sp){
                /*puts("REVERTING TO NAME PHASE for first ' '");*/
                goto CONT;
            }
            sec_sp = strchr(sp+1, ' ');
            #if 0
            if(!sp){
                puts("REVERTING TO NAME PHASE for second ' '");
                goto CONT;
            }
            #endif
            // there's a chance name is alone on a line, in which case we'll interpret the rest of the
            // line as name
            /*char* sp = strchr(strchr(prev_ln, ' ')+1, ' ');*/
            if(sec_sp)*sec_sp = 0;
            /*printf("name: \"%s\"\n", prev_ln);*/
            int nbytes = sp-prev_ln;
            strcpy(name, prev_ln);
            memcpy(note+idx, sp+1, prev_br-nbytes);
            /*printf("%i\n", idx);*/
            idx += prev_br-nbytes;
            /*printf("%i\n", idx);*/
            /*printf("%i-(%i) %i\n", prev_br, sp-prev_ln, idx);*/
            name_phase = 0;
        }
        else{
            /*printf("copying %s to note+%i\n", prev_ln, idx);*/
            /*prev_br is inaccurate*/
            memcpy(note+idx-1, prev_ln, prev_br);
            idx += prev_br;
            /*puts(note);*/
        }

        /*if(prev_br && !zombie)puts(prev_ln);*/

    CONT:
        if(consec_nl == 3){
            /*puts("4 nl");*/
            break;
        }
        prev_ln = ln;
        prev_prev_br = prev_br;
        prev_br = br;
        ln = NULL;

        /*if(zombie)continue;*/
    }
    insert_cb(cb, name, note, 1);
    free(note);
    /*memset(name, sizeof(name), 0);*/
    /*printf("%i\n", br);*/
    if(br == -1)
        return NULL;

    *initial_prev_br = br;
    return strdup(ln);

    *initial_prev_br = prev_br;
    return strdup(prev_ln);
    /*return br != -1;*/
    /*printf("name: \"%s\"\n", name);*/
    /*puts(note);*/
}

_Bool read_client(struct client_book* cb, FILE* fp){
    char c;
    /*char name_buf[1026];*/
    /*char note_buf[1026];*/
    char buf[2][1026];
    _Bool nl = 0;
    /*int num_nl = 0;*/
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

// should be printed in a box if to the screen and not to stdout
int print_clients(const struct client_book* cb, char** terms, int n_terms, FILE* fp, _Bool search_all, _Bool name_only){
    int ret = 0;
    struct client* cp;

    if(!search_all && n_terms && terms){
        for(int i = 0; i < n_terms; ++i){
            for(cp = cb->c_buckets[toupper((uint8_t)*terms[i])]; cp; cp = cp->next){
                if(strcasestr(cp->name, terms[i])){
                    print_client(cp, fp, name_only);
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
                            print_client(cp, fp, name_only);
                            /* if this client matches one term, no need to check others */
                            break;
                        }
                    }
                }
                else{
                    print_client(cp, fp, name_only);
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

void mtest(char* fn){
    struct client_book cb;
    char* prev_ln = NULL;
    int prev_br = 0;
    FILE* fp = fopen(fn, "r");
    init_cb(&cb);
    while((prev_ln = parse_client(&cb, prev_ln, &prev_br, fp))){
        /*puts("I");*/
    }
    printf("%i clients\n", cb.n_clients);
    /*
     * for(int i = 0; i < 100; ++i){
     *     parse_client(&cb, fp);
     * }
    */
    print_clients(&cb, NULL, 0, stdout, 0, 1);
    fclose(fp);
}

/*
 * should it be put in a file?
 *
 * can just have -i, -o, -f and -u flags
 * everything else is considered a filter
 *
 * output will be written to ofile
 * input read from ifile
 * -u enables updating an existing entry
 * -f provides the persistent file to edit
 *
 * persistent file will be read from on startup and appended to with -i data
 * 
 * -i inserts data from -i file
 *      
 *
 * -u enables updates
 *
 * -o outputs data to -o file
 *
 *  without any flags data will be printed to stdout
 *  
 */

/* returns NULL terminated char** */
char** parse_args(int argc, char** argv, char* flags[UINT8_MAX], int* nargs){
    char** ret = calloc(sizeof(char*), argc+1);
    char** arg = ret;

    for(int i = 1; i < argc; ++i){
        if(*argv[i] == '-'){
            if(argc > i+1){
                flags[(int)argv[i][1]] = argv[i+1];
                ++i;
            }
            else{
                return ret;
            }
        }
        else{
            *(arg++) = argv[i];
        }
    }

    *nargs = arg-ret;
    return ret;
}

int main(int argc, char** argv){
    /*mtest("clients.txt");*/
    int nargs;
    char* flags[UINT8_MAX] = {0};
    char** args = parse_args(argc, argv, flags, &nargs);
    struct client_book cb;
    FILE* outfp = stdout;
    FILE* infp = NULL;

    int prev_br;
    char* prev_ln;

    init_cb(&cb);

    if(flags['i']){
        /*puts(flags['i']);*/
        infp = fopen(flags['i'], "r");
        prev_ln = NULL;
        prev_br = 0;
        while((prev_ln = parse_client(&cb, prev_ln, &prev_br, infp)));
        /*print_clients(&cb, NULL, 0, stdout, 0, 1);*/
        fclose(infp);
    }
    
    if(flags['o']){
        outfp = fopen(flags['o'], "w");
    }

    print_clients(&cb, nargs ? args : NULL, nargs, outfp, 1, 0);
    /*print_clients(const struct client_book* cb, char** terms, int n_terms, FILE* fp, _Bool search_all, _Bool name_only){*/

    if(outfp != stdout){
        fclose(outfp);
    }

    free_cb(&cb);
}
