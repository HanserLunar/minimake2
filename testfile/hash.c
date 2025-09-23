#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include<unistd.h>
#include"readfile.h"
#include<fcntl.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<time.h>
#include<wait.h>
#include"hash.h"

// 哈希表的大小
#define TABLE_SIZE 256
typedef struct Hash_n //哈希节点 结构体
{
    char *key;  //键名
    char* value;//键值,本项目里应该是字符串
    struct Hash_n *next; //指向下一个节点
};

struct Hash_t
{
    struct Hash_n *list[TABLE_SIZE];//存放链表头？并不是按顺序存的
}hash_table;

/*                              节点1                                 节点2          
    hash_table.list   |  hash10{ key=?   value=? }  |       -> hash11{ key=?   value=? }
                      |  hash20{ key=?   value=? }  |
                      |  hash30{ key=?   value=? }  |
                        .....   

*/
//创建哈希节点                  
struct Hash_n* create_hash(char* key,char* value)
{
    struct Hash_n* newone=(struct Hash_n*)malloc(sizeof(struct Hash_n));
    if(newone==NULL)
        {
            printf("Hash.c:创建新节点失败\n");
            return NULL;
        }

    //赋值
    newone->key=strdup(key);//复制
    if(newone->key==NULL)
    {
        free(newone);
        printf("Hash.c:复制键名失败\n");
        return NULL;
    }
    newone->value=strdup(value);
    newone->next=NULL;
    return newone;
}

//删除哈希节点




//哈希函数，将输入转化成索引，输入的“键”（Key，如字符串、对象等）转换成一个固定范围的数字（即数组索引）
unsigned int hash_func(char* key)//DJB2
{
    unsigned int hash_value=5381;
    int c=0;
    while((c=*key++))
    {
        hash_value=(hash_value<<5)+hash_value+c;/* hash * 33 + c */
    }
    return (hash_value %TABLE_SIZE);
}

//初始化哈希表
struct Hash_t*init_hash()
{
    struct Hash_t* table=(struct Hash_t *)malloc(sizeof(struct Hash_t));
    if(table==NULL)
    {
        printf("hash.c:初始化哈希表失败\n");
        return table;
    }
    for(int i=0;i<TABLE_SIZE;i++)
        table->list[i]=NULL;
    return table;
}


//摧毁整个哈希表，从节点头开始一个一个往下销毁节点，table里存储的就是节点头的指针
void destroy_hashtable(struct Hash_t* table)
{
    struct Hash_n *current;
    for(int  i=0;i<TABLE_SIZE;i++)
    {
        current=table->list[i];
        while(current!=NULL)
        {
            struct Hash_n* temp=current;
            current=current->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(table);
}

//创建哈希节点并加入节点链表
bool add_hash_n(struct Hash_t*table,char*key,char* value)
{
    unsigned int index=hash_func(key);
    struct Hash_n*temp=table->list[index];

    while(temp!=NULL)
    {
        if(strcmp(temp->key,key)==0)//找到已经定义过的键名，仅修改键值
        {    
           strncpy(temp->value,value,sizeof(value));
            return true;
        }
        temp=temp->next;
    }
    
    //找完了都没找到相同的键名，定义个键名
    struct Hash_n* newone=create_hash(key,value);
    if(newone==NULL)
        return false;
    newone->next=table->list[index];//最新的放表头
    table->list[index]=newone;
    return true;

} 

//摧毁节点
bool destroy_hash_n(struct Hash_t * table,char *key)
{
    unsigned int index=hash_func(key);
    struct Hash_n* temp=table->list[index];
    struct Hash_n* pre=NULL;
    while(temp!=NULL)
    {
        if(strcmp(temp->key,key)==0)
        {
            if(pre)
            {
                pre->next=temp->next;
            }
            else table->list[index]->next=temp->next;
            free(temp->key);
            free(temp);
            return true;
        }
        pre=temp;
        temp=temp->next;
    }
    return false; //没找到
}


//查找键对应的数值
char* look_up_value(struct Hash_t *table,char * key)
{
    unsigned int index=hash_func(key);
    struct Hash_n* temp=table->list[index];

    while(temp!=NULL)
    {
        if(strcmp(temp->key,key)==0)
        {
            return temp->value;
        }
        temp=temp->next;
    }
    printf("hash.c:没找到键值\n");
    return 0;
}