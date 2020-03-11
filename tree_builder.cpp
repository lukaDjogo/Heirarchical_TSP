#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <string>

using namespace std;
int offset = 0;
int* gList;

void fill_list(int num_cities){
	gList = new int[num_cities];
	int n = num_cities;
	for(int i = 0; i < n; i++){
		gList[i] = i;
	}
	//randomly shuffling city order
	for(int i = 0; i < n; i++){
		int randIndex = rand()%n;
		int temp = gList[i];
		gList[i] = gList[randIndex];
		gList[randIndex] = temp;
	}
}

void print_list(int num_cities){
	int n = num_cities;
	cout<<"Printing cities indeces..."<<endl;
	for(int i = 0; i < n; i++)
		cout<<gList[i]<<" ";
	cout<<endl<<"Done."<<endl;
}

//function for a degenerate tree
void degen(std::ofstream *file, int cities){
	if(cities == 0)	return;
	if(cities == 1){
		*file<<gList[offset]<<endl;
		offset++;
		return;
	}
	*file<<'+'<<endl;
	degen(file, 1);
	degen(file, cities - 1);
	return;
}

//function for a random tree
void build(std::ofstream *file, int cities){
	if(cities == 0)	return;
	//base case, prints out a city index
	if(cities == 1){
		*file<<gList[offset]<<endl;
		offset++;
		return;
	}
	int front = rand()%cities;
	if(front == 0) front++;
	int back = cities - front;
	//left recursive call
	*file<<'+'<<endl;
	//left recursive call
	build(file, front);
	//right recursive call
	build(file, back);
}

void make_cities(ofstream *file, int cities){
	int n = cities;
	int boundary = n * 2;
	int x, y;
	string city_name;
	for(int i = 0; i < n; i++){
		x = rand()%boundary;
		y = rand()%boundary;
		//x += 0.1*(rand()%10);
		//y += 0.1*(rand()%10);
		city_name = "city_"+to_string(i+1);
		*file<<x<<" "<<y<<" "<<city_name<<endl;
	}
	return;
}

int main(){
	int input;
	int degenerate;
	cout<<"Enter 1 for a degenerate tree (worst case runtime), anything else otherwise"<<endl;
	cin>>degenerate;
	//int seed;
	//cout<<"Enter a rand seed (-1 for no seed):"<<endl;
	//cin>>seed;
	//if(seed != -1)	srand(seed);
	//else	srand(time(NULL));
	srand(time(NULL));
	cout<<"Enter a number of cities:\n";
	cin>>input;
	fill_list(input);
	

	int r = rand()%1000;
	string filename = std::to_string(r);
	ofstream file("complexT"+filename+".txt");
	if(degenerate != 1){
		build(&file, input);
	}
	else{
		degen(&file, input);
	}
	file.close();
	cout<<"Finished creating treefile complexT"<<filename<<".txt"<<endl;
//print_list(input);

	ofstream cfile("complexC"+filename+".txt");
	make_cities(&cfile, input);
	cfile.close();
	cout<<"Finished creating citiesfile complexC"<<filename<<".txt"<<endl;

	return 0;
}
