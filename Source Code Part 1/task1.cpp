#include "task1.h"

int main() {
	ifstream netlist;
	ofstream outFile;
	netlist.open("netlist.txt");
	outFile.open("out.txt");
	int num_gates=0;        
	num_gates = count_components(netlist);
	vector<gate>gate_arr;
	fill_component_arr_general(netlist, gate_arr, num_gates);
	netlist.close();

	//Printing the gates names, types, and aspect ratios
	outFile << "//====================================================//" <<"\n\t\tAt Beginning\n"<<"//====================================================//\n";
	for (auto& g : gate_arr)
	{
		print_aspect_ratio(g, outFile);
	}	
	
	//====================================================//
	//       	First iteration				//
	//====================================================//
	vector<gate>Big_lvl_1;
	bool good_fit=1;
	Big_lvl_1 = merge(gate_arr);

	//Printing the gates names, types, and aspect ratios
	outFile << "//====================================================//" <<"\n\t\tAfter 1 Iteration\n"<<"//====================================================//\n";
	for (auto& g : Big_lvl_1)
	{
		print_aspect_ratio(g, outFile);	
	}
	//==================================================================================
	
	//====================================================//
	//       	Second iteration		      //
	//====================================================//
	reverse(Big_lvl_1.begin(), Big_lvl_1.end());	//Reverse the vector because the last gate that wasn't merged last iteration should be served first
	vector<gate>Big_lvl_2;
	good_fit=1;
	bool vertical=0;
	Big_lvl_2 = merge(Big_lvl_1);

	outFile << "//====================================================//" <<"\n\t\tAfter 2 Iterations\n"<<"//====================================================//\n";
	for (auto& g : Big_lvl_2)
	{
		print_aspect_ratio(g, outFile);
	} 
	//==================================================================================
	outFile<<endl;

	//====================================================//
	//       	Third iteration			      //
	//====================================================//
	reverse(Big_lvl_2.begin(), Big_lvl_2.end());	//Reverse the vector because the last gate that wasn't merged last iteration should be served first
	vector<gate>Big_lvl_3;
	good_fit=1;
	vertical=0;
	Big_lvl_3 = merge(Big_lvl_2);

	outFile << "//====================================================//" <<"\n\t\tAfter 3 Iterations\n"<<"//====================================================//\n";
	for (auto& g : Big_lvl_3)
	{
		//print_aspect_ratio(g, outFile);
	} 
	//==================================================================================
	outFile<<endl;

	//====================================================//
	//       	Fourth iteration		      //
	//====================================================//
	reverse(Big_lvl_3.begin(), Big_lvl_3.end());	//Reverse the vector because the last gate that wasn't merged last iteration should be served first
	vector<gate>Big_lvl_4;
	good_fit=1;
	vertical=0;
	Big_lvl_4 = merge(Big_lvl_3);

	outFile << "//====================================================//" <<"\n\t\tAfter 4 Iterations\n"<<"//====================================================//\n";
	for (auto& g : Big_lvl_4)
	{
		print_aspect_ratio(g, outFile);
	} 
	//==================================================================================
	outFile<<endl;

	//====================================================//
	//       	Fifth iteration			      //
	//====================================================//
	reverse(Big_lvl_4.begin(), Big_lvl_4.end());	//Reverse the vector because the last gate that wasn't merged last iteration should be served first
	vector<gate>Big_lvl_5;
	good_fit=1;
	vertical=0;
	//Big_lvl_5 = merge(Big_lvl_4);
	vertical = vert_or_horz(Big_lvl_4[0], Big_lvl_4[1]);
	Big_lvl_5.push_back(shape_gen(Big_lvl_4[0], Big_lvl_4[1], vertical));

	outFile << "//====================================================//" <<"\n\t\tAfter 5 Iterations\n"<<"//====================================================//\n";
	outFile<<endl;
	
	//====================================================//
	//       	Printing Outputs		      //
	//====================================================//
	
	//Finding the minimum area 	
	int 	Min_Area 	= Big_lvl_5[0].asp_vec[0].x_dim	* Big_lvl_5[0].asp_vec[0].y_dim;
	aspect 	min_asp 	= Big_lvl_5[0].asp_vec[0];
	int 	k		= 0;
	int 	min_area_pos	= 0;	//Holds the index of the minimum area aspect ratio
	for (auto& asp : Big_lvl_5[0].asp_vec){	
		if(asp.x_dim*asp.y_dim<Min_Area){
			Min_Area = asp.x_dim*asp.y_dim;
			min_asp  = asp;
			min_area_pos = k;	
		}
		k++;
	}
	int core_area=0;
	for (auto& g : gate_arr){	
		core_area=core_area + g.asp_vec[0].x_dim * g.asp_vec[0].y_dim;
	}
	string min_area_children = Big_lvl_5[0].asp_vec[min_area_pos].child_chain;
	//File as output
	outFile<<"\nThe Top Floorplan Slicing Tree is: "<<Big_lvl_5[0].name<<endl;
	outFile<<"The minimum area: "<<Min_Area<<"\t--\t Aspect Ratio: "<<min_asp.x_dim<<" by "<<min_asp.y_dim<<endl;
	outFile<<"The core area: "<<core_area<< "\t--\tThe core utilization: "<<(double)core_area/Min_Area<<endl;
	outFile<<"Tracing Back: "<<min_area_children;
	outFile.close();
	cout<<"\n\n\n \t\t\t Seeee Yaaaa!\n\n\n";
	//==================================================================================

	return 0;
}