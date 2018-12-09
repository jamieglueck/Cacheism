#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

char memory[1048576 * 16];
struct frame{
	int tag;
	int valid;
	int data;
};

int main(int argc, char* argv[]){
	FILE *ftpr;
	ftpr = fopen(argv[1],"r");
	//reading in cache size, associativity, and block size
	int cache_size = atoi(argv[2]);
	int ways = atoi(argv[3]);
	int block_size = atoi(argv[4]);
	//now i convert cachesize from KB to bytes
	cache_size = cache_size*1024;
	int sets = (cache_size/block_size)/(ways);
	//calculate size of block offset
	int boffset_size = log2(block_size);
	//calculate set index size
	int sindex_size = log2(sets);

	//now i make a 2d array of my chache setsXways
	struct frame** cache = (struct frame**)(malloc(sets*sizeof(struct frame*)));
	for(int x = 0; x < sets; x++){
		cache[x] = (struct frame*)(malloc(ways*sizeof(struct frame)));
		for(int i = 0; i<ways;i++){
			//valid is one if something is there, tag is the unique block id, data is a count for LRU
			//for every instruction, .data is increased for every block, but the .data for the block being accessed
			//is set to 0
			cache[x][i].tag = -1;
			cache[x][i].valid = 0;
			cache[x][i].data = 0;
		}
	}
	
	char LoS[24];
	int address;
	//int addressc = address;
	int asize;
	char val;
	char HoM[24];
	char Lval;
	
	//int lowest = 0;
	//read in Load or store, address, size of access, and val to be written (if store)
	while(fscanf(ftpr, "%s",LoS) != EOF){
	strcpy(HoM,"miss");
	//fscanf(ftpr,"%s",LoS);
	fscanf(ftpr,"%x",&address);
	fscanf(ftpr,"%d",&asize);
	//find the tag
	int tag = address>>(boffset_size + sindex_size);
	//now i find the value of block offset
	int mask = (1 << boffset_size) - 1;
	int block_offset = address&mask;
	//find set index
	mask = (1 << (boffset_size +sindex_size)) - 1;
	int set_index = (address&mask)>>boffset_size;

	if(strcmp(LoS,"store") == 0){
		for(int x=0;x<asize;x++){
			fscanf(ftpr,"%02hhx",&val);
			memory[address + x] = val;
		}
		//go through cache and look for block, if found, its a hit
		for(int x = 0; x<ways;x++){
			cache[set_index][x].data += 1;
			if(cache[set_index][x].valid == 1 &&(cache[set_index][x].tag == tag)){
				strcpy(HoM,"hit");
				cache[set_index][x].data = 0;
			
			}
		}
		printf("%s ",LoS);
		printf("%s","0x");
		printf("%x ",address);
		printf("%s",HoM);
		printf("\n");
	}
	
	if(strcmp(LoS,"load") == 0){
		//go through cache and look for block
		for(int x = 0; x<ways;x++){
			cache[set_index][x].data +=1;
			if(cache[set_index][x].valid == 1 &&(cache[set_index][x].tag == tag)){
				strcpy(HoM,"hit");
				cache[set_index][x].data = 0;
			}
		}
		if((strcmp(HoM,"miss") == 0)){
			int LRU = 0;
			int val = 0;
			for(int x = 0; x<ways;x++){
				if(cache[set_index][x].valid == 0){
					LRU = x;
					break;
				}
				//least recently used will have the highest data val
				if(cache[set_index][x].data > val){
					LRU = x;
					val = cache[set_index][x].data;
				}
			}
			cache[set_index][LRU].tag = tag;
			cache[set_index][LRU].valid = 1;
			cache[set_index][LRU].data = 0;
		}
			
			//if(strcmp(HoM,"miss") == 0){
				//cache[set_index][0] = address;
			//}
		
		printf("%s ",LoS);
		printf("%s","0x");
		printf("%x ",address);
		printf("%s ",HoM);
	for(int x=0;x<asize;x++){
			Lval = memory[address + x];
			printf("%02hhx",Lval);
		}
		
		//printf("%x",Lval);
		printf("\n");
	}
	
}

//free(memory);
fclose(ftpr);
return EXIT_SUCCESS;
}
