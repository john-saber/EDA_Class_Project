//====================================================//
//       Credits: John Saber & Abdullah Allam         //
//        	    KL Algorithm     		      //
//====================================================//

#ifndef NETLIST_MATRIX_H
#define NETLIST_MATRIX_H
#include <iostream>
#include <fstream>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <vector>
#include <sstream>

using namespace std;

struct aspect
{
	int x_dim;
	int y_dim;
	string child_chain;
};

class gate 
{
public: 
	string			name;
	string 			gate_type;		//FF, AND, OR, ...
	vector<aspect> 		asp_vec;		//Vector of possible aspect ratios of the block
	bool			merged=0;
};

gate shape_gen(gate &A, gate &B, bool vertical){
	gate G_BIG; 
	int k=0;
	aspect tmp;
	//Prevent them from merging again in the same iteration
	A.merged = 1;
	B.merged = 1;
	//Choosing composition
	if (!vertical)
	{
		G_BIG.name = "(" + A.name + " H " + B.name + ")";
		for (auto& asp_A_i : A.asp_vec)
		{
			for (auto& asp_B_j : B.asp_vec)
			{
				//First possiblity x_big=x_A0+x_B0 -- y_big=max(y_A0,y_B0) ____ A.asp_vec[0] means A0 and A.asp_vec[1] means A1
				tmp.x_dim	=	asp_A_i.x_dim	+	asp_B_j.x_dim;
				tmp.y_dim	=	max(	asp_A_i.y_dim	,	asp_B_j.y_dim	);
				tmp.child_chain	= 	asp_A_i.child_chain	+	"  &  "	+	asp_B_j.child_chain;	//asp_A_i.chain is something like F1 = F1.asp[0] -- F2 = F2.asp[1]
				//Fill the aspect ratio vector
				G_BIG.asp_vec.push_back(tmp);
				//


				/*			On hold till I try trees
				//Fill the children corresponding aspect ratios vector -- for tracing back
				G_BIG.children_asp.push_back({ asp_A_i , asp_B_j });
				*/
				//k++;
			}
			//k++;
		}
	}
	else
	{
		G_BIG.name = "(" + A.name + " V " + B.name + ")";
		for (auto& asp_A_i : A.asp_vec)
		{
			for (auto& asp_B_j : B.asp_vec)
			{
				//First possiblity x_big=x_A0+x_B0 -- y_big=max(y_A0,y_B0) ____ A.asp_vec[0] means A0 and A.asp_vec[1] means A1
				tmp.y_dim	=	asp_A_i.y_dim	+	asp_B_j.y_dim;
				tmp.x_dim	=	max(	asp_A_i.x_dim	,	asp_B_j.x_dim	);
				tmp.child_chain	= 	asp_A_i.child_chain	+	"  &  "	+	asp_B_j.child_chain;	//asp_A_i.chain is something like F1 = F1.asp[0] -- F2 = F2.asp[1]
				G_BIG.asp_vec.push_back(tmp);
				//
				

				/*			On hold till I try trees
				//Fill the children corresponding aspect ratios vector -- for tracing back
				G_BIG.children_asp.push_back({ asp_A_i , asp_B_j });
				*/
				//k++;
			}
			//k++;	
		}
	}
	return G_BIG;
}

void print_aspect_ratio(gate g, ofstream &outFile)
{
	outFile<<"Gate Name: "<<g.name<<"\tGate type: "<<g.gate_type<<"\nPrinting aspect ratios:\n";
	for (auto& A_i : g.asp_vec)
	{
		outFile<<"x_dim= "<<A_i.x_dim<<"\ty_dim= "<<A_i.y_dim;
		outFile<<"\t--\tChildren:\t"<< A_i.child_chain<<endl;
	}
}

int count_components (ifstream &file) {
    int n=0;
    string tmp;
    while (file){
        getline(file,tmp);
        n++;
    }
    n = n-1;
    file.clear();
    file.seekg(0);
    return n;
}

