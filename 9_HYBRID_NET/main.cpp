#include "main.h"

/*	*************************************************************************************
	*************************************  MAIN *****************************************
	************************************************************************************* */

double initTimeModelCPU;

int main(int argc, char **argv){
	
	initTimeModelCPU = getCPUTime();

	// local variables
	Allocation allo ;
	allo.infos.timeCPU.resize(6);
	string filein = argv[2];
	string path = argv[1];
	string pathAndFileout = argv[3];
	
	// functions
	allo.load(path,filein);
	allo.printProb();

	allo.infos.timeCPU[1] =  getCPUTime() - initTimeModelCPU;
	
	allo.isActivatedA.resize(allo.arcs.size(),1);
	allo.isActivatedC.resize(allo.cycles.size(),1);

	allo.fails.resize(4,0);
	allo.RCA.resize(4);
	allo.RCC.resize(4);
	allo.tObjs.resize(4,-1);
	allo.sols.resize(4,-1);
	
	bool back0 = false; bool back1 = false; 
	int currFail1 = 0;
	int currFail2 = 0;
	int nbC1 = 2;
	int nbC2 = 2;
	
	for(int i=0;i<=3;i++){
		if(i == 0){
			if(back0){
				allo.fails[0]++; allo.sols[0]--; back0 = false; back1 = false; allo.objs.resize(1); allo.objs[0] = allo.sols[0];
			}
			else{
				picefLP(allo); allo.sols[0] = floor(allo.tObjs[0] + EPSILON);
				allo.objs.push_back(allo.sols[0]);
			}
			cout << "Sol0 is at " << allo.sols[0] << endl;
			// Deactivation Cycle
			for (int j = 0; j < allo.cycles.size(); j++){
				if(allo.tObjs[0] + allo.RCC[0][j] + EPSILON <  allo.sols[0]) allo.isActivatedC[j] = -1;
				else allo.isActivatedC[j] = 1;
			}
			// Deactivation Arc
			for (int j = 0; j < allo.arcs.size(); j++){
				if(allo.tObjs[0] + allo.RCA[0][j] + EPSILON <  allo.sols[0]) allo.isActivatedA[j] = -1;
				else allo.isActivatedA[j] = 1;
			}
		}
		if(i == 1){
			if(back1){
				allo.fails[1]++; allo.sols[1]--; currFail1++; back1 = false; allo.objs.resize(2); allo.objs[1] = allo.sols[1];
			}
			else{
				if(picefLP(allo) == -1){
					allo.fails[1]++; i = -1; back0 = true; currFail1 = 0;
				}
				allo.sols[1] = floor(allo.tObjs[1] + EPSILON);
				allo.objs.push_back(allo.sols[1]);
			}
			if(allo.sols[1] >= 0){
				cout << "Sol0 is at " << allo.sols[0] << endl;	
				if(currFail1 < nbC1 - 1){	
					cout << "Sol1 is at " << allo.sols[1] << endl;	
					// Deactivation Cycle
					for (int j = 0; j < allo.cycles.size(); j++){
						if (allo.tObjs[0] + allo.RCC[0][j] + EPSILON < allo.sols[0] || allo.tObjs[1] + allo.RCC[1][j] + EPSILON < allo.sols[1]) allo.isActivatedC[j] = -1;
						else allo.isActivatedC[j] = 1;
					}
					// Deactivation Arc
					for (int j = 0; j < allo.arcs.size(); j++){
						if (allo.tObjs[0] + allo.RCA[0][j] + EPSILON < allo.sols[0] || allo.tObjs[1] + allo.RCA[1][j] + EPSILON < allo.sols[1]) allo.isActivatedA[j] = -1;
						else allo.isActivatedA[j] = 1;
					}
				}
				else{
					cout << " --- Last try SECOND OBJ --- " << endl;
					allo.sols[1] = -1;
					allo.objs.resize(1);
					// Deactivation Cycle
					for (int j = 0; j < allo.cycles.size(); j++){
						if(allo.tObjs[0] + allo.RCC[0][j] + EPSILON < allo.sols[0]) allo.isActivatedC[j] = -1;
						else allo.isActivatedC[j] = 1;		
					}
					// Deactivation Arc
					for (int j = 0; j < allo.arcs.size(); j++){
						if(allo.tObjs[0] + allo.RCA[0][j] + EPSILON < allo.sols[0]) allo.isActivatedA[j] = -1;
						else allo.isActivatedA[j] = 1;		
					}
					if (picef(allo) == 0 && (allo.objs[1] == allo.sols[1] || currFail1 == nbC1 - 1)){
						allo.sols[1] = allo.objs[1];
						// Deactivation Cycle
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0){
								if(allo.tObjs[1] + allo.RCC[1][j] + EPSILON < allo.objs[1]) allo.isActivatedC[j] = -1;
							}
						}
						// Deactivation Arc
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0){
								if(allo.tObjs[1] + allo.RCA[1][j] + EPSILON < allo.objs[1]) allo.isActivatedA[j] = -1;
							}
						}
					}		
					else{
						i = -1; back0 = true; currFail1 = 0;
					}
				}
			}
		}
		if(i == 2){
			if(picefLP(allo) == -1){
				allo.fails[2]++; i = 0; back1 = true; currFail2 = 0;
			}
			else{
				allo.sols[2] = floor(allo.tObjs[i] + EPSILON);
				for(;;){
					if(getCPUTime() - initTimeModelCPU > 36000){ allo.objs[2] = -1; allo.infos.opt = false; break;}
					cout << "Sol0 is at " << allo.sols[0] << endl;
					cout << "Sol1 is at " << allo.sols[1] << endl;
					// Deactivation
					if(currFail2 < nbC2 - 1){	
						cout << "Sol2 is at " << allo.sols[2] << endl;
						// Deactivation Cycle
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0){
								if(allo.tObjs[2] + allo.RCC[2][j] + EPSILON < allo.sols[2]) allo.isActivatedC[j] = 0;
								else allo.isActivatedC[j] = 1;	
							}
						}
						// Deactivation Arc
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0){
								if(allo.tObjs[2] + allo.RCA[2][j] + EPSILON < allo.sols[2]) allo.isActivatedA[j] = 0;
								else allo.isActivatedA[j] = 1;	
							}
						}
					}
					else{
						cout << " --- Last try SECOND OBJ --- " << endl;
						allo.sols[2] = -1;
						// Deactivation Cycle
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0) allo.isActivatedC[j] = 1;
						}
						// Deactivation Arc
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0) allo.isActivatedA[j] = 1;
						}
					}
					if (picef(allo) == 0 && (allo.objs[2] == allo.sols[2] || currFail2 == nbC2 - 1)){
						allo.sols[2] = allo.objs[2];
						// Deactivation Cycle
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0){
								if(allo.tObjs[2] + allo.RCC[2][j] + EPSILON < allo.objs[2]) allo.isActivatedC[j] = -1;
							}
						}
						// Deactivation Arc
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0){
								if(allo.tObjs[2] + allo.RCA[2][j] + EPSILON < allo.objs[2]) allo.isActivatedA[j] = -1;
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
			picef(allo);
		}
		allo.infos.timeCPU[i+2] =  getCPUTime() - initTimeModelCPU;
		for(int j=0;j<=i;j++) allo.infos.timeCPU[i+2]  -= allo.infos.timeCPU[j+1];
	}
	allo.infos.timeCPU[0] =  getCPUTime() - initTimeModelCPU;
	allo.printInfo(pathAndFileout);
}

