#include <vector>
#include <iostream>
#include <fstream>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <string>
#include <bits/stdc++.h> 

using namespace std;


//class used to store problem formulation and algorithm to dynamically solve problem of H-TSP
class Traveler {
	public: 
		
		//--------------------------------------Objects Used by Traveler----------------------------------------------------------------

		//These are the objects used in the table so path and path length can be extracted
		//will use linked list to keep track of path so constant time path keeping
		class TableNode {
			public:
				std::vector<int> path;  //this is the path between two vertices
				int pathLength;  //this is the ideal length between the two vertice

				TableNode() { 
					pathLength = INT_MAX;  //change all million hardcodings
				}
		};

		//definition of a tree node
		class Node {
			public:
				Node* left;
				Node* right;
				bool isCity;  //this specifies whether inner node or leaf(city)
				//isCity true
				int cityID;  //this will tell us which city node is(if city)
				
				std::vector<int> leftSet;
				std::vector<int> rightSet;
				
				//Constructor used when inner node
				Node() {
					left = right = nullptr;
					isCity = false;
				}

				//constructor that is used when city created
				Node(int cityID){
					left = right = nullptr;
					isCity = true;
					this->cityID = cityID;
					leftSet.push_back(cityID);
					//added this case because algorithm becomes complicated if one set is empty as graph will never
					// be proper. So when we build the tree we make sure the set at each non-city node that has a 
					// child that is a city only takes one vector for its left and right set
					rightSet.push_back(cityID); 
				}

		};

		//definition of a city
		class City {
			public:
				double x;
				double y;
				string name;
				// generic constructor so program doesn't complain
				City(){
					x = 0;
					y = 0;
					name = "invalid city";
				}
				//constructor used
				City(double xVal, double yVal, string input_name){
					x = xVal;
					y = yVal;
					name = input_name;
				}
		};


		//this is our city list vector
		std::vector<City> cities;

		//this is our tree root
		Node* root;

		//This is the 2d-table of tableNode's we will dynamically fill in during the algorithm
		TableNode **opt;

		//Objects used in fillInTable function to retrieve paths
		struct threeSet {
			int optDist = INT_MAX;
			int cameFrom = 0;
		};

		struct fourSet {
			int optDist = INT_MAX;
			int cameFrom2 = 0;
			int cameFrom3 = 0;
		};

		//-------------------------------------End of Objects used for Traveler Solver----------------------------------------------

		//>>> constructor used <<<
		Traveler() {
			this->root = nullptr;
		}


		//---------------------------------------------File Reading-----------------------------------------------------------------
		
		//Dynamically create OPT table
		void create_OPT(int num){
			opt = new TableNode*[num];
			for (int i = 0; i < num; ++i) {
  				opt[i] = new TableNode[num];
  			}
			//values will be implicitly set to Inf through constructor of TableNode
		}
		
		
		//create a vector of the cities
		void create_cities(std::string filename){
			double xVal = 0;
			double yVal = 0;
			City c;
			string line;
			string input_name = "";
			ifstream iFile;
			iFile.open(filename);
			while(iFile >> xVal >> yVal >> input_name){
				c = City(xVal, yVal, input_name);
				cities.push_back(c);
			}
			iFile.close();
			//once we know amount of cities create table for DP
			create_OPT(cities.size());
		}


		//recursive build function called with completed city list and .tree filename
		Node* build(std::vector<std::string>& line, int& curr){ //we plug in zero and the size of the array initially
			
			//calling right and left subtrees
			if(line[curr].compare("+") == 0){
				Node* temp = new Node();
				temp->left = build(line, ++curr);
				temp->right = build(line, curr);  //don't need to increment curr because else did it already for left subtree
				
				//here we create sets for specific subtree root we are on using iterators
				//need to build tree proper in terms of sets so we avoid setting sets with double elements
				if( !(temp->left->isCity) && !(temp->right->isCity) ) {  //if neither children nodes cities
					(temp->leftSet).insert((temp->leftSet).begin(), (temp->left->leftSet).begin(), (temp->left->leftSet).end());
					(temp->leftSet).insert((temp->leftSet).end(), (temp->left->rightSet).begin(), (temp->left->rightSet).end());
					(temp->rightSet).insert((temp->rightSet).begin(), (temp->right->leftSet).begin(), (temp->right->leftSet).end());
					(temp->rightSet).insert((temp->rightSet).end(), (temp->right->rightSet).begin(), (temp->right->rightSet).end());
				}
				else if(temp->left->isCity) {
					if(temp->right->isCity) { //if both nodes are cities this loop entered
						(temp->leftSet).insert((temp->leftSet).begin(), (temp->left->leftSet).begin(), (temp->left->leftSet).end());
						(temp->rightSet).insert((temp->rightSet).begin(), (temp->right->leftSet).begin(), (temp->right->leftSet).end());
					}
					else{ //if only left node is a city
						(temp->leftSet).insert((temp->leftSet).begin(), (temp->left->leftSet).begin(), (temp->left->leftSet).end());
						(temp->rightSet).insert((temp->rightSet).begin(), (temp->right->leftSet).begin(), (temp->right->leftSet).end());
						(temp->rightSet).insert((temp->rightSet).end(), (temp->right->rightSet).begin(), (temp->right->rightSet).end());
					}
				}
				else { //if only right node is a city
					(temp->leftSet).insert((temp->leftSet).begin(), (temp->left->leftSet).begin(), (temp->left->leftSet).end());
					(temp->leftSet).insert((temp->leftSet).end(), (temp->left->rightSet).begin(), (temp->left->rightSet).end());
					(temp->rightSet).insert((temp->rightSet).begin(), (temp->right->leftSet).begin(), (temp->right->leftSet).end());
				}

				//at this point we created subtree
				return temp;
			}
			else {
				Node* temp = new Node(std::stoi(line[curr]));  //we need to convert string int to our respective integer
				curr++;
				return temp;
			}
			
		}

