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

	bool back0 = false; 
	int currFail1 = 0;
	int nbC1 = 2;

	// n
	for(int i=0;i<=3;i++){
		if(i == 0){
			if(back0){
				allo.fails[0]++; allo.sols[0]--; back0 = false; allo.objs.resize(1); allo.objs[0] = allo.sols[0];
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
			if(picefLP(allo) == -1){
				allo.fails[1]++; i = -1; back0 = true; currFail1 = 0; continue;
			}
			else{
				allo.sols[1] = floor(allo.tObjs[1] + EPSILON);
				for(;;){
					if(getCPUTime() - initTimeModelCPU > 3600){allo.objs[1] = -1; allo.infos.opt = false; break;}
					cout << "Sol0 is at " << allo.sols[0] << endl;
					if(currFail1 < nbC1 - 1){	
						cout << "Sol1 is at " << allo.sols[1] << endl;
						// Deactivation Cycle				
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0){
								if(allo.tObjs[1] + allo.RCC[1][j] + EPSILON < allo.sols[1]) allo.isActivatedC[j] = 0;
								else allo.isActivatedC[j] = 1;
							}
						}
						// Deactivation Chain
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0){
								if(allo.tObjs[1] + allo.RCA[1][j] + EPSILON < allo.sols[1]) allo.isActivatedA[j] = 0;
								else allo.isActivatedA[j] = 1;
							}
						}
					}
					else{
						cout << " --- Last try SECOND OBJ--- " << endl;
						allo.sols[1] = -1;
						// Deactivation Cycle
						for (int j = 0; j < allo.cycles.size(); j++){
							if (allo.isActivatedC[j] >= 0) allo.isActivatedC[j] = 1;
						}
						// Deactivation Arc
						for (int j = 0; j < allo.arcs.size(); j++){
							if (allo.isActivatedA[j] >= 0) allo.isActivatedA[j] = 1;
						}
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
						break;
					}		
					else{
						allo.objs.resize(1);
						if(getCPUTime() - initTimeModelCPU < 3600){ allo.fails[1]++; allo.sols[1]--; currFail1++;}	
					}
					if(currFail1 == nbC1){i = -1; back0 = true; currFail1 = 0; break;}
				}
			}
		}
		if(i == 2){
			// Chains are replaced by cycles
			vector<vector<int> > succ0(allo.maxId+2); vector<vector<vector<vector<double> > > > memRC(allo.maxId+2);
			vector<vector<int> > succ1(allo.maxId+2); 
			vector<vector<int> > succ2(allo.maxId+2); 
			vector<vector<int> > succ3(allo.maxId+2); 
			vector<vector<int> > succ4(allo.maxId+2); 
			vector<vector<int> > succ5(allo.maxId+2); 
			for(int j=0;j<allo.maxId+2;j++){
				memRC[j].resize(allo.maxId+2);
				for(int k=0;k<allo.maxId+2;k++){
					memRC[j][k].resize(6);
					for(int l=0;l<6;l++){
						memRC[j][k][l].resize(2,-1.0);
					}
				}
			}
			for (int j = 0; j < allo.arcs.size(); j++){
				if(allo.isActivatedA[j] == 1){
					 memRC[allo.arcs[j][0]][allo.arcs[j][1]][allo.arcs[j][2]][0] = allo.RCA[0][j];
					 memRC[allo.arcs[j][0]][allo.arcs[j][1]][allo.arcs[j][2]][1] = allo.RCA[1][j];
				}
			}
			for (int j = 0; j < allo.arcs.size(); j++){
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 0) succ0[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 1) succ1[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 2) succ2[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 3) succ3[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 4) succ4[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
				if (allo.isActivatedA[j] == 1 &&  allo.arcs[j][2] == 5) succ5[allo.arcs[j][0]].push_back(allo.arcs[j][1]); 
			}

			for(int j=0;j<allo.maxId+1;j++){
				for(int k=0;k<succ0[j].size();k++){
					int idx1 = succ0[j][k];
					if(idx1 == allo.maxId+1){
						bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + EPSILON < allo.sols[0]) keep1 = false; 
						bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + EPSILON < allo.sols[1]) keep2 = false; 
						if(keep1 && keep2){
							Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;
							c.size = 1;  c.idX.push_back(j); 
							c.score = 0; 
							allo.cycles.push_back(c);
							allo.types[2]++;
							allo.isActivatedC.push_back(1);
						}
					}
					for(int l = 0; l<succ1[idx1].size();l++){
						int idx2 = succ1[idx1][l];
						if(idx2 == allo.maxId+1){
							bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + memRC[idx1][idx2][1][0] + EPSILON < allo.sols[0]) keep1 = false; 
							bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + memRC[idx1][idx2][1][1] + EPSILON < allo.sols[1]) keep2 = false; 
							if(keep1 && keep2){
								Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;
								c.size = 2;  c.idX.push_back(j); c.idX.push_back(idx1); 
								c.score = 0; c.score+= allo.scores[j][idx1]; c.score+= allo.scores[idx1][idx2]; 
								allo.cycles.push_back(c);
								allo.types[3]++;
								allo.isActivatedC.push_back(1);
							}
						}
						for(int m = 0; m<succ2[idx2].size();m++){
							int idx3 = succ2[idx2][m];
							if(idx3 == idx1) continue;
							if(idx3 == allo.maxId+1){
								bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + memRC[idx1][idx2][1][0] + memRC[idx2][idx3][2][0] + EPSILON < allo.sols[0]) keep1 = false; 
								bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + memRC[idx1][idx2][1][1] + memRC[idx2][idx3][2][1] + EPSILON < allo.sols[1]) keep2 = false; 
								if(keep1 && keep2){
									Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;				
									c.size = 3;  c.idX.push_back(j); c.idX.push_back(idx1); c.idX.push_back(idx2);
									c.score = 0; c.score+= allo.scores[j][idx1]; c.score+= allo.scores[idx1][idx2]; c.score+= allo.scores[idx2][idx3];
									if(allo.comp[j][idx2] == 1) c.nbBA++; 
									if(allo.comp[idx2][idx1] != 1){ 
										allo.cycles.push_back(c);
										allo.types[4]++;
										allo.isActivatedC.push_back(1);
									}
								}
							}
							for(int n = 0; n<succ3[idx3].size();n++){
								int idx4 = succ3[idx3][n];
								if(idx4 == idx1 || idx4 == idx2) continue; 
								if(idx4 == allo.maxId+1){
									bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + memRC[idx1][idx2][1][0] + memRC[idx2][idx3][2][0] + memRC[idx3][idx4][3][0] + EPSILON < allo.sols[0]) keep1 = false; 
									bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + memRC[idx1][idx2][1][1] + memRC[idx2][idx3][2][1] + memRC[idx3][idx4][3][1] + EPSILON < allo.sols[1]) keep2 = false; 
									if(keep1 && keep2){
										Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;				
										c.size = 4;	 c.idX.push_back(j); c.idX.push_back(idx1); c.idX.push_back(idx2); c.idX.push_back(idx3);
										c.score = 0; c.score+= allo.scores[j][idx1]; c.score+= allo.scores[idx1][idx2]; c.score+= allo.scores[idx2][idx3]; c.score+= allo.scores[idx3][idx4];
										if(allo.comp[j][idx2] == 1) c.nbBA++; 
										if(allo.comp[j][idx3] == 1) c.nbBA++; 
										if(allo.comp[idx2][idx1] == 1) c.nbBA++; 
										if(allo.comp[idx1][idx3] == 1) c.nbBA++;
										if(allo.comp[idx3][idx2] != 1 && allo.comp[idx3][idx1] != 1 && (allo.comp[j][idx3] != 1 || allo.comp[idx2][idx1] != 1)){
											allo.cycles.push_back(c);
											allo.types[5]++;
											allo.isActivatedC.push_back(1);
										}
									}
								}
								for(int o = 0; o<succ4[idx4].size();o++){
									if(getCPUTime() - initTimeModelCPU > 3600 || (allo.types[2] + allo.types[3] +allo.types[4] +allo.types[5] +allo.types[6] + allo.types[7] > 75000000))goto end;
									int idx5 = succ4[idx4][o];
									if(idx5 == idx1 || idx5 == idx2 || idx5 == idx3) continue; 
									if(idx5 == allo.maxId+1){
										bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + memRC[idx1][idx2][1][0] + memRC[idx2][idx3][2][0] + memRC[idx3][idx4][3][0] + memRC[idx4][idx5][4][0] + EPSILON < allo.sols[0]) keep1 = false; 
										bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + memRC[idx1][idx2][1][1] + memRC[idx2][idx3][2][1] + memRC[idx3][idx4][3][1] + memRC[idx4][idx5][4][1] + EPSILON < allo.sols[1]) keep2 = false; 
										if(keep1 && keep2){
											Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;				
											c.size = 5;	 c.idX.push_back(j); c.idX.push_back(idx1); c.idX.push_back(idx2); c.idX.push_back(idx3); c.idX.push_back(idx4);
											c.score = 0; c.score+= allo.scores[j][idx1]; c.score+= allo.scores[idx1][idx2]; c.score+= allo.scores[idx2][idx3]; c.score+= allo.scores[idx3][idx4]; c.score+= allo.scores[idx4][idx5];
											if(allo.comp[j][idx2] == 1) c.nbBA++; 
											if(allo.comp[j][idx3] == 1) c.nbBA++; 
											if(allo.comp[j][idx4] == 1) c.nbBA++; 
											if(allo.comp[idx1][idx3] == 1) c.nbBA++;
											if(allo.comp[idx1][idx4] == 1) c.nbBA++;
											if(allo.comp[idx2][idx1] == 1) c.nbBA++; 
											if(allo.comp[idx2][idx4] == 1) c.nbBA++; 
											if(allo.comp[idx3][idx2] == 1) c.nbBA++; 
											if(allo.comp[idx3][idx1] == 1) c.nbBA++; 
											if(allo.comp[idx4][idx1] == 1) c.nbBA++; 
											if(allo.comp[idx4][idx2] == 1) c.nbBA++; 
											if(allo.comp[idx4][idx3] == 1) c.nbBA++; 
											if(allo.comp[idx4][idx3] != 1 && 
												allo.comp[idx4][idx2] != 1 && 
												(allo.comp[j][idx4] != 1 || allo.comp[idx3][idx1] != 1) && 
												(allo.comp[j][idx3] != 1 || allo.comp[idx2][idx1] != 1) && 
												(allo.comp[idx1][idx4] != 1 || allo.comp[idx3][idx2] != 1) &&
												(allo.comp[idx2][idx1] != 1 || allo.comp[idx4][idx3] != 1)){
												allo.cycles.push_back(c);
												allo.types[6]++;
												allo.isActivatedC.push_back(1);
											}
										}
									}
									for(int p = 0; p<succ5[idx5].size();p++){
										int idx6 = succ5[idx5][p];
										if(idx6 == allo.maxId+1){
											bool keep1 = true; if(allo.tObjs[0] + memRC[j][idx1][0][0] + memRC[idx1][idx2][1][0] + memRC[idx2][idx3][2][0] + memRC[idx3][idx4][3][0] + memRC[idx4][idx5][4][0] + memRC[idx5][idx6][5][0] + EPSILON < allo.sols[0]) keep1 = false; 
											bool keep2 = true; if(allo.tObjs[1] + memRC[j][idx1][0][1] + memRC[idx1][idx2][1][1] + memRC[idx2][idx3][2][1] + memRC[idx3][idx4][3][1] + memRC[idx4][idx5][4][1] + memRC[idx5][idx6][5][1] + EPSILON < allo.sols[1]) keep2 = false; 
											if(keep1 && keep2){
												Cycle c; c.id = allo.cycles.size(); c.isC = 1; c.nbBA = 0;				
												c.size = 6;	 c.idX.push_back(j); c.idX.push_back(idx1); c.idX.push_back(idx2); c.idX.push_back(idx3); c.idX.push_back(idx4); c.idX.push_back(idx5);
												c.score = 0; c.score+= allo.scores[j][idx1]; c.score+= allo.scores[idx1][idx2]; c.score+= allo.scores[idx2][idx3]; c.score+= allo.scores[idx3][idx4]; c.score+= allo.scores[idx4][idx5]; c.score+= allo.scores[idx5][idx6];
												if(allo.comp[j][idx2] == 1) c.nbBA++; 
												if(allo.comp[j][idx3] == 1) c.nbBA++; 
												if(allo.comp[j][idx4] == 1) c.nbBA++; 
												if(allo.comp[j][idx5] == 1) c.nbBA++; 
												if(allo.comp[idx1][idx3] == 1) c.nbBA++;
												if(allo.comp[idx1][idx4] == 1) c.nbBA++;
												if(allo.comp[idx1][idx5] == 1) c.nbBA++;
												if(allo.comp[idx2][idx1] == 1) c.nbBA++; 
												if(allo.comp[idx2][idx4] == 1) c.nbBA++; 
												if(allo.comp[idx2][idx5] == 1) c.nbBA++; 
												if(allo.comp[idx3][idx1] == 1) c.nbBA++; 
												if(allo.comp[idx3][idx2] == 1) c.nbBA++; 
												if(allo.comp[idx3][idx5] == 1) c.nbBA++; 
												if(allo.comp[idx4][idx1] == 1) c.nbBA++; 
												if(allo.comp[idx4][idx2] == 1) c.nbBA++; 
												if(allo.comp[idx4][idx3] == 1) c.nbBA++; 
												if(allo.comp[idx5][idx1] == 1) c.nbBA++; 
												if(allo.comp[idx5][idx2] == 1) c.nbBA++; 
												if(allo.comp[idx5][idx3] == 1) c.nbBA++; 
												if(allo.comp[idx5][idx4] == 1) c.nbBA++; 
												if(allo.comp[idx5][idx4] != 1 && allo.comp[idx5][idx3] != 1 && 
													(allo.comp[j][idx4] != 1 || allo.comp[idx3][idx1] != 1) && 
													(allo.comp[j][idx3] != 1 || allo.comp[idx2][idx1] != 1) && 
													(allo.comp[idx1][idx5] != 1 || allo.comp[idx4][idx2] != 1) && 
													(allo.comp[idx1][idx4] != 1 || allo.comp[idx3][idx2] != 1) && 
													(allo.comp[idx2][idx5] != 1 || allo.comp[idx4][idx3] != 1) &&
													(allo.comp[idx5][idx4] != 1 || allo.comp[idx3][idx2] != 1)){
													allo.cycles.push_back(c);
													allo.types[7]++;
													allo.isActivatedC.push_back(1);
												}
											}
										}
									}	
								}	
							}	
						}
					}
				}
			}
			if(getCPUTime() - initTimeModelCPU > 3600 || (allo.types[2] + allo.types[3] +allo.types[4] +allo.types[5] +allo.types[6]+allo.types[7] > 75000000)){
				end: allo.objs[2] = -1; allo.infos.opt = false; break;
			}
			cycleLP(allo);
			allo.sols[2] = floor(allo.tObjs[i] + EPSILON);
			for(;;){
				if(getCPUTime() - initTimeModelCPU > 3600){ allo.objs[2] = -1; allo.infos.opt = false; break;}
				cout << "Sol2 is at " << allo.sols[2] << endl;
				// Deactivation
				for (int j = 0; j < allo.cycles.size(); j++){
					if (allo.isActivatedC[j] >= 0){
						if(allo.tObjs[2] + allo.RCC[2][j] + EPSILON < allo.sols[2]) allo.isActivatedC[j] = 0;
						else allo.isActivatedC[j] = 1;
					}
				}
				if(cycle(allo) == 0 && allo.objs[2] == allo.sols[2]){
					for (int j = 0; j < allo.cycles.size(); j++){
						if (allo.isActivatedC[j] == 0) allo.isActivatedC[j] = -1;
					}
					break;
				}		
				else{
					allo.objs.resize(2);
					if(getCPUTime() - initTimeModelCPU < 3600){
						allo.fails[2]++; allo.sols[2]--;
					}	
				}
			}
		}
		if(i == 3){
			cycleLP(allo);
			allo.sols[3] = floor(allo.tObjs[i] + EPSILON);
			for(;;){
				if(getCPUTime() - initTimeModelCPU > 3600){ allo.objs[3] = -1; allo.infos.opt = false; break;}
				cout << "Sol3 is at " << allo.sols[3] << endl;
				// Deactivation
				for (int j = 0; j < allo.cycles.size(); j++){
					if (allo.isActivatedC[j] >= 0){
						if(allo.tObjs[3] + allo.RCC[3][j] + EPSILON < allo.sols[3]) allo.isActivatedC[j] = 0;
						else allo.isActivatedC[j] = 1;
					}
				}
				if (cycle(allo) == 0 && allo.objs[3] == allo.sols[3]){
					for (int j = 0; j < allo.cycles.size(); j++){
						if (allo.isActivatedC[j] == 0) allo.isActivatedC[j] = -1;
					}
					break;
				}		
				else{
					allo.objs.resize(3);
					if(getCPUTime() - initTimeModelCPU < 3600){
						allo.fails[3]++; allo.sols[3]--;
					}	
				}
			}
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
		
		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		vector<GRBVar> isArcUsed (allo.arcs.size());
		vector<vector<GRBLinExpr> > cIn(6);
		vector<vector<GRBLinExpr> > cOut(6);

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

		for (int i = 0; i < allo.maxId+1; i++) isPatientUsed[i] = 0;

		for (int i = 0; i < 6; i++){
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
				objFun2 += isCycleUsed[i];
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				cIn[allo.arcs[i][2]][allo.arcs[i][1]] += isArcUsed[i];
				cOut[allo.arcs[i][2]][allo.arcs[i][0]] += isArcUsed[i];
				isPatientUsed[allo.arcs[i][0]] +=  isArcUsed[i];
				isPatientIdUsed[allo.arcs[i][0]] = true;
				objFun1 += isArcUsed[i];
				if(allo.arcs[i][1] == allo.maxId + 1) objFun2 += isArcUsed[i];
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
			if(allo.nodes4[i]) model.addConstr(cOut[4][i] == cIn[3][i]);	
			if(allo.nodes5[i]) model.addConstr(cOut[5][i] == cIn[4][i]);	
		}

		if(allo.objs.size() == 0){
			model.setObjective(objFun1, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 1){
			model.addConstr(objFun1 == allo.objs[0]);
			model.setObjective(objFun2, GRB_MAXIMIZE);
		}
	
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
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
		
		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		vector<GRBVar> isArcUsed (allo.arcs.size());
		vector<vector<GRBLinExpr> > cIn(6);
		vector<vector<GRBLinExpr> > cOut(6);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				isArcUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++) isPatientUsed[i] = 0;

		for (int i = 0; i < 6; i++){
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
				objFun2 += isCycleUsed[i];
			}
		}

		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				cIn[allo.arcs[i][2]][allo.arcs[i][1]] += isArcUsed[i];
				cOut[allo.arcs[i][2]][allo.arcs[i][0]] += isArcUsed[i];
				isPatientUsed[allo.arcs[i][0]] +=  isArcUsed[i];
				isPatientIdUsed[allo.arcs[i][0]] = true;
				objFun1 += isArcUsed[i];
				if(allo.arcs[i][1] == allo.maxId + 1) objFun2 += isArcUsed[i];
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
			if(allo.nodes4[i]) model.addConstr(cOut[4][i] == cIn[3][i]);	
			if(allo.nodes5[i]) model.addConstr(cOut[5][i] == cIn[4][i]);	
		}

		if(allo.objs.size() == 0){
			model.setObjective(objFun1, GRB_MAXIMIZE);
		}

		if(allo.objs.size() == 1){
			model.addConstr(objFun1 == allo.objs[0]);
			model.setObjective(objFun2, GRB_MAXIMIZE);
		}
	
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		if(allo.sols[allo.objs.size()] != -1)	model.getEnv().set(GRB_DoubleParam_Cutoff, allo.sols[allo.objs.size() - EPSILON]);
		model.optimize();

		// Filling Info
		allo.infos.UB = ceil(model.get(GRB_DoubleAttr_ObjBound) - EPSILON);
		allo.infos.opt = false;

		// If no solution found
		if (model.get(GRB_IntAttr_SolCount) < 1){
			cout << "Failed to optimize ILP. " << endl;
			allo.infos.LB  = 0;
			return -1;
		}

		// If solution found
		allo.objs.push_back(ceil(model.get(GRB_DoubleAttr_ObjVal) - EPSILON));

		// Filling Solution
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) > 0.5){
					allo.cycles[i].print() ;
				}
			}
		}
		for (int i = 0; i < allo.arcs.size(); i++){
			if(allo.isActivatedA[i] == 1){
				if(isArcUsed[i].get(GRB_DoubleAttr_X) > 0.5){
					cout << allo.arcs[i][0] << " " << allo.arcs[i][1] << " " << allo.arcs[i][2] << endl;
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
		
		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_CONTINUOUS);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++){
			isPatientUsed[i] = 0;
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
				objFun2 += isCycleUsed[i];
				objFun3 += allo.cycles[i].nbBA * isCycleUsed[i];
				objFun4 += allo.cycles[i].score * isCycleUsed[i];
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
			model.setObjective(objFun4, GRB_MAXIMIZE);
		}
	
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_IntParam_Crossover, 0); 
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);
		model.optimize();

		// If solution found
		allo.tObjs[allo.objs.size()] = model.get(GRB_DoubleAttr_ObjVal);	

		// Filling Solution
		allo.RCC[allo.objs.size()].resize(0);
		allo.RCC[allo.objs.size()].resize(allo.cycles.size(), 0);
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) < EPSILON){
					allo.RCC[allo.objs.size()][i] = isCycleUsed[i].get(GRB_DoubleAttr_RC);
				}
				else{
					allo.RCC[allo.objs.size()][i] = 0.0;
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
		
		vector<GRBVar> isCycleUsed (allo.cycles.size());
		vector<GRBLinExpr> isPatientUsed(allo.maxId+1);
		vector<bool> isPatientIdUsed(allo.maxId+1,false);

		// Initialization
		for (int i = 0; i < allo.cycles.size(); i++){
			if(allo.isActivatedC[i] == 1){
				isCycleUsed[i] = model.addVar(0, 1, 0, GRB_INTEGER);
			}
		}

		for (int i = 0; i < allo.maxId+1; i++){
			isPatientUsed[i] = 0;
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
				objFun2 += isCycleUsed[i];
				objFun3 += allo.cycles[i].nbBA * isCycleUsed[i];
				objFun4 += allo.cycles[i].score * isCycleUsed[i];
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
			model.setObjective(objFun4, GRB_MAXIMIZE);
		}
				
		// Setting of Gurobi
		model.getEnv().set(GRB_DoubleParam_TimeLimit,  3600 - (getCPUTime() - initTimeModelCPU));
		model.getEnv().set(GRB_IntParam_Threads, 1);
		model.getEnv().set(GRB_IntParam_Method, 2);
		model.getEnv().set(GRB_DoubleParam_MIPGap, 0);

		if(allo.sols[allo.objs.size()] != -1){
		//	cout << " cut at " <<allo.sols[allo.objs.size()] << " " << " for obj " << allo.objs.size() << " because sol is at " << allo.sols[allo.objs.size()] << endl;
			model.getEnv().set(GRB_DoubleParam_Cutoff, allo.sols[allo.objs.size()] - EPSILON);
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
			if(allo.isActivatedC[i] == 1){
				if(isCycleUsed[i].get(GRB_DoubleAttr_X) > 0.001){
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
