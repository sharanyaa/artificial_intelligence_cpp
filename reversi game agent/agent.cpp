//#include "agent.h"
#include<iostream>
#include<vector>
#include<fstream>
#include<algorithm>
#include<sstream>
#include<list>
using namespace std;

ifstream infile;
ofstream outfile;
struct node
{
	int rowNum, colNum; string name;
	char colChar; int depth, value, a, b;
	//bool legal;
};
string gameLog;
char line[100];
int searchCh;
static int twoPass = 0;
int cutOffDepth = 0;
char opponentPlayer;
bool passMin = false, passMax = false;
char myPlayer; // black - x and white - o
int i = 0, j = 0, row = 0, col = 0, errflag = 0, ii = 0, jj = 0, k = 0;
vector<vector<char> > initialStateMatrix;
int weightMatrix[8][8] =
{ { 99, -8, 8, 6, 6, 8, -8, 99 },
{ -8, -24, -4, -3, -3, -4, -24, -8 },
{ 8, -4, 7, 4, 4, 7, -4, 8 },
{ 6, -3, 4, 0, 0, 4, -3, 6 },
{ 6, -3, 4, 0, 0, 4, -3, 6 },
{ 8, -4, 7, 4, 4, 7, -4, 8 },
{ -8, -24, -4, -3, -3, -4, -24, -8 },
{ 99, -8, 8, 6, 6, 8, -8, 99 } };

int Minimax_MaxValue(vector<vector<char> >, node, char, char, node&, vector<vector<char> >&);
int Minimax_MinValue(vector<vector<char> >, node, char, char, node&, vector<vector<char> >&);
int AlphaBeta_MaxValue(vector<vector<char> >, node, char, char, node&, vector<vector<char> >&, int, int);
int AlphaBeta_MinValue(vector<vector<char> >, node, char, char, node&, vector<vector<char> >&, int, int);

