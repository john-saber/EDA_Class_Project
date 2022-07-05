//==================================================//
//       Credits: John Saber & Abdullah Allam       //
//        	       MinCut_Placement	      			//
//==================================================//

#ifndef MIN_CUT_H
#define MIN_CUT_H
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>

using namespace std;

struct pin {
	string pin_name;
	string net;
};
struct gate {
	string gate_name;
	string input1;
	string input2;
	string output;
	bool fixed=0;									//Indicates whether it is fixed or not
};

//==================================================================================//
//	2D map (like 2D associative array with string indices) for connectivity matrix	//
//==================================================================================//
map<string,map<string,int>> connectivity;           




//==============================================================//
//			Function to count pins and gates in a file			//
//==============================================================//
void count_nodes (ifstream &file , int count_arr[]) {
	int p_n = 0;
	int g_n = 0;
	int n;
	string temp;
	while (getline(file,temp))
	{
		n++;
		if (temp.find(",") == temp.npos){       //if no commas found >> pin
			p_n++;
		}
		else{                                   //else it is gate
			g_n++;
		}
	}
	file.close();
	count_arr[0] = n;
	count_arr[1] = p_n;
	count_arr[2] = g_n;
	cout << endl;
	cout << "Number of Lines: "<<  count_arr[0] << endl;
	cout << "Number of Pins:  "<<  count_arr[1] << endl;
	cout << "Number of Gates: "<< count_arr[2] << endl;
}





//==========================================================================================//
//		Functions to erase unneeded string characters, find char, find its position			//
//==========================================================================================//
string erase_char (string str, char c){
	str.erase(remove(str.begin(), str.end(), c), str.end());
	return str;
}

bool found_str(string str, char c){
	bool flag = 0;
	for (int i = 0; i < str.length(); i++){
		if (str[i] == c){
			flag = 1;
		}
	}
	return flag;
}

int find_pos(string str, char c){
	int pos = 0;
	for (int i = 0; i < str.length(); i++){
		if (str[i] == c){
			pos = i;
			break;
		}
	}
	return pos;
}





//==============================================================//
//				Function to parse file content					//
//==============================================================//
void fill_struct (ifstream &file, gate gate_i [], pin pin_i [], int n){
	string str,temp;
	int k = 0;
	int j = 0;
	for (int i = 0; i < n; i++){
		getline(file, str);
		if (str.find("out") != string::npos){                      			//if there is "out" then it is a gate
			gate_i[j].gate_name = str.substr(str.find('(')-3, 2);			//get substring from '(' (the name of the gate)
			temp = str.substr(str.find("(in")+4 , 2);						//get substring after finding "(in)" (input 1)
			gate_i[j].input1 = temp;										

			if (str.find(", in") != string::npos){ 							//if there is ", in" then it is a 2 input gate otherwise it is left 0
				temp = str.substr(str.find(", in")+5 , 2);					//get substring (input 2)
				gate_i[j].input2 = temp;									
			}
			if (str.find("clk") != string::npos){ 							//if there is clk then it is another input for a flipflop
				temp = str.substr(str.find("clk")+4 , 4);					//get substring after finding "clk" and put it in (input 2)
				gate_i[j].input2 = temp;									
			}
			temp = str.substr(str.find("out")+4 , 2);						//get substring after finding "out" for (output net)
			gate_i[j].output = temp;										
			//For debugging	
			//cout << "Gate "<< j+1 << " Name: " << gate_i[j].gate_name <<  "    >>>>   IN1: " << gate_i[j].input1 <<  "    >>>>   IN2: " << gate_i[j].input2 << "    >>>>   OUT: " << gate_i[j].output << endl; //<< gate_i[i].gate_name << endl;  
			j++;
		}
		else{                                   							//else it is a pin
			pin_i[k].pin_name = str.substr(0,str.find(':'));
			temp = str.substr(str.find("N")+1 , str.find(")")-1);
			pin_i[k].net = stoi(temp);
			//For debugging
			//cout << "Pin  "<< k+1 << " Name: " << pin_i[k].pin_name << "     >>>>   Net: " << pin_i[k].net << endl;
			k++;
		}
	}
	file.close();
}




