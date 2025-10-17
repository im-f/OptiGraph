#include <iostream>
#include <map>
#include <tuple>
#include <list>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>

using namespace std;

struct Node
{
    int node;
    tuple<int, int> minMax;
};

enum typeIt
{
    pire,
    optimiste,
    prudent,
    stable
};

//prints a list of nodes
ostream& operator<<(ostream& os, list<Node> l)
{
    os << "| ";
    for(auto n : l)
    {
        os << n.node << " {" << get<0>(n.minMax) << ".." << get<1>(n.minMax) << "} | ";
    }
    return os;
}


class Graph {

    int init = 1;
    int term;
    map<int, list<Node>> g;

private:

    list<int> buildChemin(vector<tuple<int, int>> distPrec)
    {
        list<int> chemin = {get<1>(distPrec[term-1])};

        int n = term;
        while(n != 0)
        {
            chemin.push_front(n);
            n = get<0>(distPrec[n-1]);
        }
        chemin.push_front(1);

        return chemin;
    }

public:

    Graph()
    {
        g[1] = {};
    }

    void AddConnect(int initiale, Node terminale)
    {
        assert(get<0>(terminale.minMax) < get<1>(terminale.minMax));
        assert(get<0>(terminale.minMax) > 0 && get<1>(terminale.minMax) > 0);

        g[initiale].push_back(terminale);
        g[terminale.node] = {};
        term = terminale.node;
    }

    void PrintGraph()
    {
        for(auto n : g)
        {
            cout << n.first << " --> " << n.second << endl;
        }
    }


        /************************************************************************************************************/
        /********************************************* khan + dijkstra **********************************************/
        /************************************************************************************************************/


    //tri topologique (kahn)
    vector<int> triTopo(){
        map<int, int> degres;
        for(auto n : g){
            degres[n.first] = 0;
        }
        for(auto n : g){
            for(auto m : n.second){
                degres[m.node]++;
            }
        }

        queue<int> q;
        for(auto n : degres){
            if(n.second == 0){
                q.push(n.first);
            }
        }

        vector<int> topo;
        while(!q.empty()){
            int n = q.front();
            q.pop();
            topo.push_back(n);
            for(auto m : g[n]){
                degres[m.node]--;
                if(degres[m.node] == 0){
                    q.push(m.node);
                }
            }
        }
        return topo;
    }

    list<int> plc(typeIt cas){

        int def = cas == pire? INT_MIN : INT_MAX;
        vector<tuple<int, int>> distPrec(term, {-1, def});
        distPrec[0] = {0, 0};

        vector<int> topo = triTopo();
        for(auto n : topo){

            int dis = get<1>(distPrec[n-1]);

            if (dis != def){
                for(auto m: g[n]){

                    int v = m.node-1;

                    int w = cas == optimiste? get<0>(m.minMax) : 
                            cas == stable? get<1>(m.minMax) - get<0>(m.minMax) : 
                            get<1>(m.minMax); 

                    bool cond = cas == pire? get<1>(distPrec[v]) < dis + w : get<1>(distPrec[v]) > dis + w;

                    if(cond){

                        distPrec[v] = {n, dis + w};

                    } 
                }
            }
        }
        return buildChemin(distPrec);
    }


        /************************************************************************************************************/
        /********************************************* dijkstra-moore ***********************************************/
        /************************************************************************************************************/


    // Algorithme dijkstra-moore 
    void bis(vector<bool>* marked, vector<tuple<int, int>>* distPrec, int i, typeIt cas)
    {
        if(not (*marked)[term - 1])
        {
            (*marked)[i] = true;
            list<Node> n = g[i+1];
            for(auto a:n)
            {
                int j = a.node-1;
                int di = get<1>((*distPrec)[i]), dj = get<1>((*distPrec)[j]);

                if(not (*marked)[j])
                {
                    int val = cas == optimiste? get<0>(a.minMax) : 
                              cas == prudent? get<1>(a.minMax) : 
                              get<1>(a.minMax) - get<0>(a.minMax); 

                    if(dj > di + val)
                    {
                        (*distPrec)[j] = {i, di + val};
                    } 
                }
            }

            int min = INT_MAX;
            for(int b = 0; b < (*marked).size(); b++)
            {
                int db = get<1>((*distPrec)[b]);
                if(not (*marked)[b])
                {
                    if(min > db)
                    {
                        min = db; i = b;
                    }
                }
            }

            bis(marked, distPrec, i, cas);
        }

    }

    list<int> dijkstra(typeIt cas)
    {
        vector<bool> marked(term, false);
        vector<tuple<int, int>> distPrec(term, {0, INT_MAX});
        distPrec[0] = {0,0};

        bis(&marked, &distPrec, 0, cas);

        return buildChemin(distPrec);
    }


        /************************************************************************************************************/
        /*********************************************** Algo simple ************************************************/
        /************************************************************************************************************/


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

    list<int> optimisteIt1 ()
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

    list<int> prudentIt1 ()
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

    list<int> stableIt1 ()
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


    cout << "Pire cas global : ";

    pc = g.plc(pire);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;


    cout << "Plus court : ";

    pc = g.plc(optimiste);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;


    cout << "Cas optimiste : ";

    pc = g.dijkstra(optimiste);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    cout << "Cas prudent : ";

    pc = g.dijkstra(prudent);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    cout << "Cas stable : ";

    pc = g.dijkstra(stable);

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