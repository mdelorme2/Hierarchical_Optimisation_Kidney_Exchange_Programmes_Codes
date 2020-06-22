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
	for(int i=0;i<=3;i++){
		picef(allo);
		allo.infos.timeCPU[i+2] =  getCPUTime() - initTimeModelCPU;
		for(int j=0;j<=i;j++){
			allo.infos.timeCPU[i+2]  -= allo.infos.timeCPU[j+1];
		}
	}
	allo.infos.timeCPU[0] =  getCPUTime() - initTimeModelCPU;
	allo.printInfo(pathAndFileout);
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
		for (int i = 0; i < allo.cycles.size(); i++)
			isCycleUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);	

		for (int i = 0; i < allo.arcs.size(); i++){
			isArcUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
		}

		if(allo.startA.size() == allo.arcs.size() && allo.startC.size() == allo.cycles.size()){
			for (int i = 0; i < allo.cycles.size(); i++) isCycleUsed[i].set(GRB_DoubleAttr_Start, allo.startC[i]);
			for (int i = 0; i < allo.arcs.size(); i++) isArcUsed[i].set(GRB_DoubleAttr_Start, allo.startA[i]);
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
			for(int j=0;j<allo.cycles[i].idX.size();j++){
				isPatientUsed[allo.cycles[i].idX[j]] += isCycleUsed[i];
				isPatientIdUsed[allo.cycles[i].idX[j]] = true;
			}
			objFun1 += allo.cycles[i].idX.size() * isCycleUsed[i];
			objFun2 += allo.cycles[i].score * isCycleUsed[i];
			objFun3 += allo.cycles[i].nbDTM * isCycleUsed[i];
			model.addConstr(of4 >= allo.cycles[i].idX.size() * isCycleUsed[i]);
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			cIn[allo.arcs[i][2]][allo.arcs[i][1]] += isArcUsed[i];
			cOut[allo.arcs[i][2]][allo.arcs[i][0]] += isArcUsed[i];
			isPatientUsed[allo.arcs[i][0]] +=  isArcUsed[i];
			isPatientIdUsed[allo.arcs[i][0]] = true;
			objFun1 += isArcUsed[i];
			objFun2 += allo.scores[allo.arcs[i][0]][allo.arcs[i][1]] * isArcUsed[i];
			objFun3 += allo.isDTM[allo.arcs[i][0]]  * isArcUsed[i];
			if(allo.arcs[i][2] == 3) model.addConstr(of4 >= 4 * isArcUsed[i]); 
			if(allo.arcs[i][2] == 2 && allo.arcs[i][1] == allo.maxId + 1) model.addConstr(of4 >= 3 * isArcUsed[i]); 
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
			if(isCycleUsed[i].get(GRB_DoubleAttr_X) > 0.5){
				allo.cycles[i].print() ;
				allo.startC.push_back(1);
			}
			else allo.startC.push_back(0);
		}
		for (int i = 0; i < allo.arcs.size(); i++){
			if(isArcUsed[i].get(GRB_DoubleAttr_X) > 0.5){
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

