#include<iostream>
#include<sstream>
#include<fstream>
#include<list>
#include<vector>
#include<algorithm>
#include<string>
using namespace std;

void UCS(void);
void BFS(void);
void DFS(void);
struct node
{
	int rowId;
	int id, pathCost, depth;
	string parent, state, parentId;
};

bool comparePathCost(node first, node second)
{
	if (first.pathCost < second.pathCost)
		return true;
	else if (first.pathCost == second.pathCost)
	{
		if (first.state.compare(second.state)<0)
			return true;
	}
	else
		return false;

}
bool compareLevelCost(node first, node second)
{
	if (first.depth < second.depth)
		return true;
	else if (first.depth == second.depth)
	{
		if (first.state.compare(second.state)>0)
	return true;
	}
	else
		return false;
}

struct state
{
	string stateName; int rowId;
};
int arraySize = 0; int searchCh = 0; int i, j, row, col;
int nodeId = 0;
ifstream infile;
ofstream outfile;
stringstream ss;
struct node currnode; struct state *stemp; struct node *ntemp;
char sourceNode[20], destNode[20];
bool stateExistsInOpenQ = false, stateExistsInClosedQ = false;

list<state> states;
list<node> openQ;
list<node> closedQ;
list<node> children;
list<state>::iterator its = states.begin();
list<node>::iterator itn = closedQ.begin();
list<node>::iterator it1 = closedQ.begin();
list<node>::iterator it2 = closedQ.begin();
list<node>::iterator posIt = closedQ.begin();
struct node child;
vector<vector<int> > matrix;
int errflag = 0;
char line[100];
struct node n;
int main()
{

	struct state s;
	infile.open("input.txt");
	if (infile.is_open())
	{
		infile.getline(line, 20); //search choice
		searchCh = atoi(line);
		infile.getline(sourceNode, 20); //source node
		infile.getline(destNode, 20); //destination node
		infile.getline(line, 20); // number of states or arraySize
		arraySize = atoi(line);

		for (i = 0; i < arraySize; i++) //states
		{
			infile.getline(line, 20);
			s.rowId = i;
			s.stateName = line;
			states.push_back(s);
		}

		matrix.resize(arraySize); // num. of rows
		for (i = 0; i < arraySize; i++)
			matrix[i].resize(arraySize); // num. of cols
		for (row = 0; row < arraySize; row++) //populate matrix
		{
			infile.getline(line, 100);

			col = 0;
			std::istringstream iss(line); int a;
			while (iss >> a)
				matrix[row][col++] = a;


		}
		if (searchCh <1 || searchCh>3)
		{
			outfile << "Error: Incorrect search choice! (Enter 1: BFS, 2: DFS or 3: UCS)";
			errflag = 1;
		}
		if (strcmp(sourceNode, destNode) == 0)
		{
			errflag = 1;
			outfile << "NoPathAvailable";
		}
		string s1 = sourceNode; string s2 = destNode; int f1=0,f2 = 0;
		for (its = states.begin(); its != states.end(); ++its)
		{
			if (s1.compare(its->stateName) == 0)
			{
				f1 = 0; break;
			}
			else
			{
				f1 = 1;
			}
		}
		f2 = 0;
		for (its = states.begin(); its != states.end(); ++its)
		{
			if (s2.compare(its->stateName) == 0)
			{
				f2 = 0; break;
			}
			else
			{
				f2 = 1;
			}
		}
		if (f1 == 1 || f2==1)
		{
			outfile << "Error in input: Initial state or goal state not found in state space.";
			errflag = 1;
		}
		if (arraySize != states.size())
		{
			outfile << "Error in input: Check number of states!";
			errflag = 1;
		}
	}
	else
	{
		outfile << "Input file not found";
		errflag = 1;
	}
	infile.close();
	if (errflag != 1)
	{
		if (searchCh == 1)
		{
			BFS();
				}
		else if (searchCh == 2)
		{
			DFS();
			
		}
		else if (searchCh == 3)
		{
			UCS();
			}
	}
	getchar();
	return 0;
}
void BFS()
{
	struct node tempchk;
	outfile.open("output.txt");
	// push first node into openQ
	n.state = sourceNode;
	n.id = ++nodeId;
	n.depth = 0;
	n.pathCost = 0;
	n.parent = ' '; n.parentId = ' ';
	for (its = states.begin(); its != states.end(); ++its) //rowId for first node
	{
		stemp = &*its;
		if (stemp->stateName.compare(n.state) == 0)
		{
			n.rowId = stemp->rowId; break;
		}
	}

	openQ.push_back(n); // first node into openQ 
	while (true)
	{
		if (openQ.empty() == true)
		{
			outfile << "NoPathAvailable"; outfile.close();
			return;
		}

		//BEFORE POPPING CHECK IF THEY HAVE EQUAL LEVEL COSTS.
		//IF THEY DO, POP IN ALPHABETICAL ORDER
		tempchk = openQ.front();
		list<node>::iterator tempit = openQ.begin();
		for (itn = openQ.begin(); itn != openQ.end(); ++itn)
		{
			if (itn->depth < tempchk.depth)
			{
				tempchk = *itn; tempit = itn;
			}
			else if (itn->depth == tempchk.depth)
			{
				if (tempchk.state.compare(itn->state) > 0)
				{
					tempchk = *itn; tempit = itn;
				}
			}
		}
		currnode = tempchk;
		openQ.erase(tempit);
		
		if (currnode.state.compare(destNode) == 0) // Goal Test
		{
			closedQ.push_back(currnode);
			list<string> path;
			string p = "", s = "";;
			struct node n = closedQ.back();
			s = n.state;
			p = n.parent;
			int id = n.id;
			path.push_front(s);
			do
			{
				for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
					//if (itn->id == id)
				if (itn->state.compare(p) == 0)
				{
					s = itn->state;
					p = itn->parent;
					id = itn->id;
					break;
				}
				path.push_front(s);
			} while (id != 1);
			string s1 = "";
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				s1 = s1 + itn->state + "-";
			}
			int a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';
			s1 = "";
			for (list<string>::iterator ss = path.begin(); ss != path.end(); ++ss)
			{
				s1 = s1 + *ss + "-";
				
			}
			a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';
			outfile << currnode.pathCost;
			
			outfile.close();
			return;
		}

		row = currnode.rowId; col = 0; int r, c;
		while (col < arraySize)
		{
			if (matrix[row][col] != 0)
			{
				n.id = 65535; //set dummy id
				n.parentId = currnode.id;
				for (its = states.begin(); its != states.end(); ++its) //set parent and state name
				{
					stemp = &*its;
					if (stemp->rowId == row)
					{
						n.parent = stemp->stateName;

					}
					if (stemp->rowId == col)
					{
						n.state = stemp->stateName;
						n.rowId = stemp->rowId;
					}
				} //end for
				n.pathCost = currnode.pathCost + matrix[row][col];
				n.depth = currnode.depth + 1;
				children.push_back(n);
			}
			col++;
		}

		// Slide112
		struct node *onode = NULL, *cnode = NULL; int flag = 0;
		while (!children.empty())
		{
			child = children.front(); //remove first child node
			children.pop_front();
			stateExistsInClosedQ = false; stateExistsInOpenQ = false;
			for (itn = openQ.begin(); itn != openQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInOpenQ = true;
					onode = ntemp; it1 = itn;
				}
			}
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInClosedQ = true;
					cnode = ntemp; it2 = itn;
				}
			}
			// CASE 1
			if (stateExistsInClosedQ == false && stateExistsInOpenQ == false)
				// if no node in open or closed has child’s state
			{
				child.id = ++nodeId;
				openQ.push_back(child);
			} // end if state does in exist in openQ or closedQ

			// CASE 2
			else if (stateExistsInOpenQ) // else if there exists node in open that has child’s state
			{
				if (child.depth < onode->depth)
				{
					openQ.erase(it1);
					child.id = ++nodeId;
					openQ.push_back(child);
					
				}
			}// end if state exists in openQ

			// CASE 3
			else if (stateExistsInClosedQ) // else if there exists node in closed that has child’s state
			{
				if (child.depth < cnode->depth)
				{
					closedQ.erase(it2);
					child.id = ++nodeId;\
					openQ.push_back(child);
				}
			} // end if state exists in closedQ
		} // end while children not empty

		closedQ.push_back(currnode); //Log the popped node in closedQ
		

	}
}
void DFS()
{
	struct node tempchk;
	outfile.open("output.txt");
	// push first node into openQ
	n.state = sourceNode;
	n.id = ++nodeId;
	n.depth = 0;
	n.pathCost = 0;
	n.parent = ' '; n.parentId = ' ';
	for (its = states.begin(); its != states.end(); ++its) //rowId for first node
	{
		stemp = &*its;
		if (stemp->stateName.compare(n.state) == 0)
		{
			n.rowId = stemp->rowId; break;
		}
	}

	openQ.push_front(n); // first node into openQ 
	while (true)
	{
		if (openQ.empty() == true)
		{
			outfile << "NoPathAvailable"; outfile.close();
			return;
		}

		//BEFORE POPPING CHECK IF THEY HAVE EQUAL LEVEL COSTS.
		//IF THEY DO, POP IN ALPHABETICAL ORDER
		tempchk = openQ.front();
		list<node>::iterator tempit = openQ.begin();
		for (itn = openQ.begin(); itn != openQ.end(); ++itn)
		{
			
			if (itn->depth == tempchk.depth)
			{
				if (tempchk.state.compare(itn->state) > 0)
				{
					tempchk = *itn; tempit = itn;
				}
			}
		}
		currnode = tempchk;
		openQ.erase(tempit);
		//currnode = openQ.front();
		//openQ.remove(tempchk);

		if (currnode.state.compare(destNode) == 0) // Goal Test
		{
			closedQ.push_back(currnode);
			list<string> path;
			string p = "", s = "";;
			struct node n = closedQ.back();
			s = n.state;
			p = n.parent;
			int id = n.id;
			path.push_front(s);
			do
			{
				for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
					//if (itn->id == id)
				if (itn->state.compare(p) == 0)
				{
					s = itn->state;
					p = itn->parent;
					id = itn->id;
					break;
				}
				path.push_front(s);
			} while (id != 1);
			string s1 = "";
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				s1 = s1 + itn->state + "-";
					}
			int a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';
			s1 = "";
			for (list<string>::iterator ss = path.begin(); ss != path.end(); ++ss)
			{
				s1 = s1 + *ss + "-";
				//outfile << *ss<<"-";
				//cout << *ss << "-";
			}
			a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';
			outfile << currnode.pathCost;
			outfile.close();
			return;
		}

		row = currnode.rowId; col = 0; int r, c;
		while (col < arraySize)
		{
			if (matrix[row][col] != 0)
			{
				n.id = 65535; //set dummy id
				n.parentId = currnode.id;
				for (its = states.begin(); its != states.end(); ++its) //set parent and state name
				{
					stemp = &*its;
					if (stemp->rowId == row)
					{
						n.parent = stemp->stateName;

					}
					if (stemp->rowId == col)
					{
						n.state = stemp->stateName;
						n.rowId = stemp->rowId;
					}
				} //end for
				n.pathCost = currnode.pathCost + matrix[row][col];
				n.depth = currnode.depth + 1;
				children.push_back(n);
			}
			col++;
		}

		// Slide112
		struct node *onode = NULL, *cnode = NULL; int flag = 0;
		while (!children.empty())
		{
			child = children.front(); //remove first child node
			children.pop_front();
			stateExistsInClosedQ = false; stateExistsInOpenQ = false;
			for (itn = openQ.begin(); itn != openQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInOpenQ = true;
					onode = ntemp; it1 = itn;
				}
			}
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInClosedQ = true;
					cnode = ntemp; it2 = itn;
				}
			}
			// CASE 1
			if (stateExistsInClosedQ == false && stateExistsInOpenQ == false)
				// if no node in open or closed has child’s state
			{
				child.id = ++nodeId;
				openQ.push_front(child);
			} // end if state does in exist in openQ or closedQ

			// CASE 2
			else if (stateExistsInOpenQ) // else if there exists node in open that has child’s state
			{
				if (child.depth < onode->depth)
				{
					openQ.erase(it1);
					child.id = ++nodeId;
					openQ.push_front(child);
				}
			}// end if state exists in openQ

			// CASE 3
			else if (stateExistsInClosedQ) // else if there exists node in closed that has child’s state
			{
				if (child.depth < cnode->depth)
				{
					closedQ.erase(it2);
					child.id = ++nodeId;
					openQ.push_front(child);
				}
			} // end if state exists in closedQ
		} // end while children not empty

		closedQ.push_back(currnode); //Log the popped node in closedQ
	
	}
}
void UCS()
{
	
	outfile.open("output.txt");
	// push first node into openQ
	n.state = sourceNode;
	n.id = ++nodeId;
	n.pathCost = 0;
	n.parent = ' ';
	for (its = states.begin(); its != states.end(); ++its) //rowId for first node
	{
		stemp = &*its;
		if (stemp->stateName.compare(n.state) == 0)
		{
			n.rowId = stemp->rowId; break;
		}
	}

	openQ.push_back(n); // first node into openQ 

	while (true)
	{
		if (openQ.empty() == true)
		{
			outfile << "NoPathAvailable";
			outfile.close();
			return;
		}

		currnode = openQ.front();
		openQ.pop_front();
		if (currnode.state.compare(destNode) == 0) // Goal Test
		{
			
			closedQ.push_back(currnode); 
			list<string> path; 
			string p = "", s = "";;
			list<node>::reverse_iterator rit = closedQ.rbegin();
			
			struct node n= closedQ.back();
			s = n.state;
			p = n.parent;
			path.push_front(s);
			do
			{
				for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
				if (itn->state.compare(p) == 0)
				{
					s = itn->state;
					p = itn->parent;
					break;
				}
				path.push_front(s);
			} while (p != " ");
			string s1="";
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				s1 = s1 + itn->state + "-";
			}
			int a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';  
			s1 = "";
			for (list<string>::iterator ss = path.begin(); ss != path.end(); ++ss)
			{
				s1 = s1 + *ss + "-";
			}
			a = s1.find_last_of("-");
			s1.replace(a, 1, " ");
			outfile << s1;
			outfile << '\n';
			outfile<< currnode.pathCost;
				outfile.close();
			return;
		}

		row = currnode.rowId; col = 0; int r, c;
		while (col < arraySize)
		{
			if (matrix[row][col] != 0)
			{
				n.id = 65535; //set dummy id
				for (its = states.begin(); its != states.end(); ++its) //set parent and state name
				{
					stemp = &*its;
					if (stemp->rowId == row)
						n.parent = stemp->stateName;
					if (stemp->rowId == col)
					{
						n.state = stemp->stateName;
						n.rowId = stemp->rowId;
					}
				} //end for
				n.pathCost = currnode.pathCost + matrix[row][col];
				children.push_back(n);
			}
			col++;
		}

		// Slide112
			struct node *onode = NULL, *cnode = NULL; int flag = 0;
		while (!children.empty()) 
		{
			child = children.front(); //remove first child node
			children.pop_front();
			stateExistsInClosedQ = false; stateExistsInOpenQ = false;
			for (itn = openQ.begin(); itn != openQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInOpenQ = true;
					onode = ntemp; it1 = itn;
				}
			}
			for (itn = closedQ.begin(); itn != closedQ.end(); ++itn)
			{
				ntemp = &*itn;
				if (child.state.compare(ntemp->state) == 0)
				{
					stateExistsInClosedQ = true;
					cnode = ntemp; it2 = itn;
				}
			}
			// CASE 1
			if (stateExistsInClosedQ == false && stateExistsInOpenQ == false)
				// if no node in open or closed has child’s state
			{
					flag = 0;
					for (itn = openQ.begin(); itn != openQ.end(); ++itn) //in between
					{
						if (comparePathCost(child,*itn)==true)
						{
							child.id = ++nodeId;
							openQ.insert(itn, child);
							flag = 1; break;
						}
						
					 }
					if (flag != 1)
					{
						child.id = ++nodeId;
						openQ.push_back(child);
					}
				
			} // end if state does in exist in openQ or closedQ
			// CASE 2
			else if (stateExistsInOpenQ) // else if there exists node in open that has child’s state
			{
				if (child.pathCost < onode->pathCost)
				{
					openQ.erase(it1);
					for (itn = openQ.begin(); itn != openQ.end(); ++itn) //in between
					{
						if (comparePathCost(child, *itn) == true)
						{
							child.id = ++nodeId;
							openQ.insert(itn, child);
							flag = 1;
						}
					}
					if (flag != 1)
					{
						child.id = ++nodeId;
						openQ.push_back(child);
					}
				}
			}// end if state exists in openQ
		
			// CASE 3
			else if (stateExistsInClosedQ) // else if there exists node in closed that has child’s state
			{
				if (child.pathCost < cnode->pathCost)
				{
					closedQ.erase(it2);
					for (itn = openQ.begin(); itn != openQ.end(); ++itn) //in between
					{
						if (comparePathCost(child, *itn) == true)
						{
							child.id = ++nodeId;
							openQ.insert(itn, child);
							flag = 1;
						}
					}
					if (flag != 1)
					{
						child.id = ++nodeId;
						openQ.push_back(child);
					}
				} 
			} // end if state exists in closedQ
		} // end while children not empty

		closedQ.push_back(currnode); //Log the popped node in closedQ
		
	}
}
	
//uninformed_search::uninformed_search()
//{
//	
//}
//
//uninformed_search::~uninformed_search()
//{
//}
