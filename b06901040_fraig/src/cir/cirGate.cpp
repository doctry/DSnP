/****************************************************************************
  FileName     [ cirGate.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define class CirAigGate member functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdarg.h>
#include <cassert>
#include "cirGate.h"
#include "cirMgr.h"
#include "util.h"

using namespace std;

extern CirMgr *cirMgr;

// TODO: Implement memeber functions for class(es) in cirGate.h

/**************************************/
/*   class CirGate member functions   */
/**************************************/
unsigned CirGate::_ref = 1;
unsigned CirGate::_num = 0;

void
CirGate::reportGate() const
{
   cout << "================================================================================" << endl
        << "= ";
   //basic
   string str;
   switch(_type)
   {
      case PI_GATE:
         str += "PI";
         break;
      case PO_GATE:
         str += "PO";
         break;
      case AIG_GATE:
         str += "AIG";
         break;
      case CONST_GATE:
         str += "CONST";
         break;
      case UNDEF_GATE:
         str += "UNDEF";
      default: break;
   }
   str += '('; str += to_string(_id); str += ')';
   if(_sym)
   { str += '\"'; str += _sym; str += "\""; }
   str += ", line "; str += to_string(_line);
   cout << str << endl;
   //FEC
   cout << "= FECs:";
   for(unsigned i = 0;i < _FEC.size();i++)
   {
      cout << " ";
      if(_FEC[i]->_simval!=_simval)
         cout << "!";
      cout << _FEC[i]->_id;
   }
   cout << endl;
   //value
   str = "= Value: ";
   for(int i = 63; i >= 0; --i)
   {
      str += to_string((_simval >> i) & 1);
      if(i%8 == 0 && i > 0) str += '_';
   }
   cout << str << endl;
   cout << "================================================================================" << endl;
}

void
CirGate::reportFaninR(int level,int space)
{
   assert (level >= 0);
   //Print gate
   switch(_type)
   {
      case PI_GATE:
         cout << "PI ";
         break;
      case PO_GATE:
         cout << "PO ";
         break;
      case AIG_GATE:
         cout << "AIG ";
         break;
      case CONST_GATE:
         cout << "CONST ";
         break;
      case UNDEF_GATE:
         cout << "UNDEF ";
      default: break;
   }
   cout << _id;
   //if tail
   if(level == 0) return;
   //if passed
   if(_mark == _ref && _fanin.size() > 0)
   { cout << " (*)"; return;}
   //passed
   _mark = _ref;
   //if no fanin
   if(_fanin.size() == 0 || _type == UNDEF_GATE)
      return;
   //fanin1
   space++;
   cout << endl;
   for(int i = 0;i < (2*space);i++)
      cout << " ";
   if(_inv[0])
      cout << "!";
   _fanin[0]->reportFaninR((level-1),space);
   if(_fanin.size() == 1)
      return;
   //fanin2
   cout << endl;
   for(int i = 0;i < (2*space);i++)
      cout << " ";
   if(_inv[1])
      cout << "!";
   _fanin[1]->reportFaninR((level-1),space);
}

void
CirGate::reportFanin(int level)
{
   assert (level >= 0);
   reportFaninR(level,0);
   _ref++;
   cout << endl;
}

void
CirGate::reportFanoutR(int level,int space)
{
   assert (level >= 0);
   //Print gate
   switch(_type)
   {
      case PI_GATE:
         cout << "PI ";
         break;
      case PO_GATE:
         cout << "PO ";
         break;
      case AIG_GATE:
         cout << "AIG ";
         break;
      case CONST_GATE:
         cout << "CONST ";
         break;
      case UNDEF_GATE:
         cout << "UNDEF ";
      default: break;
   }
   cout << _id;
   //if tail
   if(level == 0) return;
   //if passed
   if(_mark == _ref && _fanout.size() > 0)
   { cout << " (*)"; return;}
   //passed
   _mark = _ref;
   //if no fanout
   if(_fanout.size() == 0)
      return;
   //fanouts
   space++;
   unsigned front;
   for(unsigned j = 0;j < _fanout.size();j++)
   {
      if(_fanout[j] == 0) break;
      cout << endl;
      for(int i = 0;i < (2*space);i++)
         cout << " ";
      if(_fanout[j]->_fanin[0] == this)
         front = 0;
      else
         front = 1;
      if(_fanout[j]->_inv[front])
         cout << "!";
      _fanout[j]->reportFanoutR((level-1),space);
   }
}

