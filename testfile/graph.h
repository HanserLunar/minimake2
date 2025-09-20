#ifndef graph_H
#define graph_H

struct graph* createGraph();
bool addEdge(struct graph* G,int src,int dest);
bool addVertex(struct graph* G,char v);
void destroy_Graph(struct graph* G);
void BFS(struct graph* G,int value);
bool DFS(struct graph* G,int value);


#endif // !graph_H