#include <stdio.h>
#include <iostream>
#include <map>
#include <list>

using namespace std;

typedef struct 
{
    int node;
    tuple<int, int> minMax;
} Node;

class Graph {

    map<int, list<Node>> g;

    private:

    bool existsInGraph(int n)
    {
        //checks if n exists in the map
        return true;
    }

    public:

    void AddConnect(int initiale, Node terminale)
    {
        g[initiale].push_back(terminale);
        //add key to map lmao
    }
};