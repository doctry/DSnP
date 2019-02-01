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
#include "sat.h"

using namespace std;

class CirGate;

//------------------------------------------------------------------------
//   Define classes
//------------------------------------------------------------------------
// TODO: Define your own data members and member functions, or classes
class CirGate
{
public:
   friend class StrashKey;
   CirGate(unsigned i,GateType t,unsigned l):_id(i),_type(t),_line(l) { _mark = 0; _sym = 0; _fanin.clear(); _fanout.clear(); _inv.clear(); _simval = 0; _var = 0;}
   ~CirGate() { if(_sym) delete[] _sym; }

   // Basic access methods
   bool isAig() const { return _type == AIG_GATE; }
   unsigned getID() const { return _id; }
   string getTypeStr() const { return ""; }
   char* getSymbol() const { return _sym; }
   unsigned getLineNo() const { return _line; }
   GateType getType() const { return _type; }
   size_t getSimVal() const { return _simval; }
   Var getVar() const { return _var; }
   bool getLeftInv() const { return _inv[0]; }
   bool getRightInv() const { return _inv[1]; }
   Var getLeftVar() const { return _fanin[0]->_var; }
   Var getRightVar() const { return _fanin[1]->_var; }
   bool floating() const
   { 
      if( ((_type == AIG_GATE) && ((_fanin[0]->_type == UNDEF_GATE) || (_fanin[1]->_type == UNDEF_GATE))) || ((_type == PO_GATE) && (_fanin[0]->_type == UNDEF_GATE)))
         return true;
      else return false;
   }
   bool unused() const
   {
      if( (_fanout.size() == 0) && (_type == UNDEF_GATE || _type == PI_GATE || _type == AIG_GATE) )
         return true; 
      else return false;
   }
   void DFS(IdList& DFSlist);
   size_t StrashKey() const
   {
      size_t f0 = 2*(_fanin[0]->_id);
      if(_inv[0]) f0++;
      size_t f1 = 2*(_fanin[1]->_id);
      if(_inv[1]) f1++;
      if(f1 < f0)
      { size_t tmp = f1; f1 = f0; f0 = tmp; }
      return( (f0 << 32) + f1);
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
   void resetPrint() { _ref++; _num = 0; }
   void sortFanout() { sort(_fanout.begin(),_fanout.end(),PtrComp); }
   void changeFanout(const GateList& g) { _fanout = g; }
   void DeleteFanin(const GateList& LeftNewFanout,const GateList& RightNewFanout);
   void DeleteFanout(CirGate* newFanin,bool inv);
   bool OPT(CirGate* CONST);
   void AddFanouts(const CirGate* g) { for(unsigned i = 0;i < g->_fanout.size();i++) _fanout.push_back(g->_fanout[i]); }
   void AddFEC(const GateList&);
   void SetVar(const Var& v) { _var = v; }
   void fraig(SatSolver& solver,GateList& list,unsigned&);
   void proof(SatSolver& slover,CirGate* fec,GateList&,unsigned&);

   //simulation
   void SimFanin()
   {
      if(_type == AIG_GATE)
      {
         size_t f0 = _fanin[0]->_simval;
         if(_inv[0])
            f0 = ~f0;
         size_t f1 = _fanin[1]->_simval;
         if(_inv[1])
            f1 = ~f1;
         _simval = (f0 & f1);
      }
      else if(_type == PO_GATE)
      {
         if(_inv[0])
            _simval = ~(_fanin[0]->_simval);
         else
            _simval = _fanin[0]->_simval;
      }
      else if((_type == UNDEF_GATE) || (_type == CONST_GATE))
         _simval = 0;
      else
         cerr << "Please enter input value." << endl;
   }

   void SimInput(size_t input)
   {
     if(_type == PI_GATE)
        _simval = input;
     else
        cerr << "Don't enter input value." << endl;
   }

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
   size_t           _simval;
   GateList         _FEC;
   vector<bool>     _FECinv;
   Var              _var;
};

#endif // CIR_GATE_H
