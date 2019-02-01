/****************************************************************************
  FileName     [ myMinHeap.h ]
  PackageName  [ util ]
  Synopsis     [ Define MinHeap ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_MIN_HEAP_H
#define MY_MIN_HEAP_H

#include <algorithm>
#include <vector>

template <class Data>
class MinHeap
{
public:
   MinHeap(size_t s = 0) { if (s != 0) _data.reserve(s); }
   ~MinHeap() {}

   void clear() { _data.clear(); }

   // For the following member functions,
   // We don't respond for the case vector "_data" is empty!
   const Data& operator [] (size_t i) const { return _data[i]; }   
   Data& operator [] (size_t i) { return _data[i]; }

   size_t size() const { return _data.size(); }

   // TODO
   const Data& min() const { return _data[0]; }
   void insert(const Data& d)
   {
      _data.push_back(d);
      size_t t = _data.size() - 1;
      size_t p;
      while(t>0)
      {
         p = (t-1)/2;
         if(!(d < _data[p])) break;
         _data[t] = _data[p];
         t = p;
      }
      _data[t] = d;
   }
   void delMin() { delData(0); }
   void delData(size_t i)
   {
      if(i >= size()) return;
      _data[i] = _data.back();
      _data.pop_back();
      //search parent
      size_t t = i;
      size_t p;
      while(t > 0)
      {
         p = (t-1)/2;
         if(!(_data[t] < _data[p])) break;
         swap(_data[t],_data[p]);
         t = p;
      }
      //search child
      p = 2*i + 1;
      while(p < size())
      {
         if(p < size()-1 )
            if(_data[p+1] < _data[p]) p++;
         if(!(_data[p] < _data[i])) break;
         swap(_data[i],_data[p]);
         i = p;
         p = 2*i + 1;
      }
   }

private:
   // DO NOT add or change data members
   vector<Data>   _data;
   void swap(Data& d1,Data& d2)
   {
      Data tmp = d1;
      d1 = d2;
      d2 = tmp;
   }
};

#endif // MY_MIN_HEAP_H