int picefLP(Allocation& allo){

	GRBEnv env = GRBEnv();

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun1 = 0;
		GRBLinExpr objFun2 = 0;
		GRBLinExpr objFun3 = 0;
		GRBVar of4 = model.addVar(0, 4, 0, GRB_CONTINUOUS);
		GRBLinExpr objFun4 = 0;

		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		vector<GRBVar> isArcUsed (allo.arcs.size());
		vector<vector<GRBLinExpr> > cIn(4);
		vector<vector<GRBLinExpr> > cOut(4);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_CONTINUOUS);
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				isArcUsed[i] = model.addVar(0, 1, 0, GRB_CONTINUOUS);
			}
		}
	
		for (int i = 0; i < allo.maxId+1; i++) 
			isPatientUsed[i] = 0;

		for (int i = 0; i < 4; i++){
			cIn[i].resize(allo.maxId + 2);
			cOut[i].resize(allo.maxId + 2);
			for(int j=0;j<allo.maxId + 2;j++){
				cIn[i][j] = 0; cOut[i][j] = 0; 
			}	
		}
		model.update();

		// Perform values
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				for(int j=0;j<allo.cycles[i].idX.size();j++){
					isPatientUsed[allo.cycles[i].idX[j]] += isCycleUsed[i];
					isPatientIdUsed[allo.cycles[i].idX[j]] = true;
				}
				objFun1 += allo.cycles[i].idX.size() * isCycleUsed[i];
				objFun2 += allo.cycles[i].score * isCycleUsed[i];
				objFun3 += allo.cycles[i].nbDTM * isCycleUsed[i];
				model.addConstr(of4 >= allo.cycles[i].idX.size() * isCycleUsed[i]);
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				cIn[allo.arcs[i][2]][allo.arcs[i][1]] += isArcUsed[i];
				cOut[allo.arcs[i][2]][allo.arcs[i][0]] += isArcUsed[i];
				isPatientUsed[allo.arcs[i][0]] +=  isArcUsed[i];
				isPatientIdUsed[allo.arcs[i][0]] = true;
				objFun1 += isArcUsed[i];
				objFun2 += allo.scores[allo.arcs[i][0]][allo.arcs[i][1]] * isArcUsed[i];
				objFun3 += allo.isDTM[allo.arcs[i][0]]  * isArcUsed[i];;
				if(allo.arcs[i][2] == 3) model.addConstr(of4 >= 4 * isArcUsed[i]); 
				if(allo.arcs[i][2] == 2 && allo.arcs[i][1] == allo.maxId + 1) model.addConstr(of4 >= 3 * isArcUsed[i]); 
			}
		}

		// Unique assignment for patients
		for (int i = 0; i < allo.maxId+1; i++){
			if(isPatientIdUsed[i] == true) model.addConstr(isPatientUsed[i] <= 1);
		}

		// Flow conservation
		for (int i = 0; i < allo.maxId+1; i++){
			if(allo.nodes1[i]) model.addConstr(cOut[1][i] == cIn[0][i]);
			if(allo.nodes2[i]) model.addConstr(cOut[2][i] == cIn[1][i]);	
			if(allo.nodes3[i]) model.addConstr(cOut[3][i] == cIn[2][i]);
		}

		if(allo.objs.size() == 0){
			model.setObjective(objFun1, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 1){
			model.addConstr(objFun1 == allo.objs[0]);
			model.setObjective(objFun2, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 2){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.setObjective(objFun3, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 3){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.addConstr(objFun3 == allo.objs[2]);
			model.setObjective(objFun4, GRB_MINIMIZE);
		}

		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  360000 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_IntParam_Crossover, 0);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.optimize();

		if(model.get(GRB_IntAttr_Status) == 3 || model.get(GRB_IntAttr_Status) == 4) return -1;

		// If solution found
		allo.tObjs[allo.objs.size()] = model.get(GRB_DoubleAttr_ObjVal);	

		// Filling Solution
		allo.RCC[allo.objs.size()].resize(0);
		allo.RCC[allo.objs.size()].resize(allo.cycles.size(), 0);
		allo.RCA[allo.objs.size()].resize(0);
		allo.RCA[allo.objs.size()].resize(allo.arcs.size(), 0);
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) < EPSILON)
					allo.RCC[allo.objs.size()][i] = isCycleUsed[i].get(GRB_DoubleAttr_RC);
				else
					allo.RCC[allo.objs.size()][i] = 0.0;
				if(i < 0){
					cout << isCycleUsed[i].get(GRB_DoubleAttr_X) << " (" << isCycleUsed[i].get(GRB_DoubleAttr_RC) << ") x " ; 
					allo.cycles[i].print() ;
				}
			}
		}
		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				if(isArcUsed[i].get(GRB_DoubleAttr_X) < EPSILON){
					allo.RCA[allo.objs.size()][i] = isArcUsed[i].get(GRB_DoubleAttr_RC);
				}
				else{
					allo.RCA[allo.objs.size()][i] = 0.0;
				}
				if(i < 0){
					cout << allo.arcs[i][0] << " " << allo.arcs[i][1] << " " << allo.arcs[i][2] << " "  << isArcUsed[i].get(GRB_DoubleAttr_X) << " (" << isArcUsed[i].get(GRB_DoubleAttr_RC) << ")" << endl; 
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

int picef(Allocation& allo){

	GRBEnv env = GRBEnv();

	// Model
	try{
		// Local variables
		GRBModel model = GRBModel(env);
		GRBLinExpr objFun1 = 0;
		GRBLinExpr objFun2 = 0;
		GRBLinExpr objFun3 = 0;
		GRBVar of4 = model.addVar(0, 4, 0, GRB_INTEGER);
		GRBLinExpr objFun4 = of4;

		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		vector<GRBVar> isArcUsed (allo.arcs.size());
		vector<vector<GRBLinExpr> > cIn(4);
		vector<vector<GRBLinExpr> > cOut(4);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++) {
			if(allo.isActivatedC[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				isArcUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		if(allo.startA.size() == allo.arcs.size() && allo.startC.size() == allo.cycles.size()){
			for (int i = 0; i < allo.cycles.size(); i++){
				if(allo.isActivatedC[i] == 1) 
					isCycleUsed[i].set(GRB_DoubleAttr_Start, allo.startC[i]);
			}
			for (int i = 0; i < allo.arcs.size(); i++){
				if(allo.isActivatedA[i] == 1)
					isArcUsed[i].set(GRB_DoubleAttr_Start, allo.startA[i]);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++) 
			isPatientUsed[i] = 0;

		for (int i = 0; i < 4; i++){
			cIn[i].resize(allo.maxId + 2);
			cOut[i].resize(allo.maxId + 2);
			for(int j=0;j<allo.maxId + 2;j++){
				cIn[i][j] = 0; cOut[i][j] = 0; 
			}	
		}
		model.update();

		// Perform values
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				for(int j=0;j<allo.cycles[i].idX.size();j++){
					isPatientUsed[allo.cycles[i].idX[j]] += isCycleUsed[i];
					isPatientIdUsed[allo.cycles[i].idX[j]] = true;
				}
				objFun1 += allo.cycles[i].idX.size() * isCycleUsed[i];
				objFun2 += allo.cycles[i].score * isCycleUsed[i];
				objFun3 += allo.cycles[i].nbDTM * isCycleUsed[i];
				model.addConstr(of4 >= allo.cycles[i].idX.size() * isCycleUsed[i]);
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				cIn[allo.arcs[i][2]][allo.arcs[i][1]] += isArcUsed[i];
				cOut[allo.arcs[i][2]][allo.arcs[i][0]] += isArcUsed[i];
				isPatientUsed[allo.arcs[i][0]] +=  isArcUsed[i];
				isPatientIdUsed[allo.arcs[i][0]] = true;
				objFun1 += isArcUsed[i];
				objFun2 += allo.scores[allo.arcs[i][0]][allo.arcs[i][1]] * isArcUsed[i];
				objFun3 += allo.isDTM[allo.arcs[i][0]]  * isArcUsed[i];;
				if(allo.arcs[i][2] == 3) model.addConstr(of4 >= 4 * isArcUsed[i]); 
				if(allo.arcs[i][2] == 2 && allo.arcs[i][1] == allo.maxId + 1) model.addConstr(of4 >= 3 * isArcUsed[i]); 
			}
		}

		// Unique assignment for patients
		for (int i = 0; i < allo.maxId+1; i++){
			if(isPatientIdUsed[i] == true) model.addConstr(isPatientUsed[i] <= 1);
		}

		// Flow conservation
		for (int i = 0; i < allo.maxId+1; i++){
			if(allo.nodes1[i]) model.addConstr(cOut[1][i] == cIn[0][i]);
			if(allo.nodes2[i]) model.addConstr(cOut[2][i] == cIn[1][i]);	
			if(allo.nodes3[i]) model.addConstr(cOut[3][i] == cIn[2][i]);
		}

		if(allo.objs.size() == 0){
			model.setObjective(objFun1, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 1){
			model.addConstr(objFun1 == allo.objs[0]);
			model.setObjective(objFun2, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 2){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.setObjective(objFun3, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 3){
			model.addConstr(objFun1 == allo.objs[0]);
			model.addConstr(objFun2 == allo.objs[1]);
			model.addConstr(objFun3 == allo.objs[2]);
			model.setObjective(objFun4, GRB_MINIMIZE);
		}
	
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  360000 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		if(allo.sols[allo.objs.size()] != -1)	model.getEnv().set(GRB_DoubleParam_Cutoff, allo.sols[allo.objs.size()]);
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
		if(allo.objs.size() != 2){
			allo.infos.LB = ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON);	
			if(allo.infos.LB == allo.infos.UB) allo.infos.opt = true;
			allo.objs.push_back(ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON));
		}
		else{
			allo.infos.LB = model.get(GRB_DoubleAttr_ObjVal) ;	
			if(allo.infos.LB == allo.infos.UB) allo.infos.opt = true;
			allo.objs.push_back(model.get(GRB_DoubleAttr_ObjVal));
		}
		
		allo.startA.resize(0);
		allo.startC.resize(0);

		// Filling Solution
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1 && isCycleUsed[i].get(GRB_DoubleAttr_X) > 0.5){
				allo.cycles[i].print();
				allo.startC.push_back(1);
			}
			else allo.startC.push_back(0);
		}
		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1 && isArcUsed[i].get(GRB_DoubleAttr_X) > 0.5){
				cout << allo.arcs[i][0] << " " << allo.arcs[i][1] << " " << allo.arcs[i][2] << endl;
				allo.startA.push_back(1);
			}
			else allo.startA.push_back(0);
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