		//this builds our tree according to the file given
		void buildTree(std::string filename){
			ifstream treeFile;  //this ifstream is where we read in the tree and build it
			std::vector<std::string> tree;
			std::string x;
			treeFile.open(filename);
			
			//check that file is good
			// if (!treeFile) {
			// 	cout << "Unable to open file" << std::endl;
			// 	exit(1); // terminate with error
			// }
			
			while (treeFile >> x) {
				tree.push_back(x);
			}
			
			treeFile.close();

			//this sets our root with the tree
			int curr = 0;
			this->root = build(tree, curr);
		}

		//-----------------------------------------------End of File Reading--------------------------------------------------------------





		//---------------------------------------------Functions For Solving Tree-----------------------------------------------------

		//this will return recta-linear distance between two cities in constant time
        int dist(int city1, int city2){
            //check that cities are valid
            return abs(cities[city1].x-cities[city2].x) + abs(cities[city1].y-cities[city2].y);
        }

		//this is the function where we fill in the table according to the graph configuration
		//We will first calculate the opt dist to each third set vertex, then use that to calculate to the fourth
		//
		//Two indexes will be filled at each start to end filling : (enter,exit)(exit,enter)
		//
		//We will store the result opt dist in the table along with two combining paths (First->Second, Third->Fourth)
		//or (Fourth->Third, Second->First)
		
		
		void fillInTable(std::vector<int>& firstSet,std::vector<int>& secondSet,
		std::vector<int>& thirdSet,std::vector<int>& fourthSet)
		{
			//how we will create our optimal distances
			std::vector<threeSet> optToThird;  //opt distance from the entering vertex to each vertex in the third set
			std::vector<fourSet> optToFourth; //opt distance from the entering vertex to each vertex in the fourth set

			//filling in vectors to be length of their respective sets
			threeSet temp1; 
			fourSet temp2;
			for(int x = 0; x < thirdSet.size(); x++){
					optToThird.push_back(temp1);
			}
			
			for(int y = 0; y < fourthSet.size(); y++){
					optToFourth.push_back(temp2);
			}

			int j, k, l, n, o, p, q; //looping variables
			
			for(int i : firstSet) {  // i will be the entering point of the path
				//we want to set every optimal to be INF for both OPT vectors
				for(j = 0; j < optToThird.size(); j++){
					optToThird[j].optDist = INT_MAX;
				}
				for(k = 0; k < optToFourth.size(); k++){
					optToFourth[k].optDist = INT_MAX;
				}

				//here we fill in optToThird according to starting vertex
				for(int m : secondSet) {
					for(l = 0; l < thirdSet.size(); l++) {
						if ( (opt[i][m].pathLength + dist(m, thirdSet[l]) ) < optToThird[l].optDist) {
							optToThird[l].cameFrom = m;
							optToThird[l].optDist = opt[i][m].pathLength + dist(m, thirdSet[l]);
						}
					}
				}

				//here we fill in optToFourth according to optToThird
				for(n = 0; n < thirdSet.size(); n++){
					for(o = 0; o < fourthSet.size(); o++){
						if ( (opt[thirdSet[n]][fourthSet[o]].pathLength + optToThird[n].optDist ) < optToFourth[o].optDist){
							optToFourth[o].cameFrom2 = optToThird[n].cameFrom;
							optToFourth[o].cameFrom3 = thirdSet[n];
							optToFourth[o].optDist = opt[thirdSet[n]][fourthSet[o]].pathLength + optToThird[n].optDist;
						}
					}
				}

				//here we finally store our results in the opt table
				for(p = 0; p < optToFourth.size(); p++) {
					opt[i][fourthSet[p]].pathLength = optToFourth[p].optDist;
			
					//here we in linear time set the optimal path by concatenating the two paths making up the optimal distance
					(opt[i][fourthSet[p]].path).clear();
					(opt[i][fourthSet[p]].path).insert((opt[i][fourthSet[p]].path).begin(), (opt[i][optToFourth[p].cameFrom2].path).begin(), (opt[i][optToFourth[p].cameFrom2].path).end());
					(opt[i][fourthSet[p]].path).insert((opt[i][fourthSet[p]].path).end(), (opt[optToFourth[p].cameFrom3][fourthSet[p]].path).begin(), (opt[optToFourth[p].cameFrom3][fourthSet[p]].path).end());
					
					//we do the same for the exit being the entering node as it is the same optimal path
					opt[fourthSet[p]][i].pathLength = optToFourth[p].optDist;
				
					(opt[fourthSet[p]][i].path).clear();
					for(q = opt[i][fourthSet[p]].path.size() - 1; q > -1; q--) {
						(opt[fourthSet[p]][i].path).push_back((opt[i][fourthSet[p]].path)[q]);
					}
				}

			}
		}



