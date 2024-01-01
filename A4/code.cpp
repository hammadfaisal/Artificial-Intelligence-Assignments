#include <cassert>
#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

struct Node {
    vector<int> children;
    vector<int> parents;
    vector<double> cpt;
    vector<string> values;
    int nvalues;
};

struct Graph {
    unordered_map<string, int> node_map;
    vector<Node> nodes;

    int add_node(string name, vector<string> &values) {
        int index = nodes.size();
        node_map[name] = index;
        nodes.push_back(Node());
        nodes[index].nvalues = values.size();
        nodes[index].values = values;
        return index;
    }

    void add_parents(int node_index, vector<string> &parents) {
        for (int i = 0; i < parents.size(); i++) {
            int parent_index = node_map[parents[i]];
            nodes[node_index].parents.push_back(parent_index);
            nodes[parent_index].children.push_back(node_index);
        }
    }

    void add_cpt(int node_index, vector<double> &cpt) {
        nodes[node_index].cpt = cpt;
    }
};

Graph read_network(string filename) {
    ifstream file(filename);
    string line;
    Graph graph;
    vector<pair<int, vector<string>>> later_parents;
    while (getline(file, line)) {
        stringstream ss(line);
        string type;
        ss >> type;
        if (type == "variable") {
            string name;
            ss >> name;
            getline(file, line);
            stringstream ss(line);
            string tmp;
            for (int i = 0; i < 4; i++) {
                ss >> tmp;
            }
            vector<string> values;
            while (tmp != "};") {
                values.push_back(tmp);
                ss >> tmp;
            }
            int index = graph.add_node(name, values);
        } else if (type == "probability") {
            /*
            example:
            probability (  "StrokeVolume"  "LVFailure"  "Hypovolemia" ) { //3
            variable(s) and 12 values table -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1
            ;
            }*/
            string name;
            ss >> name;
            ss >> name;
            vector<string> parents;
            string tmp;
            ss >> tmp;
            while (tmp != ")") {
                parents.push_back(tmp);
                ss >> tmp;
            }
            int node_index = graph.node_map[name];
            later_parents.push_back(make_pair(node_index, parents));
            vector<double> cpt;
            getline(file, line);
            stringstream ss2(line);
            ss2 >> tmp;
            ss2 >> tmp;
            while (tmp != ";") {
                cpt.push_back(atof(tmp.c_str()));
                ss2 >> tmp;
            }
            graph.add_cpt(node_index, cpt);
        }
    }
    for (int i = 0; i < later_parents.size(); i++) {
        graph.add_parents(later_parents[i].first, later_parents[i].second);
    }
    return graph;
}

void print_network(string input_file, string output_file, Graph &graph) {
    ifstream file(input_file);
    ofstream out(output_file);
    // out << fixed << setprecision(4);
    string line;
    while (getline(file, line)) {
        stringstream ss(line);
        string type;
        ss >> type;
        if (type == "probability") {
            string name;
            ss >> name;
            ss >> name;
            out << line << "\n";
            getline(file, line);
            out << "\ttable ";
            int node_index = graph.node_map[name];
            for (int i = 0; i < graph.nodes[node_index].cpt.size(); i++) {
                out << graph.nodes[node_index].cpt[i] << " ";
            }
            out << ";\n";

        } else {
            out << line << "\n";
        }
    }
}

void random_init(Graph &graph, double low, double high) {
    static mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    static uniform_real_distribution<double> dist(low, high);
    for (int i = 0; i < graph.nodes.size(); i++) {
        // fill cpt with random values and normalize
        int paretn_value_count = 1;
        for (int j = 0; j < graph.nodes[i].parents.size(); j++) {
            paretn_value_count *=
                graph.nodes[graph.nodes[i].parents[j]].nvalues;
        }
        for (int j = 0; j < graph.nodes[i].cpt.size(); j++) {
            graph.nodes[i].cpt[j] = dist(rng);
        }
        for (int j = 0; j < paretn_value_count; j++) {
            double sum = 0;
            for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                sum += graph.nodes[i].cpt[k * paretn_value_count + j];
            }
            for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                graph.nodes[i].cpt[k * paretn_value_count + j] /= sum;
            }
        }
    }
}

