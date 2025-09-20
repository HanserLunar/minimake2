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
#include "graph.h"

#define LINE_LENTH 256      //每行最大长度

struct data_t{
    char target[32]; //目标文件名字
    char dependency[LINE_LENTH][LINE_LENTH]; //依赖文件名字
    char command[LINE_LENTH][LINE_LENTH]; //命令 

    int dep_count; //依赖个数
    int order_count; //命令个数
}data[100]; 
 

int data_count=0;//数据个数

void divide_dependencys(char* dependency_list, char dependencies[LINE_LENTH][LINE_LENTH], int* dep_count);
bool same_target_check(char* target, struct data_t data[], int data_count);
bool file_exists(const char *filename);
bool dependency_is_target_check(char* dependency, struct data_t data[], int data_count);
int command_execute(char* command);

int main(int argc, char *argv[])
{

    bool help=false;    //帮助标志
    bool error=false;   //错误标志
    bool orderloss=false; //缺省标志
    bool verbose=false; //详细标志

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

        if(error)
    {
        printf("没有这种指令\n");
        exit(0);
    }
    else if (help)
    {
        printf("Help: This is a help message.\n");
        exit(0);
    }
    else if(verbose)
    {

    }

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
    int stage=1; //处理状态机 1-读目标 2-读命令
//读入数据
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

                        data_count++;
                        //提取目标
                        char temp[strlen(line_data)];
                        strncpy(temp, line_data, strcspn(line_data, ":"));
                        temp[strcspn(line_data, ":")] = '\0'; // 手动添加字符串结束符

                        //检查目标是否重复
                        if(same_target_check(temp, data, data_count)==true)
                        {
                            //处理错误情况
                            printf("错误：目标重复定义\n");
                            exit(1);
                        }
                        strcpy(data[data_count].target, temp);

                        printf("Targetsss: %s\n", data[data_count].target);

                        //提取依赖
                        int s=strcspn(line_data, ":");
                        char dependency_list[LINE_LENTH];
                        dependency_list[0]='\0';
                        strcpy(dependency_list, line_data + s + 2); //复制依赖部分，跳过冒号和空格
                        printf("Dependency List: %s\n", dependency_list);
                        divide_dependencys(dependency_list, data[data_count].dependency, &data[data_count].dep_count);//分割依赖
                       //检查依赖是否存在
                        for(int m=0;m < data[data_count].dep_count;m++)
                        {
                            if(file_exists(data[data_count].dependency[m])==false)
                            {
                                if(dependency_is_target_check(data[data_count].dependency[m], data, data_count)==false)
                                {
                                    printf("警告：Invalid dependency '%s'\n",data[data_count].dependency[m]);
                                    continue;
                                }
                                else    
                                    continue;
                            }
                        }
                        stage=2; //进入读命令状态
                    }
                    else if(line_data[0]=='\t')//没有找到 :,但找到\t
                    {
                        printf("Line%d:命令出现在规则前面\n",line_count);
                        //处理错误情况
                        //exit(1);
                    }
                    else
                    {
                        printf("Line%d:没有冒号\n",line_count);
                        //处理错误情况
                        exit(1);
                    }
                    break;
                case 2:                 //读命令
                    if(line_data[0]=='\t') //命令行以制表符开头
                    {
                        for(int k=0;k<strlen(line_data);k++)
                        {
                            line_data[k]=line_data[k+1]; //去掉制表符
                        }
                        printf("Order: %s\n", line_data);
                        data[data_count].order_count++;
                        strcpy(data[data_count].command[data[data_count].order_count-1], line_data);//保存命令

                    }
                    else if(strcspn(line_data, ":") < strlen(line_data)) //找到了下一个目标
                    {
                        data_count++;//进入下一条数据节点
                        //提取目标
                        char temp[strlen(line_data)];
                        strncpy(temp, line_data, strcspn(line_data, ":"));
                        temp[strcspn(line_data, ":")] = '\0'; // 手动添加字符串结束符
                        //检查目标是否重复
                        if(same_target_check(temp, data, data_count)==true)
                        {
                            //处理错误情况
                            printf("错误：目标重复定义\n");
                            exit(1);
                        }
                        strcpy(data[data_count].target, temp);
                        printf("Targetsss: %s\n", data[data_count].target);

                        //提取依赖
                        int s=strcspn(line_data, ":");
                        char dependency_list[LINE_LENTH];
                        dependency_list[0]='\0';
                        strcpy(dependency_list, line_data + s + 2); //复制依赖部分，跳过冒号和空格
                        printf("Dependency List: %s\n", dependency_list);
                        divide_dependencys(dependency_list, data[data_count].dependency, &data[data_count].dep_count);//分割依赖
                        //检查依赖是否存在
                        for(int m=0;m < data[data_count].dep_count;m++)
                        {
                            if(file_exists(data[data_count].dependency[m])==false)
                            {
                                if(dependency_is_target_check(data[data_count].dependency[m], data, data_count)==false)
                                {
                                    printf("警告：Invalid dependency '%s'\n",data[data_count].dependency[m]);
                                    continue;
                                }
                                else    
                                    continue;
                            }
                        }
                        stage=2; //继续读命令状态
                    }
                    else
                    {
                        printf("Line%d:没有TAB起头\n",line_count);
                        //处理错误情况
                        exit(1);
                    }
                    break;
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
        for(int i=1;i<data_count+1;i++)
        {
            printf("第%d个\ndep_count=%d\norder_count=%d\n\n",i,data[i].dep_count,data[i].order_count);
        }