		//calls algorithm to fill in OPT table according to four possible graph configurations
        void fillInTableHelper(std::vector<int>& leftLeft,std::vector<int>& leftRight,
		std::vector<int>& rightLeft, std::vector<int>& rightRight) 
		{ 
			fillInTable(leftLeft, leftRight, rightLeft, rightRight);
			fillInTable(leftRight, leftLeft, rightLeft, rightRight);
			fillInTable(leftLeft, leftRight, rightRight, rightLeft);
			fillInTable(leftRight, leftLeft, rightRight, rightLeft);
		}

		
		//This function fills in opt using single-source as a subroutine
		void fillOpt (Node *top) {
			if (top == nullptr) return;
			if (top->isCity) {
				opt[top->cityID][top->cityID].pathLength = 0;
				(opt[top->cityID][top->cityID].path).push_back(top->cityID);
				return;
			}
			else {
				fillOpt(top->left);
				fillOpt(top->right);
				fillInTableHelper(top->left->leftSet, top->left->rightSet, top->right->leftSet, top->right->rightSet);
			}
			//after this recusively should be done
		}



		//---------------------------------------------End of Functions For Solving Tree-----------------------------------------------------


		//Function to output Solution
		void printFinalPath() {
			std::string units = "";
			// Comment below if we don't want to specify units in our program
			// std::cout << "Define Unit System Please: " << std::endl;
			// std::cin >> units;


			std::cout << "" << std::endl;
			int x, y;
			x = y = 0;
			int minDist = 1000001;
			for(int i : (root -> leftSet)) {
				for(int j : (root -> rightSet)) {
					if (opt[i][j].pathLength < minDist){
						minDist = opt[i][j].pathLength;
						x = i;
						y = j;
					}
				}
			}
			
			//print the path of our optimal solution
			std::cout << "Optimal Path:  " << std::endl;
			for(int j : opt[x][y].path) {
				std::cout << cities[j].name << std::endl;	
			}

			std::cout << "\n";
			std::cout << "Optimal Distance: " << minDist << " " << units << std::endl;
		}


		//this function is used to debug tree input and make sure it was properly created
		void print_Tree(Node* temp){
			if(temp == nullptr){
				return;
			}

			for(int i = 0; i < temp->leftSet.size(); i++){
				std::cout << temp->leftSet[i] << endl;
			}

			for(int j = 0; j < temp->rightSet.size(); j++){
				std::cout << temp->rightSet[j] << endl;
			}


			print_Tree(temp->left);
			print_Tree(temp->right);
			
		}
};


//this is where object created and things are output
int main(int num_args, char* argv[]) {
	
	// if we want time this is where we uncomment to enable
	// time_t start, end;
	// time(&start);
	// ios_base::sync_with_stdio(false);

	//This object is where all specification of problem is stored and where all functions to solve specification lay
	Traveler* specs = new Traveler();

	//Here we create the vector of input cities and then 2d table for optimal paths
	specs->create_cities(((string)argv[1]));
	
	// uncomment to test that cities were built properly
	// for (int i = 0; i < specs->cities.size(); i++){
	// 	std::cout << specs->cities[i].name << std::endl;
	// }

	//Here we will have a tree built correlating directly to our input tree specifications
	specs->buildTree(((string)argv[2]));
	//At this point everything is brought in from input
	
	
	//uncommenting this line shows tree brought in
	// specs->print_Tree(specs->root);


	//Here we fill our optimal paths table from city to city
	specs->fillOpt(specs->root);
	
	//Uncomment to test that table filled in
	// std::cout << specs->opt[0][0].pathLength << std::endl;
	// std::cout << specs->opt[1][1].pathLength << std::endl;
	// std::cout << specs->opt[2][2].pathLength << std::endl;
	// std::cout << specs->opt[3][3].pathLength << std::endl;
	// std::cout << specs->opt[3][3].path[0] << std::endl;
	// std::cout << specs->opt[1][3].pathLength << std::endl;
	// std::cout << specs->opt[3][1].pathLength << std::endl;
	

	//We call a function to print the optimal path and its distance
	specs->printFinalPath();

	//end of time (uncomment to enable time testing)
	// time(&end);
	// double time_taken = double(end - start); 
    // cout << "Time taken by program is : " << fixed 
    //      << time_taken << setprecision(5); 
    // cout << " sec " << endl;

	return 0;
}