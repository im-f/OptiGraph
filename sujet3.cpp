#include <iostream>
#include <map>
#include <tuple>
#include <list>
#include <vector>
#include <queue>
#include <climits>
#include <algorithm>
#include <cassert>

using namespace std;

//structure d'un noeud avec son numero et son min et max de temps
struct Node
{
    int node;
    tuple<int, int> minMax;
};

//type d'itineraire
enum typeIt
{
    pire, //plus grande durée en temps max 
    optimiste, //plus petite durée en temps min)
    prudent, //plus petite durée en temps max
    stable //plus petite différence entre temps min et temps max
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

//graph orienté avec des arcs pondérés (min et max)
class Graph {

    int init = 1;
    int term;
    map<int, list<Node>> g;

private:

    //construit le chemin à partir du tableau distPrec 
    list<int> buildChemin(vector<tuple<int, int>> distPrec)
    {
        int time = get<1>(distPrec[term-1]); //temps total
        list<int> chemin ;

        int n = term;
        while(n != 0 && n != -1)
        {
            chemin.push_front(n);
            n = get<0>(distPrec[n-1]);
        }
        chemin.push_back(time); //ajoute le temps total à la fin du chemin
        return chemin;
    }

public:
    //constructeur
    Graph()
    {
        g[1] = {};
    }

    //ajoute une connexion entre deux noeuds
    void AddConnect(int initiale, Node terminale)
    {
        assert(get<0>(terminale.minMax) < get<1>(terminale.minMax)); //vérifie que le min est inférieur au max
        assert(get<0>(terminale.minMax) > 0 && get<1>(terminale.minMax) > 0); //vérifie que les temps sont positifs

        g[initiale].push_back(terminale); //ajoute la connexion
        g[terminale.node] = {}; //ajoute le noeud terminal s'il n'existe pas déjà
        term = terminale.node; //met à jour le noeud terminal
    }

    //affiche le graphe
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


    /*tri topologique (kahn)
    * 1. Calculer le degré entrant de chaque nœud.
    * 2. Initialiser une file avec tous les nœuds de degré entrant 0.
    * 3. Tant que la file n'est pas vide :
    *   a. Retirer un nœud de la file et l'ajouter à l'ordre topologique.
    *  b. Pour chacun de ses successeur, décrémenter son degré entrant.
    *     c. Si le degré entrant devient 0, l'ajouter à la file.
    * 4. Si l'ordre topologique contient tous les nœuds, le tri est réussi.
    *   Sinon, le graphe contient un cycle.
    * @return un vecteur contenant l'ordre topologique des nœuds
    */
    vector<int> triTopo(){
        //calcul des degrés entrants
        map<int, int> degres;
        for(auto &in : g){
            degres[in.first] = 0; 
        }
        for(auto &in : g){
            for(auto &suc : in.second){
                degres[suc.node]++;
            }
        }
        //initialisation de la file avec les noeuds de degré 0
        queue<int> q;
        for(auto &n : degres){
            if(n.second == 0){
                q.push(n.first);
            }
        }

        //traitement de la file
        vector<int> topo;
        while(!q.empty()){
            int cur = q.front();
            q.pop();
            topo.push_back(cur);
            for(auto &suc : g[cur]){
                degres[suc.node]--; 
                if(degres[suc.node] == 0){ 
                    q.push(suc.node);
                }
            }
        }
        return topo;
    }

    /*version optimisée de l'algorithme de plus court chemin pour les DAGs
    * @param cas type d'itinéraire (pire, optimiste, prudent, stable)
    * @return une liste contenant le chemin et le temps total
    */
    list<int> plc(typeIt cas){

        int def = cas == pire? INT_MIN : INT_MAX;
        vector<tuple<int, int>> distPrec(term, {-1, def});
        distPrec[0] = {-1, 0};

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
    void dij(vector<bool>* marked, vector<tuple<int, int>>* distPrec, int i, typeIt cas)
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
                        (*distPrec)[j] = {i+1, di + val};
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

            dij(marked, distPrec, i, cas);
        }

    }

    list<int> dijkstra(typeIt cas)
    {
        vector<bool> marked(term, false);
        vector<tuple<int, int>> distPrec(term, {0, INT_MAX});
        distPrec[0] = {0,0};

        dij(&marked, &distPrec, 0, cas);

        list<int> res = buildChemin(distPrec);

        return res;
    }


        /************************************************************************************************************/
        /*********************************************** Algo simple ************************************************/
        /************************************************************************************************************/

    /* Algorithme simple itératif pour le pire cas
    * @return une liste contenant le chemin et le temps total
    */
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

    /* Algorithme simple itératif pour le cas optimiste
    * @return une liste contenant le chemin et le temps total
    */
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

    /* Algorithme simple itératif pour le cas prudent
    * @return une liste contenant le chemin et le temps total
    */
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

    /* Algorithme simple itératif pour le cas stable
    * @return une liste contenant le chemin et le temps total
    */
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
//Teste les différentes fonctionnalités du graphe
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

    //tests des différents algorithmes
    //pire cas (local) itératif 
    cout << "Pire cas : ";

    list<int> pc = g.pireCasIt();

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    //pire cas (global) avec algo optimisé 
    cout << "Pire cas global : ";

    pc = g.plc(pire);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    //cas optimiste avec algo optimisé
    cout << "Plus court : ";

    pc = g.plc(optimiste);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    //cas optimiste avec dijkstra
    cout << "Cas optimiste : ";

    pc = g.dijkstra(optimiste);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    //cas prudent avec algo dijkstra
    cout << "Cas prudent : ";

    pc = g.dijkstra(prudent);

    cout << pc.back() << " min" << endl;
    pc.pop_back();

    for(auto i : pc)
    {
        cout << i << "-";
    }
    cout << endl;

    //cas stable avec algo dijkstra
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