void E_Step(Graph &graph, vector<vector<pair<int, int>>> &in_edges,
            vector<int> &groups, vector<double> &weights) {
    // calculate weights for each data entry
    int i = 0;
    for (int j = 0; j < groups.size(); j++) {
        double sum = 0;
        // assert(i+groups[j] <= weights.size());
        for (int k = 0; k < groups[j]; k++) {
            weights[i + k] = 1;
            for (int l = 0; l < in_edges[i + k].size(); l++) {
                int node_index = in_edges[i + k][l].first;
                int cpt_index = in_edges[i + k][l].second;
                // if (cpt_index >= graph.nodes[node_index].cpt.size()) {
                //     cout << "ERROR: "<<node_index<<" " << cpt_index << " "
                //          << graph.nodes[node_index].cpt.size() << endl;
                // }
                weights[i + k] *= graph.nodes[node_index].cpt[cpt_index];
            }
            sum += weights[i + k];
        }
        for (int k = 0; k < groups[j]; k++) {
            weights[i + k] /= sum;
        }
        i += groups[j];
    }
}

void M_Step(Graph &graph, vector<vector<pair<int, int>>> &out_edges,
            vector<int> &groups, vector<double> &weights, double smooth, double &max_change) {
    vector<vector<double>> old_cpt(graph.nodes.size());
    for (int i = 0; i < graph.nodes.size(); i++) {
        old_cpt[i] = graph.nodes[i].cpt;
        for (int j = 0; j < graph.nodes[i].cpt.size(); j++) {
            graph.nodes[i].cpt[j] = 0;
        }
    }
    // update parameters
    for (int i = 0; i < out_edges.size(); i++) {
        for (int j = 0; j < out_edges[i].size(); j++) {
            int node_index = out_edges[i][j].first;
            int cpt_index = out_edges[i][j].second;
            graph.nodes[node_index].cpt[cpt_index] += weights[i];
        }
    }
    for (int i = 0; i < graph.nodes.size(); i++) {
        // normalize
        int paretn_value_count = 1;
        for (int j = 0; j < graph.nodes[i].parents.size(); j++) {
            paretn_value_count *=
                graph.nodes[graph.nodes[i].parents[j]].nvalues;
        }
        for (int j = 0; j < paretn_value_count; j++) {
            double sum = 0;
            for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                sum += graph.nodes[i].cpt[k * paretn_value_count + j];
            }
            if (sum != 0) {
                for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                    graph.nodes[i].cpt[k * paretn_value_count + j] /= sum;
                }
                sum = 1 + smooth * (graph.nodes[i].nvalues );
                for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                    graph.nodes[i].cpt[k * paretn_value_count + j] =
                        (graph.nodes[i].cpt[k * paretn_value_count + j] +
                         smooth) / sum;
                }
            }
            else {
                for (int k = 0; k < graph.nodes[i].nvalues; k++) {
                    graph.nodes[i].cpt[k * paretn_value_count + j] = 1.0/graph.nodes[i].nvalues;
                }
            }
        }
    }
    // calculate change
    max_change = 0;
    for (int i = 0; i < graph.nodes.size(); i++) {
        for (int j = 0; j < graph.nodes[i].cpt.size(); j++) {
            max_change =
                max(max_change, abs(graph.nodes[i].cpt[j] - old_cpt[i][j]));
        }
    }
}

