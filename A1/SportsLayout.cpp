#include <fstream>
#include <iostream>
#include <bits/stdc++.h>

using namespace std;

#include "SportsLayout.h"

    int get_random_number(int min, int max) {
        static random_device rd;
        static mt19937 gen(rd());
        uniform_int_distribution<> dis(min, max);
        return dis(gen);
    }

    SportsLayout::SportsLayout(string inputfilename)
    {
          
        readInInputFile(inputfilename);
        mapping= new int[z];
        inverse_mapping= new int[l];
        champion= new int[z];
        champ_cost = LLONG_MAX;
        start_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

    }

    bool SportsLayout::check_output_format()
    {

        vector<bool> visited(l,false);
        for(int i=0;i<z;i++)
        {
            if((champion[i]>=1 && champion[i]<=l))
            {
                if(!visited[champion[i]-1])
                visited[champion[i]-1]=true;
                else
                {
                    cout<<"Repeated locations, check format\n";
                    return false;
                }
            }
            else
            {
                cout<<"Invalid location, check format\n";
                return false;
            }
        }

        return true;

    }

    // void SportsLayout::readOutputFile(string output_filename)
    // {
    //         fstream ipfile;
	//         ipfile.open(output_filename, ios::in);
    //         if (!ipfile) {
    //             cout << "No such file\n";
    //             exit( 0 );
    //         }
    //         else {
                
    //             vector<int> ip;

    //             while (1) {
    //                 int t;
    //                 ipfile >> t;
    //                 ip.push_back(t);
    //                 if (ipfile.eof())
    //                     break;
                    
    //             }
            
    //         if(ip.size()!=z)
    //         {
    //             cout<<"number of values not equal to number of zones, check output format\n";
    //             exit(0);
    //         }
    //         for(int i=0;i<z;i++)
    //         mapping[i]=ip[i];
    //     ipfile.close();

    //     if(!check_output_format())
    //         exit(0);
    //     cout<<"Read output file, format OK"<<endl;

    //         }
        
    // }

    long long mapping_cost_fn(int* mapping, int z, int l, int **T, int **N) {
        long long cost = 0;
        for (int i = 0; i < z; i++) {
            for (int j = 0; j < z; j++) {
                cost+=(long long)N[i][j]*(long long)T[mapping[i]-1][mapping[j]-1];
            }
        }
        return cost;
    }


    long long SportsLayout::cost_fn(){
        return champ_cost;
    }

    void SportsLayout::readInInputFile(string inputfilename)
    {
            fstream ipfile;
	        ipfile.open(inputfilename, ios::in);
            if (!ipfile) {
                cout << "No such file\n";
                exit( 0 );
            }
            else {
                

                ipfile>> time;
                ipfile >> z;
                ipfile >> l;

                if(z>l)
                {
                    cout<<"Number of zones more than locations, check format of input file\n";
                    exit(0);
                }


            

            int **tempT;
            int **tempN;

          tempT = new int*[l];
         for (int i = 0; i < l; ++i)
            tempT[i] = new int[l];
        
        tempN = new int*[z];
        for (int i = 0; i < z; ++i)
            tempN[i] = new int[z];

        for(int i=0;i<z;i++)
        {
            for(int j=0;j<z;j++)
            ipfile>>tempN[i][j];
        }

        for(int i=0;i<l;i++)
        {
            for(int j=0;j<l;j++)
            ipfile>>tempT[i][j];
        }

        ipfile.close();

        T= tempT;
        N= tempN;
            }

    }

    void SportsLayout::write_to_file(string outputfilename){

         // Open the file for writing
    ofstream outputFile(outputfilename);

    // Check if the file is opened successfully
    if (!outputFile.is_open()) {
        cerr << "Failed to open the file for writing." << std::endl;
        exit(0);
    }

    for(int i=0;i<z;i++)
    outputFile<<champion[i]<<" ";

    // Close the file
    outputFile.close();

    cout << "Allocation written to the file successfully." << endl;

    }

    bool SportsLayout::search(struct tabu &tabu, long long &hill_cost) {
        long long cost = mapping_cost_fn(mapping,z,l, T, N);
        long long current_best_cost = LLONG_MAX;
        pair<int,int> best_swap = make_pair(-1,-1);
        for (int i = 0; i < z; i++) {
            for (int j = 0; j < l; j++) {
                if (inverse_mapping[j]==0 || inverse_mapping[j] > i + 1) {
                    int l1 = j, l2 = mapping[i] - 1;
                    int zone_at_l1 = i;
                    int zone_at_l2 = inverse_mapping[j] - 1;
                    long long new_cost = cost;
                    for (int k = 0; k < z; k++) {
                        if (k != zone_at_l1 && k != zone_at_l2) {
                            new_cost -= (long long)(N[k][zone_at_l1]+N[zone_at_l1][k]) * (long long)T[l2][mapping[k] - 1];
                            new_cost += (long long)(N[k][zone_at_l1]+N[zone_at_l1][k]) * (long long)T[l1][mapping[k] - 1];
                            if (zone_at_l2 >=0) {
                                new_cost -= (long long)(N[k][zone_at_l2]+N[zone_at_l2][k]) * (long long)T[l1][mapping[k] - 1];
                                new_cost += (long long)(N[k][zone_at_l2]+N[zone_at_l2][k]) * (long long)T[l2][mapping[k] - 1];
                            }
                        }
                    }
                    if (new_cost < current_best_cost && ((!tabu.is_tabu(zone_at_l1,l1) && !tabu.is_tabu(zone_at_l2,l2)) || new_cost < hill_cost)) {
                        current_best_cost = new_cost;
                        best_swap = make_pair(i, j);
                    }
                }
            }
        }
        if (best_swap.first != -1) {
            int l1 = best_swap.second, l2 = mapping[best_swap.first] - 1;
            int zone_at_l1 = best_swap.first;
            int zone_at_l2 = inverse_mapping[l1] - 1;
            mapping[best_swap.first] = l1 + 1;
            inverse_mapping[l1] = best_swap.first + 1;
            if (zone_at_l2 >= 0) {
                mapping[zone_at_l2] = l2 + 1;
            }
            inverse_mapping[l2] = zone_at_l2 + 1;
            if (current_best_cost < champ_cost) {
                champ_cost = current_best_cost;
                for (int i = 0; i < z; i++) {
                    champion[i] = mapping[i];
                }
            }
            if (current_best_cost >= hill_cost) {
                assert (!tabu.is_tabu(zone_at_l1,l1) && !tabu.is_tabu(zone_at_l2,l2));
                tabu.add_tabu(zone_at_l1, l2);
                if (zone_at_l2 >= 0) {
                    tabu.add_tabu(zone_at_l2, l1);
                }
            }
        }
        if (current_best_cost >= hill_cost)
            return false;
        hill_cost = current_best_cost;
        return true;
    }

    void random_restart(int* mapping, int* inverse_mapping, int z, int l, int** N, int** T, int* champion) {
        for (int i=0; i < l; i++) {
            inverse_mapping[i] = 0;
        }

        for(int i =0; i<z;i++) {
            mapping[i] = 0;
        }
        vector<pair<long long, int> > zone_cost(z);
        for(int i =0; i<z; i++) {
            zone_cost[i] = make_pair(0,i);
            for(int j=0;j<z;j++) {
                zone_cost[i].first += (N[i][j]+N[j][i])*T[champion[i]-1][champion[j]-1];
            }
        }
        sort(zone_cost.begin(), zone_cost.end());
        for(int i=0; i<z; i++) {
            if (get_random_number(0,z)<=i) {
                mapping[zone_cost[i].second] = champion[zone_cost[i].second];
                inverse_mapping[champion[zone_cost[i].second]-1] = zone_cost[i].second + 1;
            }
            else {
                mapping[zone_cost[i].second] = 0;
            }
        }
        for (int i = 0; i < z; i++) {
            if (mapping[i]!=0)
                continue;
            int t = get_random_number(1, l);
            while (inverse_mapping[t-1] != 0) {
                t = get_random_number(1, l);
            }
            mapping[i] = t;
            inverse_mapping[t-1] = i + 1;
        }
    }

    void initial_mapping(int* mapping, int* inverse_mapping, int z, int l, int** N,int **T) {
        // try to find a feasible solution
        // cout << "initial mapping" << endl;
        for(int i =0;i<z;i++)
            N[i][i]=0;
        for (int i = 0; i < l; i++) {
            inverse_mapping[i] = 0;
        }
        for (int i = 0; i < z; i++) {
            mapping[i] = 0;
        }
        
    
        // cout << "initial mapping2" << endl;
        vector<pair<long long,int> > zone_cost(z);
        vector<pair<long long,int> > location_cost(l);
        for(int i =0; i<z;i++) {
            zone_cost[i] = make_pair(0,i);
            for(int j=0; j<z; j++) {
                zone_cost[i].first += N[i][j]+N[j][i];
            }
        }
        // cout << "initial mapping3" << endl;
        sort(zone_cost.begin(), zone_cost.end());
        for (int i = 0; i < l; i++) {
            location_cost[i] = make_pair(0,i);
            for (int j = 0; j < l; j++) {
                location_cost[i].first += T[i][j];
            }
        }
        // cout << "initial mapping4" << endl;
        sort(location_cost.begin(), location_cost.end());
        for(int i=0; i<z; i++) {
            mapping[zone_cost[i].second] = location_cost[z-1-i].second + 1;
            inverse_mapping[location_cost[z-1-i].second] = zone_cost[i].second + 1;
        }
        // cout << "initial mapping5" << endl;
    }

    
    


    void SportsLayout::compute_allocation()
    {
        
        time *= 60000;
        time-=2000;
        int iterations=0;
        initial_mapping(mapping, inverse_mapping,z,l,N,T);
        // cout << "initial mapping done" << endl;
        while(1) {
            // random_restart(mapping, inverse_mapping,z,l);
            struct tabu tabu = SportsLayout::tabu(40);
            // cout << "tabu done" << endl;
            int non_improving_iterations = 0;
            long long hill_cost = LLONG_MAX;
            while(non_improving_iterations < 40) {
                if (!search(tabu, hill_cost)) {
                    non_improving_iterations++;
                }
                else {
                    non_improving_iterations = 0;
                }

                if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_time) >= time) {

                    return;
                }
            }
            if ((chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count() - start_time) >= time) {
                return;
            }
            random_restart(mapping, inverse_mapping,z,l, N, T , champion);
            iterations++;
        }
  
    }


