#include <iostream>
#include <map>
#include <tuple>
#include <list>

using namespace std;

struct Node
{
    int node;
    tuple<int, int> minMax;
    
} ;

//prints a list of nodes
ostream& operator<<(ostream& os, list<Node> l)
{
    os << "| ";
    for(auto n : l)
    {
        os << n.node << " {" << get<0>(n.minMax) << ".." << get<1>(n.minMax) <<  " } | ";
    }
    return os;
}

class Graph {

    map<int, list<Node>> g;

public:

    Graph()
    {
        g[1] = {};
    }

    void AddConnect(int initiale, Node terminale)
    {
        g[initiale].push_back(terminale);
        g[terminale.node] = {};
    }

    void PrintGraph()
    {
        for(auto n : g)
        {
            cout << n.first << " --> " << n.second << endl;
        }
    }



    //À completer 

    list<int> pireCasIt ()
    {
        list<int> chemin = {1}; 
        list<Node> n = g[1];
        int time = 0;
        while(!n.empty())
        {
            int i = 0, max = 0;
            for(auto a:n)
            {
                if(get<1>(a.minMax) >= max)
                {
                    max = get<1>(a.minMax); 
                    i = a.node;
                }
            }
            time += max;
            chemin.push_back(i);
            n = g[i];
        }
        chemin.push_back(time);
        return chemin;
    }

    list<int> optimisteIt ()
    {
        list<int> chemin = {1}; 
        list<Node> n = g[1];
        int time = 0;
        while(!n.empty())
        {
            int i = 0, min = -1;
            for(auto a:n)
            {
                if(get<0>(a.minMax) <= min || min < 0)
                {
                    min = get<0>(a.minMax); 
                    i = a.node;
                }
            }
            time += min;
            chemin.push_back(i);
            n = g[i];
        }
        chemin.push_back(time);
        return chemin;
    }

    list<int> prudentIt ()
    {
        list<int> chemin = {1}; 
        list<Node> n = g[1];
        int time = 0;
        while(!n.empty())
        {
            int i = 0, min = -1;
            for(auto a:n)
            {
                if(get<1>(a.minMax) <= min || min < 0)
                {
                    min = get<1>(a.minMax); 
                    i = a.node;
                }
            }
            time += min;
            chemin.push_back(i);
            n = g[i];
        }
        chemin.push_back(time);
        return chemin;
    }

    list<int> stableIt ()
    {
        list<int> chemin = {1}; 
        list<Node> n = g[1];
        int time = 0;
        while(!n.empty())
        {
            int i = 0, min = -1;
            for(auto a:n)
            {
                int dif = get<1>(a.minMax) - get<0>(a.minMax);
                if(dif <= min || min < 0)
                {
                    min = dif; 
                    i = a.node;
                }
            }
            time += min;
            chemin.push_back(i);
            n = g[i];
        }
        chemin.push_back(time);
        return chemin;
    }

};


//Graphe du TP

void TestGraph()
{
    Graph g = Graph();

    Node n1[3];
    n1[0] = {2, {3,7}};
    n1[1] = {3, {4,6}}; 
    n1[2] = {4, {3,8}};
    for(auto d:n1){
        g.AddConnect(1, d);
    } 

    g.AddConnect(2, {5, {2,5}});

    Node n[2];

    n[0] = {5, {5,8}};
    n[1] = {6, {4,6}};
    for(auto d:n){
        g.AddConnect(3, d);
    } 

    n[0] = {6, {7,10}};
    n[1] = {7, {3,8}};
    for(auto d:n){
        g.AddConnect(4, d);
    }

    g.AddConnect(5, {8, {4,9}}); 

    n[0] = {8, {2,4}};
    n[1] = {9, {5,6}};
    for(auto d:n){
        g.AddConnect(6, d);
    }

    n[0] = {9, {2,4}};
    n[1] = {10, {4,7}};
    for(auto d:n){
        g.AddConnect(7, d);
    }

    g.AddConnect(8, {11, {3,7}});

    g.AddConnect(9, {11, {3,6}});

    g.AddConnect(10, {11, {3,4}});

    g.PrintGraph();

    cout << endl;
    cout << "Pire cas : ";

    list<int> pc = g.pireCasIt();

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    cout << "Cas optimiste : ";

    pc = g.optimisteIt();

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    cout << "Cas prudent : ";

    pc = g.prudentIt();

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    cout << "Cas stable : ";

    pc = g.stableIt();

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;
}

int main () 
{
    TestGraph();
    return 0;
}