void
CirGate::reportFanout(int level)
{
   assert (level >= 0);
   reportFanoutR(level,0);
   _ref++;
   cout << endl;
}

void
CirGate::printGate()
{
   if(_fanin.size())
      if((_fanin[0]->_type != UNDEF_GATE) && (_fanin[0]->_mark != _ref))
         _fanin[0]->printGate();
   if(_fanin.size() > 1)
      if((_fanin[1]->_type != UNDEF_GATE) && (_fanin[1]->_mark != _ref))
         _fanin[1]->printGate();
   cout << "[" << _num << "] ";
   switch(_type)
   {
      case PI_GATE:
         cout << "PI ";
         break;
      case PO_GATE:
         cout << "PO ";
         break;
      case AIG_GATE:
         cout << "AIG";
         break;
      case CONST_GATE:
         cout << "CONST0";
         break;
      default: break;
   }
   if(_id)
      cout << " " << _id;
   if(_fanin.size())
   {
      cout << " ";
      if(_fanin[0]->_type == UNDEF_GATE)
         cout << "*";
      if(_inv[0])
         cout << "!";
      cout << _fanin[0]->_id;
   }
   if(_fanin.size() > 1)
   {
      cout << " ";
      if(_fanin[1]->_type == UNDEF_GATE)
         cout << "*";
      if(_inv[1])
         cout << "!";
      cout << _fanin[1]->_id;
   }
   if(_sym)
      cout << " (" << _sym << ")";
   cout << endl;
   _mark = _ref;
   _num++;
}

void
CirGate::AIGwrite(vector<CirGate*>& Q)
{
   for(unsigned i = 0;i < _fanin.size();i++)
      if((_fanin[i]->_type == AIG_GATE) && (_fanin[i]->_mark!=_ref))
         _fanin[i]->AIGwrite(Q);
   if(_type == PO_GATE) return;
   Q.push_back(this);
   _mark = _ref;
}

void
CirGate::Gatewrite(ostream& outfile) const
{
   switch(_type)
   {
      case PI_GATE:
         outfile << _id*2;
         break;
      case PO_GATE:
      {
         if(_inv[0]) outfile << (_fanin[0]->_id)*2 + 1;
         else outfile << (_fanin[0]->_id)*2;
         break;
      }
      case AIG_GATE:
      {
         outfile << _id*2 << " ";
         //fanin1
         if(_inv[0]) outfile << (_fanin[0]->_id)*2 + 1;
         else outfile << (_fanin[0]->_id)*2;
         outfile << " ";
         //fanin2
         if(_inv[1]) outfile << (_fanin[1]->_id)*2 + 1;
         else outfile << (_fanin[1]->_id)*2;
      }
      default: break;
   }
   outfile << endl;
}

void
CirGate::DFS(IdList& DFSlist)
{
   if(_fanin.size())
      if(_fanin[0]->_mark != _ref)
         _fanin[0]->DFS(DFSlist);
   if(_fanin.size() > 1)
      if(_fanin[1]->_mark != _ref)
         _fanin[1]->DFS(DFSlist);
   DFSlist.push_back(_id);
   _mark = _ref;
}

void
CirGate::DeleteFanin(const GateList& LeftNewFanout,const GateList& RightNewFanout)
{
   for(unsigned i = 0;i < _fanin[0]->_fanout.size();i++)
   {
      if(_fanin[0]->_fanout[i] == this)
      {
         _fanin[0]->_fanout.erase(_fanin[0]->_fanout.begin()+i);
         for(unsigned i = 0;i < LeftNewFanout.size();i++)
            _fanin[0]->_fanout.push_back(LeftNewFanout[i]);
         break;
      }
   }
   for(unsigned i = 0;i < _fanin[1]->_fanout.size();i++)
   {
      if(_fanin[1]->_fanout[i] == this)
      {
         _fanin[1]->_fanout.erase(_fanin[1]->_fanout.begin()+i);
         for(unsigned i = 0;i < RightNewFanout.size();i++)
            _fanin[1]->_fanout.push_back(RightNewFanout[i]);
         break;
      }
   }
}

