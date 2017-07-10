/*
 * TAtests.cpp
 *
 *  Created on: Jul 3, 2017
 *      Author: divya
 */


#include "../src/topK.h"
#include "../utils/posting_list.h"
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <memory>

typedef shared_ptr<CPostingList> posting_list;

void GeneratePostingListFromFile(std::string filename, TopKAlgos* TopAlgos_ptr)
{
	int doc_id;
		double score;

		std::string line;
		ifstream readFile(filename);

		shared_ptr<CPostingList>  posting_list_obj (new CPostingList());

		if(!readFile.good())
		{
			cout << "file not found!" << endl;
		}


			while(!readFile.eof())
			{
			readFile >> doc_id >> score;
		    posting_list_obj->insertScore(doc_id, score);
			}



		readFile.close();
		std::cout << "posting list size = " << posting_list_obj->doc_list_size << endl;
		TopAlgos_ptr->posting_list_vector.push_back(posting_list_obj);
}


bool compFunc(const pair<int,double>& p1, const pair<int, double>& p2)
{
	return  p1.second > p2.second;
}

void check_correctness(vector<posting_list> posting_list_vector, vector<pair<int,double>> result)
{

	vector<pair<int,double>> brute_force_result(posting_list_vector[0]->posting_list.size(), std::make_pair(-1, 0.0));
	for(size_t i = 0; i < posting_list_vector.size(); i++)
	{
		for (std::unordered_map<int,double>::iterator it=posting_list_vector[i]->posting_list.begin(); it!=posting_list_vector[i]->posting_list.end(); ++it)
		{
			double sum = brute_force_result[it->first].second;
			sum += it->second;
			brute_force_result[it->first]= std::make_pair(it->first, sum);

		}
	}

	std::sort(brute_force_result.begin(), brute_force_result.end(),compFunc);

	for(size_t i = 0; i < result.size(); i++)
	{
		if(brute_force_result[i].first != result[i].first && brute_force_result[i].second != result[i].second)
		{
			std::cout << brute_force_result[i].first << "= " << brute_force_result[i].second << endl;
			std::cout << "Correctness failed!!!\n";
		}
	}
}

int main()
{

	ofstream stats_file;
	int k = 30;

	stats_file.open("/home/dbis/workspace/TopK_dev/Files/stats.txt",std::ofstream::out | std::ofstream::app);
	stats_file << "N_PROCS," << "N_POSTING_LISTS," << "N_DOCS," << "K,"<<  "TIME (ms)\n";


	TopKAlgos* topK_algos_obj = new TopKAlgos();

	string file_path = "/home/dbis/workspace/TopK_dev/Files/postingList";
	for(int j = 1; j <= 9 ; j++)
	{
	string file_name =  std::to_string(j) + "-1000000.txt";
	string file1 = file_path+file_name;
	cout << file1 << endl;

	GeneratePostingListFromFile(file1, topK_algos_obj);

	}


	vector<pair<int, double>> result;
    double total_time = 0;
	for (int i = 0; i < 10; i++) {
		auto t1 = std::chrono::high_resolution_clock::now();
		 result = topK_algos_obj->TAFaginAlgorithm(k);
		auto t2 = std::chrono::high_resolution_clock::now();
		double time_elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(t2-t1).count();
    total_time += time_elapsed;
    	}

	topK_algos_obj->printStats();
	int nProcs = omp_get_num_procs();
	stats_file << nProcs << "," << topK_algos_obj->posting_list_vector.size() << "," <<
			topK_algos_obj->MAX_DOCS -1 <<  "," << k << "," << total_time/10 << "\n";


    cout << "TaFaginAlgorithm took "
              << total_time/10
              << " milliseconds\n";
	check_correctness(topK_algos_obj->posting_list_vector, result);
	topK_algos_obj->print(result, "Merged Top k docs are..\n");

	cout << "avg time" << total_time/10 << "\n";
	std::cout << "The End" << endl;




stats_file.close();


}
