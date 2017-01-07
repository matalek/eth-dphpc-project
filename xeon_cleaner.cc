#include <vector>
#include <memory>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

string algorithms[3] = {"SimpleParallel", "NaiveParallel", "HullTree"};
string threads_count[6] = {"2", "4", "8", "16", "32", "64"};
string file_path = "/mnt/hostfs/team08/log_files/";

int main(int argc, char* argv[]) {
        cout << "START\n";
        for (auto algorithm_name : algorithms){
                for (auto n_threads : threads_count){
                        ofstream output_file;
                        output_file.open (file_path + algorithm_name + "_" + n_threads + ".log");
                        output_file << "";
                        output_file.close();
                }
        }

        ofstream output_file;
        output_file.open (file_path + "Sequential_1.log");
        output_file << "";
        output_file.close();

        return 0;
}

