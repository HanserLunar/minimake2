
#ifndef graph_H
#define graph_H

struct graph* createGraph();
bool addEdge(struct graph* G,int src,int dest);
bool addVertexs(struct graph* G,char* name);
void destroy_Graph(struct graph* G);
void BFS(struct graph* G,int value);
void DFSs(struct graph* G,int v,bool visited[]);


#endif // !graph_H