int main(int argc, char *argv[]) {
    string alarm_file = argv[1];
    Graph graph = read_network(alarm_file);
    long long int start = chrono::duration_cast<chrono::milliseconds>(
                              chrono::system_clock::now().time_since_epoch())
                              .count();
    // PREPROCESSING
    // each data entry is expanded for all possible values of the unknown
    // variable in_edges[i] is a vector of pairs of the form (node_index,
    // cpt_index) of parametes required to calculate the weight of the ith
    // "expanded" data entry (markov blanket of the unknown variable)
    // out_edges[i] is a vector of pairs of the form (node_index, cpt_index) of
    // parametes affected by the ith "expanded" data entry

    vector<vector<pair<int, int>>> in_edges, out_edges;

    // groups[i] is the number of possible values of the unknown variable in the
    // ith "original" data entry
    vector<int> groups;

    string data_file = argv[2];
    ifstream file(data_file);
    string line;
    int data_count = 0;
    while (getline(file, line)) {
        data_count++;
        stringstream ss(line);
        vector<string> values(graph.nodes.size());
        int unknown = -1;
        for (int i = 0; i < graph.nodes.size(); i++) {
            ss >> values[i];
            if (values[i] == "\"?\"") {
                unknown = i;
            }
        }
        // cout <<"checkpoint 1"<<endl;
        if (unknown != -1) {
            groups.push_back(graph.nodes[unknown].nvalues);
            for (int i = 0; i < graph.nodes[unknown].nvalues; i++) {
                // find parameters to multiply to find this weight and store
                // indices of those parameters in in_edges

                vector<pair<int, int>> in_edge;
                values[unknown] = graph.nodes[unknown].values[i];
                int cpt_index = i;

                for (int j = 0; j < graph.nodes[unknown].parents.size(); j++) {
                    int parent = graph.nodes[unknown].parents[j];
                    int parent_value = 0;
                    while (graph.nodes[parent].values[parent_value] !=
                           values[parent]) {
                        parent_value++;
                    }
                    cpt_index =
                        cpt_index * graph.nodes[parent].nvalues + parent_value;
                }
                // if (cpt_index == 41 && unknown == 10) {
                //     cout <<i<<" "<<data_count<<"
                //     "<<graph.nodes[unknown].nvalues<<endl;

                // }
                in_edge.push_back(make_pair(unknown, cpt_index));
                for (int j = 0; j < graph.nodes[unknown].children.size(); j++) {
                    int child = graph.nodes[unknown].children[j];
                    int child_value = 0;
                    while (graph.nodes[child].values[child_value] !=
                           values[child]) {
                        child_value++;
                    }
                    int cpt_index = child_value;
                    for (int k = 0; k < graph.nodes[child].parents.size();
                         k++) {
                        int parent = graph.nodes[child].parents[k];
                        int parent_value = 0;
                        while (graph.nodes[parent].values[parent_value] !=
                               values[parent]) {
                            parent_value++;
                        }
                        cpt_index = cpt_index * graph.nodes[parent].nvalues +
                                    parent_value;
                    }
                    in_edge.push_back(make_pair(child, cpt_index));
                }
                in_edges.push_back(in_edge);
            }
            for (int i = 0; i < graph.nodes[unknown].nvalues; i++) {
                // find parameters affected by this data entry and store indices
                // of those parameters in out_edges
                vector<pair<int, int>> out_edge;
                values[unknown] = graph.nodes[unknown].values[i];
                // only one parameter for each node is affected
                for (int j = 0; j < graph.nodes.size(); j++) {
                    int node_value = 0;
                    while (graph.nodes[j].values[node_value] != values[j]) {
                        node_value++;
                    }
                    int cpt_index = node_value;
                    for (int k = 0; k < graph.nodes[j].parents.size(); k++) {
                        int parent = graph.nodes[j].parents[k];
                        int parent_value = 0;
                        while (graph.nodes[parent].values[parent_value] !=
                               values[parent]) {
                            parent_value++;
                        }
                        cpt_index = cpt_index * graph.nodes[parent].nvalues +
                                    parent_value;
                    }
                    out_edge.push_back(make_pair(j, cpt_index));
                }
                out_edges.push_back(out_edge);
            }
        } else {
            // no unknown variable, so no expansion
            groups.push_back(1);
            vector<pair<int, int>> in_edge;
            vector<pair<int, int>> out_edge;
            in_edges.push_back(in_edge);
            // out_edges still there because parameters are affected by this
            // data entry
            for (int j = 0; j < graph.nodes.size(); j++) {
                int node_value = 0;
                while (graph.nodes[j].values[node_value] != values[j]) {
                    node_value++;
                }
                int cpt_index = node_value;
                for (int k = 0; k < graph.nodes[j].parents.size(); k++) {
                    int parent = graph.nodes[j].parents[k];
                    int parent_value = 0;
                    while (graph.nodes[parent].values[parent_value] !=
                           values[parent]) {
                        parent_value++;
                    }
                    cpt_index =
                        cpt_index * graph.nodes[parent].nvalues + parent_value;
                }
                out_edge.push_back(make_pair(j, cpt_index));
            }
            out_edges.push_back(out_edge);
        }
    }
    // cout << "REACHED HERE" << endl;
    vector<double> weights(in_edges.size());
    // EM
    random_init(graph, 0.1, 0.9);
    double max_change = 100;
    unordered_map<double,int> count;
    while(max_change > 1e-9 && count[max_change] < 1000 && (chrono::duration_cast<chrono::milliseconds>(
                              chrono::system_clock::now().time_since_epoch())
                              .count() - start) < 115000) {
        E_Step(graph, in_edges, groups, weights);
        // cout << "E_Step done" << endl;
        M_Step(graph, out_edges, groups, weights,0.0002, max_change);
        count[max_change]++;
        // cout << "M_Step done" << endl;
        // cout << "max_change: " << max_change << endl;
    }

    for (int i =0;i < graph.nodes.size();i++){
        //round to 4 decimal places
        for (int j = 0; j < graph.nodes[i].cpt.size(); j++) {
            graph.nodes[i].cpt[j] = round(graph.nodes[i].cpt[j] * 10000) /
            10000;
        }
    }

    print_network(alarm_file, "solved_" + alarm_file, graph);
}