//==============================================================//
//			Functions to print structs / partitions				//
//==============================================================//
void print_pin_struct_info (pin pin_i[], int n){
	cout << endl;
	for (int i = 0; i < n; i++){
		cout << "Pin  "<< i+1 << " Name: " << pin_i[i].pin_name << "    \t";
		cout << "Net: \t" << pin_i[i].net << "\t" << endl;   
	}
}
void print_gate_struct_info (gate gate_i[], int n){
	for (int i = 0; i < n; i++){  
		cout << "Gate "<< i+1 << " Name: " << gate_i[i].gate_name << "    \t";
		cout << "IN1: \t" << gate_i[i].input1 << "\t";   
		cout << "IN2: \t" << gate_i[i].input2 << "\t";   
		cout << "OUT: \t" << gate_i[i].output << "\t"   << endl; 
	}
}

void print_partition (gate gate_i[], int n){
	for (int i = 0; i < n; i++){  
		cout << "Gate "<< i+1 << " Name: " << gate_i[i].gate_name << endl; 
	}
}

void print_MinCut_partition (map<string,map<int,gate>> &partition, string cut_n){
	int k = 0;
	for (auto i = partition[cut_n].begin();
         i != partition[cut_n].end(); ++i) {
		cout << "Gate "<< k+1 << " Name: " << partition[cut_n][k].gate_name << endl; 
		k++;
	}
}






//==============================================================//
//					Function to fill matrix map					//
//==============================================================//
void fill_con_matrix(pin pin_i [], gate gate_i[], int n, int p_n, int g_n){
	for (int i = 0; i < p_n; i++){
		for (int j = 0; j < g_n; j++){
			if ((pin_i[i].net == gate_i[j].input1) || (pin_i[i].net == gate_i[j].input2) || (pin_i[i].net == gate_i[j].output)) {				//Compare all pin net to every input and output in the netlist
				connectivity[pin_i[i].pin_name][gate_i[j].gate_name] = 1;
				connectivity[gate_i[j].gate_name][pin_i[i].pin_name] = connectivity[pin_i[i].pin_name][gate_i[j].gate_name];

			}
			else {
				connectivity[pin_i[i].pin_name][gate_i[j].gate_name] = 0;
				connectivity[gate_i[j].gate_name][pin_i[i].pin_name] = connectivity[pin_i[i].pin_name][gate_i[j].gate_name];
			}
		}
	}
	
	for (int i = 0; i < g_n; i++){
		for (int j = 0; j < g_n; j++){
			if ((i != j) && ((gate_i[i].input1 == gate_i[j].input1) || (gate_i[i].input1 == gate_i[j].input2) || (gate_i[i].input1 == gate_i[j].output))) {			//Compare all gate input 1 to every input and output in the netlist
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = 1;																							//(i != j) >>>> don't compare the gate to itself
			}
			else {
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = 0;
			}
		}
	}

	for (int i = 0; i < g_n; i++){
		for (int j = 0; j < g_n; j++){
			if ((i != j) && ((gate_i[i].input2 == gate_i[j].input1) || (gate_i[i].input2 == gate_i[j].input2) || (gate_i[i].input2 == gate_i[j].output))) {			//Compare all gate input 2 to every input and output in the netlist
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = connectivity[gate_i[i].gate_name][gate_i[j].gate_name] + 1;								//(i != j) >>>> don't compare the gate to itself
			}
			else {
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = connectivity[gate_i[i].gate_name][gate_i[j].gate_name] + 0;
			}
		}
	}

	for (int i = 0; i < g_n; i++){
		for (int j = 0; j < g_n; j++){
			if ((i != j) && ((gate_i[i].output == gate_i[j].input1) || (gate_i[i].output == gate_i[j].input2) || (gate_i[i].output == gate_i[j].output))) {			//Compare all gate input 2 to every input and output in the netlist
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = connectivity[gate_i[i].gate_name][gate_i[j].gate_name] + 1;								//(i != j) >>>> don't compare the gate to itself
			}
			else {
				connectivity[gate_i[i].gate_name][gate_i[j].gate_name] = connectivity[gate_i[i].gate_name][gate_i[j].gate_name] + 0;
			}
		}
	}

	for (int i = 0; i < g_n; i++){
		for (int j = 0; j < g_n; j++){
			connectivity[gate_i[j].gate_name][gate_i[i].gate_name] = connectivity[gate_i[i].gate_name][gate_i[j].gate_name];			//Mirror the matrix
		}   
	} 
}




