#include <iostream>
#include <fstream>
#include <chrono>


int main(int argc, char* argv[]) {
    std::ios_base::sync_with_stdio(false);
    std::ifstream satoutput,graph;
    std::ofstream mapping;
    satoutput.open(argv[1]+(std::string)".satoutput");
    graph.open(argv[1]+(std::string)".graph");
    mapping.open(argv[1]+(std::string)".mapping");
    int n;
    graph >> n;
    std::string sat;
    getline(satoutput,sat);
    // std::cout << sat << std::endl;
    getline(satoutput,sat);
    // std::cout << sat << std::endl;
    std::string ot1 = "";
    std::string ot2 = "";
    int num =0;
    for (int i = 0; i < sat.length(); i++) {
        if (sat[i] == ' ') {
            if (num!=0) {
                if (num <=n && num>0)
                    ot1 += std::to_string(num) + " ";
                else if (num <= 2*n && num>n)
                    ot2 += std::to_string(num-n) + " ";
                else
                    break;
                num = 0;
            }
            continue;
        } else if (sat[i]=='-') {
            while(i<sat.length() && sat[i]!=' ') {
                i++;
            }
            num=0;
        }
        else if (sat[i]>='0' && sat[i]<='9'){
            num = num*10 + (sat[i]-'0');
        }
    }
    if (ot1.size()!=0)
    ot1.pop_back();    
    if (ot2.size()!=0)
    ot2.pop_back();
    if (ot1.size()==0 || ot2.size()==0) {
        mapping << "0\n";
        return 0;
    }
    ot1 += "\n";
    ot2 += "\n";
    mapping <<"#1\n"<< ot1<<"#2\n"<<ot2;

}