/****************************************************************************
  FileName     [ cirFraig.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir FRAIG functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2012-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include <unordered_map>
#include "cirMgr.h"
#include "cirGate.h"
#include "sat.h"
#include "myHashMap.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::strash()" and "CirMgr::fraig()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/*******************************************/
/*   Public member functions about fraig   */
/*******************************************/
// _floatList may be changed.
// _unusedList and _undefList won't be changed
void
CirMgr::strash()
{
   unordered_map<size_t,CirGate*> Strash;
   vector<unsigned> DELlist;
   GateList g;
   //maintain gate
   for(unsigned i = 0;i < _DFSlist.size();i++)
   {
      if(_list[_DFSlist[i]] -> getType() == AIG_GATE)
      {
         auto kpos = Strash.find(_list[_DFSlist[i]]->StrashKey());
         if(kpos != Strash.end())
         {
            _list[_DFSlist[i]]->DeleteFanin(g,g);
            _list[_DFSlist[i]]->DeleteFanout(kpos->second,false);
            cout << "Strashing: " << kpos->second->getID() << " merging " << _DFSlist[i] << "..." << endl;
            kpos->second->AddFanouts(_list[_DFSlist[i]]);
            DELlist.push_back(i);
         }
         else Strash.insert({_list[_DFSlist[i]]->StrashKey(),_list[_DFSlist[i]]});
      }
   }
   _A -= DELlist.size();
   //delete gates
   for(unsigned i = 0;i < DELlist.size();i++)
   {
      delete _list[_DFSlist[DELlist[i]]];
      _list[_DFSlist[DELlist[i]]] = 0;
   }
   //update dfslist
   _DFSlist.clear();
   for(unsigned i = (_M+1);i <=(_M + _O);i++)
      _list[i]->DFS(_DFSlist);
   _list[0]->resetPrint();
   //update floating gate
   _UNDEFlist.clear();
   for(unsigned i = 0;i < _list.size();i++)
      if(_list[i])
         if(_list[i]->floating())
            _UNDEFlist.push_back(i);
}

void
CirMgr::fraig() 
{
   //proof model.h
   SatSolver solver;
   solver.initialize();
   genProofModel(solver);
   //fec groups
   for(unsigned i = 0;i < _FECgroups.size();i++)
      _list[_FECgroups[i]->at(0)]->fraig(solver,_list,_A);
   //update dfslist
   _DFSlist.clear();
   for(unsigned i = (_M+1);i <=(_M + _O);i++)
      _list[i]->DFS(_DFSlist);
   _list[0]->resetPrint();
   strash();
   for(unsigned i = 0;i < _FECgroups.size();i++)
      delete _FECgroups[i];
   _FECgroups.clear();
   SortFEC();
}

/********************************************/
/*   Private member functions about fraig   */
/********************************************/
void
CirMgr::genProofModel(SatSolver& solver)
{
   //CONST 0
   Var CONST_FANIN = solver.newVar();
   _list[0]->SetVar(solver.newVar());
   solver.addAigCNF(_list[0]->getVar(),CONST_FANIN,true,CONST_FANIN,false);
   //for each gate add Var
   for(unsigned i = 0;i < _DFSlist.size();i++)
      if((_list[_DFSlist[i]]->getType() == AIG_GATE) || (_list[_DFSlist[i]]->getType() == PI_GATE))
         _list[_DFSlist[i]]->SetVar(solver.newVar());
   //for each gate add AIGCNF
   for(unsigned i = 0;i < _DFSlist.size();i++)
      if(_list[_DFSlist[i]]->getType() == AIG_GATE)
         solver.addAigCNF(_list[_DFSlist[i]]->getVar(),_list[_DFSlist[i]]->getLeftVar(),_list[_DFSlist[i]]->getLeftInv(),_list[_DFSlist[i]]->getRightVar(),_list[_DFSlist[i]]->getRightInv());
}
