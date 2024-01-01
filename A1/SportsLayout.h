
#ifndef SPORTSLAYOUT_H
#define	SPORTSLAYOUT_H

#include <fstream>
#include <iostream>
#include <bits/stdc++.h>
using namespace std;

class SportsLayout{

    public:
    int z,l;
    int** T;
    int **N;
    long double time;
    long long start_time;
    int *mapping;
    int *inverse_mapping;
    int *champion;
    long long champ_cost;

    
    struct tabu {
        unordered_set<int> tabu_set;
        queue<int> tabu_queue;
        int tabu_size;
        tabu(int size) : tabu_size(size) {}
        bool is_tabu(int i, int j) {
            int val = i*1000+j;
            return tabu_set.find(val) != tabu_set.end();
        }
        void add_tabu(int i, int j) {
            int val = i*1000+j;
            if (tabu_set.find(val) == tabu_set.end()) {
                tabu_set.insert(val);
                tabu_queue.push(val);
                if (tabu_queue.size() > tabu_size) {
                    tabu_set.erase(tabu_queue.front());
                    tabu_queue.pop();
                }
            }
        }
    };
    SportsLayout(string inputfilename);

    bool check_output_format();

    // void readOutputFile(string output_filename);
    
    long long cost_fn();

    void write_to_file(string outputfilename);

    void readInInputFile(string inputfilename);

    bool search(struct tabu &tabu, long long &hill_cost);
    // bool search();

    void compute_allocation();

    // static void* compute_allocation_helper(void* arg) {
    //     return ((SportsLayout*)arg)->compute_allocation();
    // }


};


#endif