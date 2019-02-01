/****************************************************************************
  FileName     [ cirMgr.h ]
  PackageName  [ cir ]
  Synopsis     [ Define circuit manager ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef CIR_MGR_H
#define CIR_MGR_H

#include <unordered_map>
#include <vector>
#include <string>
#include <fstream>
#include <iostream>

using namespace std;

#include "cirDef.h"

extern CirMgr *cirMgr;

// TODO: Define your own data members and member functions
class CirMgr
{
public:
   CirMgr(){}
   ~CirMgr();

   // Access functions
   // return '0' if "gid" corresponds to an undefined gate.
   CirGate* getGate(unsigned gid) const;

   // Member functions about circuit construction
   bool readCircuit(const string&);

   // Member functions about circuit optimization
   void sweep();
   void optimize();

   // Member functions about simulation
   void randomSim();
   void fileSim(ifstream&);
   void setSimLog(ofstream *logFile) { _simLog = logFile; }

   // Member functions about fraig
   void strash();
   void printFEC() const;
   void fraig();

   // Member functions about circuit reporting
   void printSummary() const;
   void printNetlist() const;
   void printPIs() const;
   void printPOs() const;
   void printFloatGates() const;
   void printFECPairs() const;
   void writeAag(ostream&) const;
   void writeGate(ostream&, CirGate*) const;

private:
   unsigned                       _M;
   unsigned                       _I;
   unsigned                       _L;
   unsigned                       _O;
   unsigned                       _A;           //maintain
   GateList                       _list;        //maintain
   IdList                         _PIlist;
   IdList                         _UNDEFlist;   //maintain
   IdList                         _UNUSEDlist;  //maintain
   IdList                         _DFSlist;     //maintain
   ofstream*                      _simLog;
   vector<IdList*>                _FECgroups;


   //private function
   void Sim(const vector<size_t>& pats);
   void SepFEC(vector<IdList*>& grps);
   unsigned ReadPat(ifstream& patternFile,vector<size_t>& inputs);
   void SortFEC();
   void Olog(ofstream*,unsigned patnum);
   void randPI(vector<size_t>& inputs);
   void genProofModel(SatSolver& solver);
};

#endif // CIR_MGR_H
