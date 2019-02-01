/****************************************************************************
  FileName     [ myHashSet.h ]
  PackageName  [ util ]
  Synopsis     [ Define HashSet ADT ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2014-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef MY_HASH_SET_H
#define MY_HASH_SET_H

#include <vector>

using namespace std;

//---------------------
// Define HashSet class
//---------------------
// To use HashSet ADT,
// the class "Data" should at least overload the "()" and "==" operators.
//
// "operator ()" is to generate the hash key (size_t)
// that will be % by _numBuckets to get the bucket number.
// ==> See "bucketNum()"
//
// "operator ==" is to check whether there has already been
// an equivalent "Data" object in the HashSet.
// Note that HashSet does not allow equivalent nodes to be inserted
//
template <class Data>
class HashSet
{
public:
   HashSet(size_t b = 0) : _numBuckets(0), _buckets(0) { if (b != 0) init(b); }
   ~HashSet() { reset(); }

   // TODO: implement the HashSet<Data>::iterator
   // o An iterator should be able to go through all the valid Data
   //   in the Hash
   // o Functions to be implemented:
   //   - constructor(s), destructor
   //   - operator '*': return the HashNode
   //   - ++/--iterator, iterator++/--
   //   - operators '=', '==', !="
   //
   class iterator
   {
      friend class HashSet<Data>;

   public:
      iterator(vector<Data>* b,size_t n,size_t ab = 0,size_t a = 0):_buckets(b),_numBuckets(n),_at_bucket(ab),_at(a) {}
      ~iterator() {}
      const Data& operator * () const { return _buckets[_at_bucket][_at]; }
      Data& operator * (){ return _buckets[_at_bucket][_at]; }
      iterator& operator ++ ()
      {
         if(_at < (_buckets[_at_bucket].size() - 1))
         { _at++; return *this; }
         _at = 0;
         while(_at_bucket < _numBuckets)
         {
            _at_bucket++;
            if(!_buckets[_at_bucket].empty())
            break;
         }
         return *this;
      }
      iterator operator ++ (int) { iterator i = *this; ++*this; return i; }
      iterator& operator -- ()
      {
         if(_at_bucket == 0 && _at == 0) return *this;
         if(_at != 0)
         { _at--; return *this;}
         do{ _at_bucket--;}while(!_buckets[_at_bucket].empty());
         return *this;
      }
      iterator operator -- (int) { iterator i = *this; --*this; return i; }

      iterator& operator = (const iterator& i) { _buckets = i._buckets; _numBuckets = i._numBuckets; _at_bucket = i._at_bucket; _at = i._at; } 

      bool operator == (const iterator& i) const { return ((_buckets == i._buckets)&&(_at_bucket == i._at_bucket)&&(_at == i._at)); }
      bool operator != (const iterator& i) const { return ((_buckets != i._buckets)||(_at_bucket != i._at_bucket)||(_at != i._at)); }
   private:
      vector<Data>*   _buckets;
      size_t          _numBuckets;
      size_t          _at_bucket;
      size_t          _at;
   };

   void init(size_t b) { _numBuckets = b; _buckets = new vector<Data>[b]; }
   void reset() {
      _numBuckets = 0;
      if (_buckets) { delete [] _buckets; _buckets = 0; }
   }
   void clear() {
      for (size_t i = 0; i < _numBuckets; ++i) _buckets[i].clear();
   }
   size_t numBuckets() const { return _numBuckets; }

   vector<Data>& operator [] (size_t i) { return _buckets[i]; }
   const vector<Data>& operator [](size_t i) const { return _buckets[i]; }

   // TODO: implement these functions
   //
   // Point to the first valid data
   iterator begin() const
   {
      for(unsigned i = 0;i < _numBuckets;i++)
      if(!_buckets[i].empty())
         return iterator(_buckets,_numBuckets,i,0);
      return end();
   }
   // Pass the end
   iterator end() const { return iterator(_buckets,_numBuckets,_numBuckets,0); }
   // return true if no valid data
   bool empty() const { return size() == 0;}
   // number of valid data
   size_t size() const
   {
      size_t s = 0;
      for(unsigned i = 0;i < _numBuckets;i++)
         s += _buckets[i].size();
      return s;
   }

   // check if d is in the hash...
   // if yes, return true;
   // else return false;
   bool check(const Data& d) const
   {
      size_t at_bucket = bucketNum(d);
      for(unsigned i = 0;i < _buckets[at_bucket].size();i++)
         if(d == _buckets[at_bucket][i])
            return true;
      return false;
   }

   // query if d is in the hash...
   // if yes, replace d with the data in the hash and return true;
   // else return false;
   bool query(Data& d) const
   {
      size_t at_bucket = bucketNum(d);
      for(size_t i = 0;i < _buckets[at_bucket].size();i++)
         if(d == _buckets[at_bucket][i])
         { d = _buckets[at_bucket][i]; return true; }
      return false;
   }

   // update the entry in hash that is equal to d (i.e. == return true)
   // if found, update that entry with d and return true;
   // else insert d into hash as a new entry and return false;
   bool update(const Data& d)
   {
      size_t at_bucket = bucketNum(d);
      for(size_t i = 0;i < _buckets[at_bucket].size();i++)
         if(d == _buckets[at_bucket][i])
         { _buckets[at_bucket][i] = d; return true;}
      insert(d);
      return false;
   }

   // return true if inserted successfully (i.e. d is not in the hash)
   // return false is d is already in the hash ==> will not insert
   bool insert(const Data& d)
   {
      if(check(d)) return false;
      _buckets[bucketNum(d)].push_back(d);
      return true;
   }

   // return true if removed successfully (i.e. d is in the hash)
   // return fasle otherwise (i.e. nothing is removed)
   bool remove(const Data& d)
   {
      size_t at_bucket = bucketNum(d);
      for(size_t i = 0;i < _buckets[at_bucket].size();i++)
         if(d == _buckets[at_bucket][i])
         {
            _buckets[at_bucket][i] = _buckets[at_bucket].back();
            _buckets[at_bucket].pop_back();
            return true;
         }
      return false;
   }

private:
   // Do not add any extra data member
   size_t            _numBuckets;
   vector<Data>*     _buckets;

   size_t bucketNum(const Data& d) const {
      return (d() % _numBuckets); }
};

#endif // MY_HASH_SET_H
