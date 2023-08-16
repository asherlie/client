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

void print_client(const struct client* c, FILE* fp){
    fprintf(fp, "%s\n  %s\n\n", c->name, c->notes);
}

/*
 * ignore \nline\n
 * ignore leading non-ascii chars
 * separate by multi-\n
 * first (two?) ascii words
*/

_Bool parse_client(struct client_book* cb, FILE* fp){
    char* prev_ln = NULL;
    char* ln;
    size_t sz;
    ssize_t br, prev_br = 0, prev_prev_br = 0;

    int idx = 0;
    char name[80];
    char note[9048];

    // need to:
    //  consider first two words a name
    //  ignore leading whitespace before a name
    //  ignore lone lines
    //
    //  stop parse-ing after multiple \n\n are found
    //      IF not a lone line AND 
    //
    
    _Bool zombie;
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
            char* sp = strchr(strchr(prev_ln, ' ')+1, ' ');
            *sp = 0;
            /*printf("name: \"%s\"\n", prev_ln);*/
            int nbytes = sp-prev_ln;
            strcpy(name, prev_ln);
            memcpy(note+idx, sp+1, prev_br-nbytes);
            /*printf("%i\n", idx);*/
            idx += prev_br-nbytes;
            printf("%i\n", idx);
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
        prev_ln = ln;
        prev_prev_br = prev_br;
        prev_br = br;
        ln = NULL;

        /*if(zombie)continue;*/
    }
    printf("name: \"%s\"\n", name);
    puts(note);
}

_Bool parse_client_exp(struct client_book* cb, FILE* fp){
    char c, prev;
    char name[80];
    char note[9048];
    int running_nl = 1;
    int running_txt = 0;
    int n_words = 0;
    int running_nonalph = 0;
    int running_word = 0;
    _Bool n_phase = 1;
    _Bool alnum;
    int idx = 0;
    _Bool lone_line;
    _Bool possible_single_line = 1;

    int count = 0;

    while((c = fgetc(fp)) != EOF){
        /*printf("%c\n", c);*/
        // this is catching a lot
        // alnum is fine as long as it's not at beginning
        // we need a \r
        #if 0
        alnum = isalnum(c);
        if(alnum)running_nonalph = 0;
        else{
            ++running_nonalph;
            /*++running_nl;*/
            /*if(n_phase && running_nonalph > 1 && !(running_word || running_txt))*/
            if(n_phase && running_nonalph && !(running_word || running_txt))
                continue;
        }
        #endif
        /*lone_line = (!idx || running_nl) && */
        /* discard leading nonalnum chars */
        if(idx == 0 && n_phase && !isalpha(c)){
            puts("discarding nonalph");
            continue;
        }
        switch(c){
            case '\r':
            case '\n':
                if(running_txt){
                    running_nl = 0;
                }
                puts("FOUND NL");
                // discard single lines of text surrounded by \n
                if(running_txt == 1){
                    puts("running_txt == 1, discarding");
                    // is this adequate to discard?
                    /*(n_phase ? name : note)[idx] = 0;*/
                    n_phase = 1;
                    /*printf("\"%s\"\n", n_phase ? name : note);*/
                    idx = 0;
                }
                ++running_nl;
                printf("++running_nl -> %i\n", running_nl);
                if(running_nl > 1){
                    running_txt = 0;
                    printf("running_nl > 1, running_txt = 0\n");
                    // if we've had consecutive \n then we're back on name
                    if(!n_phase){
                        printf("finished parsing client number %i\n", ++count);
                        puts("setting idx to 0 and switching to name phase");
                        note[idx] = 0;
                        /*printf("note: \"%s\"\n", note);*/
                        idx = 0;
                        n_phase = 1;
                        n_words = 0;
                    }
                }
                // lone line not working, 11/_ is being considered a name
                printf("poss single line? %i\n", possible_single_line);
                if(possible_single_line){
                    lone_line = 1;
                    (n_phase ? name : note)[idx] = 0;
                    idx = 0;
                    printf("found a lone line: %s\n", n_phase ? name : note);
                    possible_single_line = 0;
                    /*possible_single_line*/
                    /*continue;*/
                }
                // need to somehow increment running_txt if nl after a line of text
                // what about lone line?
                if(running_nl == 1){
                    ++running_txt;
                }
                break;
            case ' ':
                if(running_word){
                    ++n_words;
                }
                else{
                    puts("skipping whitespace");
                    continue;
                }
                /* consider this a name */
                printf("found ' ', n_words == %i\n", n_words);
                if(n_words == 2 && n_phase){
                    name[idx] = 0;
                    printf("name: \"%s\"\n", name);
                    /*
                     * okay, name is valid - i see why
                     * we need to lookahead and see if we're on a lone line
                     * if so, discard recent name
                     *
                     * although we're essentially doing that
                    */

                    idx = 0;
                    n_phase = 0;
                    n_words = 0;
                    continue;
                }
                /*break;*/
            default:
                if(running_nl){
                    possible_single_line = 1;
                }
                // aha, this disqualifies single lines before the line is even over
                // this shouldn't be set to 0 until we get our new \n
                // if newline and running_word, then we know 
                // if newline and running_text
                /*else possible_single_line = 0;*/
                ++running_word;
                /*running_nl = 0;*/
                // need to set both running_nl and possible_single_line to 0 when appropriate
                // i'll only know at the end of a line
                if(n_phase){
                    name[idx++] = c;
                }
                else{
                    note[idx++] = c;
                }
        }
        prev = c;
    }
    return 0;
}

_Bool read_client(struct client_book* cb, FILE* fp){
    char c;
    /*char name_buf[1026];*/
    /*char note_buf[1026];*/
    char buf[2][1026];
    _Bool nl = 0;
    int num_nl = 0;
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

void mtest(char* fn){
    struct client_book cb;
    FILE* fp = fopen(fn, "r");
    init_cb(&cb);
    parse_client(&cb, fp);
    print_clients(&cb, NULL, 0, stdout, 0);
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
char** parse_args(int argc, char** argv, char** flags){
    char** ret = calloc(sizeof(char*), argc+1);
    char** arg = ret;

    for(int i = 1; i < argc; ++i){
        if(*argv[i] == '-'){
            if(argc > i+1){
                flags[argv[i][1]-'a'] = argv[i+1];
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

    return ret;
}

int main(int argc, char** argv){
    mtest("t.txt");
    return 0;

    struct client_book cb;
    FILE* fp = stdout;
    char* flags[26] = {0};

    char** args = parse_args(argc, argv, flags);

    puts("FLAGS");
    for(int i = 0; i < 26; ++i){
        if(flags[i])printf("  -%c: \"%s\"\n", 'a'+i, flags[i]);
    }
    puts("ARGS");
    for(char** i = args; *i; ++i){
        printf("  %s\n", *i);
    }
    exit(0);

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