//==============================================================//
//				Function to print the matrix on screen			//
//==============================================================//
void print_con_matrix (pin pin_i [], gate gate_i[], int n, int p_n, int g_n){
	cout << "\nConnectivity Matrix: " << endl;
	for (int i = 0; i < p_n; i++){
		cout <<  "\t" << pin_i[i].pin_name;
	}
	for (int i = 0; i < g_n; i++){
		cout <<  "\t" << gate_i[i].gate_name;
	}
	cout << endl;

	int k = 0;
	int m = 0;
	for (int i = 0; i < n; i++){
		if (i < p_n){
			cout << pin_i[i].pin_name;
		}
		else {
			cout << gate_i[k].gate_name; k++;
		}
		int g = 0;
		int l = 0;
		for (int j = 0; j < n; j++){
			if (i < p_n){
				if (j < p_n){
					cout << "\t" <<connectivity[pin_i[i].pin_name][pin_i[j].pin_name] ;
				}
				else {
					cout <<  "\t"  << connectivity[pin_i[i].pin_name][gate_i[g].gate_name];g++;
				}
			m = -1;
			}
			else {
				if (j < p_n){
					cout << "\t" << connectivity[gate_i[m].gate_name][pin_i[j].pin_name] ;
				}
				else {
					cout <<  "\t" << connectivity[gate_i[m].gate_name][gate_i[l].gate_name];l++;
				}
			}
		}
		m++;
		cout << endl;
	}
}



//===================================================================================================================//
//		Function to split the netlist into initial partitions and also resolve odd netlist by adding dummy gate		 //
//===================================================================================================================//
void Construct_Initial_Partitions(gate part_A[], gate part_B[], gate gate_i[], int g_n, int oddflag){

	if (oddflag){									//if Odd >> resolve for KL don't operate on odd netlist
		gate temp[g_n];
		for (int i = 0; i < (g_n-1); i++){
			temp[i] = gate_i[i];
		}
		temp[g_n-1].gate_name = "dummy";			//add dummy to last index
		for (int i = 0; i < g_n/2; i++){			//Split gates in each partition after adding dummy gate
			part_A[i] = temp[i];			
			part_B[i] = temp[i+(g_n/2)];
		}
	}
	else {
		for (int i = 0; i < g_n/2; i++){			//Split gates in each partition
			part_A[i] = gate_i[i];			
			part_B[i] = gate_i[i+(g_n/2)];
		}
	}

	for (int i = 0; i < g_n/2; i++){				//Unfix all gates
		part_B[i].fixed = 0;
		part_A[i].fixed = 0;
	}
}



