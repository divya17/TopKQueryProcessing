/*
 * TopKAlgos.h
 *
 *  Created on: Jun 26, 2017
 *      Author: divya
 */

#ifndef TOPKALGOS_H_
#define TOPKALGOS_H_

#include<vector>
#include <omp.h>
#include <iostream>
#include<unordered_set>
#include <memory>
#include "../utils/posting_list.h"

using namespace std;


typedef shared_ptr<CPostingList> posting_list;
class TopKAlgos
{
public:
	int MAX_DOCS;
	int BLOCK_SIZE;

	/* input for the top-k algorithms */
	vector<posting_list> posting_list_vector;


	/*Compute overall score for every object by looking into
	each sorted list and return the top-k with k highest overall scores */
	//vector<int> NaiveAlgorithm(vector<CPostingList*>& posting_lists_vector, int k);

	/*Sequentially access all the sorted lists in parallel until there are k objects that have been seen in all lists */
	//vector<int> FaginAlgorithm(vector<CPostingList*>& posting_lists_vector, int k);

	/*
	******* TA Algorithm ************
	1. Access the elements sequentially
	2. At each sequential access
	(a) Set the threshold t to be the aggregate of the scores seen in this
	access.
	(b) Do random accesses and compute the scores of the seen
	objects.
	(c) Maintain a list of top-k objects seen so far
	(d) Stop, when the scores of the top-k are greater or equal to the
	threshold.
	3. Return the top-k seen so far
	*/
	vector<pair<int,double>> TAFaginAlgorithm(int k);


	vector<pair<int,double>> calcTopK(long start, long end, int k, std::unordered_set<int>& docs_seen_set);

	/*
	 ******* NRA Algorithm ************
	 * 1. Access sequentially all lists in parallel until there are k
	 objects for which the lower bound is higher than the upper
	 bound of all other objects.
	2. Return top-k objects for which the lower bound is higher
	than the upper bound of all other objects.
	*/
	//vector<int> NoRandomAccessAlgorithm(vector<CPostingList*>& posting_lists_vector, int k);

	/* Utility Functions */

	void print(vector<pair<int,double>>& print_vector,string message);
	void TopKInsert(pair<int,double> p, vector<pair<int, double>>& topK_vector);
	vector<pair<int, double>> MergeTopK(vector<pair<int, double>>& shared_topK_vector, vector<pair<int, double>> topK_vector);
	/*
	 * Constructors
	 */
	TopKAlgos(){}




	/* Print utilities */
	void printStats()
	{
		int nProcs = omp_get_num_procs();
		cout << "num of procs =" << nProcs << endl;
		cout << "posting list vector size = " << posting_list_vector.size() << endl;
		cout << "MAX DOCS =" << posting_list_vector[1]->doc_list_size <<endl;
		cout << "BLOCK_SIZE =" << MAX_DOCS/ omp_get_num_procs() <<endl;
	}

};

#endif /* TOPKALGOS_H_ */
