#include<iostream>
#include<stdlib.h>
#include<list>
#include<fstream>
#include<vector>
#include<algorithm>
#include<sstream>
using namespace std;

struct factStruct
	{
	string predicateName;
	string arg1;
	string arg2;
	};
factStruct CreateNewFactStruct(string p="",string a1="",string a2="")
	{
	factStruct f;
	f.predicateName=p;
	f.arg1=a1;
	f.arg2=a2;
	return f;
	}
struct atomicSentenceStruct
	{
	string predicateName;
	string arg1;
	string arg2;
	};
atomicSentenceStruct CreateNewAtomicSentenceStruct(string p="", string a1="", string a2="")
	{
	atomicSentenceStruct a;
	a.predicateName=p;
	a.arg1=a1;
	a.arg2=a2;
	return a;
	}
struct ruleStruct
	{
	list<atomicSentenceStruct> premises;
	atomicSentenceStruct conclusion;
	};
list<factStruct> factList;
list<ruleStruct> ruleList;
list<string> constantsList;
atomicSentenceStruct query;
bool IsBinaryPredicate(string s)
	{
	size_t f=s.find(",");
	if(f!=string::npos)
		return true;
	else
		return false;
	}
bool HasVariable(string s)
	{
	size_t f1=s.find("(x,");
	size_t f2=s.find(",x)");
	size_t f3=s.find("(x)");
	if(f1!=string::npos || f2!=string::npos || f3!=string::npos)
		return true;
	else 
		return false;

	}
atomicSentenceStruct GetParsedAtomicSentence(string sentence) //sentence: dad(x) or dad(sam,john)
	{
	atomicSentenceStruct temp; string a1="", a2="", p="";
	size_t p1=sentence.find("(");
	size_t p2=sentence.find(")");
	if(!IsBinaryPredicate(sentence))
		a1=sentence.substr(p1+1,p2-p1-1);
	else
		{
		size_t p3=sentence.find(",");
		a1=sentence.substr(p1+1, p3-p1-1);
		a2=sentence.substr(p3+1,p2-p3-1);
		}
	p=sentence.substr(0,p1);
	temp=CreateNewAtomicSentenceStruct(p,a1,a2);
	return temp;
	}
factStruct GetParsedFact(string sentence)
	{
	factStruct temp; string a1="",a2="",p="";
	size_t p1=sentence.find("(");
	size_t p2=sentence.find(")");
	if(!IsBinaryPredicate(sentence))
		a1=sentence.substr(p1+1,p2-p1-1);
	else
		{
		size_t p3=sentence.find(",");
		a1=sentence.substr(p1+1, p3-p1-1);
		a2=sentence.substr(p3+1,p2-p3-1);
		}
	p=sentence.substr(0,p1);
	temp=CreateNewFactStruct(p,a1,a2);
	return temp;
	}
ruleStruct GetParsedRule(string sentence)
	{
	ruleStruct temp;
	size_t count = std::count(sentence.begin(), sentence.end(),'&');
	if(count==0) //A=>B
	{
				
		size_t f=sentence.find("=");
		string p = sentence.substr (0,f); //premise
		temp.premises.push_back(GetParsedAtomicSentence(p));
		p=sentence.substr(f+2, string::npos); // conclusion
		temp.conclusion=GetParsedAtomicSentence(p);
	}
	else // A&B&C...=>Z
	{
		size_t f1=sentence.find("&"); //first &
		string s1=sentence.substr(0,f1);
		temp.premises.push_back(GetParsedAtomicSentence(s1));
		size_t f2=f1;
		size_t f3=sentence.find("&",f2+1,1);
		while(f3!=string::npos) //more than one &
		{
			int l=f3-(f2+1);
			string s2=sentence.substr(f2+1,l);
			temp.premises.push_back(GetParsedAtomicSentence(s2));
			f2=f3;
			f3=sentence.find("&",f2+1,1);
		}

		//no more &, so look for =>
				
		size_t f=sentence.find("=");
		int l=f-(f2+1);
		string str2 = sentence.substr (f2+1,l);
		temp.premises.push_back(GetParsedAtomicSentence(str2));
		str2=sentence.substr(f+2, string::npos); // conclusion
		temp.conclusion=GetParsedAtomicSentence(str2);
	}
	return temp;
	}
