#include "Allocation.h" 

/*	*************************************************************************************
	***********************************  DOCTOR *****************************************
	************************************************************************************* */

void Altruistic::print(){
	cout << "Altruistic " << id << "\t age " << age << "\t blood type " << blood << "\t nbEdges " << nbEdges <<  " \t";
	for(int i=0; i<nbEdges; i++) cout << "(" << targets[i] << " - " << scores[i] << ") ";
	cout << endl;
}

/*	*************************************************************************************
	************************************* PAIR * ****************************************
	************************************************************************************* */

void Pair::print(){
	cout << "Pair " << id << "\t source " << source << "\t age " << age <<  "\t blood types " << bloodP << " " << bloodD << " infos " << wt << " " << pra << " " << hs << "\t nbEdges " << nbEdges <<  " \t";
	for(int i=0; i<nbEdges; i++) cout << "(" << targets[i] << " - " << scores[i] << ") ";
	cout << endl;
}

/*	*************************************************************************************
	************************************* CYCLE *****************************************
	************************************************************************************* */

void Cycle::print(){
	if(isC == 1)
		cout << "Chain ";
	else
		cout << "Cycle ";
	cout << setw(5) << id << "\t size " << size <<  " ";
	for(int i=0; i<size-1; i++) cout << setw(5) << idX[i] << " ";
	cout << setw(5) << idX.back() << setw(5) << "\t nbDTM " << nbDTM << "\t score " << score << endl;
}

/*	*************************************************************************************
	********************************** ALLOCATION ***************************************
	************************************************************************************* */

