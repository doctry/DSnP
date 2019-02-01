/****************************************************************************
  FileName     [ cirSim.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir optimization functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <cassert>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;

// TODO: Please keep "CirMgr::sweep()" and "CirMgr::optimize()" for cir cmd.
//       Feel free to define your own variables or functions

/*******************************/
/*   Global variable and enum  */
/*******************************/

/**************************************/
/*   Static varaibles and functions   */
/**************************************/

/**************************************************/
/*   Public member functions about optimization   */
/**************************************************/
// Remove unused gates
// DFS list should NOT be changed
// UNDEF, float and unused list may be changed
void
CirMgr::sweep()
{
   //updating DFSlist
   _DFSlist.clear();
   for(unsigned i = (_M+1);i <=(_M + _O);i++)
      _list[i]->DFS(_DFSlist);
   _list[0]->resetPrint();
   vector<bool> DELlist;
   for(unsigned i = 0;i < _list.size();i++)
      DELlist.push_back(true);
   _UNDEFlist.clear();
   //check DFSlist
   for(unsigned i = 0;i < _DFSlist.size();i++)
   {
      DELlist[_DFSlist[i]] = false; 
      if(_list[_DFSlist[i]]->floating())
         _UNDEFlist.push_back(_DFSlist[i]);
   }
   //delete gate not in DFSlist
   _UNUSEDlist.clear();
   GateList g;
   for(unsigned i = 1;i <= _M;i++)
   {
      if(_list[i])
      {
         GateType t = _list[i]->getType();
         if( (t == PI_GATE) && _list[i]->unused() )
            _UNUSEDlist.push_back(i);
         if(DELlist[i])
         {
            if(t == AIG_GATE || t == UNDEF_GATE)
            {
               cout << "Sweeping: ";
               switch(t)
               {
                  case UNDEF_GATE:
                     cout << "UNDEF";
                     break;
                  case AIG_GATE:
                     _A--;
                     cout << "AIG";
                     _list[i]->DeleteFanin(g,g);
                     break;
                  default:
                     break;
               }
               cout << "(" << i << ") removed..." << endl;
               delete _list[i];
               _list[i] = 0;
            }
         }
      }
   }
}

// Recursively simplifying from POs;
// _dfsList needs to be reconstructed afterwards
// UNDEF gates may be delete if its fanout becomes empty...
void
CirMgr::optimize()
{
   vector<unsigned> DELlist;
   for(unsigned i = 0;i < _DFSlist.size();i++)
   {
      if(_list[_DFSlist[i]]->OPT(_list[0]))
         DELlist.push_back(i);
   }
   _A -= DELlist.size();
   //delete DEL gate
   for(unsigned i = 0;i < DELlist.size();i++)
   {
      delete _list[_DFSlist[DELlist[i]-i]];
      _list[_DFSlist[DELlist[i]-i]] = 0;
      _DFSlist.erase(_DFSlist.begin()+DELlist[i]-i);
   }
   //delete unused UNDEF
   for(unsigned i = 0;i < _DFSlist.size();i++)
   {
      if( (_list[_DFSlist[i]]->getType() == UNDEF_GATE) && (_list[_DFSlist[i]]->unused()) )
      {
         delete _list[_DFSlist[i]];
         _list[_DFSlist[i]] = 0;
         _DFSlist.erase(_DFSlist.begin()+i);
         i--;
      }
   }
   //updating UNDEFlist and UNUSEDlist
   _UNDEFlist.clear();
   _UNUSEDlist.clear();
   for(unsigned i = 0;i <= _M;i++)
      if(_list[i])
      {
         if(_list[i]->floating())
            _UNDEFlist.push_back(i);
         if(_list[i]->unused())
            _UNUSEDlist.push_back(i);
      }
   //updating DFSlist
   _DFSlist.clear();
   for(unsigned i = (_M+1);i <=(_M + _O);i++)
      _list[i]->DFS(_DFSlist);
   _list[0]->resetPrint();
}

/***************************************************/
/*   Private member functions about optimization   */
/***************************************************/
