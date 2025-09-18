#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>

int main(int argc, char *argv[])
{

    bool help=false;    //帮助标志
    bool error=false;   //错误标志
    bool verbose=false;

    printf("%d.\n",argc);
    
    for (int j=0;j < argc;j++)
    {
        printf("%s.\n",argv[j]);
    }

//处理接受的命令行参数
    int    i;
    if(argc < 2) //没有参数
    {
        printf("No arguments.\n");
        exit(0);
    }

    for( i=1;i<argc;i++) 
    {
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) 
        {
            help=true;
        }
        else 
        {
            error=true;
        }

    }    

//根据不同的标志位，执行不同的操作
    if(error)
    {
        printf("Error.\n");
        exit(0);
    }
    else if (help)
    {
        printf("Help: This is a help message.\n");
        exit(0);
    }


    return 0;
}