void Allocation::load(const string& path, const string& filein){
	// Local variables 
	istringstream iss;
	istringstream tempIss;
	string parser;
	string garbage;
	string nameFile = path + filein;
	string tempString;
	int temp;
	char tempChar;

	// File opening
	ifstream file(nameFile.c_str(), ios::in);

	// File lecture
	if (file){
		set<double> praS;
		// Name of the instance is filein
		name = filein;

		// Initialize idToIdxA, idToIdxP, maxId
		idToIdxA.resize(10000);
		idToIdxP.resize(10000);
		maxId = 0;
		nbPairs = 0;
		nbAltruistics = 0;

		// 2 first lines are garbage
		getline(file, parser); iss.str(parser); iss.clear(); 
		getline(file, parser); iss.str(parser); iss.clear(); 

		// For each donor or altruistic
		for(;;){
			int id = -1;
			int age = -1;
			string blood = "";
			int source = -1;
			int nbEdges = 0;
			vector<int> t;
			vector<int> s;

			getline(file, parser); iss.str(parser);
			do{
				tempChar = iss.get();
			}
			while(tempChar != '}' && tempChar != '"');
//			cout << "tempChar is " << tempChar << endl;
	
			if(tempChar == '}'){
				iss.clear();
				break;
			}

			if(tempChar == '"'){
				getline(iss, tempString, '"');
				tempIss.str(tempString);
				tempIss >> id;
				maxId = max(id,maxId);
			//	cout << "doing id " << id << endl;
				tempIss.clear(); 
			}
			iss.clear(); 

			getline(file, parser); iss.str(parser);
			do{
				tempChar = iss.get();
			}
			while(tempChar != 's' && tempChar != 'a');
			if(tempChar == 's'){
				iss.clear();
				getline(file, parser); iss.str(parser);
				iss >> source;
				maxId = max(source,maxId);
				iss.clear();
				getline(file, parser); iss.str(parser); iss.clear(); 
			}
			else{
				iss.clear();
			}

			getline(file, parser); iss.str(parser);
			getline(iss, tempString, ',');
			tempIss.str(tempString);
			tempIss >> garbage;
			tempIss >> age;
			tempIss.clear(); 
			iss.clear(); 

			getline(file, parser); iss.str(parser); iss.clear();
			getline(file, parser); iss.str(parser);
			iss >> garbage;
			iss >> blood;
			iss.clear(); 
			getline(file, parser); iss.str(parser); iss.clear();

			getline(file, parser); iss.str(parser);
			do{
				tempChar = iss.get();
			}
			while(tempChar != '}' && tempChar != '"');
			if(tempChar == '"'){	
				iss.clear(); 
				for(;;){
					getline(file, parser); iss.str(parser); 
					do{
						tempChar = iss.get();
					}
					while(tempChar != ']' && tempChar != '{');
	//				cout << "tempChar is " << tempChar << endl;
					if(tempChar == ']'){
						iss.clear(); 
						getline(file, parser); iss.str(parser);iss.clear(); 
						break;
					}
					iss.clear(); 
					getline(file, parser); iss.str(parser);
					getline(iss, tempString, ',');
					tempIss.str(tempString);
					tempIss >> garbage;
					tempIss >> temp;
					t.push_back(temp);
					tempIss.clear(); 
					iss.clear(); 
					getline(file, parser); iss.str(parser);
					iss >> garbage;
					iss >> temp;
					s.push_back(temp);
					iss.clear(); 
					nbEdges++;
					getline(file, parser); iss.str(parser); 
				}
			}
			else iss.clear(); 
			if(source == -1){
				Altruistic al;
				al.id = id;
				al.age = age;
				al.nbEdges = nbEdges;
				al.targets = t;
				al.scores = s;
				if(blood.compare("\"O\"") == 0) al.blood = 0;
				if(blood.compare("\"A\"") == 0) al.blood = 1;
				if(blood.compare("\"B\"") == 0) al.blood = 2;
				if(blood.compare("\"AB\"") == 0) al.blood = 3;
				altruistics.push_back(al);
				idToIdxA[al.id].push_back(nbAltruistics);
				nbAltruistics++;
			}
			else{
				Pair p;
				p.source = source;
				p.id = id;
				p.age = age;
				p.nbEdges = nbEdges;
				p.targets = t;
				p.scores = s;
				if(blood.compare("\"O\"") == 0) p.bloodD = 0;
				if(blood.compare("\"A\"") == 0) p.bloodD = 1;
				if(blood.compare("\"B\"") == 0) p.bloodD = 2;
				if(blood.compare("\"AB\"") == 0) p.bloodD = 3;
				pairs.push_back(p);
				idToIdxP[p.source].push_back(nbPairs);
				nbPairs++;
			}
		}	
		getline(file, parser); iss.str(parser); iss.clear();
		vector<string> bloods(10000);
		vector<int> wts(10000); 
		vector<double> pras(10000);
		vector<string> hss(10000);

		for(;;){
			int idx;
			getline(file, parser); iss.str(parser); 
			do{
				tempChar = iss.get();
			}
			while(tempChar != '}' && tempChar != '"');
			if(tempChar == '"'){
				// 1
				getline(iss, tempString, '"');
				tempIss.str(tempString); tempIss >> idx; tempIss.clear();
				iss.clear();

				// 2
				getline(file, parser); iss.str(parser); iss.clear();
				getline(file, parser); iss.str(parser);
				iss >> garbage;
				iss >> bloods[idx];
				iss.clear(); 
				getline(file, parser); iss.str(parser); iss.clear();

				// 3 
				getline(file, parser); iss.str(parser);
				getline(iss, tempString, ',');
				tempIss.str(tempString); tempIss >> garbage; tempIss >> wts[idx]; tempIss.clear();
				iss.clear();

				// 4
				getline(file, parser); iss.str(parser);
				getline(iss, tempString, ',');
				tempIss.str(tempString); tempIss >> garbage; tempIss >> pras[idx]; tempIss.clear();
				iss.clear();

				// 5
				getline(file, parser); iss.str(parser);
				iss >> garbage;
				iss >> hss[idx];
				iss.clear();

				getline(file, parser); iss.str(parser); tempChar = iss.get();
			}
			else{
				iss.clear();
				break;
			}
		}

		for(int i=0;i<pairs.size();i++){
			if(bloods[pairs[i].source].compare("\"O\"") == 0) pairs[i].bloodP = 0;
			if(bloods[pairs[i].source].compare("\"A\"") == 0) pairs[i].bloodP = 1;
			if(bloods[pairs[i].source].compare("\"B\"") == 0) pairs[i].bloodP = 2;
			if(bloods[pairs[i].source].compare("\"AB\"") == 0) pairs[i].bloodP = 3;
			pairs[i].wt = wts[pairs[i].source];
			pairs[i].pra = pras[pairs[i].source]; 
			praS.insert(pras[pairs[i].source]);
			//pairs[i].pra = round(pras[pairs[i].source] * 1000.0) / 1000.0; 
			if(hss[pairs[i].source].compare("false") == 0) pairs[i].hs = false;
			if(hss[pairs[i].source].compare("true") == 0) pairs[i].hs = true;
		}

		// Copy the set
		vector<double> praV(praS.begin(), praS.end());
		
		// Create the compatibility/scores matrix
		comp.resize(maxId+1);
		scores.resize(maxId+1);
		isDTM.resize(maxId+1,0);
		for(int i=0;i<maxId+1;i++){ 
			comp[i].resize(maxId+1,-1);
			scores[i].resize(maxId+2,0);
		}

		// Fill the compatibility/scores matrix
		for(int i=0; i<nbAltruistics; i++){
			for(int j=0;j<altruistics[i].nbEdges;j++){
				comp[altruistics[i].id][altruistics[i].targets[j]] = 1;
				if(altruistics[i].blood == pairs[idToIdxP[altruistics[i].targets[j]][0]].bloodP)
					scores[altruistics[i].id][altruistics[i].targets[j]] = 1;
			}
		}
		for(int i=0; i<nbPairs; i++){
			for(int j=0;j<pairs[i].nbEdges;j++){
				comp[pairs[i].source][pairs[i].targets[j]] = 1;
				if(pairs[i].bloodD == pairs[idToIdxP[pairs[i].targets[j]][0]].bloodP) 
					scores[pairs[i].source][pairs[i].targets[j]] = 1;
			}
			for(int j=0;j<praV.size();j++){
				if(praV[j] == pairs[i].pra){
					isDTM[pairs[i].source] = j;
					break;
				}
			}
		}

		// Create the cycles
		types.resize(7);
		for(int i=0;i<maxId;i++){
			vector<bool> hbP(maxId+1,false);
			for(int j=0;j<idToIdxP[i].size();j++){
				int idx = idToIdxP[i][j];
				for(int k=0;k<pairs[idx].nbEdges;k++){
					int tId = pairs[idx].targets[k];
					if(hbP[tId] == false){
						if(comp[tId][i] == 1 && i <= tId){
							Cycle c;
							c.id = cycles.size();
							c.size = 2;
							c.isC = 0;
							c.nbDTM = 0.0; c.nbDTM += isDTM[tId] ; c.nbDTM += isDTM[i];
							c.idX.push_back(i); c.idX.push_back(tId); 
							c.score = 0; c.score+= scores[i][tId]; c.score+= scores[tId][i]; 
							cycles.push_back(c);
							types[0]++;
						}				
						vector<bool> hbP2(maxId+1,false);
						for(int l=0;l<idToIdxP[tId].size();l++){
							int idx2 = idToIdxP[tId][l];
							for(int m=0;m<pairs[idx2].nbEdges;m++){
								int tId2 = pairs[idx2].targets[m];
								if(hbP2[tId2] == false){
									if(comp[tId2][i] == 1 && i <= tId && i <= tId2){
										Cycle c;
										c.id = cycles.size();
										c.size = 3;
										c.isC = 0;
										c.nbDTM = 0; c.nbDTM += isDTM[tId] ; c.nbDTM += isDTM[i];  c.nbDTM += isDTM[tId2];
										c.idX.push_back(i); c.idX.push_back(tId); c.idX.push_back(tId2); 
										c.score = 0; c.score+= scores[i][tId]; c.score+= scores[tId][tId2]; c.score+= scores[tId2][i]; 
										cycles.push_back(c);
										types[1]++;
									}
									vector<bool> hbP3(maxId+1,false);
									for(int n=0;n<idToIdxP[tId2].size();n++){
										int idx3 = idToIdxP[tId2][n];
										for(int o=0;o<pairs[idx3].nbEdges;o++){
											int tId3 = pairs[idx3].targets[o];
											if(hbP3[tId3] == false){
												if(comp[tId3][i] == 1 && i <= tId && i <= tId2 && i <= tId3 && tId3 != tId && i != tId2){
													Cycle c;
													c.id = cycles.size();
													c.size = 4;
													c.isC = 0;
													c.nbDTM = 0; c.nbDTM += isDTM[tId] ; c.nbDTM += isDTM[i];  c.nbDTM += isDTM[tId2]; c.nbDTM += isDTM[tId3];
													c.idX.push_back(i); c.idX.push_back(tId); c.idX.push_back(tId2); c.idX.push_back(tId3); 
													c.score = 0; c.score+= scores[i][tId]; c.score+= scores[tId][tId2]; c.score+= scores[tId2][tId3]; c.score+= scores[tId3][i]; 
													cycles.push_back(c);
													types[2]++;
												}
											}
											hbP2[tId2] = true;
										}					
									}
								}
								hbP2[tId2] = true;
							}					
						}
					}
					hbP[tId] = true;
				}
			}
		}

		// Create the chains
		vector<int> arc(3);
		nodes1.resize(maxId+1,false);
		nodes2.resize(maxId+1,false);
		nodes3.resize(maxId+1,false);

		for(int i=0;i<nbAltruistics;i++){
			for(int j=0;j<altruistics[i].nbEdges;j++){
				arc[0] = altruistics[i].id; 	
				arc[1] = altruistics[i].targets[j]; 
				arc[2] = 0; 
				nodes1[arc[1]] = true;
				arcs.push_back(arc);
				types[3]++;
			}
			// Final link
			arc[0] = altruistics[i].id;
			arc[1] = maxId+1; 
			arc[2] = 0; 
			arcs.push_back(arc);
			types[3]++;
		}

		for(int i=0;i<maxId+1;i++){
			if(nodes1[i] == true){
				vector<bool> hbP(maxId+1,false);
				for(int j=0;j<idToIdxP[i].size();j++){
					int idx = idToIdxP[i][j];
					for(int k=0;k<pairs[idx].nbEdges;k++){
						if(hbP[pairs[idx].targets[k]] == false){
							arc[0] = i;
							arc[1] = pairs[idx].targets[k];
							arc[2] = 1; 
							nodes2[arc[1]] = true;
							arcs.push_back(arc);
							types[4]++;
							hbP[pairs[idx].targets[k]] = true;
						}
					}			
				}
				// Final link
				arc[0] = i;
				arc[1] = maxId+1; 
				arc[2] = 1; 
				arcs.push_back(arc);
				types[4]++;
			}
		}

		for(int i=0;i<maxId+1;i++){
			if(nodes2[i] == true){
				vector<bool> hbP(maxId+1,false);
				for(int j=0;j<idToIdxP[i].size();j++){
					int idx = idToIdxP[i][j];
					for(int k=0;k<pairs[idx].nbEdges;k++){
						if(hbP[pairs[idx].targets[k]] == false){
							arc[0] = i;
							arc[1] = pairs[idx].targets[k];
							arc[2] = 2; 
							nodes3[arc[1]] = true;
							arcs.push_back(arc);
							types[5]++;
							hbP[pairs[idx].targets[k]] = true;
						}
					}			
				}
				// Final link
				arc[0] = i;
				arc[1] = maxId+1; 
				arc[2] = 2; 
				arcs.push_back(arc);
				types[5]++;
			}
		}

		for(int i=0;i<maxId+1;i++){
			if(nodes3[i] == true){
				// Final link
				arc[0] = i;
				arc[1] = maxId+1; 
				arc[2] = 3; 
				arcs.push_back(arc);
				types[6]++;
			}
		}
	}
	else cout << "Could not open the file " << nameFile << endl;
}

