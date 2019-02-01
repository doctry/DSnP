/****************************************************************************
  FileName     [ dlist.h ]
  PackageName  [ util ]
  Synopsis     [ Define doubly linked list package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef DLIST_H
#define DLIST_H

#include <cassert>

template <class T> class DList;

// DListNode is supposed to be a private class. User don't need to see it.
// Only DList and DList::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class DListNode
{
   friend class DList<T>;
   friend class DList<T>::iterator;

   DListNode(const T& d, DListNode<T>* p = 0, DListNode<T>* n = 0):
      _data(d), _prev(p), _next(n) {}

   // [NOTE] DO NOT ADD or REMOVE any data member
   T              _data;
   DListNode<T>*  _prev;
   DListNode<T>*  _next;
};


template <class T>
class DList
{
public:
   // TODO: decide the initial value for _isSorted
   DList() {
      _head = new DListNode<T>(T());
      _head->_prev = _head->_next = _head; // _head is a dummy node
      _isSorted = true;
   }
   ~DList() { clear(); delete _head; }

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class DList;

   public:
      iterator(DListNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ () { _node = _node->_next; return *(this);}
      iterator operator ++ (int) { _node = _node->_next; return iterator(_node->_prev); }
      iterator& operator -- () { _node = _node->_prev; return *(this); }
      iterator operator -- (int) { _node = _node->_prev; return iterator(_node->_next); }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      DListNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const { return iterator(_head); }
   iterator end() const { return iterator(_head -> _prev); }
   bool empty() const { return(_head == _head -> _prev); }

   size_t size() const 
   {
      size_t ret = 0;
      for(iterator i =  begin();i != end();i++)
         ret ++;
      return ret;
   }

   void push_back(const T& x)
   {
      if(empty())
      {
        _head = new DListNode<T>(x,_head,_head);
        Connect(_head->_prev,_head,_head->_next);
      }
      else
      {
        _isSorted = false;
        DListNode<T>* node = new DListNode<T>(x,_head->_prev->_prev,_head->_prev);
        Connect(node->_prev,node,node->_next);
      }
      return;
   }
   void pop_front()
   {
      if(empty())
         return;
      Delete(_head);
   }
   void pop_back()
   {
      if(empty())
         return;
      Delete(_head->_prev->_prev);
      return;
   }

   // return false if nothing to erase
   bool erase(iterator pos)
   {
      if(empty())
         return false;
      Delete(pos._node);
      return true;
   }
   bool erase(const T& x) 
   {
      if(empty())
         return false;
      for(iterator i = begin();i != end();i++)
         if(*i == x)
         {
            Delete(i._node);
            return true;
         }
      return false;
   }

   iterator find(const T& x)
   {
       if(empty())
          return end();
       for(iterator i = begin();i != end();i++)
          if(*i == x)
             return i;
       return end();
   }

   void clear() { size_t j = size(); for(size_t i = 0;i < j;i++) pop_front(); } // delete all nodes except for the dummy node 

   void sort() 
   {
      if(empty())
         return;
      if(_isSorted)
         return;
      for(iterator i = begin();i != end();i++)
      {
         T min = *i;
         iterator minptr = i;
         for(iterator j = i;j != end();j++)
         {
            if(*j < min)
            {
               min = *j;
               minptr = j;
            }
         }
         *minptr = *i;
         *i = min;
      }
      _isSorted = true;
      return;
   }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   DListNode<T>*  _head;     // = dummy node if list is empty
   mutable bool   _isSorted; // (optionally) to indicate the array is sorted

   // [OPTIONAL TODO] helper functions; called by public member functions

   void Delete(DListNode<T>* trash)//used when > 1 object
   {
      if(trash == _head)
         _head = _head->_next;
      trash->_prev->_next = trash->_next;
      trash->_next->_prev = trash->_prev;
      delete trash;
      return;
   }
   void Connect(DListNode<T>* prev,DListNode<T>* mid,DListNode<T>* next)
   {
      prev->_next = mid;
      next->_prev = mid;
      mid->_prev = prev;
      mid->_next = next;
   }
};

#endif // DLIST_H
