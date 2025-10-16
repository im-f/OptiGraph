#include <iostream>
#include <map>
#include <tuple>
#include <list>
#include <vector>
#include <queue>
#include <climits>

using namespace std;

struct Node
{
    int node;
    tuple<int, int> minMax;
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

        int n = term - 1;
        while(n != 0)
        {
            chemin.push_front(n+1);
            n = get<0>(distPrec[n]);
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

    list<int> pireCasGlobal(int s, int t){
        map<int, int> dist;
        map<int, int> pred;
        for(auto n : g){
            dist[n.first] = INT_MIN;
            pred[n.first] = -1;
        }
        dist[s] = 0;

        vector<int> topo = triTopo();
        for(auto n : topo){
            if (dist[n] != INT_MIN){
                for(auto m: g[n]){
                    int v = m.node;
                    int w = get<1>(m.minMax);
                    if(dist[v] < dist[n] + w){
                        dist[v] = dist[n] + w;
                        pred[v] = n;
                    } 
                }
            }
        }

        list<int> chemin;
        int n = t;
        while(n != -1){
            chemin.push_front(n);
            n = pred[n];
        }
        chemin.push_back(dist[t]);
        return chemin;
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

    // Algorithme Dijiskra-Moore 
    void bis(vector<bool>* marked, vector<tuple<int, int>>* distPrec, int i, int algo)
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
                    int val = algo == 0? get<0>(a.minMax) : //si algo est egale a 0, on fait le cas optimiste
                              algo == 1? get<1>(a.minMax) : //si il est egale a 1, on fait le cas prudent
                              get<1>(a.minMax) - get<0>(a.minMax); //sinon, on prend le cas stable

                    if(dj > di + val || dj == 0)
                    {
                        (*distPrec)[j] = {i, di + val};
                    } 
                }
            }

            int min = -1;
            for(int b = 0; b < (*marked).size(); b++)
            {
                int db = get<1>((*distPrec)[b]);
                if(not (*marked)[b])
                {
                    if((min < 0 || min > db) && db != 0)
                    {
                        min = db; i = b;
                    }
                }
            }

            bis(marked, distPrec, i, algo);
        }

    }

    
    list<int> optimisteIt ()
    {
        vector<bool> marked(term, false);
        vector<tuple<int, int>> distPrec(term, {0,0});

        bis(&marked, &distPrec, 0, 0);

        return buildChemin(distPrec);
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

    list<int> optimisteIt2 ()
    {
        vector<bool> marked(term, false);
        vector<int> dist(term, 0);
        vector<int> pred(term, 0); 

        int i = 0;

        while(not marked[term-1])
        {
            marked[i] = true;
            list<Node> n = g[i+1];
            for(auto a:n)
            {
                int j = a.node-1;
                if(not marked[j])
                {
                    if((dist[j] > dist[i] + get<0>(a.minMax)) || dist[j] == 0)
                    {
                        dist[j] = dist[i] + get<0>(a.minMax);
                        pred[j] = i;
                    }
                }
            }
            int min = -1;
            for(int b = 0; b < marked.size(); b++)
            {
                if(not marked[b])
                {
                    if((min < 0 || min > dist[b]) && dist[b] != 0)
                    {
                        min = dist[b];
                        i = b;
                    }
                }
            }
        }
        
        list<int> chemin = {dist[term-1]};

        int n = term - 1;
        while(n != 0)
        {
            chemin.push_front(n+1);
            n = pred[n];
        }
        chemin.push_front(1);
        return chemin;
    }

    
    
    list<int> prudentIt ()
    {
        vector<bool> marked(term, false);
        vector<tuple<int, int>> distPrec(term, {0,0});

        bis(&marked, &distPrec, 0, 1);

        return buildChemin(distPrec);
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

    
    
    list<int> stableIt ()
    {
        vector<bool> marked(term, false);
        vector<tuple<int, int>> distPrec(term, {0,0});

        bis(&marked, &distPrec, 0, 2);

        return buildChemin(distPrec);
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

    pc = g.pireCasGlobal(1,11);

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