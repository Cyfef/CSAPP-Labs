#include "cachelab.h"
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <stdio.h>

//cacheline结构体
typedef struct
{
    int valid;
    int tag;
    int time_stamp;
} cache_line;

//全局参数
int v=0,s=0,E=0,b=0,timestamp=0;
int hit_count=0,miss_count=0,eviction_count=0;
int t=0;


//定义缓存
typedef cache_line* set;
set* cache;

void cache_op(size_t address,int is_M)
{
    size_t set_num=(address<<t)>>(t+b);
    int tag=address>>(s+b);

    int hit=0;
    int has_avil=0,row_num=0;
    int lru=0;

    for (int i = 0; i < E; i++)
    {
        //检查cacheline
        if (cache[set_num][i].valid==1)
        {
            if (cache[set_num][i].tag==tag)
            {
                //命中
                cache[set_num][i].time_stamp=timestamp;
                hit_count++;
                hit=1;                 
                break;
            }
            else
            {
                //记录LRU替换行
                if (cache[set_num][lru].time_stamp>cache[set_num][i].time_stamp)
                {
                    lru=i;
                }
            }
        }
        else 
        {
            //有空行
            has_avil=1;
            row_num=i;
        }          
    }

    //未命中
    if (hit==0)
    {
        miss_count++;
        if (has_avil==1)
        {
            //有空行
            cache[set_num][row_num].valid=1;
            cache[set_num][row_num].tag=tag;
            cache[set_num][row_num].time_stamp=timestamp;
        }
        else 
        {
            //LRU
            cache[set_num][lru].tag=tag;
            cache[set_num][lru].time_stamp=timestamp;
            eviction_count++;
        }
    }    
    //modify    
    if (is_M)
    {
        hit_count++;        
    }
}


int main(int argc,char* argv[])
{
    int option;
    FILE* trace_file;

    while ((option = getopt(argc, argv, "vs:E:b:t:")) != -1) 
    {
        switch (option) 
        {
        case 'v':
            v = 1;
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
            trace_file = fopen(optarg, "r");
            break;
        }          
    }
    t=64-s-b;

    //初始化缓存
    cache=(set*)malloc(sizeof(set)*(1<<s));
    for (int i = 0; i < (1<<s); i++)
    {
        cache[i]=(cache_line*)malloc(sizeof(cache_line)*E);
        for (int j = 0; j < E; j++)
        {
            cache[i][j].valid=0;
            cache[i][j].tag=0;
            cache[i][j].time_stamp=0;
        }
    }
    
    char operation;
    int size;
    size_t address;

    while (fscanf(trace_file, "%s %lx,%d\n", &operation, &address, &size) == 3) 
    {
        timestamp++;
        if (v) 
        {
            printf("%c %lx,%d ", operation, address, size);
        }
        switch (operation) 
        {
        case 'I':
            continue;
        case 'M': // Modify = Load + Store
            cache_op(address, 1);
            break;
        case 'L': // Load
        case 'S': // Store
            cache_op(address, 0);
            break;
        }
    }

    for (int i = 0; i < (1<<s); i++)
    {
        free(cache[i]);        
    }
    
    free(cache);
    printSummary(hit_count,miss_count,eviction_count);
    return 0;
}