//===========================================================================================================//		
//		Function to calculate the cost of transitioning a gate to the other partition (D = Cut - Uncut)		 //
//===========================================================================================================//
map<string,int> calc_D (gate part_A[], gate part_B[], int g_n){

	map<string,int> cut_A;						//Number of cut nets for each gate in partition A
	map<string,int> cut_B;						//Number of cut nets for each gate in partition B	
	map<string,int> uncut_A;					//Number of uncut nets for each gate in partition A
	map<string,int> uncut_B;					//Number of uncut nets for each gate in partition B
	
	map<string,int> D; 							//Cut cost for each gate (cut - uncut)
	int cut_cost = 0;
	for (int i = 0; i < (g_n/2); i++){					//Calc Cut_A
		if((part_A[i].fixed==0)){ 						//Don't consider fixed gates
			cut_A[part_A[i].gate_name] = 0;        		//Initialization
			for (int j = 0; j < (g_n/2); j++){
				cut_A[part_A[i].gate_name] = connectivity[part_A[i].gate_name][part_B[j].gate_name] + cut_A[part_A[i].gate_name] ;
			}
			//cout << "Cut nets of " << part_A[i].gate_name << " = " << cut_A[part_A[i].gate_name] << endl;
			cut_cost = cut_cost + cut_A[part_A[i].gate_name];
		}
	}
	
	for (int i = 0; i < (g_n/2); i++){					//Calc Cut_B
		if((part_B[i].fixed==0)){ 						//Don't consider fixed gates
			cut_B[part_B[i].gate_name] = 0;        		//Initialization
			for (int j = 0; j < (g_n/2); j++){
				cut_B[part_B[i].gate_name] = connectivity[part_B[i].gate_name][part_A[j].gate_name] + cut_B[part_B[i].gate_name];
			}
			//cout << "Cut nets of " << part_B[i].gate_name << " = " << cut_B[part_B[i].gate_name] << endl;
			cut_cost = cut_cost + cut_B[part_B[i].gate_name];
		}
	}

	for (int i = 0; i < (g_n/2); i++){					//Calc Uncut_A
		if((part_A[i].fixed==0)){ 						//Don't consider fixed gates
			uncut_A[part_A[i].gate_name] = 0;       	//Initialization
			for (int j = 0; j < (g_n/2); j++){
				uncut_A[part_A[i].gate_name] = connectivity[part_A[i].gate_name][part_A[j].gate_name] + uncut_A[part_A[i].gate_name];
			}
			//cout << "Uncut nets of " << part_A[i].gate_name << " = " << uncut_A[part_A[i].gate_name] << endl;
		}
	}

	for (int i = 0; i < (g_n/2); i++){					//Calc Uncut_B
		if((part_B[i].fixed==0)){ 						//Don't consider fixed gates
			uncut_B[part_B[i].gate_name] = 0;       	//Initialization
			for (int j = 0; j < (g_n/2); j++){
				uncut_B[part_B[i].gate_name] = connectivity[part_B[i].gate_name][part_B[j].gate_name] + uncut_B[part_B[i].gate_name];
			}
			//cout << "Uncut nets of " << part_B[i].gate_name << " = " << uncut_B[part_B[i].gate_name] << endl;
		}
	}

	//cout << "\nCut cost & delta_g : " << endl;
	for (int i = 0; i < (g_n/2); i++){
		if(part_A[i].fixed==0){							//Don't consider fixed gates
			D[part_A[i].gate_name] = cut_A[part_A[i].gate_name] - uncut_A[part_A[i].gate_name];
			//cout << "D(" << part_A[i].gate_name << ") = " << D[part_A[i].gate_name] << ",\t";
		}
		if(part_B[i].fixed==0){
			D[part_B[i].gate_name] = cut_B[part_B[i].gate_name] - uncut_B[part_B[i].gate_name];
			//cout << "D(" << part_B[i].gate_name << ") = " << D[part_B[i].gate_name] << endl;
		}  
	}
	return D;
}




//====================================================================================//
//			Function to swap a pair of gates after in each KL iteration		     	  //
//====================================================================================//
void KL_swap(gate part_A[], gate part_B[], int  highest_gain_pair_indeces[]){
	gate temp;
	//cout << "Swap and Fix " << part_A[highest_gain_pair_indeces[0]].gate_name << " and " << part_B[highest_gain_pair_indeces[1]].gate_name << endl;
	temp                                            = part_A[highest_gain_pair_indeces[0]];
	part_A[highest_gain_pair_indeces[0]]            = part_B[highest_gain_pair_indeces[1]];
	part_B[highest_gain_pair_indeces[1]]            = temp;
	part_A[highest_gain_pair_indeces[0]].fixed      = 1;
	part_B[highest_gain_pair_indeces[1]].fixed      = 1;
}




