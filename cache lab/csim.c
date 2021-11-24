//2019017410 Moon MyeongKyun

#include "cachelab.h"
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

int hits = 0, misses = 0, evictions = 0;
int vFlag=0, s=0, E=0, b=0;
int time = 0;
FILE *inputf;

typedef struct _Line {
    int v;
    unsigned long long tag;
    int lru;
} Line;

typedef struct _Set {
    Line *lines;
} Set;

typedef struct _Cache {
    Set *sets;
} Cache;

Cache cache;

void docache(unsigned long long addr){
    unsigned long long tag = addr >> (s+b);
    int sBits = (addr >> b) - (tag << s);
    Set *tempset = &cache.sets[sBits];
    time++;

    // hit part
    for(int i=0; i<E; i++){
        Line *templine = &tempset->lines[i];
        if((templine->tag == tag)&&(templine->v)){
            if(vFlag) {
            	printf("hit ");
			}
            hits++;
            templine->lru = time;
            return;
        }
    }

    // miss part
    if(vFlag){
    	printf("miss ");
	}
    misses++;
    for(int i=0; i<E; i++){
        Line *templine = &tempset->lines[i];
        if(!(templine->v)){
            templine->v = 1;
            templine->tag = tag;
            templine->lru = time;
            return;
        }
    }

    // eviction part
    if(vFlag){
    	printf("eviction ");
	}
    evictions++;
    int oldidx=0, max=0;
    for(int i=0; i<E; i++){
        Line *findLine = &tempset->lines[i];
        if(time-findLine->lru >= max){
            max = time-findLine->lru;
            oldidx = i;
        }
    }
    Line *templine = &tempset->lines[oldidx];
    templine->v = 1;
    templine->tag = tag;
    templine->lru = time;
    return;
}

int main(int argc, char *argv[]) {
    
    int opt;
    while((opt = getopt(argc, argv, "vs:E:b:t:")) != -1){
        switch(opt){
        case 'v':
            vFlag = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            inputf = fopen(optarg, "r");
            break;
        default:
            return 0;
        }
    }
    
    // 2. make cache
    cache.sets = malloc(sizeof(Set) * (1<<s));
    for(int i=0; i<(1<<s); i++){
        cache.sets[i].lines = malloc(sizeof(Line) * E);
        for(int j=0; j<E; j++){
            cache.sets[i].lines[j].v = 0;
            cache.sets[i].lines[j].tag = 0;
            cache.sets[i].lines[j].lru = 0;
        }
    }

    // 3. read file
    char cmd, comma;
    unsigned long long addr;
    int size;
    while(fscanf(inputf, " %c %llx%c%d", &cmd, &addr, &comma, &size) != EOF){
        if(cmd == 'L'){
            if(vFlag) printf("L %llx,%d ", addr, size);
            docache(addr);
        }
        else if(cmd == 'S'){
            if(vFlag) printf("S %llx,%d ", addr, size);
            docache(addr);
        }
        else if(cmd == 'M'){
            if(vFlag) printf("M %llx,%d ", addr, size);
            docache(addr);
            docache(addr);
        }
        if(vFlag) printf("\n");
    }

    // 4. print
    printSummary(hits, misses, evictions);

    // 5. free
    fclose(inputf);
    for(int i=0; i<(1<<s); i++) free(cache.sets[i].lines);
    free(cache.sets);
    return 0;
}



