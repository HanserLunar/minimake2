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

#include "graph.h"

#define LINE_LENTH 256      //每行最大长度
#define MAXVEX 256

struct data_t{
    char target[LINE_LENTH]; //目标文件名字
    char dependency[LINE_LENTH][LINE_LENTH]; //依赖文件名字
    char command[LINE_LENTH][LINE_LENTH]; //命令 

    int dep_count; //依赖个数
    int order_count; //命令个数
    bool construct_flag; //是否需要构建目标
}data[100]; 


struct graph{
    char vexs[MAXVEX][MAXVEX];        //顶点表,每个顶点是一串字符
    int indegree[MAXVEX];//顶点的入度
    int outdegree[MAXVEX];//顶点的出度
    int arc[MAXVEX][MAXVEX];  //邻接矩阵
    int numVertexes,numEdges; //图中当前的顶点数和边数
    struct stat fileinfo[MAXVEX]; //存储每个顶点的文件信息
};

int data_count=0;//data个数
int line[LINE_LENTH]={0};//存放入度为0的顶点的索引
int line_count=0;   //入度为0的顶点的数目


void divide_dependencys(char* dependency_list, char dependencies[LINE_LENTH][LINE_LENTH], int* dep_count);
void divide_command(char* dependency_list, char dependencies[LINE_LENTH][LINE_LENTH], int* dep_count);
bool same_target_check(char* target, struct data_t data[], int data_count);
bool file_exists(const char *filename);
bool dependency_is_target_check(char* dependency, struct data_t data[], int data_count);
int command_execute(char* command);
bool Kahn(struct graph *G);
void visit(struct graph*G ,int v);
void get_file_message(struct graph *GG);
bool which_file_fresh(char* file_A,char* file_B);


struct graph* createGraph();
bool addEdge(struct graph* G,int src,int dest);
bool addVertexs(struct graph* G,char* name);
void destroy_Graph(struct graph* G);
void BFS(struct graph* G,int value);
void DFSs(struct graph* G,int v,bool visited[]);