//执行命令
        for(int i=1;i<data_count+1;i++)
        {
                if(data[i].order_count == 0)
                {
                    printf("错误：没有命令来构建目标%s\n", data[i].target);
                    exit(1);
                }
                for(int k=0;k < data[i].order_count;k++)
                {
                    int ret = command_execute(data[i].command[k]);
                    if(ret == -1)
                    {
                        printf("命令执行失败: %s\n", data[i].command[k]);
                        exit(1);
                    }
                    else if(ret != 0)
                    {
                        printf("命令正常退出 (%d): %s\n", ret, data[i].command[k]);
                    }
                }
        }       
        

            
    return 0;
}





//输入是依赖串   输出是一个个分离的依赖
void divide_dependencys(char* dependency_list, char dependencies[LINE_LENTH][LINE_LENTH], int* dep_count)
{
    int s=strcspn(dependency_list, " ");
    if(s < strlen(dependency_list)) //找到了空格
    {
        strncpy(dependencies[*dep_count], dependency_list, s);
        dependencies[*dep_count][s] = '\0'; // 手动添加字符串结束符
        printf("Dependency: %s\n", dependencies[*dep_count]);
        (*dep_count)++;
        divide_dependencys(dependency_list + s + 1, dependencies, dep_count);
        return ; //递归处理剩余部分
    }
    else if(strlen(dependency_list) > 0) //没有找到空格，但还有剩余部分
    {
        strncpy(dependencies[*dep_count], dependency_list, strlen(dependency_list));
        dependencies[*dep_count][strlen(dependency_list)] = '\0'; // 手动添加字符串结束符
        printf("Dependency: %s\n", dependencies[*dep_count]);
        (*dep_count)++;
        return; //处理完毕
    }
    else
    {
        return ; //没有更多依赖
    }
    return ;
}


//检查目标是否重复
bool same_target_check(char* target, struct data_t data[], int data_count)
{
    for(int m=0;m < data_count;m++)
    {
        if(strcmp(data[m].target, target) == 0)
        {
            return true;
        }
    }
    return false;
}

bool file_exists(const char *filename) {
    return access(filename, F_OK) != -1;
}

//检查涉及的依赖是否是以前定义过的目标
//如果是，返回true，否则返回false
bool dependency_is_target_check(char* dependency, struct data_t data[], int data_count)
{
    for(int m=0;m < data_count;m++)
    {
        if(strcmp(data[m].target, dependency) == 0)
        {
            return true;    
        }
    }
    return false;
}

//执行命令，返回值-1表示执行失败（调用sys失败或命令未正常退出），其他值表示命令调用sys成功且正常推出
int command_execute(char* command)
{
    int status = system(command);
    if (status == -1) {
        // 处理 system 调用失败的情况
        return -1;
    } 
    else 
    {
        // 获取命令的退出状态
        if (WIFEXITED(status))
        {
            return WEXITSTATUS(status);
        } 
        else 
        {
            // 命令未正常退出
            return -1;
        }
    }
}