//=============================================//
//		Function to calc delta g and G 		   //
//=============================================//
void calc_delta_g (map<string,int> D,  gate part_A[], gate part_B[], int g_n, int &G){
	map<string,map<string,int>> delta_g;				//2D map for delta_g (swaping pairs)
	string  highest_gain_pair[2] = {part_B[0].gate_name , part_A[0].gate_name};
	int     highest_gain_pair_indeces[2] = {0 , 0};
	float   highest_gain =  -1 * INFINITY;
	for (int i = 0; i < (g_n/2); i++){
		if(part_A[i].fixed==0){							//Don't consider fixed gates
			for (int j = 0; j < (g_n/2); j++){
				if(part_B[j].fixed==0){					//Don't consider fixed gates
					delta_g[part_A[i].gate_name][part_B[j].gate_name] = D[part_A[i].gate_name] + D[part_B[j].gate_name] - 2*connectivity[part_A[i].gate_name][part_B[j].gate_name];
					//update the highest gain pair info
					if (delta_g[part_A[i].gate_name][part_B[j].gate_name] > highest_gain) {
						highest_gain = delta_g[part_A[i].gate_name][part_B[j].gate_name];
						highest_gain_pair_indeces[0] = i;
						highest_gain_pair_indeces[1] = j;
					}
					//cout << "delta_g(" << part_A[i].gate_name << "," << part_B[j].gate_name << ") = " << delta_g[part_A[i].gate_name][part_B[j].gate_name] << endl;
				}
			}
		}
	}
	//cout << "\nHighest gain is: " << highest_gain << "  --  Pair to switch are: " << part_A[highest_gain_pair_indeces[0]].gate_name << " and " << part_B[highest_gain_pair_indeces[1]].gate_name << endl;
	KL_swap(part_A, part_B, highest_gain_pair_indeces);				//Swap gates
	G = G + highest_gain;											//Calculate overall gain G
	//cout << "G = " << G <<endl;
}




//=====================================================//
//			Self contained KL_Alg Function			   //
//=====================================================//
void Apply_KL(gate gate_i[], int g_n, map<string,map<int,gate>> &partitionA, map<string,map<int,gate>> &partitionB, string cut_n) {
	cout << "\n||================================= Applying Kernighan-Lin (KL) Algorithm =================================|| " << endl;
	int G = 0;								//Initialized Cumulative Gain
	float G_m = INFINITY;					//Criterion To Do another Pass (G_m > 0)
	float G_BEST;							//Cummulative gain in each iteration
    bool all_fixed = 0;
	map<string,int> D;						//An assosiative array of D (transition cost)




	int oddflag = 0;
	if ((g_n % 2) != 0){ 					//Test if the netlist is Odd
		g_n = g_n + 1;							
		oddflag = 1;						//This flag is used in partitioning function (Construct_Initial_Partitions)
	}
	gate part_A[g_n/2], part_A_BEST[g_n/2]; //The "best" array is to store the iteration of best gain
    gate part_B[g_n/2], part_B_BEST[g_n/2];


	Construct_Initial_Partitions(part_A, part_B, gate_i, g_n, oddflag);

	//Print initial partitions
	/*cout <<"\nInitial Partitions: \n";
	cout <<"Partition A: \n";
    print_partition(part_A, g_n/2);
    cout <<"\nPartition B: \n";
    print_partition(part_B, g_n/2);*/


	//Begin Pass 1
	int p = 0;											//Pass counter
	while (G_m > 0){
		p++;
		/*cout << "\n|--------------------- Pass ";
		cout << p;
		cout << " ----------------------| " << endl;
		*/G_BEST = -1 * INFINITY;						//Best Cumulative Gain in an iteration set to negative infinity (will be overwritten by G in each iteration)
				
		//Begin Iterations
		int k = 0; 										//Iteration counter
		while (!all_fixed){
			//Printing iteration number and fixed gates
			/*cout << "\n*********************************************\n" <<"Iteration: " << k <<"\t Unfixed gates:";*/
			k++;
			/*for (int n = 0; n < g_n/2; n++)			//Print unfixed gates
			{
				if(part_B[n].fixed==0){
					cout<< "  " << part_B[n].gate_name;
				}
				if(part_A[n].fixed==0){
					cout<< "  " << part_A[n].gate_name;
				}
			}
			cout<< "\n*********************************************\n";*/
			

			D = calc_D (part_A, part_B, g_n);					//Calc D for all gates 
			calc_delta_g (D, part_A, part_B, g_n, G);			//Calc highest gain in single iteration
			
			//Storing the iteration of BEST gain
			if(G > G_BEST){
				G_BEST = G;
				for(int i=0; i < (g_n/2); i++) {
					part_A_BEST[i] = part_A[i];
					part_B_BEST[i] = part_B[i];
				}   
			}
			all_fixed = (k == (g_n/2)); 						//If all fixed (Total num of iteration is half the gate count) --> exit the loop 
		}	
		//End of a pass

		for(int i=0; i < (g_n/2); i++) {						//Update partitions for next pass
			part_A[i] = part_A_BEST[i];
			part_B[i] = part_B_BEST[i];
		}
		for (int i = 0; i < g_n/2; i++){						//Unfix all gates
			part_B[i].fixed = 0;
			part_A[i].fixed = 0;
		}
		all_fixed = 0;
		G_m = G_BEST;

		/*cout << "|------------------ End of Pass ";			//To print config after each pass
		cout << p;
		cout << " ------------------| " << endl; 

		cout <<"\nG_m = " << G_m << endl;
		cout <<"\nConfiguration after Pass ";
		cout << p << " :\n";
		cout <<"Partition A: \n";
		print_partition(part_A_BEST, g_n/2);
		cout <<"\nPartition B: \n";
		print_partition(part_B_BEST, g_n/2);*/
	}
	
	for (int i = 0; i < g_n/2; i++){	
		partitionA [cut_n][i] = part_A_BEST[i];
		partitionB [cut_n][i] = part_B_BEST[i];
	}

	//Printing output after finishing KL
    cout <<"\n\n-> Best configuration after ";
	cout << p << " Pass(es):\n---------------------------------------\n";
    cout <<"Partition A ["<< cut_n << "]: \n";
    print_MinCut_partition(partitionA, cut_n);
    cout <<"\nPartition B ["<< cut_n << "]: \n";
    print_MinCut_partition(partitionB, cut_n);
}




