/****************************************************************************
  FileName     [ p2Json.cpp ]
  PackageName  [ p2 ]
  Synopsis     [ Define member functions of class Json and JsonElem ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2018-present DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <string>
#include "p2Json.h"
#include <fstream>
#include <vector>
#include <iomanip>

void despace(string&);
string ext_key(string&);
int ext_val(string&);

using namespace std;

// Implement member functions of class Row and Table here
bool
Json::read(const string& jsonFile)
{
   fstream jsonf;
   jsonf.open(jsonFile);
   if(jsonf.is_open())//read in
   {
   		while(true)//line by line
   		{
   			char arr[10000];
   			jsonf.getline(arr,10000);
   			if(arr[0] == '\0')//check if empty
   				;
   			else
   			{
   				string line(arr);
   				despace(line);
   				if(line.at(0) == '{')
   					;
   				else if(line.at(0) == '}')
   				{
				return true;
   				}
				else//object
				{
					JsonElem temp(ext_key(line) , ext_val(line));
					_obj.push_back(temp);
				}
   			}
   		}
   }
   else return false;//not read in
}

void
Json::print()
{
	cout << "{" << endl;
	for(unsigned int q = 0; q < _obj.size() ; q++)
	{
		cout << "  " << _obj[q];
		if(q < (_obj.size() - 1))
			cout << ",";
		cout << endl; 
	}
	cout << "}" << endl;
}

void
Json::sum()
{
	if(_obj.size() == 0)
		cout << "Error: No element found!!"<<endl;
	else
	{
		int sums = 0;
		for(unsigned int q = 0; q < _obj.size() ; q++)
		{
			sums += _obj[q].get_value(); 
		}
		cout <<"The summantion of the values is: " << sums << "." << endl;
	}	
}

void
Json::ave()
{
	if(_obj.size() == 0)
		cout << "Error: No element found!!"<< endl;
	else
	{
		float sums = 0;
		for(unsigned int q = 0; q < _obj.size() ; q++)
		{
			sums += _obj[q].get_value(); 
		}
		sums/=_obj.size();
		cout <<"The average of the values is: " << fixed << setprecision(1) << sums << "." << endl;
	}
}

void
Json::max()
{
	if(_obj.size() == 0)
		cout << "Error: No element found!!"<< endl;
	else
	{
		unsigned int pos_max = 0;
		int maxs = _obj[0].get_value();
		for(unsigned int q = 1; q < _obj.size() ; q++)
		{
			if(maxs < _obj[q].get_value())
			{
				maxs = _obj[q].get_value();
				pos_max = q;
			}
		}
		cout <<"The maximum element is: { " << _obj[pos_max] << " }." << endl;
	}
}

void
Json::min()
{
	if(_obj.size() == 0)
		cout << "Error: No element found!!"<< endl;
	else
	{
		unsigned int pos = 0;
		int mins = _obj[0].get_value();
		for(unsigned int q = 1; q < _obj.size() ; q++)
		{
			if(mins > _obj[q].get_value())
			{
				mins = _obj[q].get_value();
				pos = q;
			}
		}
		cout <<"The minimum element is: { " << _obj[pos] << " }." << endl;
	}
}

void
Json::add(string key,int val)
{
	JsonElem temp(key,val);
	_obj.push_back(temp);
}

ostream&
operator << (ostream& os, const JsonElem& j)
{
   return (os << "\"" << j._key << "\" : " << j._value);
}

void despace(string& line)
{
	while(line.find(' ') != string::npos)
	{
		line.erase(line.find(' '),1);
	}
	while(line.find('\t') != string::npos)
	{
		line.erase(line.find('\t'),1);
	}
	while(line.find('\b') != string::npos)
	{
		line.erase(line.find('\b'),1);
	}
	while(line.find('\r') != string::npos)
	{
		line.erase(line.find('\r'),1);
	}
	while(line.find('\n') != string::npos)
	{
		line.erase(line.find('\n'),1);
	}
	line.push_back(',');
}

string ext_key(string& line)
{
	char key_arr[10000];
	for(int i = 0; i<10000;i++)
	{
		key_arr[i] = '\0';
	}
	line.copy( key_arr , (line.rfind('\"') -1) ,1);
	string key(key_arr);
	return key;
}

int ext_val(string& line)
{
	char val_arr[10000];
	for(int i = 0; i<10000;i++)
	{
		val_arr[i] = '\0';
	}
	line.copy( val_arr , (line.find(',') - line.find(':') - 1) , (line.find(':') + 1));
	string val(val_arr);
	return stoi(val,nullptr);
}