int numOfInputs=0;
bool Search(atomicSentenceStruct query, string unify)
	{
	bool result=false;
	string u="";
	if(unify.compare("x")!=0)
		{
		if(query.arg1.compare("x")==0)
			query.arg1=unify;
		else if(query.arg2.compare("x")==0)
			query.arg2=unify;
		}
	// 1. CHECK IN FACT LIST
	for(list<factStruct>::iterator fit=factList.begin();fit!=factList.end();++fit)
		{
		if(query.predicateName.compare(fit->predicateName)==0 && query.arg1.compare(fit->arg1)==0 && query.arg2.compare(fit->arg2)==0 )
			return true;
		}
	// 2. NO VAR - CHECK RHS OF EACH RULE IN RULE LIST
	if(query.arg1.compare("x")!=0 && query.arg1.compare("")!=0 && query.arg2.compare("x")!=0)
		{
		for(list<ruleStruct>::iterator rit=ruleList.begin();rit!=ruleList.end();++rit)
			{
			if(query.predicateName.compare(rit->conclusion.predicateName)==0 
				&& query.arg1.compare(rit->conclusion.arg1)==0 && query.arg2.compare(rit->conclusion.arg2)==0) //matches rhs
				{
				result=true;
				list<atomicSentenceStruct> premises=rit->premises;
				for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
					{
					u="x";
					// IF THE PREMISE arg1 HAS A VARIABLE
					//if((p->arg1.compare("")!=0 && p->arg1.compare("x")==0))
					//	{
					//	/*for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
					//		{
					//		u=*sit;
					//		result=result&Search(*p,u);
					//		}*/
					//	u=query.arg1;
					//	// a(x)=>b(John)
					result=result&Search(*p,u);
					}
				}
			else if(query.predicateName.compare(rit->conclusion.predicateName)==0
				&& (rit->conclusion.arg1.compare("x")==0 && rit->conclusion.arg2.compare(query.arg2)==0))
				{
				result=true;
				u=query.arg1; list<atomicSentenceStruct> premises=rit->premises;
				
				for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
					{
					result=result&Search(*p,u);
					}
				}
			else if(query.predicateName.compare(rit->conclusion.predicateName)==0
				&& (rit->conclusion.arg2.compare("x")==0 && rit->conclusion.arg1.compare(query.arg1)==0))
				{
				result=true;
				u=query.arg2; list<atomicSentenceStruct> premises=rit->premises;
				
				for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
					{
					result=result&Search(*p,u);
					}
				}
			}
			}
	// 3. HAS VAR - CHECK FOR EACH RULE IN RULE LIST
	               //IF RHS UNIFIES, SEARCH FOR NEW QUERY WITH SAME UNIFICATION
	else if(query.arg1.compare("x")==0) 
		{
		atomicSentenceStruct newquery=query; result=false;
		for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
			{
			newquery.arg1=*sit;
			result=result||Search(newquery, "x");
			/*
			for(list<ruleStruct>::iterator rit=ruleList.begin();rit!=ruleList.end();++rit)
				{
				if(newquery.predicateName.compare(rit->conclusion.predicateName)==0) //matches rhs
				{
				result=true;
				list<atomicSentenceStruct> premises=rit->premises;
				for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
					{
					u="x";
					// IF THE PREMISE arg1 HAS A VARIABLE
					if((p->arg1.compare("")!=0 && p->arg1.compare("x")==0))
						{
						/*for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
							{
							u=*sit;
							}
						u=newquery.arg1;
						result=result&Search(*p,u);
						}
					// IF THE PREMISE arg2 HAS A VARIABLE
					else if((p->arg2.compare("")!=0 && p->arg2.compare("x")==0))
						{
						/*for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
							{
							u=*sit;
							}
						u=query.arg1;
						result=result&Search(*p,u);
						}
					else
						result=result&Search(*p,u);
					}

				}
			}
		}
		*/
			}
		}
	else if(query.arg2.compare("x")==0)
		{
		atomicSentenceStruct newquery=query;
		for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
			{
			newquery.arg1=*sit;
			result=Search(newquery,"x");
			//for(list<ruleStruct>::iterator rit=ruleList.begin();rit!=ruleList.end();++rit)
			//	{
			//		//if(newquery.predicateName.compare(rit->conclusion.predicateName)==0) //matches rhs
			//		//	{
			//		//	result=true;
			//		//	list<atomicSentenceStruct> premises=rit->premises;
			//		//	for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
			//		//		{
			//		//		u="x";
			//		//		result=result&Search(*p,u);
			//		//		}
			//		//	}
			//		if(newquery.predicateName.compare(rit->conclusion.predicateName)==0) //matches rhs
			//		{
			//		result=true;
			//		list<atomicSentenceStruct> premises=rit->premises;
			//		for(list<atomicSentenceStruct>::iterator p=premises.begin();p!=premises.end();++p) //search for each premise
			//			{
			//			u="x";
			//			// IF THE PREMISE arg1 HAS A VARIABLE
			//			if((p->arg1.compare("")!=0 && p->arg1.compare("x")==0))
			//				{
			//				for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
			//					{
			//					u=*sit;
			//					}
			//				}
			//			// IF THE PREMISE arg2 HAS A VARIABLE
			//			else if((p->arg2.compare("")!=0 && p->arg2.compare("x")==0))
			//				{
			//				for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();++sit)
			//					{
			//					u=*sit;
			//					}
			//				}
			//			result=result&Search(*p,u);
			//			}
			//		}
			//	}
			}
		}
	return result;
    }