//===============================================================//
//		A Function to swap gates in placement refinement		 //
//===============================================================//
void swap_gates(map<string,map<int,gate>> &partitionA, map<string,map<int,gate>> &partitionB, string cut_n){
	int k = 0;
	gate temp;
	for (auto i = partitionA[cut_n].begin();
         i != partitionA[cut_n].end(); ++i) {
		temp = partitionA[cut_n][k];
		partitionA[cut_n][k] = partitionB[cut_n][k];
		partitionB[cut_n][k] = temp;
		k++;
	}
	cout << "-> Swap " << cut_n << " partitions\n";
}




//================================================================//
//  	A Function to print the placement of any 2x4 grid		  //
//================================================================//
void print_placement(map<string,map<int,gate>> &partitionA, map<string,map<int,gate>> &partitionB, string cut_n[]){
	int k = 0;
	cout << "\n\t-----------------------------------------------------------------\n";

	for (int i = 3; i < 6; i = i+2)
	{
		cout << "\t|\t" << partitionA[cut_n[i]][0].gate_name << "\t|\t" << partitionB[cut_n[i]][0].gate_name;

	}
	cout << "\t|";
	cout << "\n\t-----------------------------------------------------------------\n";
	for (int i = 4; i < 7; i = i+2)
	{
		cout << "\t|\t" << partitionA[cut_n[i]][0].gate_name << "\t|\t" << partitionB[cut_n[i]][0].gate_name;

	}
	cout << "\t|";
	cout << "\n\t-----------------------------------------------------------------\n\n";
}



//================================================================//
//  	Apply Generic Min_Cut Placement for maximum 14 cuts		  //
//================================================================//
void MinCutP_Alg (gate gate_i[], int g_n, map<string,map<int,gate>> &partitionA, map<string,map<int,gate>> &partitionB, string cut_n[]){
	int k = 0;
	int i = 0;
	while (((partitionA[cut_n[i]].size() != 1) && (i != 7)))		//Stop when the partitions has only 1 gate or reached the max. cuts (2*7 = 14)
	{
		g_n = partitionA[cut_n[i]].size();
		for (int j = 0; j < g_n; j++)
		{
			gate_i[j] = partitionA[cut_n[i]][j];
		}
		k++;
		Apply_KL(gate_i, g_n, partitionA, partitionB, cut_n[k]);

		for (int j = 0; j < g_n; j++)
		{
			gate_i[j] = partitionB[cut_n[i]][j];
		}
		k++;
		Apply_KL(gate_i, g_n, partitionA, partitionB, cut_n[k]);
		i++;
	}
	cout << "\n||=========================================== Done ALL KL in MinCut ==========================================||\n\n\n";
}