int count_in (string str, string word) {
    int i=0, j, temp, countW=0, chk;
    while(str[i]!='\0')
    {
        temp = i;
        j=0;
        while(word[j]!='\0')
        {
            if(str[i]==word[j])
                i++;
            j++;
        }
        chk = i-temp;
        if(chk==j)
            countW++;
        i = temp;
        i++;
    }
    return countW;
}
void fill_comp(gate &g1, string str){ 	
	//Filling the gate type and name
	stringstream line(str);
	line >> g1.gate_type;
	line >> g1.name;
	
	//Counting number if inputs
	int n, x_dim, y_dim;
	n=count_in(str,"in");

	//Calculate the aspect ratio --> filling the asp_vec vector
	aspect tmp_asp1, tmp_asp2;
	if(g1.gate_type=="Flipflop"){
		tmp_asp1.x_dim = 1; 
		tmp_asp1.y_dim = 8;
		tmp_asp2.x_dim = 8; 
		tmp_asp2.y_dim = 1;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="And"){
		tmp_asp1.x_dim = n+1; 
		tmp_asp1.y_dim = n+1;
		tmp_asp2.x_dim = n+1; 
		tmp_asp2.y_dim = n+1;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Or"){
		tmp_asp1.x_dim = n+1; 
		tmp_asp1.y_dim = n+1;
		tmp_asp2.x_dim = n+1; 
		tmp_asp2.y_dim = n+1;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Xor"){
		tmp_asp1.x_dim = 2*n; 
		tmp_asp1.y_dim = n+2;
		tmp_asp2.x_dim = n+2; 
		tmp_asp2.y_dim = 2*n;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Inv"){
		tmp_asp1.x_dim = 2; 
		tmp_asp1.y_dim = 1;
		tmp_asp2.x_dim = 1; 
		tmp_asp2.y_dim = 2;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Nand"){
		tmp_asp1.x_dim = n; 
		tmp_asp1.y_dim = n;
		tmp_asp2.x_dim = n; 
		tmp_asp2.y_dim = n;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Nor"){
		tmp_asp1.x_dim = n; 
		tmp_asp1.y_dim = n;
		tmp_asp2.x_dim = n; 
		tmp_asp2.y_dim = n;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	else if(g1.gate_type=="Xnor"){
		//g1.asp_vec.push_back({2*n,n});	//First aspect ratio
		//g1.asp_vec.push_back({n,2*n});	//Second aspect ratio
		tmp_asp1.x_dim = 2*n; 
		tmp_asp1.y_dim = n;
		tmp_asp2.x_dim = n; 
		tmp_asp2.y_dim = 2*n;
		g1.asp_vec.push_back(tmp_asp1);	//First aspect ratio
		g1.asp_vec.push_back(tmp_asp2);	//Second aspect ratio
	}
	//
	g1.asp_vec[0].child_chain = g1.name + "=" + to_string(g1.asp_vec[0].x_dim) + "by" + to_string (g1.asp_vec[0].y_dim);
	g1.asp_vec[1].child_chain = g1.name + "=" + to_string(g1.asp_vec[1].x_dim) + "by" + to_string (g1.asp_vec[1].y_dim);
}

void fill_component_arr_general (ifstream &file, vector<gate> &gates, int n){
    string tmp;
    for (int i=0; i<n; i++){
    	gate   g;
        getline(file,tmp);
        fill_comp(g, tmp);
	gates.push_back(g);
    }
    file.clear();
    file.seekg(0);
}

bool vert_or_horz(gate A, gate B){
	int sum_X_A=0, sum_Y_A=0, sum_X_B=0, sum_Y_B=0;
	bool vertical=0;	//By default, it's horizontal
	for (auto& asp_A : A.asp_vec)
	{
		sum_X_A = sum_X_A + asp_A.x_dim;
		sum_Y_A = sum_Y_A + asp_A.y_dim;
	}
	for (auto& asp_B : B.asp_vec)
	{
		sum_X_B = sum_X_B + asp_B.x_dim;
		sum_Y_B = sum_Y_B + asp_B.y_dim;
	}
	if( (sum_X_A+sum_X_B) > (sum_Y_A+sum_Y_B) )
	{
		vertical = 1;	//Putting horizontally make it too wide; thus, choose vertical
	}
	return vertical;
}

bool is_good_fit(gate &A, gate &B){
	bool good_fit=0;

	return good_fit;
}

double calc_area(gate &A, gate &B, bool vertical){
	gate big;
	double area_A_B = INFINITY;
	big = shape_gen(A , B, vertical);
	A.merged = 0;
	B.merged = 0;
	for (auto& asp : big.asp_vec){	
		if(asp.x_dim*asp.y_dim<area_A_B){
			area_A_B = asp.x_dim*asp.y_dim;
		}
	}
	return area_A_B;
}

vector<gate> merge(vector<gate>&gate_arr)
{
	vector<gate>Big;
	gate* best_fit;	double least_area; double area_A_B; 
	bool vertical=0;
	for (auto& A : gate_arr)		//This loop goes through all gates to merge them one by one
	{
		least_area=INFINITY;
		if (A.merged){continue;}	//Skip merged blocks
		for (auto& B : gate_arr)	//This loop determines the best fit to be merged with gate A.
		{
			if (B.merged||A.merged||A.name==B.name){
				continue;
			}	//Skip merged blocks
			
			//See if will apply vertical or horizontal or vertical composition
			vertical = vert_or_horz(A,B); 

			//See if B is best fit to be merged 
			area_A_B = calc_area(A, B, vertical);
			//cout<<"A: "<< A.name <<" \t--\tB: "<<B.name;
			//cout<<endl;
			if( least_area>area_A_B ){
				least_area = area_A_B;	//Block B is the current best fit for A according to the least_area function (This metric can be changed with a better one)
				best_fit = &B;
			}			
		}
		if (!best_fit->merged){		//To avoid merging twice in odd netlists 
			Big.push_back(shape_gen(A, *best_fit, vertical));	
		}	//Skip merged blocks
	}
	//Is there unmerged blocks?
	for (auto& g : gate_arr)
	{
		if (!g.merged)
		{
			Big.push_back(g);
		}
		
	}
	return Big;
}
#endif		