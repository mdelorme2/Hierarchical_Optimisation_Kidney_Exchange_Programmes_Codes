#include "main.h"

/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

double initTimeModelCPU;

int main(int argc, char **argv){
	
	initTimeModelCPU = getCPUTime();

	// local variables
	Allocation allo ;
	allo.infos.timeCPU.resize(7);
	string filein = argv[2];
	string path = argv[1];
	string pathAndFileout = argv[3];
	
	// functions
	allo.load(path,filein);
	allo.printProb();

	allo.infos.timeCPU[1] =  getCPUTime() - initTimeModelCPU;
	for(int i=0;i<=4;i++){
		cycle(allo);
		allo.infos.timeCPU[i+2] =  getCPUTime() - initTimeModelCPU;
		for(int j=0;j<=i;j++){
			allo.infos.timeCPU[i+2]  -= allo.infos.timeCPU[j+1];
		}
	}
	allo.infos.timeCPU[0] =  getCPUTime() - initTimeModelCPU;

//	allo.printSol();
	//allo.checkSolution();
	allo.printInfo(pathAndFileout);
}


int cycle(Allocation& allo){

	GRBEnv env = GRBEnv();

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun1 = 0;
		GRBLinExpr objFun2 = 0;
		GRBLinExpr objFun3 = 0;
		GRBLinExpr objFun4 = 0;
		GRBLinExpr objFun5 = 0;

		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++){
			isCycleUsed[i] = model.addVar(0, 1, 0, GRB_BINARY);
		}

		if(allo.startC.size() == allo.cycles.size()){
			for (int i = 0; i < allo.cycles.size(); i++) isCycleUsed[i].set(GRB_DoubleAttr_Start, allo.startC[i]);
		}

		for (int i = 0; i < allo.maxId+1; i++){
			isPatientUsed[i] = 0;
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.cycles.size(); i++){
			for(int j=0;j<allo.cycles[i].idX.size();j++){
				isPatientUsed[allo.cycles[i].idX[j]] += isCycleUsed[i];
				isPatientIdUsed[allo.cycles[i].idX[j]] = true;
			}
			objFun1 += allo.cycles[i].idX.size() * isCycleUsed[i];
			if(allo.cycles[i].idX.size() == 4) objFun2 += isCycleUsed[i];
			if(allo.cycles[i].idX.size() == 3) objFun3 += isCycleUsed[i];
			objFun4 += allo.cycles[i].nbBA * isCycleUsed[i];
			objFun5 += allo.cycles[i].score * isCycleUsed[i];
		}

		// Unique assignment for patients
		for (int i = 0; i < allo.maxId+1; i++){
			if(isPatientIdUsed[i] == true) model.addConstr(isPatientUsed[i] <= 1);
		}

		if(allo.objs.size() == 0){
			model.setObjective(objFun1, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 1){
			model.addConstr(objFun1 == allo.objs[0]);
			model.setObjective(objFun2, GRB_MINIMIZE);
		}

		if(allo.objs.size() == 2){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.setObjective(objFun3, GRB_MINIMIZE);
		}

		if(allo.objs.size() == 3){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.addConstr(objFun3 == allo.objs[2]);
			model.setObjective(objFun4, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 4){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.addConstr(objFun3 == allo.objs[2]);
			model.addConstr(objFun4 == allo.objs[3]);
			model.setObjective(objFun5, GRB_MAXIMIZE);
		}

				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600000 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.optimize();

		// Filling Info
		allo.infos.UB = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
		allo.infos.opt = false;

		// Get Info pre preprocessing
		allo.infos.nbVar =  model.get(GRB_IntAttr_NumVars);
		allo.infos.nbCons = model.get(GRB_IntAttr_NumConstrs);
		allo.infos.nbNZ = model.get(GRB_IntAttr_NumNZs);

		// If no solution found
		if (model.get(GRB_IntAttr_SolCount) < 1){
			cout << "Failed to optimize ILP. " << endl;
			allo.infos.LB  = 0;
			return -1;
		}

		// If solution found
		allo.infos.LB = ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON);	
		if(allo.infos.LB == allo.infos.UB) allo.infos.opt = true;
		allo.objs.push_back(ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON));

		allo.startC.resize(0);
		// Filling Solution
		for (int i = 0; i < allo.cycles.size(); i++){
			if(ceil(isCycleUsed[i].get(GRB_DoubleAttr_X) - EPSILON) == 1){
				allo.cycles[i].print();
				allo.startC.push_back(1);
			}
			else allo.startC.push_back(0);
		}
	}

	// Exceptions
	catch (GRBException e) {
		cout << "Error code = " << e.getErrorCode() << endl;
		cout << e.getMessage() << endl;
	}
	catch (...) {
		cout << "Exception during optimization" << endl;
	}


	// End
	return 0;
}
