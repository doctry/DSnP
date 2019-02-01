/****************************************************************************
  FileName     [ cirMgr.cpp ]
  PackageName  [ cir ]
  Synopsis     [ Define cir manager functions ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2008-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#include <iostream>
#include <iomanip>
#include <cstdio>
#include <ctype.h>
#include <cassert>
#include <cstring>
#include "cirMgr.h"
#include "cirGate.h"
#include "util.h"

using namespace std;
bool Inv(unsigned id);

// TODO: Implement memeber functions for class CirMgr

/*******************************/
/*   Global variable and enum  */
/*******************************/
CirMgr* cirMgr = 0;

enum CirParseError {
   EXTRA_SPACE,
   MISSING_SPACE,
   ILLEGAL_WSPACE,
   ILLEGAL_NUM,
   ILLEGAL_IDENTIFIER,
   ILLEGAL_SYMBOL_TYPE,
   ILLEGAL_SYMBOL_NAME,
   MISSING_NUM,
   MISSING_IDENTIFIER,
   MISSING_NEWLINE,
   MISSING_DEF,
   CANNOT_INVERTED,
   MAX_LIT_ID,
   REDEF_GATE,
   REDEF_SYMBOLIC_NAME,
   REDEF_CONST,
   NUM_TOO_SMALL,
   NUM_TOO_BIG,

   DUMMY_END
};

/**************************************/
/*   Static varaibles and functions   */
/**************************************/
static unsigned lineNo = 0;  // in printint, lineNo needs to ++
static unsigned colNo  = 0;  // in printing, colNo needs to ++
static char buf[1024];
static string errMsg;
static int errInt;
static CirGate *errGate;

