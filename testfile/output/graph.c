#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<stdbool.h>
#include<math.h>
#include"graph.h"

#define MAXVEX 256

struct graph{
    char vexs[MAXVEX][MAXVEX];        //顶点表,每个顶点是一个字符串
    int indegree[MAXVEX];//顶点的入度
    int outdegree[MAXVEX];//顶点的出度
    int arc[MAXVEX][MAXVEX];  //邻接矩阵
    int numVertexes,numEdges; //图中当前的顶点数和边数
};

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
    printf("%s ",G->vexs[v]);
    //对该顶点进行特定的操作
    /* 例如：可以在这里对每个访问到的顶点进行计数或其他处理 */
    //这里可以添加对每个访问到的顶点的具体操作

    for(int i=0;i<G->numVertexes;i++)   //固定行，找列中为1的即是有边且未访问
    {
        if(G->arc[v][i]==1 && visited[i]==false)//搜索下一个顶点
        {
            DFSs(G,i,visited);
        }
    }
}
