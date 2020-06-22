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
	allo.fails.resize(4,0);
	allo.RC.resize(4);
	allo.tObjs.resize(4,-1);
	allo.sols.resize(5,-1);

	bool back0 = false; bool back1 = false;
	int currFail1 = 0;
	int currFail2 = 0;
	int nbC1 = 2;
	int nbC2 = 2;

	for(int i=0;i<=4;i++){
		if(i == 0){
			if(back0){
				allo.fails[0]++; allo.sols[0]--; back0 = false; back1 = false; allo.objs.resize(1); allo.objs[0] = allo.sols[0];
			}
			else{
				cycleLP(allo); allo.sols[0] = floor(allo.tObjs[0] + EPSILON);
				allo.objs.push_back(allo.sols[0]);
			}
			cout << "Sol0 is at " << allo.sols[0] << endl;
			// Deactivation
			for (int j = 0; j < allo.cycles.size(); j++){
				if(allo.tObjs[0] + allo.RC[0][j] + EPSILON <  allo.sols[0]) allo.isActivated[j] = -1;
				else allo.isActivated[j] = 1;
			}
		}
		if(i == 1){
			if(back1){
				allo.fails[1]++; allo.sols[1]++; currFail1++; back1 = false; allo.objs.resize(2); allo.objs[1] = allo.sols[1];
			}
			else{
				if(cycleLP(allo) == -1){
					allo.fails[1]++; i = -1; back0 = true; currFail1 = 0;
				}
				else{ 
					allo.sols[1] = ceil(allo.tObjs[1] - EPSILON);
					allo.objs.push_back(allo.sols[1]);
				}
			}
			cout << "Sol0 is at " << allo.sols[0] << endl;	
			if(currFail1 < nbC1 - 1){	
				cout << "Sol1 is at " << allo.sols[1] << endl;	
				for (int j = 0; j < allo.cycles.size(); j++){
					if (allo.tObjs[0] + allo.RC[0][j] + EPSILON < allo.sols[0] || allo.tObjs[1] + allo.RC[1][j] - EPSILON > allo.sols[1]) allo.isActivated[j] = -1;
					else allo.isActivated[j] = 1;
				}
			}
			else{
				cout << " --- Last try SECOND OBJ --- " << endl;
				allo.sols[1] = -1;
				for (int j = 0; j < allo.cycles.size(); j++){
					if(allo.tObjs[0] + allo.RC[0][j] + EPSILON < allo.sols[0]) allo.isActivated[j] = -1;
					else allo.isActivated[j] = 1;
					allo.objs.resize(1);
				}
				if (cycle(allo) == 0 && (allo.objs[1] == allo.sols[1] || currFail1 == nbC1 - 1)){
					allo.sols[1] = allo.objs[1];
					for (int j = 0; j < allo.cycles.size(); j++){
						if (allo.isActivated[j] >= 0){
							if(allo.tObjs[1] + allo.RC[1][j] - EPSILON > allo.objs[1]) allo.isActivated[j] = -1;
						}
					}
				}		
				else{
					i = -1; back0 = true; currFail1 = 0;
				}
			}
		}
		if(i == 2){
			if(cycleLP(allo) == -1){
				allo.fails[2]++; i = 0; back1 = true; currFail2 = 0;
			}
			else{
				allo.sols[2] = ceil(allo.tObjs[i] - EPSILON);
				for(;;){
					if(getCPUTime() - initTimeModelCPU > 36000){allo.objs[2] = -1; allo.infos.opt = false; break;}
					cout << "Sol0 is at " << allo.sols[0] << endl;
					cout << "Sol1 is at " << allo.sols[1] << endl;
					// Deactivation
					if(currFail2 < nbC2 - 1){	
						cout << "Sol2 is at " << allo.sols[2] << endl;
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivated[j] >= 0){
								if(allo.tObjs[2] + allo.RC[2][j] - EPSILON > allo.sols[2]) allo.isActivated[j] = 0;
								else allo.isActivated[j] = 1;
							}
						}
					}
					else{
						cout << " --- Last try THIRD OBJ--- " << endl;
						allo.sols[2] = -1;
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivated[j] >= 0) allo.isActivated[j] = 1;
						}
					}
					if (cycle(allo) == 0 && (allo.objs[2] == allo.sols[2] || currFail2 == nbC2 - 1)){
						allo.sols[2] = allo.objs[2];
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivated[j] >= 0){
								if(allo.tObjs[2] + allo.RC[2][j] - EPSILON > allo.objs[2]) allo.isActivated[j] = -1;
							}
						}
						break;
					}		
					else{
						allo.objs.resize(2);
						if(getCPUTime() - initTimeModelCPU < 36000){ allo.fails[2]++; allo.sols[2]++; currFail2++;}	
					}
					if(currFail2 == nbC2){i = 0; back1 = true; currFail2 = 0; break;}
				}
			}
		}
		if(i == 3){
			cycleLP(allo);
			allo.sols[3] = floor(allo.tObjs[i] + EPSILON);
			for(;;){
				if(getCPUTime() - initTimeModelCPU > 36000){ allo.objs[3] = -1; allo.infos.opt = false; break;}
				cout << "Sol3 is at " << allo.sols[3] << endl;
				// Deactivation
				for (int j = 0; j < allo.cycles.size(); j++){
					if (allo.isActivated[j] >= 0){
						if(allo.tObjs[3] + allo.RC[3][j] + EPSILON < allo.sols[3]) allo.isActivated[j] = 0;
						else allo.isActivated[j] = 1;
					}
				}
				if (cycle(allo) == 0 && allo.objs[3] == allo.sols[3]){
					for (int j = 0; j < allo.cycles.size(); j++){
						if (allo.isActivated[j] == 0) allo.isActivated[j] = -1;
					}
					break;
				}		
				else{
					allo.objs.resize(3);
					if(getCPUTime() - initTimeModelCPU < 36000){
						allo.fails[3]++; allo.sols[3]--;
					}	
				}
			}
		}
		if(i==4){
			cycle(allo);
		}
		allo.infos.timeCPU[i+2] =  getCPUTime() - initTimeModelCPU;
		for(int j=0;j<=i;j++) allo.infos.timeCPU[i+2]  -= allo.infos.timeCPU[j+1];
	}
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
		if(model.get(GRB_IntAttr_Status) == 3 || model.get(GRB_IntAttr_Status) == 4) return -1;

		allo.tObjs[allo.objs.size()] = model.get(GRB_DoubleAttr_ObjVal);	

		// Filling Solution
		allo.RC[allo.objs.size()].resize(0);
		allo.RC[allo.objs.size()].resize(allo.cycles.size(), 0);
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivated[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) < EPSILON){
					allo.RC[allo.objs.size()][i] = isCycleUsed[i].get(GRB_DoubleAttr_RC);
				}
				else{
					allo.RC[allo.objs.size()][i] = 0.0;
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
		if(allo.sols[allo.objs.size()] != -1){
		//	cout << " cut at " <<allo.sols[allo.objs.size()] << " " << " for obj " << allo.objs.size() << " because sol is at " << allo.sols[allo.objs.size()] << endl;
			model.getEnv().set(GRB_DoubleParam_Cutoff, allo.sols[allo.objs.size()]);
		}
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