vector< vector<char> > CopyMatrix(vector< vector<char> > originalMatrix)
{
	vector<vector<char> > newMatrix;
	newMatrix.resize(8);
	for (int i = 0; i < 8; i++)
		newMatrix[i].resize(8);
	for (int i = 0; i < 8;i++)
	for (int j = 0; j < 8; j++)
		newMatrix[i][j] = originalMatrix[i][j];
	return newMatrix;
}
string PrintGameLog(string name, int depth, int value, int a, int b)
{
	string str = "";
	str += name + ',';
	ostringstream oss;  string s;
	oss << depth;
	s = oss.str();
	str += s + ',';
	if (value == -9999)
		str += "-Infinity,";
	//else if (n.value == std::numeric_limits<int>::max())
	else if (value == 9999)
		str += "Infinity";
	else
	{
		ostringstream oss;
		if (value < 0)
		{
			int a = -(value);
			oss << a;
			s = oss.str();
			str += "-" + s + ",";
		}
		else
		{
			oss << value;
			s = oss.str();
			str += s + ",";
		}
	}
	if (a == -9999)
		str += "-Infinity,";
	//else if (n.value == std::numeric_limits<int>::max())
	else if (a == 9999)
		str += "Infinity,";
	else
	{
		ostringstream oss; 
		if (a < 0)
		{
			int x = -(a);
			oss << x;
			s = oss.str();
			str += "-" + s + ",";
		}
		else
		{
			oss << a;
			s = oss.str();
			str += s + ",";
		}
	}
	if (b == -9999)
		str += "-Infinity\n";
	else if (b == 9999)
		str += "Infinity\n";
	else
	{
		ostringstream oss;
		if (b < 0)
		{
			int x = -(b);
			oss << x;
			s = oss.str();
			str += "-" + s + "\n";
		}
		else
		{
			oss << b;
			s = oss.str();
			str += s + "\n";
		}
	}
	return str;
}
string PrintGameLog(string name, int depth, int value)
{
	string str = "";
	str+= name + ',';
	ostringstream oss;  string s;
	oss << depth;
	s = oss.str();
	str += s + ',';
	if (value == -9999)
		str += "-Infinity\n";
	else if (value == 9999)
		str += "Infinity\n";
	else
	{
		ostringstream oss;
		if (value < 0)
		{
			int a = -(value);
			oss << a;
			s = oss.str();
			str += "-" + s + "\n";
		}
		else
		{
			oss << value;
			s = oss.str();
			str += s + "\n";
		}
	}
	return str;
}
void countCoins(vector<vector<char> > arr, int &x, int &o)
{
	x = 0; o = 0;
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
		{
			if (arr[i][j] == 'X')
				x++;
			else if (arr[i][j] == 'O')
				o++;
		}
	}
}
node TieBreak(node a, node b)
{
	if (a.rowNum < b.rowNum)
		return a;
	else if (b.rowNum < a.rowNum)
		return b;
	else
	{
		if (a.colNum < b.colNum)
			return a;
		else if (b.colNum < a.colNum)
			return b;
		else
			return a;
	}
}
int GetMaxUtility(node a, node b)
{
	if (a.value > b.value) return a.value;
	else if (b.value > a.value) return b.value;
	else
	{
		node x = TieBreak(a, b);
		return x.value;
	}
}
int GetMinUtility(node a, node b)
{
	if (a.value < b.value) return a.value;
	else if (b.value < a.value) return b.value;
	else
	{
		node x = TieBreak(a, b);
		return x.value;
	}
}
int GetMaxVal(int a, int b)
{
	if (a >= b) return a;
	else return b;
}
int GetMinVal(int a, int b)
{
	if (a <= b) return a;
	else return b;
}
list<node> FindLegalMoves(vector<vector<char> > state, char me, char opp, int anchori[][8], int anchorj[][8])
{
	list<node> legalNodeList; int anchorforMoveNumIndex = 0;
	bool tlLegal = false, tLegal = false, trLegal = false, mlLegal = false, mLegal = false;
	bool mrLegal = false, blLegal = false, bLegal = false, brLegal = false;
	bool legalFlag = false; int posi = -1, posj = -1;
	for (int x = 0; x < 10; x++)
	{

		for (int y = 0; y < 8; y++)
		{
			anchori[x][y] = -1; anchorj[x][y] = -1;
		}
	}
	for (i = 0; i < 8; i++)
	{
		legalFlag = false; posi = -1, posj = -1;
		for (j = 0; j < 8; j++)
		{
			node n;
			legalFlag = false;
			n.colChar = ' ';
			n.colNum = -1; n.rowNum = -1; n.value = 0;
			n.name = " ";
			// n.depth = 0; 
			if (state[i][j] == '*') // if the cell is empty
			{
				posi = i; posj = j;
				tlLegal = false, tLegal = false, trLegal = false, mlLegal = false, mLegal = false;
				mrLegal = false, blLegal = false, bLegal = false, brLegal = false;
				if (i - 1 >= 0 && j - 1 >= 0)
				{
					if (state[i - 1][j - 1] == opp) // Top Left - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii--; jj--;
						while (--ii >= 0 && --jj >= 0) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								tlLegal = true;
								anchori[anchorforMoveNumIndex][0] = ii;
								anchorj[anchorforMoveNumIndex][0] = jj;
								//anchorIndex++;
								break;
							}
							else if (state[ii][jj] == '*')
							{
								tlLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (i - 1 >= 0)
				{
					if (legalFlag == false && state[i - 1][j] == opp) //Top - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii--;
						//jj--;
						while (--ii >= 0) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								tLegal = true;
								anchori[anchorforMoveNumIndex][1] = ii;
								anchorj[anchorforMoveNumIndex][1] = jj;
								//anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								tLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (i - 1 >= 0 && j + 1 < 8)
				{
					if (legalFlag == false && state[i - 1][j + 1] == opp) //Top right - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii--;
						jj++;
						while (--ii >= 0 && ++jj < 8) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								trLegal = true;
								anchori[anchorforMoveNumIndex][2] = ii;
								anchorj[anchorforMoveNumIndex][2] = jj;
								//anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								trLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (j - 1 >= 0)
				{
					if (legalFlag == false && state[i][j - 1] == opp) //Middle left - adjacent opponent coin
					{
						ii = i;
						jj = j;
						//ii--;
						jj--;
						while (--jj >= 0) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								mlLegal = true;
								anchori[anchorforMoveNumIndex][3] = ii;
								anchorj[anchorforMoveNumIndex][3] = jj;
								//anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								mlLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (j + 1 < 8)
				{
					if (legalFlag == false && state[i][j + 1] == opp) //Middle right - adjacent opponent coin
					{
						ii = i;
						jj = j;
						//ii--;
						jj++;
						while (++jj < 8) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								mrLegal = true;
								anchori[anchorforMoveNumIndex][4] = ii;
								anchorj[anchorforMoveNumIndex][4] = jj;
								//	anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								mrLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (i + 1 < 8 && j - 1 >= 0)
				{
					if (legalFlag == false && state[i + 1][j - 1] == opp) // Bottom Left - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii++;
						jj--;
						while (++ii < 8 && --jj >= 0) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								blLegal = true;
								anchori[anchorforMoveNumIndex][5] = ii;
								anchorj[anchorforMoveNumIndex][5] = jj;
								//	anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								blLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (i + 1 < 8)
				{
					if (legalFlag == false && state[i + 1][j] == opp) // Bottom - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii++;
						//jj--;
						while (++ii < 8) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								bLegal = true;
								anchori[anchorforMoveNumIndex][6] = ii;
								anchorj[anchorforMoveNumIndex][6] = jj;
								///anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								bLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (i + 1 < 8 && j + 1 < 8)
				{
					if (legalFlag == false && state[i + 1][j + 1] == opp) // Bottom right - adjacent opponent coin
					{
						ii = i;
						jj = j;
						ii++;
						jj++;
						while (++ii < 8 && ++jj < 8) // 1 or more opponent coins in that direction
						{
							if (state[ii][jj] == opp)
								continue;
							else if (state[ii][jj] == me)
							{
								brLegal = true;
								anchori[anchorforMoveNumIndex][7] = ii;
								anchorj[anchorforMoveNumIndex][7] = jj;
								//anchorIndex++; 
								break;
							}
							else if (state[ii][jj] == '*')
							{
								brLegal = false; break;
								// posi = -1; posj = -1;
							}
						}
						if (tlLegal || tLegal || trLegal || mlLegal || mLegal || mrLegal || blLegal || bLegal || brLegal)
							legalFlag = true;
					}
				}
				if (legalFlag == true)
				{
					anchorforMoveNumIndex++;
					n.colNum = posj + 1;
					n.rowNum = posi + 1;
					n.colChar = (char)(n.colNum + 48);
					n.colChar = 'a' - 1 + n.colNum;
					std::ostringstream os1, os2;
					os1 << n.colChar; string s1 = os1.str();
					os2 << n.rowNum; string s2 = os2.str();
					n.name = s1 + s2;
					legalNodeList.push_back(n);
				}
			}
		}
	}
	return legalNodeList;
}
vector<vector<char> > FlipCoins(vector<vector<char> > stateMatrix, int posi, int posj, char player, int anchori[], int anchorj[])
{
	vector<vector<char> > flipMatrix = stateMatrix;
	//top left direction
	if (anchori[0] != -1 && anchorj[0] != -1)
	{
		for (int x = posi, y = posj; x >= anchori[0] && y >= anchorj[0]; x--, y--)
			flipMatrix[x][y] = player;
	}
	//top direction
	if (anchori[1] != -1 && anchorj[1] != -1)
	{
		for (int x = posi, y = posj; x >= anchori[1]; x--)
			flipMatrix[x][y] = player;
	}
	//top right direction
	if (anchori[2] != -1 && anchorj[2] != -1)
	{
		for (int x = posi, y = posj; x >= anchori[2] && y <= anchorj[2]; x--, y++)
			flipMatrix[x][y] = player;
	}
	//middle left direction
	if (anchori[3] != -1 && anchorj[3] != -1)
	{
		for (int x = posi, y = posj; y >= anchorj[3]; y--)
			flipMatrix[x][y] = player;
	}
	//middle right direction
	if (anchori[4] != -1 && anchorj[4] != -1)
	{
		for (int x = posi, y = posj; y <= anchorj[4]; y++)
			flipMatrix[x][y] = player;
	}
	//bottom left direction
	if (anchori[5] != -1 && anchorj[5] != -1)
	{
		for (int x = posi, y = posj; x <= anchori[5] && y >= anchorj[5]; x++, y--)
			flipMatrix[x][y] = player;
	}
	//bottom direction
	if (anchori[6] != -1 && anchorj[6] != -1)
	{
		for (int x = posi, y = posj; x <= anchori[6]; x++)
			flipMatrix[x][y] = player;
	}
	//bottom right direction
	if (anchori[7] != -1 && anchorj[7] != -1)
	{
		for (int x = posi, y = posj; x <= anchori[7] && y < anchorj[7]; x++, y++)
			flipMatrix[x][y] = player;
	}
	return flipMatrix;
}
int CalculateEval(vector<vector<char> > PosMatrix, char me, char opp)
{
	int weightPlayer = 0, weightOpp = 0, evalFn = 0;
	for (int x = 0; x < 8; x++)
	{
		for (int y = 0; y < 8; y++)
		{
			int w = weightMatrix[x][y];
			if (PosMatrix[x][y] == me)
				weightPlayer += w;
			else if (PosMatrix[x][y] == opp)
				weightOpp += w;
		}
		evalFn = weightPlayer - weightOpp;
	}
	return evalFn;
}

int Minimax_MaxValue(vector<vector<char> > stateMatrix, node currNode, char me, char opp, node &chosenNode, vector<vector<char> > &resultMatrix)
{
	if (currNode.depth % 2 == 0)
	{
		me = myPlayer; opp = opponentPlayer;
	}
	else
	{
		opp = myPlayer; me = opponentPlayer;
	}
	int chosenScore = -9999; int xcount = 0, ocount = 0;
	countCoins(stateMatrix, xcount, ocount);
	//currNode.depth++;
	if (currNode.depth % 2 == 0 && currNode.value == 9999)
		currNode.value = -9999;
	else if (currNode.depth % 2 != 0 && currNode.value == -9999)
		currNode.value = 9999;
	if (currNode.depth == cutOffDepth)
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	if(xcount == 0 || ocount == 0 || (xcount + ocount) >= 64) //TERMINAL TEST - cutoff or all X or all O or full board
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	list<node> legalMoves; 
	int anchori[10][8] = { -1 };
	int anchorj[10][8] = { -1 };
	int posi, posj;
	legalMoves = FindLegalMoves(stateMatrix, me, opp, anchori, anchorj);
	int bestScore = -9999; node bestMove;
	if (legalMoves.empty())
	{
		passMax = true; node newNode; newNode.value = 9999; newNode.depth = currNode.depth + 1;
		if (currNode.name.compare("pass") == 0 || passMin==true) // TWO PASS
		{
			if (currNode.depth % 2 == 0)
			{
				me = myPlayer; opp = opponentPlayer;
			}
			else
			{
				opp = myPlayer; me = opponentPlayer;
			}
			chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
			currNode.name = "pass";
			currNode.value = chosenScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
			chosenNode = currNode;
			resultMatrix = CopyMatrix(stateMatrix);
			return chosenScore;
		}
		if (currNode.name.compare("root") != 0)
			newNode.name = "pass";
		if (currNode.name.compare("root")==0 && passMin == false) //no move for root
			newNode.name = "pass";
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		//CHECK INFINITY AND -INFINITY VALUES during pass

		vector<vector<char> > resultMatrix1; node newMove;
		vector<vector<char> > tempBoard = CopyMatrix(stateMatrix);
		int newScore = (Minimax_MinValue(tempBoard, newNode, opp, me, newMove, resultMatrix1));
		newNode.value = newScore;
		if (newScore > bestScore) // if better ie greater
		{
			bestScore = newScore;
			bestMove = newNode;
			resultMatrix = CopyMatrix(tempBoard);
		}
		else if (newScore == bestScore) // if equal, tie break
		{
			node bestMove1 = TieBreak(bestMove, newNode);
			if (bestMove1.name.compare(bestMove.name) != 0)
				resultMatrix = CopyMatrix(tempBoard);
		}
		currNode.value = bestScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}
	else
	{
		passMax = false;node newMove; vector<vector<char> > resultMatrix1;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		int moveIndex = 0;
		for (list<node>::iterator it = legalMoves.begin(); it != legalMoves.end(); ++it, moveIndex++)
		{
			it->value = 9999; it->depth=currNode.depth + 1;
			posi = it->rowNum - 1; posj = it->colNum - 1;
			//bestScore = -9999; 
			vector<vector<char> > tempBoard = CopyMatrix(stateMatrix); 
			vector<vector<char> > flipMatrix = FlipCoins(tempBoard, posi, posj, me, anchori[moveIndex], anchorj[moveIndex]);
			int newScore = Minimax_MinValue(flipMatrix, *it, opp, me, newMove, resultMatrix1);
			it->value = newScore;
			if (newScore > bestScore) // if better ie greater
			{
				bestScore = newScore;
				bestMove = *it;
				resultMatrix = CopyMatrix(flipMatrix);
			}
			else if (newScore == bestScore) // if equal, tie break
			{
				node bestMove1 = TieBreak(bestMove, *it);
				if (bestMove1.name.compare(bestMove.name)!=0)
					resultMatrix = CopyMatrix(flipMatrix);
			}	
			currNode.value = bestScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		}
		
		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}

}
int Minimax_MinValue(vector<vector<char> > stateMatrix, node currNode, char me, char opp, node &chosenNode, vector<vector<char> > &resultMatrix)
{
	if (currNode.depth % 2 == 0)
	{
		me = myPlayer; opp = opponentPlayer;
	}
	else
	{
		opp = myPlayer; me = opponentPlayer;
	}
	int chosenScore = 9999; int xcount = 0, ocount = 0;
	countCoins(stateMatrix, xcount, ocount);
	//currNode.depth++;
	if (currNode.depth % 2 == 0 && currNode.value == 9999)
		currNode.value = -9999;
	else if (currNode.depth % 2 != 0 && currNode.value == -9999)
		currNode.value = 9999;
	if (currNode.depth == cutOffDepth)
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
		if(xcount == 0 || ocount == 0 || (xcount + ocount) >= 64) //TERMINAL TEST - cutoff or all X or all O or full board
	{
			if (currNode.depth % 2 == 0)
			{
				me = myPlayer; opp = opponentPlayer;
			}
			else
			{
				opp = myPlayer; me = opponentPlayer;
			}
			chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	list<node> legalMoves;
	int anchori[10][8] = { -1 };
	int anchorj[10][8] = { -1 };
	int posi, posj;
	legalMoves = FindLegalMoves(stateMatrix, me, opp, anchori, anchorj);
	int bestScore = 9999;
	node bestMove;
	if (legalMoves.empty())
	{
		passMin = true; node newNode; newNode.value = -9999; newNode.depth = currNode.depth + 1;
		if (currNode.name.compare("pass") == 0 || passMax==true) //TWO PASS
		{
			if (currNode.depth % 2 == 0)
			{
				me = myPlayer; opp = opponentPlayer;
			}
			else
			{
				opp = myPlayer; me = opponentPlayer;
			}
			chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
			currNode.name = "pass";
			currNode.value = chosenScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
			chosenNode = currNode;
			resultMatrix = CopyMatrix(stateMatrix);
			return chosenScore;

		}
		if (currNode.name.compare("root") != 0)
			newNode.name = "pass";
		if (currNode.name.compare("root")==0 && passMin == false) //no move for root
			newNode.name = "pass";
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		//CHECK INFINITY AND -INFINITY VALUES during pass
		
		vector<vector<char> > resultMatrix1; node newMove;
		vector<vector<char> > tempBoard = CopyMatrix(stateMatrix);
		int newScore = (Minimax_MaxValue(tempBoard, newNode, opp, me, newMove, resultMatrix1));
		newNode.value = newScore;
		if (newScore < bestScore) // if better ie lesser
		{
			bestScore = newScore;
			bestMove = newNode;
			resultMatrix = CopyMatrix(tempBoard);
		}
		else if (newScore == bestScore) // if equal, tie break
		{
			node bestMove1 = TieBreak(bestMove, newNode);
			if (bestMove1.name.compare(bestMove.name) != 0)
				resultMatrix = CopyMatrix(tempBoard);
		}
		currNode.value = bestScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}
	else
	{
		passMin = false; node newMove;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		int moveIndex = 0;
		for (list<node>::iterator it = legalMoves.begin(); it != legalMoves.end(); ++it, moveIndex++)
		{
			it->value = 9999; it->depth = currNode.depth + 1;
			posi = it->rowNum - 1; posj = it->colNum - 1;
			//bestScore = 9999;
			vector<vector<char> > tempBoard = CopyMatrix(stateMatrix); vector<vector<char> > resultMatrix1;
			vector<vector<char> > flipMatrix = FlipCoins(tempBoard, posi, posj, me, anchori[moveIndex], anchorj[moveIndex]);
			int newScore = (Minimax_MaxValue(flipMatrix, *it, opp, me, newMove, resultMatrix1));
			it->value = newScore;
			if (newScore < bestScore) // if better ie lesser
			{
				bestScore = newScore;
				bestMove = *it;
				resultMatrix = CopyMatrix(flipMatrix);
			}
			else if (newScore == bestScore) // if equal, tie break
			{
				node bestMove1 = TieBreak(bestMove, *it);
				if (bestMove1.name.compare(bestMove.name) != 0)
					resultMatrix = CopyMatrix(flipMatrix);
			}
			currNode.value = bestScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value);
		}

		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}

}
void MiniMaxDecision(vector<vector<char> > stateMatrix, vector<vector<char> > &resultMatrix)
{
	gameLog = "Node,Depth,Value\n";
	struct node n, maxnode;
	n.depth = 0; n.colChar = ' '; n.rowNum = -1; n.colNum = -1;
	n.value = -9999;
	n.name = "root";
	int maxResult = (Minimax_MaxValue(stateMatrix, n, myPlayer, opponentPlayer, maxnode, resultMatrix));
	cout << "\n\n" << maxnode.name<<endl<<endl;
}
int AlphaBeta_MaxValue(vector<vector<char> > stateMatrix, node currNode, char me, char opp, node &chosenNode, vector<vector<char> > &resultMatrix,int alpha, int beta)
{
	if (currNode.depth % 2 == 0)
	{
		me = myPlayer; opp = opponentPlayer;
	}
	else
	{
		opp = myPlayer; me = opponentPlayer;
	}
	int chosenScore = -9999; int xcount = 0, ocount = 0; int v = -9999;
	countCoins(stateMatrix, xcount, ocount);
	//currNode.depth++;
	if (currNode.depth % 2 == 0 && currNode.value == 9999)
		currNode.value = -9999;
	else if (currNode.depth % 2 != 0 && currNode.value == -9999)
		currNode.value = 9999;
	if (currNode.depth == cutOffDepth)
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value,alpha,beta);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	if (xcount == 0 || ocount == 0 || (xcount + ocount) >= 64) //TERMINAL TEST - cutoff or all X or all O or full board
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	list<node> legalMoves;
	int anchori[10][8] = { -1 };
	int anchorj[10][8] = { -1 };
	int posi, posj;
	legalMoves = FindLegalMoves(stateMatrix, me, opp, anchori, anchorj);
	int bestScore = -9999; node bestMove;
	if (legalMoves.empty())
	{
		passMax = true; node newNode; newNode.value = 9999; newNode.depth = currNode.depth + 1;
		if (currNode.name.compare("pass") == 0 || passMin == true) // TWO PASS
		{
			if (currNode.depth % 2 == 0)
			{
				me = myPlayer; opp = opponentPlayer;
			}
			else
			{
				opp = myPlayer; me = opponentPlayer;
			}
			chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
			currNode.name = "pass";
			currNode.value = chosenScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
			chosenNode = currNode;
			resultMatrix = CopyMatrix(stateMatrix);
			return chosenScore;
		}
		
		if (currNode.name.compare("root") != 0)
			newNode.name = "pass";
		if (currNode.name.compare("root")==0 && passMin == false) //no move for root
			newNode.name = "pass";
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		//CHECK INFINITY AND -INFINITY VALUES during pass

		vector<vector<char> > resultMatrix1; node newMove;
		vector<vector<char> > tempBoard = CopyMatrix(stateMatrix);
		int newScore = (AlphaBeta_MinValue(tempBoard, newNode, opp, me, newMove, resultMatrix1,alpha,beta));
		newNode.value = newScore;
		if (newScore > bestScore) // if better ie greater
		{
			bestScore = newScore;
			bestMove = newNode;
			resultMatrix = CopyMatrix(tempBoard);
		}
		else if (newScore == bestScore) // if equal, tie break
		{
			node bestMove1 = TieBreak(bestMove, newNode);
			if (bestMove1.name.compare(bestMove.name) != 0)
				resultMatrix = CopyMatrix(tempBoard);
		}

		v = GetMaxVal(v, newScore);
		if (v >= beta)
		{
			chosenScore = bestScore;
			chosenNode = bestMove;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
			return v;
		}
		alpha = GetMaxVal(alpha, v);

		currNode.value = bestScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}
	else
	{
		passMax = false; node newMove; vector<vector<char> > resultMatrix1;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		int moveIndex = 0;
		for (list<node>::iterator it = legalMoves.begin(); it != legalMoves.end(); ++it, moveIndex++)
		{
			it->value = 9999; it->depth = currNode.depth + 1;
			posi = it->rowNum - 1; posj = it->colNum - 1;
			//bestScore = -9999; 
			vector<vector<char> > tempBoard = CopyMatrix(stateMatrix);
			vector<vector<char> > flipMatrix = FlipCoins(tempBoard, posi, posj, me, anchori[moveIndex], anchorj[moveIndex]);
			int newScore = AlphaBeta_MinValue(flipMatrix, *it, opp, me, newMove, resultMatrix1,alpha,beta);
			it->value = newScore;
			v = GetMaxVal(v, newScore);
			if (newScore > bestScore) // if better ie greater
			{
				bestScore = newScore;
				bestMove = *it;
				resultMatrix = CopyMatrix(flipMatrix);
			}
			else if (newScore == bestScore) // if equal, tie break
			{
				node bestMove1 = TieBreak(bestMove, *it);
				if (bestMove1.name.compare(bestMove.name) != 0)
					resultMatrix = CopyMatrix(flipMatrix);
			}

			currNode.value = bestScore;
			if (v >= beta)
			{
				chosenScore = bestScore;
				chosenNode = bestMove;
				gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
				return v;
			}
			alpha = GetMaxVal(alpha, v);
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		}
		chosenScore = bestScore;
		chosenNode = bestMove;
		return v;
	}

}
int AlphaBeta_MinValue(vector<vector<char> > stateMatrix, node currNode, char me, char opp, node &chosenNode, vector<vector<char> > &resultMatrix, int alpha, int beta)
{
	if (currNode.depth % 2 == 0)
	{
		me = myPlayer; opp = opponentPlayer;
	}
	else
	{
		opp = myPlayer; me = opponentPlayer;
	}
	int chosenScore = 9999; int xcount = 0, ocount = 0;
	countCoins(stateMatrix, xcount, ocount); int v = 9999;
	//currNode.depth++;
	if (currNode.depth % 2 == 0 && currNode.value == 9999)
		currNode.value = -9999;
	else if (currNode.depth % 2 != 0 && currNode.value == -9999)
		currNode.value = 9999;
	if (currNode.depth == cutOffDepth)
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	if (xcount == 0 || ocount == 0 || (xcount + ocount) >= 64) //TERMINAL TEST - cutoff or all X or all O or full board
	{
		if (currNode.depth % 2 == 0)
		{
			me = myPlayer; opp = opponentPlayer;
		}
		else
		{
			opp = myPlayer; me = opponentPlayer;
		}
		chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
		currNode.value = chosenScore;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		chosenNode = currNode;
		resultMatrix = CopyMatrix(stateMatrix);
		return chosenScore;
	}
	list<node> legalMoves;
	int anchori[10][8] = { -1 };
	int anchorj[10][8] = { -1 };
	int posi, posj;
	legalMoves = FindLegalMoves(stateMatrix, me, opp, anchori, anchorj);
	int bestScore = 9999;
	node bestMove;
	if (legalMoves.empty())
	{
		passMin = true; node newNode; newNode.value = -9999; newNode.depth = currNode.depth + 1;
		if (currNode.name.compare("pass") == 0 || passMax == true) //TWO PASS
		{
			if (currNode.depth % 2 == 0)
			{
				me = myPlayer; opp = opponentPlayer;
			}
			else
			{
				opp = myPlayer; me = opponentPlayer;
			}
			chosenScore = CalculateEval(stateMatrix, myPlayer, opponentPlayer);
			currNode.name = "pass";
			currNode.value = chosenScore;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
			chosenNode = currNode;
			resultMatrix = CopyMatrix(stateMatrix);
			return chosenScore;

		}
		if (currNode.name.compare("root") != 0)
			newNode.name = "pass";
		if (currNode.name.compare("root")==0 && passMin == false) //no move for root
			newNode.name = "pass";
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		//CHECK INFINITY AND -INFINITY VALUES during pass

		vector<vector<char> > resultMatrix1; node newMove;
		vector<vector<char> > tempBoard = CopyMatrix(stateMatrix);
		int newScore = (AlphaBeta_MaxValue(tempBoard, newNode, opp, me, newMove, resultMatrix1,alpha,beta));
		newNode.value = newScore;
		if (newScore < bestScore) // if better ie lesser
		{
			bestScore = newScore;
			bestMove = newNode;
			resultMatrix = CopyMatrix(tempBoard);
		}
		else if (newScore == bestScore) // if equal, tie break
		{
			node bestMove1 = TieBreak(bestMove, newNode);
			if (bestMove1.name.compare(bestMove.name) != 0)
				resultMatrix = CopyMatrix(tempBoard);
		}

		v = GetMinVal(v, newScore);
		if (v <= alpha)
		{
			chosenScore = bestScore;
			chosenNode = bestMove;
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
			return v;
		}
		beta = GetMinVal(beta, v);

		currNode.value = bestScore; 
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		chosenScore = bestScore;
		chosenNode = bestMove;
		return chosenScore;
	}
	else
	{
		passMin = false; node newMove;
		gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
		int moveIndex = 0;
		for (list<node>::iterator it = legalMoves.begin(); it != legalMoves.end(); ++it, moveIndex++)
		{
			it->value = 9999; it->depth = currNode.depth + 1;
			posi = it->rowNum - 1; posj = it->colNum - 1;
			//bestScore = 9999;
			vector<vector<char> > tempBoard = CopyMatrix(stateMatrix); vector<vector<char> > resultMatrix1;
			vector<vector<char> > flipMatrix = FlipCoins(tempBoard, posi, posj, me, anchori[moveIndex], anchorj[moveIndex]);
			int newScore = (AlphaBeta_MaxValue(flipMatrix, *it, opp, me, newMove, resultMatrix1,alpha,beta));
			it->value = newScore;
			v = GetMinVal(v, newScore);
			if (newScore < bestScore) // if better ie lesser
			{
				bestScore = newScore;
				bestMove = *it;
				resultMatrix = CopyMatrix(flipMatrix);
			}
			else if (newScore == bestScore) // if equal, tie break
			{
				node bestMove1 = TieBreak(bestMove, *it);
				if (bestMove1.name.compare(bestMove.name) != 0)
					resultMatrix = CopyMatrix(flipMatrix);
			}
			currNode.value = bestScore;
				if (v <= alpha)
			{
				chosenScore = bestScore;
				chosenNode = bestMove;
				gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);
				return v;
			}
			beta = GetMinVal(beta, v);
			gameLog += PrintGameLog(currNode.name, currNode.depth, currNode.value, alpha, beta);

		}

		chosenScore = bestScore;
		chosenNode = bestMove;
		return v;
	}

}
void AlphaBetaDecision(vector<vector<char> > stateMatrix, vector<vector<char> > &resultMatrix)
{
	gameLog = "Node,Depth,Value,Alpha,Beta\n";
	struct node n, maxnode;
	n.depth = 0; n.colChar = ' '; n.rowNum = -1; n.colNum = -1;
	n.value = -9999;
	n.name = "root";
	int maxResult = (AlphaBeta_MaxValue(stateMatrix, n, myPlayer, opponentPlayer, maxnode, resultMatrix,-9999,9999));
	cout << "\n\n" << maxnode.name << endl << endl;
}
int main()
{
	infile.open("input.txt");
	if (infile.is_open())
	{
		infile.getline(line, 20);
		searchCh = atoi(line);
		infile.getline(line, 20);
		myPlayer = line[0];
		if (myPlayer == 'X')
			opponentPlayer = 'O';
		else
			opponentPlayer = 'X';
		infile.getline(line, 20);
		cutOffDepth = atoi(line);
		initialStateMatrix.resize(8);
		for (i = 0; i < 8; i++) // 8 rows
			initialStateMatrix[i].resize(8); // 8 cols
		for (row = 0; row < 8; row++)
		{
			infile.getline(line, 100);
			col = 0; char a;

			while (col < 8)
			{
				a = line[col];
				initialStateMatrix[row][col] = a;
				col++;
			}
		}
		if (searchCh <1 || searchCh>4)
		{
			outfile << "Error: Incorrect search choice! (Enter 1: Greedy, 2: MiniMax, 3: AlphaBeta or 4: Competetion)";
			errflag = 1;
		}
		if (errflag != 1)
		{
			vector<vector<char> > resultMatrix;
			outfile.open("output.txt");
			switch (searchCh)
			{
			case 1:
			{
					  cutOffDepth = 1;
					  cout << "GREEDY\n";
					  MiniMaxDecision(initialStateMatrix, resultMatrix);
					  break;
			}
			case 2:
			{
					  cout << "MINIMAX\n";
					  MiniMaxDecision(initialStateMatrix, resultMatrix);
					  break;
			}
			case 3:
			{
					  cout << "ALPHABETA\n";
					  AlphaBetaDecision(initialStateMatrix, resultMatrix);
					  break;
			}
			default:
				break;
			}
			for (int i = 0; i < 8; i++)
			{
				for (int j = 0; j < 8; j++)
				{
					cout << resultMatrix[i][j];
					outfile << resultMatrix[i][j];
				}
				cout << endl;
				outfile << "\n";
			}
			cout << gameLog;
			outfile << gameLog;
		}
		infile.close();
		outfile.close();
		return 0;
	}
}
