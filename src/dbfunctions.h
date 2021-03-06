#ifndef DBFUNCTIONS_H
#define DBFUNCTIONS_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <boost/tokenizer.hpp>
#include "entry.h"
#include "dice.h"

//bag of words contaioner
const string BOW[3] = {"is", "the", "are"};
const int BOW_SIZE = 3;

string remove(string);
char* make_c_str(string);

//function to import the database to be used from the txr file
void importDataBase(vector <vector<Entry> >& db, const char* db_name){


	//temp cluster
	vector <Entry> group;
	
	//flag for first entry
	bool init_flag = 1;

	ifstream input;
	input.open(db_name);
	//error check with file opening
	if(!(input.is_open())){
		cout << "Error opening file.\n";
		exit(-1);
	}

	Entry current;			//temp object to push to vector
	string element = "";		//temp element string to push elements to vector
	int itt = 0;
	while(getline(input,element)){
		//if empty line ignore
		if(element == ""){
			continue;
		}
		//switch statement to choose which element to update
		switch(itt){
			//updating question
			case 0:
				current.SetQuestion(element);
				itt++;
				break;
			//updating answer
			case 1:
				current.SetAnswer(element);
				itt++;
				break;
			//updating cluster
			case 2:
				//string -> int
				int numb;
				istringstream(element) >> numb;
				current.SetCluster(numb);

				//cluster construction cases
				//initial entry starts as own cluster
				if(init_flag){
					group.push_back(current);
					init_flag = 0;
				}
				else{
					//if entry is part of current cluster
					if(group.at(0).GetCluster() == numb){
						group.push_back(current);
					}
					else{
						db.push_back(group);
						group.clear();
						group.push_back(current);
					}
		
				}
				itt = 0;
				break;
			//should not get here	
			default:
				cout << "Error: Should not be here\n";
				exit(0);
		}
	}
	input.close();
	
	//push last cluster once EOF
	db.push_back(group);

	//test the size
	//cout << db.size() << endl;

}

//outputs database into file
void export_DataBase(const vector<Entry>  db, const char* db_name){

	ofstream output;
	output.open(db_name);

	//error check with file opening
	if(!(output.is_open())){
		cout << "Error opening file.\n";
		exit(-1);
	}
	
	//outputing database to file
	for(int i = 0; i < db.size(); ++i){
		output << db.at(i).GetQuestion() << endl;
		output << db.at(i).GetAnswer() << endl;
		output << db.at(i).GetCluster() << endl;
		output << endl;
	}

	output.close();
	return;
}


//function to return a list of one entry from each cluster
//used for initial comparision metrics
vector <Entry> sample_list(const vector< vector <Entry> > & db){

	vector<Entry> cluster_sample;

	for(int i = 0; i < db.size(); ++i){
		cluster_sample.push_back( db.at(i).at( (db.at(i).size() / 2) ) );
	}

	/*for(int i = 0; i < cluster_sample.size(); ++i){
		cluster_sample.at(i).printEntry();	
	}*/

	return cluster_sample;
}

void sortDatabase(vector<Entry>& db){
	vector<Entry> temp;
	temp.push_back(db.at(0));
	db.erase(db.begin());
	string copy;
	while(db.size() != 0){
		int max = 0;
		int next = 0;
		for(int i = 0; i < temp.size(); ++i){
			char* tmp = make_c_str(temp.at(i).GetQuestion());
			for(int j = 0; j < db.size(); ++j){
				char* tmp2 = make_c_str(db.at(j).GetQuestion());
				int curr = dice_match(tmp, tmp2);
				if(curr > max){
					max = curr;
					next = j;
				}
				//delete tmp2;
			}
			//delete tmp;
		}
		temp.push_back(db.at(next));
		db.erase(db.begin() + next);
	}
	db = temp;
	export_DataBase(db, "database.txt");
	/*for(int i = 0; i < temp.size(); ++i){
		cout << temp.at(i).GetQuestion() << endl;
		cout << temp.at(i).GetAnswer() << endl;
		cout << temp.at(i).GetCluster() << endl;
	}*/
	return;
}

void cluster(vector<Entry> db){
	vector<Entry> temp;
	int clusterCnt = 0;
	string copy;
	temp.push_back(db.at(0));
	char* tmp = make_c_str(temp.at(0).GetQuestion());
	for(int i = 1; i< db.size(); ++i){
		char* tmp2 = make_c_str(db.at(i).GetQuestion());
		if(dice_match(tmp, tmp2) >55 ){
			temp.push_back(db.at(i));
			//delete tmp2;
		}
		else{
			for(int j = 0; j < temp.size(); ++j){
				for(int k = 0; k < db.size(); ++k){
					if(temp.at(j).GetQuestion() == db.at(k).GetQuestion()){
						db.at(k).SetCluster(clusterCnt);
					}
				}
			}
			++clusterCnt;
			while(!temp.empty()) temp.pop_back();
			temp.push_back(db.at(i));
			copy = remove(db.at(i).GetQuestion());
			//delete tmp;
			tmp = tmp2;
		}
	}
	for(int j = 0; j < temp.size(); ++j){
		for(int k = 0; k < db.size(); ++k){
			if(temp.at(j).GetQuestion() == db.at(k).GetQuestion()){
				db.at(k).SetCluster(clusterCnt);
			}
		}
	}
	export_DataBase(db, "database.txt");
	return;
}

char* make_c_str(string str){
	string copy = remove(str);
	char* tmp2 = new char[copy.length() + 1];
	for(int k = 0; k < copy.length(); ++k){
		tmp2[k] = copy.at(k);
	}
	tmp2[copy.length()] = '\0';
	return tmp2;
}

string remove(string parse){
	boost::tokenizer<> tok(parse);
	vector <string> tmp;
	for(boost::tokenizer<>::iterator beg=tok.begin(); beg!=tok.end();++beg){
		tmp.push_back(*beg);
	}
	for(int i = 0; i < tmp.size(); ++i){
		for(int j = 0; j < BOW_SIZE; ++j){
			if(tmp.at(i) == BOW[j]){
				tmp.erase(tmp.begin()+i);
				--i;
				break;
			}
		}
	}
	parse = "";
	for(int i = 0; i < tmp.size(); ++i){
		parse += tmp.at(i) + ' ';
	}
	return parse;
}

int find_cluster(vector <Entry> sample, string input){
	int max = 0;
	int cluster = -1;
	for(int i = 0; i < sample.size(); ++i){
		int tmp = dice_match(make_c_str(input), make_c_str(sample.at(i).GetQuestion()));
		if(tmp > max){
			cluster = i;
			max = tmp;
		}
	}
	return cluster;
}

string find_entry(vector<Entry> lookup, string input){
	int max = 0;
	string closest;
	for(int i = 0; i < lookup.size(); ++i){
		int tmp = dice_match(make_c_str(input), make_c_str(lookup.at(i).GetQuestion()));
		if(tmp > max){
			max = tmp;
			closest = lookup.at(i).GetAnswer();
		}
	}
	return closest;
}
#endif