int main()
	{
	ifstream infile;
	ofstream outfile;
	char line[256];
	int i=0;
	infile.open("input.txt");
	outfile.open("output.txt"); 
	
	if (infile.is_open())
	{
		
		string l,q;
		infile.getline(line,200);
		q=std::string(line);
		query=GetParsedAtomicSentence(q);
		infile.getline(line,10);
		numOfInputs=atoi(line);
		for(i=0;i<numOfInputs;i++)
		{
		infile.getline(line,200);
		string s=std::string(line);
		
		if(s.find("=")!=string::npos) //rule
			{
			ruleStruct rule=GetParsedRule(s);
			ruleList.push_back(rule);
			}
		else //facts
			{
			factStruct fact=GetParsedFact(s);
			factList.push_back(fact);
			}
						
		}
	//find constants
		list<string> tempconst;
	for(list<factStruct>::iterator fit=factList.begin();fit!=factList.end();++fit)
		{
		if((fit->arg1.compare("")!=0) && (fit->arg1.compare("x")!=0))
			tempconst.push_back(fit->arg1);
		if((fit->arg2.compare("")!=0) && (fit->arg2.compare("x")!=0))
			tempconst.push_back(fit->arg2);
		}
	for(list<ruleStruct>::iterator rit=ruleList.begin();rit!=ruleList.end();++rit)
		{ 
		list<atomicSentenceStruct> tempPremises=rit->premises;
		for(list<atomicSentenceStruct>::iterator p=tempPremises.begin();p!=tempPremises.end();++p)
			{
			if((p->arg1.compare("")!=0) && (p->arg1.compare("x")!=0))
			tempconst.push_back(p->arg1);
			if((p->arg2.compare("")!=0) && (p->arg2.compare("x")!=0))
			tempconst.push_back(p->arg2);
			}
		if((rit->conclusion.arg1.compare("")!=0) && (rit->conclusion.arg1.compare("x")!=0))
			tempconst.push_back(rit->conclusion.arg1);
		if((rit->conclusion.arg2.compare("")!=0) && (rit->conclusion.arg2.compare("x")!=0))
			tempconst.push_back(rit->conclusion.arg2);
		}
	string c;
	while(!tempconst.empty())
		{
		bool flag=true;
		c=tempconst.front();
		tempconst.pop_front();
		for(list<string>::iterator sit=constantsList.begin();sit!=constantsList.end();sit++)
			{
			if(c.compare(*sit)==0)
				{
				flag=false;
				break;
				}
			}
		if(flag==true)
			constantsList.push_back(c);
		}	
	bool b=Search(query,"x");
		if(b==true)
			{ 
			cout<<"true\n";
			outfile<<"true";
			}
		else
			{
			cout<<"false\n";
			outfile<<"false";
			}
		}
	else
		{
		cout<<"Cannot open file";
		outfile<<"Cannot open file";
		}
	infile.close();
	outfile.close();
	return 0;
	}