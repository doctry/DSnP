/****************************************************************************
  FileName     [ dbJson.cpp ]
  PackageName  [ db ]
  Synopsis     [ Define database Json member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2015-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iomanip>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cassert>
#include <climits>
#include <cmath>
#include <string>
#include <algorithm>
#include <vector>
#include "dbJson.h"
#include "util.h"

using namespace std;

/*****************************************/
/*          Global Functions             */
/*****************************************/

void despace(string&);
string ext_key(string&);
int ext_val(string&);

ostream&
operator << (ostream& os, const DBJsonElem& j)
{
   os << "\"" << j._key << "\" : " << j._value;
   return os;
}

istream& operator >> (istream& is, DBJson& j)
{
   // TODO: to read in data from Json file and store them in a DB 
   // - You can assume the input file is with correct JSON file format
   // - NO NEED to handle error file format
   assert(j._obj.empty());
   j._readed = true;

   while(true)//line by line
      {
        char arr[10000];
        is.getline(arr,10000);
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
            return is;
          }
          else//object
          {
            DBJsonElem temp(ext_key(line) , ext_val(line));
            j._obj.push_back(temp);
          }
        }
      }
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

ostream& operator << (ostream& os, const DBJson& j)
{
   // TODO
   for(unsigned int i = 0; i < j._obj.size(); i++)
   {
    os << "  " << j._obj[i];
    if (i != (j._obj.size() - 1))
      os << ',';
    os << endl;
   }
   return os;
}

/**********************************************/
/*   Member Functions for class DBJsonElem    */
/**********************************************/
/*****************************************/
/*   Member Functions for class DBJson   */
/*****************************************/
void
DBJson::reset()
{
   // TODO
  _obj.clear();
  _readed = false;
}

// return false if key is repeated
bool
DBJson::add(const DBJsonElem& elm)
{
   // TODO
  // to see if repeated
  bool repeated = false;
  for(unsigned int i = 0; i < _obj.size(); i++)
  {
    if(elm.key() == _obj[i].key())
      repeated = true;
  }
  // add
  if(!repeated)
  {
    _obj.push_back(elm);
  }
  return !repeated;
}

// return NAN if DBJson is empty
float
DBJson::ave() const
{
   // TODO
  if(_obj.size() == 0)//to see if empty
  {
    return NAN;
  }
  float temp = 0;
  for(unsigned int i = 0; i < _obj.size(); i++)
  {
    temp += _obj[i].value();
  }
  return (temp / _obj.size());
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::max(size_t& idx) const
{
   // TODO
   int maxN = INT_MIN;
   if(_obj.size() == 0)
   {
    idx = _obj.size();
    return  maxN;
   }
  for(unsigned int i = 0; i < _obj.size(); i++)
  {
    if(_obj[i].value() > maxN)
    {
      idx = i;
      maxN = _obj[i].value();
    }
  }
  return maxN;
}

// If DBJson is empty, set idx to size() and return INT_MIN
int
DBJson::min(size_t& idx) const
{
   // TODO
   int minN = INT_MAX;
   if(_obj.size() == 0)
   {
    idx = _obj.size();
    return INT_MIN;
   }
   for(unsigned int i = 0; i < _obj.size(); i++)
   {
    if(_obj[i].value() < minN)
    {
      idx = i;
      minN = _obj[i].value();
    }
   }
   return  minN;
}

void
DBJson::sort(const DBSortKey& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

void
DBJson::sort(const DBSortValue& s)
{
   // Sort the data according to the order of columns in 's'
   ::sort(_obj.begin(), _obj.end(), s);
}

// return 0 if empty
int
DBJson::sum() const
{
   // TODO
   int s = 0;
   if(_obj.size() == 0)//to see if empty
  {
    return s;
  }
  for(unsigned int i = 0; i < _obj.size(); i++)
  {
    s += _obj[i].value();
  }
   return s;
}
