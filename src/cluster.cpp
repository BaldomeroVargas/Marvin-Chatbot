/*used when a new entry is added to the 
database. The database is reclustered for 
more accurate use when the FAQ bot runs*/

#include "dbfunctions.h"

using namespace std;

int main(int argc, char** argv){

	//arguement count error check
	if(argc != 2){
		cout << "Error: Please use [exectuable] [database] format to run program.\n";
		return -1;
	}

	//importing data base in clusters
	vector < vector<Entry> > DataBase;
	importDataBase(DataBase, argv[1]);

	//sample gets single entry from each cluster
	//in dbfunctions
	vector<Entry> db_cluster;
	db_cluster = sample_list(DataBase);

	vector <Entry> tmp;
	for(int i = 0; i  < DataBase.size(); ++i){
		for(int j = 0; j < DataBase.at(i).size(); ++j){
			tmp.push_back(DataBase.at(i).at(j));
		}
	}
	sortDatabase(tmp);
	cluster(tmp);
	cout << "Done clustering Database" << endl;
	return 0;
}