//========================================================================//
//  	Apply Refinement to the total wire length for 2x4 grid only		  //
//========================================================================//
void placement_refine (map<string,map<int,gate>> &partitionA, map<string,map<int,gate>> &partitionB, string cut_n[]){
		int highest_con = 0;
		for (int i = 0; i < 2; i++)				//We try to assess the connectivity of each gate with its neighbouring gates in the other partitions
		{										//So that we can do just horizontal swaps to achieve lower wire length
		if (
			(connectivity[partitionA[cut_n[3+i]][0].gate_name][partitionA[cut_n[5+i]][0].gate_name] + 			//If (gateA) in 3a or 3b connectivty to gateA in 3c or 3d
			 connectivity[partitionA[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name]) 			//plus (gateA) in 3a or 3b connectivty to gateB in 3c or 3d
			> 																									//is higher than the connectivity of (gateB) in 3a or 3b 
			(connectivity[partitionB[cut_n[3+i]][0].gate_name][partitionA[cut_n[5+i]][0].gate_name] + 			//connected to gateA/B in 3c or 3d
			 connectivity[partitionB[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name])
		)
		{																									 	//Swap between GateA and B in 3a or 3b
			highest_con = connectivity[partitionA[cut_n[3+i]][0].gate_name][partitionA[cut_n[5+i]][0].gate_name] + connectivity[partitionA[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name];
			cout << "gateA [" << cut_n[3+i] << "] connectivity with gates A+B in [" << cut_n[5+i] << "] has higher value (connections) than gateB in same cut index does:  " << highest_con << endl;
			swap_gates (partitionA, partitionB, cut_n[3+i]);
		}
		else {
			highest_con = connectivity[partitionB[cut_n[3+i]][0].gate_name][partitionA[cut_n[5+i]][0].gate_name] + connectivity[partitionB[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name];
			cout << "gateB [" << cut_n[3+i] << "] connectivity with gates A+B in [" << cut_n[5+i] << "] has higher value (connections) than gateA in same cut index does:  " << highest_con << endl;
			cout << "-> No Swap\n";
		}			

		if (
			(connectivity[partitionA[cut_n[5+i]][0].gate_name][partitionA[cut_n[3+i]][0].gate_name] + 
		     connectivity[partitionA[cut_n[5+i]][0].gate_name][partitionB[cut_n[3+i]][0].gate_name]) 
			> 
			(connectivity[partitionB[cut_n[5+i]][0].gate_name][partitionA[cut_n[3+i]][0].gate_name] + 
			 connectivity[partitionB[cut_n[5+i]][0].gate_name][partitionB[cut_n[3+i]][0].gate_name])
		)
		{
			highest_con = connectivity[partitionA[cut_n[5+i]][0].gate_name][partitionA[cut_n[3+i]][0].gate_name] + connectivity[partitionA[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name];
			cout << "gateA [" << cut_n[5+i] << "] connectivity with gates A+B in [" << cut_n[3+i] << "] has higher value (connections) than gateB in same cut index does:  " << highest_con << endl;
			cout << "-> No Swap\n";
		}
		else {
			highest_con = connectivity[partitionB[cut_n[5+i]][0].gate_name][partitionA[cut_n[3+i]][0].gate_name] + connectivity[partitionB[cut_n[3+i]][0].gate_name][partitionB[cut_n[5+i]][0].gate_name];
			cout << "gateB [" << cut_n[5+i] << "] connectivity with gates A+B in [" << cut_n[3+i] << "] has higher value (connections) than gateA in same cut index does:  " << highest_con << endl;
			swap_gates (partitionA, partitionB, cut_n[5+i]);
		}
	}
}

#endif