int main(int argc, char *argv[])
{

    bool help=false;    //帮助标志
    bool error=false;   //错误标志
    //bool orderloss=false; //缺省标志
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
                            if(file_exists(data[data_count].dependency[m])==false)//1不是某个已存在的文件
                            {
                                if(dependency_is_target_check(data[data_count].dependency[m], data, data_count)==false)//2也不是将要构造的文件
                                {
                                    printf("警告：Invalid dependency '%s'\n",data[data_count].dependency[m]);//3则该依赖不合法
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


        //构建图
        struct graph* G=createGraph();
        //添加目标和依赖为顶点        
        bool exist=false;//该依赖是否已经作为顶点存在
        bool found=false;//该目标是否已经作为顶点存在
        int src=-1;//依赖索引
        int dest=-1;//目标索引  src -> dest
        for(int i=1;i<data_count+1;i++)
        { 
            found=false;
            exist=false;

            for(int j=0;j<G->numVertexes;j++)
                {
                    if(strncmp(data[i].target,G->vexs[j],LINE_LENTH)==0)
                    {
                        found=true;//该目标已经是顶点了
                        dest=j;
                        printf("dest:%d\n",dest);
                        break;
                    }
                }
                if(!found)
                {            
                    addVertexs(G,data[i].target);
                    printf("grapss:%s\n",data[i].target);
                    dest=G->numVertexes-1;
                    printf("dest:%d\n",dest);
                }                             
            
            for(int q=0;q<data[i].dep_count;q++)
           {
                
                for(int j=0;j<G->numVertexes;j++)
                {
                    if(strncmp(data[i].dependency[q],G->vexs[j],LINE_LENTH)==0)
                    {
                        exist=true;//该依赖已经是顶点了
                        src=j;
                        addEdge(G,src,dest);
                        printf("src:%d\n",src);
                        break;
                    }
                }
                if(!exist)
                {   

                    addVertexs(G,data[i].dependency[q]);
                    printf("grapss:%s\n",data[i].dependency[q]);
                    src=G->numVertexes-1;
                    printf("src:%d\n",src);
                    addEdge(G,src,dest);
                }         
           } 
           
        }
        //获取每个顶点的文件的时间戳，存入fileinfo数组
        get_file_message(G);
        printf("///\n///\n///\n");
        for(int i=1;i<data_count+1;i++)
        {
            if(file_exists(data[i].target))
            {
                printf("HI\n");
                for(int j=0;j<data[i].dep_count;j++)
                {
                    if(which_file_fresh(data[i].target , data[i].dependency[j])==false)//前者比后者新则返回真，反之假
                    {
                        printf("I arrived here\n");
                        data[i].construct_flag=true;//依赖有更新的，需要重新构建目标
                    }
                }
            }
            else
            {
                printf("NO,I am here\n");
                data[i].construct_flag=true;
                for(int j=0;j<data[i].dep_count;j++)
                {
                    if(!file_exists(data[i].dependency[j]))
                    {
                        printf("警告：Invalid dependency '%s'\n",data[i].dependency[j]);
                        data[i].construct_flag=false;
                        break;
                    }
                }
                
            }
        }

     //执行命令    
        for(int i=1;i<data_count+1;i++)
        {
   
            if(data[i].order_count == 0)
            {
                    printf("错误：没有命令来构建目标%s\n", data[i].target);
                   // exit(1);
            }
            if(data[i].construct_flag==false)//没有必要更新
                continue;            
            for(int k=0;k < data[i].order_count;k++)
            {
                printf("重构了\n");
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
        




    //打印图
        printf("图的顶点数: %d\n", G->numVertexes);
        printf("图的边数: %d\n", G->numEdges);
        printf("图的邻接矩阵:\n");
        for(int i=0;i<G->numVertexes;i++)
        {
            printf("顶点 %s  ", G->vexs[i]);
            for(int j=0;j<G->numVertexes;j++)
            {
                printf("%d ",G->arc[i][j]);
            }
            printf("\n");
        }
        for(int i=0;i<G->numVertexes;i++)
        {
            printf("顶点 %s 的出度: %d\n", G->vexs[i], G->outdegree[i]);
            printf("顶点 %s 的入度: %d\n", G->vexs[i], G->indegree[i]);
        }
    printf("\n\n");
    int s=Kahn(G);
        if(!s)
        {
            printf("////////////////////////////////////////////////////////////////////\n\t\t\t这里面应该有个环\n////////////////////////////////////////////////////////////////////\n");
        }
        //销毁图
    destroy_Graph(G);
    return 0;
}





//输入是依赖串   输出是一个个分离的依赖                                                         //输出有多少个依赖
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
void divide_command(char* dependency_list, char dependencies[LINE_LENTH][LINE_LENTH], int* dep_count)
{
    int s=strcspn(dependency_list, " ");
    if(s < strlen(dependency_list)) //找到了空格
    {
        strncpy(dependencies[*dep_count], dependency_list, s);
        dependencies[*dep_count][s] = '\0'; // 手动添加字符串结束符
        printf("Command1: %s\n", dependencies[*dep_count]);
        (*dep_count)++;
        divide_command(dependency_list + s + 1, dependencies, dep_count);
        return ; //递归处理剩余部分
    }
    else if(strlen(dependency_list) > 0) //没有找到空格，但还有剩余部分
    {
        strncpy(dependencies[*dep_count], dependency_list, strlen(dependency_list));
        dependencies[*dep_count][strlen(dependency_list)] = '\0'; // 手动添加字符串结束符
        printf("Command2: %s\n", dependencies[*dep_count]);
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
    printf("command execute:命令是：%s\n",command);

    //预处理，把整行命令拆成小块
    char divided_command[LINE_LENTH][LINE_LENTH];
    char *div_com[LINE_LENTH];
    memset(div_com,NULL,sizeof(div_com));
    int num=0;
    divide_command(command,divided_command,&num);
    for(int i=0;i<num;i++)
    {
        div_com[i]=divided_command[i];
        printf("ssss:%s\n",div_com[i]);
    }
    div_com[num]=NULL;
    
    pid_t pid=fork();       //创建子进程
    if(pid==0)
    {
        printf("command_execute:子程序运行中...\n");
        execvp("gcc",div_com);

        printf("command_execute:子程序错误\n");
        return -1;
        exit(1);
    }
    else
    {
        int status=0;
        pid_t result;
        result=waitpid(pid,&status,0);
        if(result==-1)
        {
            printf("command execute:子程序执行失败\n");
        }
        else if(result==0)
        {   
            printf("command execute:子程序进行中\n");
        }
        else
        {
            printf("command execute:子程序结束\n");
            if (WIFEXITED(status)) 
            {
                printf("命令执行完成，退出状态: %d\n", WEXITSTATUS(status));
                return 1;
            } 
            else 
            {
                printf("命令异常终止\n");
                return -1;
            }
        }   
    }

}
/*   
    int status = system(command);
    if (status == -1) 
    {
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
*/
///////////////////////////////////////////////////////////////////////////////////////////////


//创建图
struct graph* createGraph( )
{

    struct graph* G=(struct graph*)malloc(sizeof(struct graph));
    G->numEdges=0;
    G->numVertexes=0;
    memset(G->indegree,0,sizeof(G->indegree));
    memset(G->outdegree,0,sizeof(G->outdegree));
    memset(G->vexs,0,sizeof(G->vexs));
    for(int i=0;i<MAXVEX;i++)
    {
        for(int j=0;j<MAXVEX;j++)
        {
            G->arc[i][j]=0;
        }
    }
    return G;
}

//添加边,我认为是有向图
bool addEdge(struct graph* G,int src,int dest)//G->vex[src] -> G->vex[dest]
{
    if( src <0|| src >=G->numVertexes||dest<0||dest>=G->numVertexes)
    {   
        printf("graph.c:边的顶点值不合法\n");
        return false;
    }
    G->arc[src][dest]=1;//没有权重
    G->numEdges++;
    G->outdegree[src]++;
    G->indegree[dest]++;
    return true;
}

//添加顶点
bool addVertexs(struct graph* G,char*name)
{
    if(G->numVertexes>=MAXVEX)
    {
        printf("graph.c:顶点数达到上限\n");
        return false;
    }
    strcpy(G->vexs[G->numVertexes],name);
    G->numVertexes++;
    return true;    
}

//销毁图
void destroy_Graph(struct graph* G)
{
    free(G);
}

//BFS                   //出发顶点下标 graph->vexs[v]
void BFS(struct graph* G,int v)
{
    bool visited[MAXVEX]; //访问标志
    memset(visited,false,sizeof(visited));//初始化为未访问

    visited[v]=true;
    printf("%s ",G->vexs[v]);

    int queue[MAXVEX];//队列，队列是指存放顶点下标的
    int front=0,rear=0; //队头和队尾指针
    queue[rear++]=v; //入队列
    while(front!=rear) //队列不为空
    {
        int u=queue[front++]; //出队列
        for(int i=0;i<G->numVertexes;i++)
        {
            if(G->arc[u][i]==1 && visited[i]==false) //固定行，找列中为1的即是有边且未访问
            {
                visited[i]=true;
                printf("%s ",G->vexs[i]);
                queue[rear++]=i; //把找到的未访问的顶点加入队列

                //对该顶点进行特定的操作
                /* 例如：可以在这里对每个访问到的顶点进行计数或其他处理 */
                //这里可以添加对每个访问到的顶点的具体操作  
            }
        }
        //之后从队列中取出下一个顶点u，继续上面的过程，直到队列为空
    }
    printf("\n");
}


//DFS 可以用递归实现                                
void DFSs(struct graph* G,int v,bool visited[])
{
    visited[v]=true;//访问标记，防止重复访问
    printf("asdAdasss:%s\n ",G->vexs[v]);
    //对该顶点进行特定的操作
    /* 例如：可以在这里对每个访问到的顶点进行计数或其他处理 */
    //这里可以添加对每个访问到的顶点的具体操作
    visit(G,v);
    for(int i=0;i<G->numVertexes;i++)   //固定行，找列中为1的即是有边且未访问
    {
        if(G->arc[v][i]==1 && visited[i]==false)//搜索下一个顶点
        {
            DFSs(G,i,visited);
        }
    }
}

bool Kahn(struct graph*G) //返回true表示没有循环结构,返回false表式有循环
{

    bool visited[LINE_LENTH];
    bool flag=false;//标志这回合有没有可以减的入度，没有就返回！flag
    memset(visited,false,sizeof(visited));
    printf("Kahn\n");
//    DFSs(G,3,visited);
//    for(int i=0;i<line_count;i++)
//       printf("%d\n",line[i]);
    struct graph* GG=G; 
    for(int i=0;i<GG->numVertexes;i++)//找入度为0的顶点
    {
        if(GG->indegree[i]==0)
        {
            printf("Kahn indegree=0:%s\n",GG->vexs[i]);
            line[line_count++]=i;
            GG->indegree[i]=-1;
            printf("line:%d\n",i);
        }
    }
    printf("line_count:%d\n\n",line_count);
    while((line_count)!=0)
    {
        for(int i=0;i<GG->numVertexes;i++)
        {
            if(GG->arc[line[(line_count)-1]][i]==1&&GG->indegree[i]>0)
            {
                flag=true;
                GG->indegree[i]-=1;
                printf("name:%s\nindegree-1=%d\n",GG->vexs[i],GG->indegree[i]); 
            }
        }
        line_count--;
    }
    printf("I am here\n\n");

    if(flag==false)
    {
        for(int i=0;i<GG->numVertexes;i++)
        {
            printf("Kahn顶点 %s 的出度: %d\n", GG->vexs[i], GG->outdegree[i]);
            printf("Kahn顶点 %s 的入度: %d\n", GG->vexs[i], GG->indegree[i]);
        }            
        for(int i=0;i<GG->numVertexes;i++) 
            if(GG->indegree[i]!=-1)
                return false;
        return true;
    }
    else Kahn(GG);
}




void visit(struct graph* G,int v)
{
    printf("reach visit\n");
    if(G->indegree[v]==0)
    {
        line[line_count++]=v;//存储入度为0的顶点的索引
    }
    return;
}

//获取顶点的文件信息
void get_file_message(struct graph *GG)
{
    for(int i=0;i<GG->numVertexes;i++)
    {
        printf("\ngetfile:I am here\n");
        if(stat(GG->vexs[i],&GG->fileinfo[i])!=0)
            continue;
            // 转换时间戳为可读格式
        
         printf("文件: %s\n", GG->vexs[i]);
         printf("最后访问时间: %s", ctime(&GG->fileinfo[i].st_atime));
         printf("最后修改时间: %s", ctime(&GG->fileinfo[i].st_mtime));
         printf("最后状态变更时间: %s", ctime(&GG->fileinfo[i].st_ctime));
    }
    printf("\n\n");

}

//比较两个文件修改时间，看哪个比较新
bool which_file_fresh(char* file_A,char* file_B)//A比B新，返回真，反之假，一样新也是假
{
    struct stat A,B;
    if(stat(file_A,&A)!=0)  printf("which_file_fresh:文件 %s 打开失败\n",file_A);
    if(stat(file_B,&B)!=0)  printf("WHICH_FILE_FRESH:文件 %s 打开失败\n",file_B);
    
    return A.st_mtime > B.st_mtime;
}