static bool
parseError(CirParseError err)
{
   switch (err) {
      case EXTRA_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Extra space character is detected!!" << endl;
         break;
      case MISSING_SPACE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing space character!!" << endl;
         break;
      case ILLEGAL_WSPACE: // for non-space white space character
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal white space char(" << errInt
              << ") is detected!!" << endl;
         break;
      case ILLEGAL_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal "
              << errMsg << "!!" << endl;
         break;
      case ILLEGAL_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Illegal identifier \""
              << errMsg << "\"!!" << endl;
         break;
      case ILLEGAL_SYMBOL_TYPE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Illegal symbol type (" << errMsg << ")!!" << endl;
         break;
      case ILLEGAL_SYMBOL_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Symbolic name contains un-printable char(" << errInt
              << ")!!" << endl;
         break;
      case MISSING_NUM:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Missing " << errMsg << "!!" << endl;
         break;
      case MISSING_IDENTIFIER:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing \""
              << errMsg << "\"!!" << endl;
         break;
      case MISSING_NEWLINE:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": A new line is expected here!!" << endl;
         break;
      case MISSING_DEF:
         cerr << "[ERROR] Line " << lineNo+1 << ": Missing " << errMsg
              << " definition!!" << endl;
         break;
      case CANNOT_INVERTED:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": " << errMsg << " " << errInt << "(" << errInt/2
              << ") cannot be inverted!!" << endl;
         break;
      case MAX_LIT_ID:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Literal \"" << errInt << "\" exceeds maximum valid ID!!"
              << endl;
         break;
      case REDEF_GATE:
         cerr << "[ERROR] Line " << lineNo+1 << ": Literal \"" << errInt
              << "\" is redefined, previously defined as "
              << errGate->getTypeStr() << " in line " << errGate->getLineNo()
              << "!!" << endl;
         break;
      case REDEF_SYMBOLIC_NAME:
         cerr << "[ERROR] Line " << lineNo+1 << ": Symbolic name for \""
              << errMsg << errInt << "\" is redefined!!" << endl;
         break;
      case REDEF_CONST:
         cerr << "[ERROR] Line " << lineNo+1 << ", Col " << colNo+1
              << ": Cannot redefine constant (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_SMALL:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too small (" << errInt << ")!!" << endl;
         break;
      case NUM_TOO_BIG:
         cerr << "[ERROR] Line " << lineNo+1 << ": " << errMsg
              << " is too big (" << errInt << ")!!" << endl;
         break;
      default: break;
   }
   return false;
}

/**************************************************************/
/*   class CirMgr member functions for circuit construction   */
/**************************************************************/
bool
CirMgr::readCircuit(const string& fileName)
{
   //open file
   ifstream f;
   f.open(fileName);
   if(!f.is_open())
   {  cerr << "Cannot open design \"" << fileName << "\"!!" << endl; return false; }
   unsigned line = 1;

   //first line
   string aag;
   f >> aag >> _M >> _I >> _L >> _O >> _A;

   for(unsigned i = 0;i <= (_M+_O);i++)
      _list.push_back(0);

   //const0
   _list[0] = new CirGate(0,CONST_GATE,0);

   //Pi
   unsigned id;
   for(unsigned i = 0;i < _I; i++)
   {
      line++;
      f >> id;
      id/=2;
      _list[id] = new CirGate(id,PI_GATE,line);
      _PIlist.push_back(id);
   }

   //PO
   bool inv = false;
   for(unsigned i = (_M+1);i <= (_M+_O);i++)
   {
      line++;
      //fanin
      f >> id;
      inv = Inv(id);
      id/=2;
      if(_list[id] == 0)
         _list[id] = new CirGate(id,UNDEF_GATE,0);
      //Po
      _list[i] = new CirGate(i,PO_GATE,line);
      _list[i]->AddFanin(_list[id],inv);
      _list[id]->AddFanout(_list[i]);
   }

   //AIG
   unsigned fanin1;
   unsigned fanin2;
   for(unsigned i = 0;i < _A;i++)
   {
      line++;
      //AIG
      f >> id;
      id/=2;
      if(_list[id] == 0)
         _list[id] = new CirGate(id,AIG_GATE,line);
      else
         _list[id]->ChangeType(AIG_GATE,line);
      //fanin1
      f >> fanin1;
      inv = Inv(fanin1);
      fanin1/=2;
      if(_list[fanin1] == 0)
         _list[fanin1] = new CirGate(fanin1,UNDEF_GATE,0);
      _list[id]->AddFanin(_list[fanin1],inv);
      _list[fanin1]->AddFanout(_list[id]);
      //fanin2
      f >> fanin2;
      inv = Inv(fanin2);
      fanin2/=2;
      if(_list[fanin2] == 0)
         _list[fanin2] = new CirGate(fanin2,UNDEF_GATE,0);
      _list[id]->AddFanin(_list[fanin2],inv);
         _list[fanin2]->AddFanout(_list[id]);
   }

   //symbol
   string idstr;
   int idint;
   char head;
   while(true)
   {
      idstr = "";
      memset(buf,0,1024);
      f >> buf;
      head = buf[0];
      if((head != 'i') && (head != 'o')) break;
      for(unsigned i = 1;buf[i] != 0;i++)
         idstr += buf[i];
      myStr2Int(idstr,idint);
      id = idint;
      memset(buf,0,1024);
      f.get();
      f.getline(buf,1024);
      if(head == 'i')
         _list[_PIlist[id]]->AddSymbol(buf);
      else if(head == 'o')
         _list[_M+1+id]->AddSymbol(buf);
   }

   //update DFSlist

   for(unsigned i = (_M+1);i <=(_M + _O);i++)
      _list[i]->DFS(_DFSlist);
   _list[0]->resetPrint();

   //sort UNDEFlist and fanouts
   for(unsigned i = 0;i < _list.size();i++)
      if(_list[i])
         {
            _list[i]->sortFanout();
            if(_list[i]->floating())
               _UNDEFlist.push_back(i);
            if(_list[i]->unused())
               _UNUSEDlist.push_back(i);
         }
    
   return true;
}

/**********************************************************/
/*   class CirMgr member functions for circuit printing   */
/**********************************************************/
/*********************
Circuit Statistics
==================
  PI          20
  PO          12
  AIG        130
------------------
  Total      162
*********************/
void
CirMgr::printSummary() const
{
   cout << endl
        << "Circuit Statistics" << endl
        << "==================" << endl
        << "  PI" << setw(12) << _I << endl
        << "  PO" << setw(12) << _O << endl
        << "  AIG" << setw(11) << _A << endl
        << "------------------" << endl
        << "  Total" << setw(9) << (_I+_O+_A) << endl;
}

void
CirMgr::printNetlist() const
{
   cout << endl;
   for(unsigned i = (_M+1);i <= (_M+_O);i++)
      _list[i]->printGate();
   _list[0]->resetPrint();
}

void
CirMgr::printPIs() const
{
   cout << "PIs of the circuit:";
   for(unsigned i = 0;i < _I;i++)
      cout << " " << _PIlist[i];
   cout << endl;
}

void
CirMgr::printPOs() const
{
   cout << "POs of the circuit:";
   for(unsigned i = (_M+1);i<=(_M+_O);i++)
      cout << " " << i;
   cout << endl;
}

void
CirMgr::printFloatGates() const
{
   if(_UNDEFlist.size())
   {
      cout << "Gates with floating fanin(s):";
      for(unsigned i = 0;i < _UNDEFlist.size();i++)
         cout << " " << _UNDEFlist[i];
      cout << endl;
   }
   if(_UNUSEDlist.size())
   { 
      cout << "Gates defined but not used  :";
      for(unsigned i = 0;i < _UNUSEDlist.size();i++)
         cout << " " << _UNUSEDlist[i];
      cout << endl;
   }
}

void
CirMgr::writeAag(ostream& outfile) const
{
   GateList Q;
   for(unsigned i = 0;i < _PIlist.size();i++)
      Q.push_back(_list[_PIlist[i]]);
   for(unsigned i = (_M + 1);i <= (_M + _O);i++)
      Q.push_back(_list[i]);
   for(unsigned i = (_M + 1);i <= (_M + _O);i++)
      _list[i]->AIGwrite(Q);
   _list[0]->resetPrint();
   outfile << "aag " << _M << ' ' << _I << ' ' << _L << ' ' << _O << ' ' << (Q.size()- _O - _I) << endl;
   for(unsigned i = 0;i < Q.size();i++)
      Q[i]->Gatewrite(outfile);
   for(unsigned i = 0;i < _PIlist.size();i++)
   {
      if(_list[_PIlist[i]]->getSymbol())
         outfile << 'i' << i << " " << _list[_PIlist[i]]->getSymbol() << endl;
   }
   for(unsigned i = (_M + 1);i <= (_M + _O);i++)
   {
      if(_list[i]->getSymbol())
         outfile << 'o' << (i - _M - 1) << " " << _list[i]->getSymbol() << endl;
   }
   outfile << 'c' << endl
           << "Zombieland SAGA" << endl;
}

bool Inv(unsigned id) { if(id%2) return true; else return false; }

CirGate* 
CirMgr::getGate(unsigned gid) const
{
   if(gid >= _list.size())
      return 0;
   return _list[gid];
}

CirMgr::~CirMgr()
{
   for(unsigned i = 0;i < _list.size();i++)
      if(_list[i])
         delete _list[i];
   for(unsigned i = 0;i < _FECgroups.size();i++)
      delete _FECgroups[i];
}

void
CirMgr::writeGate(ostream& outfile, CirGate *g) const
{
}

void
CirMgr::printFECPairs() const
{
   for(unsigned i = 0;i < _FECgroups.size();i++)
   {
      cout << "[" << i << "]";
      for(unsigned j = 0;j < _FECgroups[i]->size();j++)
      {
         cout << " ";
         if(_list[_FECgroups[i]->at(j)]->getSimVal()!=_list[_FECgroups[i]->at(0)]->getSimVal())
            cout << "!";
         cout << _FECgroups[i]->at(j);
      }
      cout << endl;
   }
}

