//==================================================//
//       Credits: John Saber & Abdullah Allam       //
//        	       MinCut_Placement	      	    //
//==================================================//

#include "MinCut_Placement.h"

	int main(){
	ifstream file;
	int count_arr[3];									//Array of counters contains(n,p_n,g_n)
	file.open("netlist_small_no_hypernets.txt");
	count_nodes(file, count_arr);								//Count all components in the netlist (gates + pins)
	int n = count_arr[0];
	int p_n = count_arr[1];
	int g_n = count_arr[2];
	pin pin_i[p_n];
	gate gate_i[g_n];
	file.open("netlist_small_no_hypernets.txt");
	fill_struct (file,gate_i,pin_i,n);
	print_pin_struct_info(pin_i,p_n);
	print_gate_struct_info(gate_i,g_n);
	fill_con_matrix(pin_i, gate_i, n, p_n, g_n);
	print_con_matrix(pin_i, gate_i, n, p_n, g_n);
	map<string,map<int,gate>> partitionA, partitionB;			//For Min-Cut placement with 2 integer indeces (cut string index and gate index)
	string cut_n[15] = {"1", "2a", "2b", "3a", "3b", "3c", "3d", "4a", "4b", "4c", "4d" , "4e", "4f", "4g", "4h"}; //Cut indeces with max 15 cuts
	Apply_KL(gate_i, g_n, partitionA, partitionB, cut_n[0]);		//Do initial cut by KL (it produces 2 partitions A & B)

	//Apply Minimun Cut placement to get Initial placement
	//----------------------------------------------------
	MinCutP_Alg(gate_i, g_n, partitionA, partitionB, cut_n);		//Apply Min-Cut Placement "Generic" Algorithm works for any number of netlist
	cout << "\n\nInitial Placement:\n";
	print_placement(partitionA,partitionB, cut_n);

	//Refinement (extra work: to get shorter total wire length)		//Only for 2x4 grid
	//---------------------------------------------------------
	placement_refine(partitionA, partitionB, cut_n);			//Apply refinement to the extracted placement to get a placement with lower wire length with same Min-Cut
	cout << "\n\nFinal Placement:\n";
	print_placement(partitionA,partitionB, cut_n);
		
}
