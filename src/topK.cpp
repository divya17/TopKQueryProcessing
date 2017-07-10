/*
 * TopKAlgos.cpp
 *
 *  Created on: Jun 26, 2017
 *      Author: divya
 */

#include "topK.h"

#include<vector>
#include<unordered_map>
#include<unordered_set>
#include<iostream>
#include <omp.h>


void TopKAlgos::print(vector<pair<int,double>>& print_vector,string message)
{
	cout << message << endl;
	for(size_t i = 0; i < print_vector.size(); i++)
	{
		cout << print_vector[i].first << " = " << print_vector[i].second << endl;
	}

}

vector<pair<int, double>> TopKAlgos::MergeTopK(vector<pair<int, double>>& shared_topK_vector, vector<pair<int, double>> topK_vector)
{
 int i = 0;
 int j = 0;
 int k = shared_topK_vector.size();
 int count_k = 0;
 vector<pair<int,double>> merged_vector(k,std::make_pair(0,0.0));

 while( count_k != k)
 {
	 if(topK_vector[i].second > shared_topK_vector[j].second)
	 {
		merged_vector[count_k] = topK_vector[i];
		i++;
	 }
	 else
	 {
		 merged_vector[count_k] = shared_topK_vector[j];
		 j++;
	 }
	 count_k++;
 }
 return merged_vector;
}

void TopKAlgos::TopKInsert(pair<int,double> p, vector<pair<int, double>>& topK_vector)
{
	if(topK_vector[topK_vector.size()-1].second < p.second)
	{
	for(int i = topK_vector.size()-2 ; i >= 0; i--)
			{
				if(p.second > topK_vector[i].second)
				{
					topK_vector[i+1] = topK_vector[i];
				}
				else
				{
					topK_vector[i+1].second = p.second;
					topK_vector[i+1].first = p.first;

					break;
				}
			}
	if(p.second >= topK_vector[0].second)
	{
		topK_vector[0].second = p.second;
		topK_vector[0].first = p.first;
	}
	}

}

vector<pair<int,double>> TopKAlgos::calcTopK(long start, long end, int k, std::unordered_set<int>& docs_seen_set)
{
	vector<pair<int,double> > local_topK(k, std::make_pair(-1, 0.0));
	for(int i = start; i <= end; i++) //loop through every row of the posting list
	{
		double threshold = 0.0;

		for(size_t ii = 0; ii < posting_list_vector.size(); ii++) // loop across posting lists
		{
			int current_doc_id = posting_list_vector[ii]->doc_list[i];
			double current_topk_score = 0;
			threshold+= posting_list_vector[ii]->getScore(current_doc_id);
#pragma omp critical
			{
			std::unordered_set<int>::const_iterator got = docs_seen_set.find (current_doc_id);
			if(got == docs_seen_set.end())
			{
			for(size_t j = 0; j < posting_list_vector.size(); j++)
			{
				current_topk_score += posting_list_vector[j]->getScore(current_doc_id);

			}
			docs_seen_set.insert(current_doc_id);
			TopKInsert(std::make_pair(current_doc_id,current_topk_score), local_topK);
			}
			}


		}
	if(local_topK[k-1].second > 0.0 && local_topK[k-1].second >= threshold) // stop when the minimum value in the Top K is greater threshold
		{
			break;
		}

	}
 return local_topK;
}

vector<pair<int,double>> TopKAlgos::TAFaginAlgorithm(int k)
{

	int nProcs = omp_get_num_procs();
	MAX_DOCS = posting_list_vector[1]->doc_list_size;
	BLOCK_SIZE = MAX_DOCS/ omp_get_num_procs();
	std::unordered_set<int> docs_seen_set;

	vector<pair<int,double>> shared_topK (k, std::make_pair(0, 0.0));

#pragma omp parallel num_threads(nProcs)
	{
		std::vector<pair<int,double>> local_topK; // local topK recorded for each parallel block
#pragma omp for nowait
	for(int i = 0; i < nProcs; i++) // Threads that finish early can proceed to merge with shared_topK as soon as they finish (no wait)
	{
		//cout << "calculating local topK for ::" << i << endl;
		local_topK = calcTopK(i*BLOCK_SIZE, i*BLOCK_SIZE + BLOCK_SIZE - 1, k, docs_seen_set);\
	}
	#pragma omp critical
	{
	//print(local_topK, "Local topK");
	shared_topK = MergeTopK(shared_topK, local_topK);
	//cout << "Finished Merging for \n";
	}

	}

	return shared_topK;
}


