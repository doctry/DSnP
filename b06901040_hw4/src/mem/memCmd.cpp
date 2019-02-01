/****************************************************************************
  FileName     [ memCmd.cpp ]
  PackageName  [ mem ]
  Synopsis     [ Define memory test commands ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2007-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/
#include <iostream>
#include <iomanip>
#include "memCmd.h"
#include "memTest.h"
#include "cmdParser.h"
#include "util.h"

using namespace std;

extern MemTest mtest;  // defined in memTest.cpp

bool
initMemCmd()
{
   if (!(cmdMgr->regCmd("MTReset", 3, new MTResetCmd) &&
         cmdMgr->regCmd("MTNew", 3, new MTNewCmd) &&
         cmdMgr->regCmd("MTDelete", 3, new MTDeleteCmd) &&
         cmdMgr->regCmd("MTPrint", 3, new MTPrintCmd)
      )) {
      cerr << "Registering \"mem\" commands fails... exiting" << endl;
      return false;
   }
   return true;
}


//----------------------------------------------------------------------
//    MTReset [(size_t blockSize)]
//----------------------------------------------------------------------
CmdExecStatus
MTResetCmd::exec(const string& option)
{
   // check option
   string token;
   if (!CmdExec::lexSingleOption(option, token))
      return CMD_EXEC_ERROR;
   if (token.size()) {
      int b;
      if (!myStr2Int(token, b) || b < int(toSizeT(sizeof(MemTestObj)))) {
         cerr << "Illegal block size (" << token << ")!!" << endl;
         return CmdExec::errorOption(CMD_OPT_ILLEGAL, token);
      }
      #ifdef MEM_MGR_H
      mtest.reset(toSizeT(b));
      #else
      mtest.reset();
      #endif // MEM_MGR_H
   }
   else
      mtest.reset();
   return CMD_EXEC_DONE;
}

void
MTResetCmd::usage(ostream& os) const
{  
   os << "Usage: MTReset [(size_t blockSize)]" << endl;
}

void
MTResetCmd::help() const
{  
   cout << setw(15) << left << "MTReset: " 
        << "(memory test) reset memory manager" << endl;
}


//----------------------------------------------------------------------
//    MTNew <(size_t numObjects)> [-Array (size_t arraySize)]
//----------------------------------------------------------------------
CmdExecStatus
MTNewCmd::exec(const string& option)
{
   // TODO
   // Use try-catch to catch the bad_alloc exception
   
   // check option
   if(option == "")
      return errorOption(CMD_OPT_MISSING, option);
   vector<string> token;
   if(!CmdExec::lexOptions(option, token))
      return CMD_EXEC_ERROR;
   // if the first option is not -array
   if(myStrNCmp("-Array",token[0],2) != 0)
   {
      //“numObjects” is not a legal integer or is not a positive number
      int num = 0;
      if(!myStr2Int(token[0], num))
         return errorOption(CMD_OPT_ILLEGAL,token[0]);
      if(num <= 0)
         return errorOption(CMD_OPT_ILLEGAL,token[0]);
      //check if the second option exist
      if(token.size() == 1)
      {
         //exe
         try
         {
            mtest.newObjs(num);
         }
         catch(const bad_alloc&)
         {
            return CMD_EXEC_ERROR;
         }
         return CMD_EXEC_DONE;
      }
      //check if the second option is -array
      if(myStrNCmp("-Array",token[1],2) != 0)
         return errorOption(CMD_OPT_EXTRA,token[1]);
      //check if there is third option
      if(token.size() == 2)
         return errorOption(CMD_OPT_MISSING,token[1]);
      //check if the third option is legal
      int num_a = 0;
      if(!myStr2Int(token[2],num_a))
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      if(num_a <= 0)
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      //check if there is extra option
      if(token.size() > 3)
         return errorOption(CMD_OPT_EXTRA,token[3]);
      //exe
      try
      {
         mtest.newArrs(num,num_a);
      }
      catch(const bad_alloc&)
      {
         return CMD_EXEC_ERROR;
      }
      return CMD_EXEC_DONE;
   }
   
   //if the first option is -array
   //check if there is second option
   if(token.size() == 1)
      return errorOption(CMD_OPT_MISSING,token[0]);
   //check if the second option is legal
   int num_a = 0;
   if(!myStr2Int(token[1],num_a))
      return errorOption(CMD_OPT_ILLEGAL,token[1]);
   if(num_a <= 0)
      return errorOption(CMD_OPT_ILLEGAL,token[1]);
   //check if there is third option
   if(token.size() == 2)
      return errorOption(CMD_OPT_MISSING,"");
   //check if the third option is legal
   int num = 0;
   if(!myStr2Int(token[2],num))
      return errorOption(CMD_OPT_ILLEGAL,token[2]);
   if(num <= 0)
      return errorOption(CMD_OPT_ILLEGAL,token[2]);
   //check if there is extra option
   if(token.size() > 3)
      return errorOption(CMD_OPT_EXTRA,token[3]);
   //exe
   try
   {
      mtest.newArrs(num,num_a);
   }
   catch(const bad_alloc&)
   {
      return CMD_EXEC_ERROR;
   }
   return CMD_EXEC_DONE;
}

void
MTNewCmd::usage(ostream& os) const
{  
   os << "Usage: MTNew <(size_t numObjects)> [-Array (size_t arraySize)]\n";
}

void
MTNewCmd::help() const
{  
   cout << setw(15) << left << "MTNew: " 
        << "(memory test) new objects" << endl;
}


//----------------------------------------------------------------------
//    MTDelete <-Index (size_t objId) | -Random (size_t numRandId)> [-Array]
//----------------------------------------------------------------------
CmdExecStatus
MTDeleteCmd::exec(const string& option)
{
   // TODO
   
   // check option
   if(option == "")
      return errorOption(CMD_OPT_MISSING, option);
   vector<string> token;
   if(!CmdExec::lexOptions(option, token))
      return CMD_EXEC_ERROR;
   // if the first option is not -array
   if(myStrNCmp("-Array",token[0],2) != 0)
   {
      // if the first option is -random
      if(myStrNCmp("-Random",token[0],2) == 0)
      {
         //check if there is second option
         if(token.size() == 1)
            return errorOption(CMD_OPT_MISSING,token[0]);
         //check if the second option is legal;
         int num = 0;
         if(!myStr2Int(token[1],num))
            return errorOption(CMD_OPT_ILLEGAL,token[1]);
         if(num < 0)
            return errorOption(CMD_OPT_ILLEGAL,token[1]);
         // check if there is third option
         if(token.size() == 2)
         {
            if(mtest.getObjListSize() == 0)
            {
               cerr << "Size of object list is 0!!" << endl;
               return errorOption(CMD_OPT_ILLEGAL,token[1]);
            }
            //exe
            for(int i = 0; i < num;i++)
               mtest.deleteObj(rnGen(mtest.getObjListSize()));
            return CMD_EXEC_DONE;
         }
         //check if the third option is -array
         if(myStrNCmp("-Array",token[2],2) != 0)
         {
            if((myStrNCmp("-Index",token[2],2) == 0) || (myStrNCmp("-Random",token[2],2) == 0))
               return errorOption(CMD_OPT_EXTRA,token[2]);
            return errorOption(CMD_OPT_ILLEGAL,token[2]);
         }
         //check if there is extra option
         if(token.size() > 3)
            return errorOption(CMD_OPT_EXTRA,token[3]);
         if(mtest.getArrListSize() == 0)
         {
            cerr << "Size of array list is 0!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL,token[0]);
         }
         //exe
         for(int i = 0; i < num; i++)
            mtest.deleteArr(rnGen(mtest.getArrListSize()));
         return CMD_EXEC_DONE;
      }
      //if the first option is -Index
      if(myStrNCmp("-Index",token[0],2) == 0)
      {
         //check if there is second option
         if(token.size() == 1)
            return errorOption(CMD_OPT_MISSING,token[0]);
         //check if the second option is legal;
         int num = 0;
         if(!myStr2Int(token[1],num))
            return errorOption(CMD_OPT_ILLEGAL,token[1]);
         if(num < 0)
            return errorOption(CMD_OPT_ILLEGAL,token[1]);
         // check if there is third option
         if(token.size() == 2)
         {
            if(mtest.getObjListSize() <= (size_t)num)
            {
               cerr << "Size of object list (" << mtest.getObjListSize() << ") is <= " << num <<"!!" << endl;
               return errorOption(CMD_OPT_ILLEGAL,token[1]);
            }
            //exe
            mtest.deleteObj(num);
            return CMD_EXEC_DONE;
         }
         //check if the third option is -array
         if(myStrNCmp("-Array",token[2],2) != 0)
         {
            if((myStrNCmp("-Index",token[2],2) == 0) || (myStrNCmp("-Random",token[2],2) == 0))
               return errorOption(CMD_OPT_EXTRA,token[2]);  
            return errorOption(CMD_OPT_ILLEGAL,token[2]);
         }
         //check if there is extra option
         if(token.size() > 3)
            return errorOption(CMD_OPT_EXTRA,token[3]);
         if(mtest.getArrListSize() <= (size_t)num)
         {
            cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << num <<"!!" << endl;
            return errorOption(CMD_OPT_ILLEGAL,token[1]);
         }
         //this exe
         mtest.deleteArr(num);
         return CMD_EXEC_DONE;
      }
      //if the first option is illegal
      return errorOption(CMD_OPT_ILLEGAL,token[0]);
   }
   //if the first option is -array
   //check if there is second oprion
   if(token.size() == 1)
      return errorOption(CMD_OPT_MISSING,"");
   // if the second option is -random
   if(myStrNCmp("-Random",token[1],2) == 0)
   {
      //check if there is third option
      if(token.size() == 2)
         return errorOption(CMD_OPT_MISSING,token[1]);
      //check if the third option is legal;
      int num = 0;
      if(!myStr2Int(token[2],num))
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      if(num < 0)
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      //check if there is extra option
      if(token.size() > 3)
      {
         int num_test = 0;
         if((myStrNCmp("-Index",token[3],2) == 0) || (myStrNCmp("-Array",token[3],2) == 0) ||((myStr2Int(token[3],num_test) && num_test >=0)))
         {
            num_test+=1;
            return errorOption(CMD_OPT_EXTRA,token[3]);
         }
         return errorOption(CMD_OPT_EXTRA,token[3]);
      }
      if(mtest.getArrListSize() == 0)
      {
         cerr << "Size of array list is 0!!" << endl;
         return errorOption(CMD_OPT_ILLEGAL,token[1]);
      }
      //exe
      for(int i = 0; i < num; i++)
         mtest.deleteArr(rnGen(mtest.getArrListSize()));
      return CMD_EXEC_DONE;
   }
   //if the second option is -Index
   if(myStrNCmp("-Index",token[1],2) == 0)
   {
      //check if there is third option
      if(token.size() == 2)
         return errorOption(CMD_OPT_MISSING,token[1]);
      //check if the third option is legal;
      int num = 0;
      if(!myStr2Int(token[2],num))
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      if(num < 0)
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      if(token.size() > 3)
      {
         int num_test = 0;
         if((myStrNCmp("-Index",token[3],2) == 0) || (myStrNCmp("-Array",token[3],2) == 0) ||((myStr2Int(token[3],num_test) && num_test >=0)))
         {
            num_test+=1;
            return errorOption(CMD_OPT_EXTRA,token[3]);
         }
         return errorOption(CMD_OPT_EXTRA,token[3]);
      }
      if(mtest.getArrListSize() <= (size_t)num)
      {
         cerr << "Size of array list (" << mtest.getArrListSize() << ") is <= " << num <<"!!" << endl;
         return errorOption(CMD_OPT_ILLEGAL,token[2]);
      }
      //this part is for exe
      mtest.deleteArr(num);
      return CMD_EXEC_DONE;
   }
   //if the second option is illegal
   return errorOption(CMD_OPT_ILLEGAL,token[1]);
}

void
MTDeleteCmd::usage(ostream& os) const
{  
   os << "Usage: MTDelete <-Index (size_t objId) | "
      << "-Random (size_t numRandId)> [-Array]" << endl;
}

void
MTDeleteCmd::help() const
{  
   cout << setw(15) << left << "MTDelete: " 
        << "(memory test) delete objects" << endl;
}


//----------------------------------------------------------------------
//    MTPrint
//----------------------------------------------------------------------
CmdExecStatus
MTPrintCmd::exec(const string& option)
{
   // check option
   if (option.size())
      return CmdExec::errorOption(CMD_OPT_EXTRA, option);
   mtest.print();

   return CMD_EXEC_DONE;
}

void
MTPrintCmd::usage(ostream& os) const
{  
   os << "Usage: MTPrint" << endl;
}

void
MTPrintCmd::help() const
{  
   cout << setw(15) << left << "MTPrint: " 
        << "(memory test) print memory manager info" << endl;
}
