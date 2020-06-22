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

	allo.isActivated.resize(allo.cycles.size(),1);

	for(int i=0;i<=3;i++){
		allo.fails.push_back(0);
		cycleLP(allo);
		int sol;
		if(i == 0 || i == 3) sol = floor(allo.tObjs[i] + EPSILON);
		else sol = ceil(allo.tObjs[i] - EPSILON);			
		for(;;){
			if(getCPUTime() - initTimeModelCPU > 36000){
				allo.objs[i] = -1;
				allo.infos.opt = false;
				break;
			}
			cout << "Sol " << i << " is at " << sol << endl;
			// Deactivation
			for (int j = 0; j < allo.cycles.size(); j++){
				if(i == 0 || i == 3){
					if (allo.isActivated[j] >= 0){
						if(allo.tObjs[i] + allo.RC[j] + EPSILON <  sol){
							allo.isActivated[j] = 0;
						}
						else {
							allo.isActivated[j] = 1;
						}
					}
				}
				else{
					if (allo.isActivated[j] >= 0){
						if( allo.tObjs[i] + allo.RC[j] - EPSILON > sol){
							allo.isActivated[j] = 0;
						}
						else {
							allo.isActivated[j] = 1;
						}
					}
				}
			}
			if (cycle(allo) == 0 && allo.objs[i] == sol){
				for (int j = 0; j < allo.cycles.size(); j++){
					if (allo.isActivated[j] == 0){  
						allo.isActivated[j] = -1;
					}
				}
				break;
			}		
			else{
				allo.objs.resize(i);
				if(getCPUTime() - initTimeModelCPU < 36000){
					allo.fails[i]++;
					if(i == 0 || i == 3) sol--;
					else sol++;
				}	
			}
		}
		allo.infos.timeCPU[i+2] =  getCPUTime() - initTimeModelCPU;
		for(int j=0;j<=i;j++) allo.infos.timeCPU[i+2]  -= allo.infos.timeCPU[j+1];
	}
	cycle(allo);
	allo.infos.timeCPU[6] =  getCPUTime() - initTimeModelCPU;
	for(int j=0;j<=4;j++) allo.infos.timeCPU[6]  -= allo.infos.timeCPU[j+1];

	allo.infos.timeCPU[0] =  getCPUTime() - initTimeModelCPU;

	allo.printInfo(pathAndFileout);
}

int cycleLP(Allocation& allo){

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
			if(allo.isActivated[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_CONTINUOUS);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++){
			isPatientUsed[i] = 0;
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivated[i] == 1){
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
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  36000 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_IntParam_Crossover, 0); 
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.optimize();

		// If solution found
		allo.tObjs.push_back(model.get(GRB_DoubleAttr_ObjVal));	

		// Filling Solution
		allo.RC.resize(0);
		allo.RC.resize(allo.cycles.size(), 0);
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivated[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) < EPSILON){
					allo.RC[i] = isCycleUsed[i].get(GRB_DoubleAttr_RC);
				}
				else{
					allo.RC[i] = 0.0;
				}
				if(i < 0){
					cout << isCycleUsed[i].get(GRB_DoubleAttr_X) << " (" << isCycleUsed[i].get(GRB_DoubleAttr_RC) << ") x " ; 
					allo.cycles[i].print() ;
				}
			}
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
			if(allo.isActivated[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++){
			isPatientUsed[i] = 0;
		}

		model.update();

		// Perform values
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivated[i] == 1){
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
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  36000 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
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

		// Filling Solution
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivated[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) > 0.001){
					cout << isCycleUsed[i].get(GRB_DoubleAttr_X) << " x " ; 
					allo.cycles[i].print() ;
				}
			}
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
