#include<iostream>
#include<math.h>
#include "memory.hpp"
#define int long long

class Cache{

    public:

    int total_block;  // total blocks in cache
    int word_length;  // denotes word length
    int cache_block;  // denotes cache_line
    int blocks;       // denotes cache_line/word_length
    int set;          // dentoes no of sets
    int ways;         // denotes cache_size/sets;
    int offset_bits;  // log cache_block
    int index_bits;   // log set
    int timer;        // timer for access times;
    int ***data;      // data array
    int **state;      // state array
    int **tag;        // tag array
    int **access;     // access_time array
    

    // 0->Invalid, 1->Miss Pending, 2->Valid, 3->Modified
    
    Cache(int total_block, int word_length, int cache_block, int sets){
        
        this->total_block = total_block;
        this->word_length = word_length;
        this->cache_block = cache_block;
        this->blocks = cache_block/word_length;
        this->set = sets;
        this->ways = total_block/sets;
        this->offset_bits = log(cache_block)/log(2);
        this->index_bits = log(set)/log(2);
        
        this->data = new int**[set];
        this->state = new int*[set];
        this->tag = new int*[set];
        this->access =new int*[set];
        
        for(int i=0;i<set;i++){
            this->data[i] = new int*[this->ways];
            this->state[i] = new int[this->ways];
            this->tag[i] = new int[this->ways];
            this->access[i] = new int[this->ways];
            for(int j=0;j<ways;j++)
                this->data[i][j] = new int[this->blocks];
        }

        for(int i=0;i<set;i++){
            for(int j=0;j<ways;j++){
                this->state[i][j] = 0;
                this->access[i][j] = 0;
            }
        }

    }


    int read(int mem_address, main_memory* ram){

        int mem_add = mem_address;
        int off = mem_address & ((1<<this->offset_bits) - 1);
        mem_address = mem_address >> this->offset_bits;
        int index = mem_address & ((1<<this->index_bits) - 1);
        int tag = mem_address >> this->index_bits;
        this->timer++;
        int div = this->word_length;

        for(int i=0;i<this->ways;i++){
            
            if(this->tag[index][i] == tag && this->state[index][i] == 2){
                this->access[index][i] = this->timer;
                return this->data[index][i][off/div];
            }
            
            else if(this->tag[index][i] == tag && this->state[index][i] == 3){
                this->access[index][i] = this->timer;
                return this->data[index][i][off/div];
            }

            else if(this->tag[index][i] == tag && this->state[index][i] == 1){
                this->access[index][i] = this->timer;
                return -1;
            }

        }

        int blk = allocate(mem_address, index, ram);

        return this->data[index][blk][off/div];
    
    }


    int allocate(int mem_address, int index, main_memory* ram){

        int blk = replace_algo(index);
        this->access[index][blk] = this->timer;

        if(this->state[index][blk] == 3){
            int temp = mem_address << (this->index_bits + this->offset_bits);
            for(int i=0;i<this->blocks;i++)
                ram->data[temp + i] = this->data[index][blk][i];
        }

        this->state[index][blk] = 1;
        this->tag[index][blk] = mem_address;

        mem_address = mem_address << (this->index_bits + this->offset_bits);
        for(int i=0;i<this->blocks;i++)
            this->data[index][blk][i] = ram->data[mem_address + i];
        this->state[index][blk] = 2;

        return blk;

    }


    int replace_algo(int index){

        int mini = 1e9;
        int blk = 0;

        for(int i=0;i<this->ways;i++){
            if(this->access[index][i] < mini){
                mini = this->access[index][i];
                blk = i;
            }
        }

        return blk;

    }


    int write(int mem_address, main_memory* ram, int data){

        int mem_add = mem_address;
        int off = mem_address & ((1<<this->offset_bits) - 1);
        mem_address = mem_address >> this->offset_bits;
        int index = mem_address & ((1<<this->index_bits) - 1);
        int tag = mem_address >> this->index_bits;
        this->timer++;
        int div = this->word_length;

        for(int i=0;i<this->ways;i++){
            
            if(this->tag[index][i] == tag && this->state[index][i] == 2){
                this->access[index][i] = this->timer;
                this->state[index][i] = 3;
                this->data[index][i][off/div] = data;
                return 1;
            }
            
            else if(this->tag[index][i] == tag && this->state[index][i] == 3){
                this->access[index][i] = this->timer;
                this->state[index][i] = 3;
                this->data[index][i][off/div] = data;
                return 1;
            }

            else if(this->tag[index][i] == tag && this->state[index][i] == 1){
                this->access[index][i] = this->timer;
                return -1;
            }

        }

        int blk = allocate(mem_address, index, ram);

        this->data[index][blk][off/div] = data;
        this->state[index][blk] = 3;
        return 1;
        
    }

};