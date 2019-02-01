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
   cout << "==================================================" << endl
        << "= ";
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
   cout << left << setw(47) << str << "=" << endl;
   cout << "==================================================" << endl;
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
