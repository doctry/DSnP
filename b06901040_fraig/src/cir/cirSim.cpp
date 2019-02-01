/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir simulation functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <fstream>
#include <bitset>
#include <cstdlib>
#include <ctime>
#include <string>
#include <unordered_map>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Keep "CirMgr::randomSim()" and "CirMgr::fileSim()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

static bool IdListComp(IdList* a,IdList* b) { return (a->at(0)) < (b->at(0)); }

/************************************************/
/*   Public member functions about Simulation   */
/************************************************/
void
CirMgr::randomSim()
{
   //generate rand
   vector<size_t> inputs;
   randPI(inputs);
   unsigned patnum = _I*10;
   //initialize FECgroup
   if(_FECgroups.empty())
   {
      IdList* initFECgrp = new IdList;
      _FECgroups.push_back(initFECgrp);
      _FECgroups[0]->push_back(0);
      for(unsigned i = 0;i < _DFSlist.size();i++)
         if((_list[_DFSlist[i]]->getType() == AIG_GATE))
            _FECgroups[0]->push_back(_DFSlist[i]);
   }
   //dosim
   vector<size_t> pats;//to seperate inputs
   unsigned patnumptr = 64;
   unsigned patsnum = patnum;
   for(unsigned i = 0;i < (inputs.size()/_I);i++)
   {
      if(patnum < 64)
         patnumptr = patnum;
      pats.clear();
      for(unsigned j = 0;j < _I;j++)
         pats.push_back(inputs[_I*i + j]);
      Sim(pats);
      SepFEC(_FECgroups);
      if(_simLog)
         Olog(_simLog,patnumptr);
      patnum -= 64;
   }
   SortFEC();
   cout << patsnum << " patterns simulated." << endl;
}

void
CirMgr::fileSim(ifstream& patternFile)
{
   //read in pattern file
   vector<size_t> inputs;
   unsigned patnum = ReadPat(patternFile,inputs);
   if(patnum == 0) 
   {
      cerr << "0 patterns simulated.";
      return;
   }
   //initialize FECgroup
   if(_FECgroups.empty())
   {
      IdList* initFECgrp = new IdList;
      _FECgroups.push_back(initFECgrp);
      _FECgroups[0]->push_back(0);
      for(unsigned i = 0;i < _DFSlist.size();i++)
         if((_list[_DFSlist[i]]->getType() == AIG_GATE))
            _FECgroups[0]->push_back(_DFSlist[i]);
   }
   //dosim
   vector<size_t> pats;//to seperate inputs
   unsigned patnumptr = 64;
   unsigned patsnum = patnum;
   for(unsigned i = 0;i < (inputs.size()/_I);i++)
   {
      if(patnum < 64)
         patnumptr = patnum;
      pats.clear();
      for(unsigned j = 0;j < _I;j++)
         pats.push_back(inputs[_I*i + j]);
      Sim(pats);
      SepFEC(_FECgroups);
      if(_simLog)
         Olog(_simLog,patnumptr);
      patnum -= 64;
   }
   SortFEC();
   cout << patsnum << " patterns simulated." << endl;
}

/*************************************************/
/*   Private member functions about Simulation   */
/*************************************************/

void
CirMgr::Sim(const vector<size_t>& pats)
{
   for(unsigned i = 0;i < _PIlist.size();i++)
   {
      _list[_PIlist[i]]->SimInput(pats[i]);
   }
   for(unsigned i = 0;i < _DFSlist.size();i++)
   {
      if(_list[_DFSlist[i]]->getType() != PI_GATE)
         _list[_DFSlist[i]]->SimFanin();
   }
}

void
CirMgr::SepFEC(vector<IdList*>& grps)
{
   //copy grps
   const vector<IdList*> tmpGrps = grps;
   grps.clear();
   //grp from grp
   for(unsigned i = 0;i < tmpGrps.size();i++)
   {
      unordered_map<size_t,IdList*> newFECgrps;
      for(unsigned j = 0;j < tmpGrps[i]->size();j++)
      {
         size_t val = _list[tmpGrps[i]->at(j)]->getSimVal();
         if(val & 1) val = ~val;
         auto gpos = newFECgrps.find(val);
         //not exist
         if(gpos == newFECgrps.end())
         {
            IdList* newgrp = new IdList;
            newgrp->push_back(tmpGrps[i]->at(j));
            newFECgrps.insert({val,newgrp});
         }
         //exist
         else
         {
            (gpos->second)->push_back(tmpGrps[i]->at(j));
         }
      }
      //collect valid fecgrps
      for(auto j = newFECgrps.begin();j != newFECgrps.end();++j)
         if(((*j).second)->size() > 1)
            grps.push_back((*j).second);
   }
   for(unsigned i = 0;i < tmpGrps.size();i++)
      delete tmpGrps[i];
}

unsigned
CirMgr::ReadPat(ifstream& patternFile,vector<size_t>& inputs)
{
   string str;
   size_t line = 0;
   size_t one = 1;
   //line by line
   while(patternFile >> str)
   {
      if(str.size() != _I)
      {
         cerr << "Error: Pattern(" << str << ") length(" << str.size() << ") does not match the number of inputs(" << _I << ") in a circuit!!" << endl;
         return 0;
      }
      if(line%64 == 0)
         for(unsigned i = 0;i < _I;i++)
            inputs.push_back(0);
      //char by char
      for(unsigned i = 0;i < str.size();i++)
      {
         if(str[i] == '0')
            ;
         else if(str[i] == '1')
            inputs[(line/64)*_I + i] += (one << (line % 64));
         else
         {
            cerr << "Error: Pattern(" << str << ") contains a non-0/1 character(‘" << str[i] << "’)." << endl;
            return 0;
         }
      }
      str = "";
      line++;
   }
   return line;
}

void 
CirMgr::SortFEC()
{
   //sort
   for(unsigned i = 0;i < _FECgroups.size();i++)
      sort(_FECgroups[i]->begin(),_FECgroups[i]->end());
   sort(_FECgroups.begin(),_FECgroups.end(),IdListComp);
   //update gates
   GateList FEClist;
   for(unsigned i = 0;i < _FECgroups.size();i++)
   {
      FEClist.clear();
      for(unsigned j = 0;j < _FECgroups[i]->size();j++)
         FEClist.push_back(_list[_FECgroups[i]->at(j)]);
      for(unsigned j = 0;j < _FECgroups[i]->size();j++)
      {
         _list[_FECgroups[i]->at(j)]->AddFEC(FEClist);
      }
   }
}

void
CirMgr::Olog(ofstream* of,unsigned patnum)
{
   vector<bitset<64>> pis;
   vector<bitset<64>> pos;
   for(unsigned j = 0;j < _I;j++)
      pis.push_back(bitset<64>(_list[_PIlist[j]]->getSimVal()));
   for(unsigned j = _M+1;j <= (_M+_O);j++)
      pos.push_back(bitset<64>(_list[j]->getSimVal()));
   for(unsigned i = 0;i <patnum;i++)
   {
      for(unsigned j = 0;j < _I;j++)
         *of << pis[j][i];
      *of << " ";
      for(unsigned j = 0;j < pos.size();j++)
         *of << pos[j][i];
      *of << '\n';
   }
}

void
CirMgr::randPI(vector<size_t>& inputs)
{
   srand(time(NULL));
   for(unsigned i = 0;i < (_I*10/64);i++)
      for(unsigned j = 0;j < _I;j++)
      {
         int a = rand();
         int b = rand();
         size_t A = a; size_t B = b;
         A += (B << 32);
         inputs.push_back(A);
      }
}