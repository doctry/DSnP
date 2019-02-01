/****************************************************************************
  FileName     [ cirGate.h ]
  PackageName  [ cir ]
  Synopsis     [ Define basic gate data structures ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_GATE_H
#define CIR_GATE_H

#include <string>
#include <cstring>
#include <vector>
#include <iostream>
#include <algorithm>
#include "cirDef.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   CirGate(unsigned i,GateType t,unsigned l):_id(i),_type(t),_line(l) { _mark = 0; _sym = 0; _fanin.clear(); _fanout.clear(); _inv.clear();}
   ~CirGate() { if(_sym) delete[] _sym; }

   // Basic access methods
   string getTypeStr() const { return ""; }
   char* getSymbol() const { return _sym; }
   unsigned getLineNo() const { return _line; }
   GateType getType() const { return _type; }
   bool floating() const
   { 
      if( ((_type == AIG_GATE) && ((_fanin[0]->_type == UNDEF_GATE) || (_fanin[1]->_type == UNDEF_GATE))) || ((_type == PO_GATE) && (_fanin[0]->_type == UNDEF_GATE)))
         return true;
      else return false;
   }
   bool unused() const
   {
      if( ((_type == AIG_GATE) && (_fanout.size() == 0)) || ((_type == PI_GATE) && (_fanout.size() == 0)) )
         return true; 
      else return false; 
   }

   // Printing functions
   void printGate();
   void reportGate() const;
   void reportFanin(int level);
   void reportFaninR(int level,int space);
   void reportFanout(int level);
   void reportFanoutR(int level,int space);
   void AIGwrite(vector<CirGate*>&);
   void Gatewrite(ostream&) const;
   void SymbolWrite(ostream&) const;

   //modifier
   void AddFanin(CirGate* c,bool i) { _fanin.push_back(c); _inv.push_back(i); }
   void AddFanout(CirGate* c) { _fanout.push_back(c); }
   void ChangeType(GateType t,unsigned l) { _type = t; _line = l; }
   void AddSymbol(char* s) { _sym = new char[1024]; strcpy(_sym,s); }
   void resetPrint() { _ref++; _num = 0;}
   void sortFanout() { sort(_fanout.begin(),_fanout.end(),PtrComp); }

   //overloading
   bool operator<(const CirGate& c) { return (_id < c._id);}
   static bool PtrComp(CirGate* a,CirGate* b) { return (a->_id < b->_id); }

private:

protected:
   unsigned         _id;
   GateType         _type;
   GateList         _fanin;
   vector<bool>     _inv;
   GateList         _fanout;
   char*            _sym;
   static unsigned  _ref;
   unsigned         _mark;
   static unsigned  _num;
   unsigned         _line;
};

#endif // CIR_GATE_H