void Allocation::printProb(){
	cout << "Instance " << name << endl;
	for(int i=0; i<nbAltruistics; i++){
		altruistics[i].print();
	}
	for(int i=0; i<nbPairs; i++){
		pairs[i].print();
	}
	cout << "Cycles: " << cycles.size() << endl;
	cout << "Arcs: " << arcs.size() << endl;
}

void Allocation::printInfo(const string& pathAndFileout){
	string nameFile = pathAndFileout;
	std::ofstream file(nameFile.c_str(), std::ios::out | std::ios::app);
	file << name << "\t" << infos.opt << "\t" << infos.timeCPU[0] << "\t" << infos.timeCPU[1] << "\t"<< infos.timeCPU[2] << "\t"<< infos.timeCPU[3] << "\t"<< infos.timeCPU[4] << "\t"<< infos.timeCPU[5] << "\t" << objs[0] << "\t" << objs[1] << "\t" << objs[2] << "\t" << objs[3] << "\t" << infos.nbVar << "\t" << infos.nbCons << "\t" << infos.nbNZ <<
		"\t" << types[0] << "\t" << types[1] <<"\t" << types[2] <<"\t" << types[3]  << "\t" << types[4]  << "\t" << types[5]  << "\t" << types[6]  << endl;
	file.close();
}