void
CirGate::DeleteFanout(CirGate* newFanin, bool inv)
{
   for(unsigned i = 0;i < _fanout.size();i++)
   {
      if(_fanout[i]->_fanin[0] == this)
      {
         _fanout[i]->_fanin[0] = newFanin;
         if(inv)
            _fanout[i]->_inv[0] = !(_fanout[i]->_inv[0]);
      }
      else
      {
         _fanout[i]->_fanin[1] = newFanin;
         if(inv)
            _fanout[i]->_inv[1] = !(_fanout[i]->_inv[1]);
      }
   }
}

bool
CirGate::OPT(CirGate* CONST)
{
   if(_type == AIG_GATE)
   {
      GateList g;
      //left fanin constant 1
      if(_fanin[0]->_type == CONST_GATE && _inv[0])
      {
         DeleteFanin(g,_fanout);
         DeleteFanout(_fanin[1],_inv[1]);
         cout << "Simplifying: " << _fanin[1]->_id << " merging ";
         if(_inv[1])
            cout << "!";
         cout << _id << "..." << endl;
         return true;
      }
      //right fanin constant 1
      if(_fanin[1]->_type == CONST_GATE && _inv[1])
      {
         DeleteFanin(_fanout,g);
         DeleteFanout(_fanin[0],_inv[0]);
         cout << "Simplifying: " << _fanin[0]->_id << " merging ";
         if(_inv[0])
            cout << "!";
         cout << _id << "..." << endl;
         return true;
      }
      //left fanin constant 0
      if( (_fanin[0]->_type == CONST_GATE) || (_fanin[1]->_type == CONST_GATE) )
      {
         DeleteFanin(g,g);
         DeleteFanout(CONST,false);
         for(unsigned i = 0;i < _fanout.size();i++)
            CONST->AddFanout(_fanout[i]);
         cout << "Simplifying: 0 merging " << _id <<"..." << endl;
         return true;
      }
      //identical fanins
      if(_fanin[0] == _fanin[1])
      {
         //const0
         if(_inv[0] != _inv[1])
         {
            DeleteFanin(g,g);
            DeleteFanout(CONST,false);
            for(unsigned i = 0;i < _fanout.size();i++)
               CONST->AddFanout(_fanout[i]);
            cout << "Simplifying: 0 merging " << _id <<"..." <<endl;
            return true;
         }
         //inverter and buffer
         DeleteFanin(_fanout,g);
         DeleteFanout(_fanin[0],_inv[0]);
         cout << "Simplifying: " << _fanin[0]->_id << " merging ";
         if(_inv[0])
            cout << "!";
         cout << _id << "..." << endl;
         return true;
      }
   }
   return false;
}

void
CirGate::AddFEC(const GateList& FECs)
{
   _FEC.clear();
   for(unsigned i = 0;i < FECs.size();i++)
      if(FECs[i] != this)
         _FEC.push_back(FECs[i]);
}

void
CirGate::fraig(SatSolver& solver,GateList& list,unsigned& A)
{
   for(unsigned i = 0;i < _FEC.size();i++)
      proof(solver,_FEC[i],list,A);
}

void 
CirGate::proof(SatSolver& solver,CirGate* fec,GateList& list,unsigned& A)
{
   bool result;
   Var newV = solver.newVar();
   solver.addXorCNF(newV,_var,false,fec->_var,(fec->_simval != _simval));
   solver.assumeRelease();
   solver.assumeProperty(newV,true);
   result = solver.assumpSolve();
   if(!result)
   {
      GateList g;
      fec->DeleteFanout(this,fec->_simval != _simval);
      fec->DeleteFanin(g,g);
      cout << "Fraig: " << _id << " merging ";
      if(_simval != fec->_simval)
         cout << "!";
      cout << fec->_id << "..." << endl;
      list[fec->_id] = 0;
      delete fec;
      A--;
   }
}