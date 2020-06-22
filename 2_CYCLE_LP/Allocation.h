#ifndef ALLOCATION_H
	#define ALLOCATION_H
	
	using namespace std;
	#include <iostream> 
	#include <iomanip> 
	#include <fstream>
	#include <sstream>
	#include <vector>
	#include <string>
	#include <set>

	class Altruistic;
	class Doctor;
	class Info;
	class Cycle;
	class Allocation;


/*	*************************************************************************************
	*******************************  ALTRUISTIC *****************************************
	************************************************************************************* */

	class Altruistic{
	public:
		int id;
		int age;
		int nbEdges;
		vector<int> targets;
		vector<int> scores;
		int blood;

		void print();
	};

/*	*************************************************************************************
	***********************************  PAIR  ******************************************
	************************************************************************************* */

	class Pair{
	public:
		int id;
		int source;
		int age;
		int nbEdges;
		vector<int> targets;
		vector<int> scores;
		int bloodD;
		int bloodP;
		int wt;
		double pra;
		bool hs;
		void print();
	};



/*	*************************************************************************************
	************************************* INFO ******************************************
	************************************************************************************* */

	class Info{
	public:
		bool opt;
		vector<double> timeCPU;
		int LB;
		int UB;
		float contUB;
		int nbCons;
		int nbVar;
		int nbNZ;
		float contUB2;
		int nbCons2;
		int nbVar2;
		int nbNZ2;
	};

/*	*************************************************************************************
	************************************* CYCLE *****************************************
	************************************************************************************* */

	class Cycle{
	public:
		int id;
		int size;
		int isC;
		int nbBA;
		int score;
		vector<int> idX;

		void print();
	};

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */
	class Allocation{
	public:
		
		// Data read from the file
		string name;
		int nbAltruistics;
		int nbPairs;
		vector<vector<int> > idToIdxA;
		vector<vector<int> > idToIdxP;
		int maxId;

		vector<Altruistic> altruistics; 
		vector<Pair> pairs;
		vector<Cycle> cycles;
		vector<vector<int> > comp;
		vector<vector<int> > scores;

		vector<int> isActivated;
		vector<int> types;

		// Given by the ILP model
		Info infos;
		vector<int> objs;

		// Given by the LP model
		vector<double> tObjs;
		vector<double> RC;
		vector<int> fails;

		void load(const string& path, const string& filein);
		void printProb();
		void printInfo(const string& pathAndFileout);
	};
	

#endif 