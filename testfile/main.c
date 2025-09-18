#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include"readfile.h"
#define LINE_LENTH 256      //每行最大长度

int main(int argc, char *argv[])
{

    bool help=false;    //帮助标志
    bool error=false;   //错误标志
    bool orderloss=false; //缺省标志
    bool verbose=false;

    FILE *fp_source,*fp_target;//文件指针
    char line_data[LINE_LENTH];//读出的数据
    int line_count=0;//行数

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
        //exit(0);
    }
    else    //有参数,读取参数
    {
     for( i=1;i<argc;i++) 
      {
        
        if(strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) 
        {
            help=true;
        }
        else if (strcmp(argv[i], "--hel") == 0 || strcmp(argv[i], "-hep") == 0|| strcmp(argv[i], "-hlp") == 0|| strcmp(argv[i], "-hel") == 0)
        {
            printf("错误指令\n");
            printf("你是说 --help 吗?\n");
            help=false;
        }
        else if(strcmp(argv[i], "--verbose") == 0 || strcmp(argv[i], "-v") == 0) 
        {
            verbose=true;
        }
        else if (strcmp(argv[i], "--verbos") == 0 || strcmp(argv[i], "-veb") == 0|| strcmp(argv[i], "-vbe") == 0|| strcmp(argv[i], "-verb") == 0|| strcmp(argv[i], "--vetb") == 0|| strcmp(argv[i], "--vetbse") == 0)
        {
            printf("错误指令\n");
            printf("你是说 --verbose 吗?\n");
            verbose=false;
        }
        else 
        {
            error=true;
        }

     }    
    }
//根据不同的标志位，执行不同的操作

//预处理
    fp_source=fopen("file_test_makefile","r");
    fp_target=fopen("file_test_out.mk","w");
    if(fp_source==NULL)
    {
        fp_source=fopen("makefile","r");
        if(fp_source==NULL)
       { 
        printf("Cannot open file.\n");
        exit(0);
       }
    }
    if(fp_target==NULL)
    {
        fp_target=fopen("Minimake_cleared.mk","w");
        if(fp_target==NULL)
        {
        printf("Cannot open target file.\n");
        exit(0);
        }
    } 
    int stage=1; //处理状态机 
    char target[LINE_LENTH]; //目标文件名字

     while(fgets(line_data,LINE_LENTH,fp_source)!=NULL)
        {   
            line_data[strcspn(line_data, "\n")] = 0; //去掉行尾换行符
            if(strlen(line_data)==0) //过滤空行
                continue;

            char *comment_pos = strchr(line_data, '#'); //查找注释符号
            if (comment_pos != NULL) {
                *comment_pos = '\0'; //去除注释，将#及其后的内容设为字符串结束符
            }

            //去除行尾空格
            int end = strlen(line_data) - 1;
            while (end >= 0 && (line_data[end] == ' ' || line_data[end] == '\t')) {
                line_data[end] = '\0';
                end--;
            }

            if(strlen(line_data)==0) //去除注释后的空行
                continue;
            
            line_count++;

            //根据读到的数据进行不同的操作
            printf("%d: %s\n",line_count,line_data);
            switch(stage)
            {
                case 1:                 //读目标
                    if(strcspn(line_data, ":")< strlen(line_data)) //找到了 :
                    {
                        strncpy(target, line_data, strcspn(line_data, ":"));
                        target[strcspn(line_data, ":")] = '\0'; // 手动添加字符串结束符
                        printf("Target: %s\n", target);
                        stage=2; //进入读依赖状态
                    }
                    else if(line_data[0]=='\t')//没有找到 :,但找到\t
                    {
                        printf("Line%d:命令出现在规则前面\n",line_count);
                        //处理错误情况
                        exit(1);
                    }
                    else
                    {
                        printf("Line%d:没有冒号\n",line_count);
                        //处理错误情况
                        exit(1);
                    }
                    break;
                case 2:                 //读依赖
                    if(line_data[0]=='\t') //依赖行以制表符开头
                    {
                        printf("Dependency: %s\n", line_data);
                        stage=3; //进入读命令状态
                    }
                    else if(strcspn(line_data, ":") < strlen(line_data)) //找到了下一个目标
                    {
                        stage=1; //重新进入读目标状态
                        int s=strcspn(line_data, ":");
                        strncpy(target, line_data, s);
                        target[s] = '\0'; // 手动添加字符串结束符
                        printf("Target: %s\n", target);
                    }
                    else
                    {
                        printf("Line%d:Expected a dependency line or a new target definition\n",line_count);
                        //处理错误情况
                        exit(1);
                    }
                    break;
                case 3:                 //读命令
                default:
                    break;
            }


            //保存到目标文件
            if(verbose==true)
            {
                fputs(line_data,fp_target);//写入目标文件
                fputs("\n",fp_target);//写入换行符
            }
        }
        fclose(fp_source);
        fclose(fp_target);



    if(error)
    {
        printf("没有这种指令\n");
    }
    else if (help)
    {
        printf("Help: This is a help message.\n");
    }
    else if(verbose)
    {

    }
    return 0;
}