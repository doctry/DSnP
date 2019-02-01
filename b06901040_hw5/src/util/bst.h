/****************************************************************************
  FileName     [ bst.h ]
  PackageName  [ util ]
  Synopsis     [ Define binary search tree package ]
  Author       [ Chung-Yang (Ric) Huang ]
  Copyright    [ Copyleft(c) 2005-present LaDs(III), GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef BST_H
#define BST_H

#include <cassert>

using namespace std;

template <class T> class BSTree;

// BSTreeNode is supposed to be a private class. User don't need to see it.
// Only BSTree and BSTree::iterator can access it.
//
// DO NOT add any public data member or function to this class!!
//
template <class T>
class BSTreeNode
{
   // TODO: design your own class!!
   friend class BSTree<T>;
   friend class BSTree<T>::iterator;

   BSTreeNode(const T& d,BSTreeNode<T>* p, bool e = false, BSTreeNode<T>* l = 0, BSTreeNode<T>* r = 0):
      _data(d), _parent(p), _left(l), _right(r), _end(e) {}

   //Data member
   T               _data;
   BSTreeNode<T>*  _parent;
   BSTreeNode<T>*  _left;
   BSTreeNode<T>*  _right;
   bool            _end;
};


template <class T>
class BSTree
{
   // TODO: design your own class!!
   public:
   // TODO: decide the initial value for _isSorted
   BSTree() { T x; _root = 0; _tail = new BSTreeNode<T>(x,0,true);}
   ~BSTree() { clear(); delete _tail;}

   // DO NOT add any more data member or function for class iterator
   class iterator
   {
      friend class BSTree;

   public:
      iterator(BSTreeNode<T>* n= 0): _node(n) {}
      iterator(const iterator& i) : _node(i._node) {}
      ~iterator() {} // Should NOT delete _node

      // TODO: implement these overloaded operators
      const T& operator * () const { return _node->_data; }
      T& operator * () { return _node->_data; }
      iterator& operator ++ ()
      {
         //go right
         if(_node->_right)
         {  
           _node = _node->_right;
           while(_node->_left)
              _node = _node->_left;
           return *this;
         }
         //go up
         while(_node->_parent)
         {
            if(_node->_parent->_data > _node->_data)
            { _node = _node->_parent; return *this; }
            _node = _node->_parent;
         }
         //root
         return *this;
      }
      iterator operator ++ (int) { iterator i = *this; ++(*this); return i; }
      iterator& operator -- ()
      {
         if(_node->_end)
         { _node = _node->_parent; return *this;}
         //go left
         if(_node->_left)
         {
            _node = _node->_left;
            while(_node->_right)
               _node = _node->_right;
            return *this;
         }
         //go up
         while(_node->_parent)
         {
            if(!(_node->_parent->_data > _node->_data))
            { _node = _node->_parent; return *this; }
            _node = _node->_parent;
         }
         return *this;
      }
      iterator operator -- (int) { iterator i = *this; --(*this); 
      return i; 
      }

      iterator& operator = (const iterator& i) { _node = i._node; return *(this); }

      bool operator != (const iterator& i) const { return (_node != i._node); }
      bool operator == (const iterator& i) const { return (_node == i._node); }

   private:
      BSTreeNode<T>* _node;
   };

   // TODO: implement these functions
   iterator begin() const
   {
      if(empty())
         return end();
      BSTreeNode<T>* ret = _root;
      while(ret->_left)
         ret = ret->_left;
      return iterator(ret);
   }
   iterator end() const { return iterator(_tail); }
   bool empty() const { return (_root == 0); }

   size_t size() const 
   {
      size_t ret = 0;
      for(iterator i = begin(); i != end();i++)
         ret++;
      return ret;
   }

   void insert(const T& x)
   {
      if(empty())
      {
         _root = new BSTreeNode<T>(x,0);
         _root->_right = _tail;
         _tail->_parent = _root;
      }
      else
      {
         BSTreeNode<T>* trav = _root;
         while(true)
         {
            if(trav == _tail)
            {
               trav = trav->_parent;
               trav->_right = new BSTreeNode<T>(x,trav);
               trav->_right->_right = _tail;
               _tail->_parent = trav->_right;
               return;
            }
            if(trav->_data > x)
            {
               if(trav->_left == 0)
               { trav->_left = new BSTreeNode<T>(x,trav); return; }
               else
                  trav = trav->_left;
            }
            else
            {
               if(trav->_right == 0)
               { trav->_right = new BSTreeNode<T>(x,trav); return; }
               else
                  trav = trav->_right;
            }
         }
      }
   }
   void pop_front()
   {
      if(empty())
         return;
      BSTreeNode<T>* trash = begin()._node;
      if(trash->_parent)
      {
         trash->_parent->_left = trash->_right;
         if(trash->_right)
            trash->_right->_parent = trash->_parent;
      }
      else if(trash->_right != _tail)
      { _root = trash->_right; _root->_parent = 0; }
      else
      { _root = 0; _tail->_parent = 0;}
      delete trash;
   }
   void pop_back()
   {
      if(empty())
         return;
      BSTreeNode<T>* trash = _tail->_parent;
      if(trash->_parent)
      {
         trash->_parent->_right = trash->_left;
         if(trash->_left)
         {
            trash->_left->_parent = trash->_parent;
            BSTreeNode<T>* suc = trash->_left;
            while(suc->_right)
               suc = suc->_right;
            suc->_right = _tail;
            _tail->_parent = suc;
         }
         else
         {  
            _tail->_parent = trash->_parent;
            trash->_parent->_right = _tail;
         }
      }
      else if(trash->_left)
      {
         _root = trash->_left;
         _root->_parent = 0;
         BSTreeNode<T>* suc = trash->_left;
         while(suc->_right)
            suc = suc->_right;
         suc->_right = _tail;
         _tail->_parent = suc;
      }
      else
      { _tail->_parent = 0; _root = 0;}
      delete trash;
   }

   // return false if nothing to erase
   bool erase(iterator pos)
   {
      if(empty())
         return false;
      //leaf
      if((pos._node->_left == 0) && (pos._node->_right == 0))
      {
         if(pos._node->_parent->_right == pos._node)
            pos._node->_parent->_right = 0;
         else
            pos._node->_parent->_left = 0;
      }
      //only left child
      else if((pos._node->_left != 0) && (pos._node->_right == 0))
      {
         if(pos._node->_parent->_right == pos._node)
            pos._node->_parent->_right = pos._node->_left;
         else
            pos._node->_parent->_left = pos._node->_left;
         pos._node->_left->_parent = pos._node->_parent;
      }
      //only right child
      else if((pos._node->_left == 0) && (pos._node->_right != 0))
      {
         if(pos._node == _root)
         { pop_front(); return true;}
         if(pos._node->_parent->_right == pos._node)
            pos._node->_parent->_right = pos._node->_right;
         else
            pos._node->_parent->_left = pos._node->_right;
         pos._node->_right->_parent = pos._node->_parent;
      }
      //right most
      else if((pos._node->_left != 0) && (pos._node->_right == _tail))
      {
         pop_back();
         return true;
      }
      //two children
      else
      {
         BSTreeNode<T>* suc = pos._node->_right;
         while(suc->_left)
            suc = suc->_left;
         *pos = suc->_data;
         if(suc->_parent->_right == suc)
            suc->_parent->_right = suc->_right;
         else
            suc->_parent->_left = suc->_right;
         if(suc->_right)
           suc->_right->_parent = suc->_parent;
         pos._node = suc;
      }
      delete pos._node;
      return true;
   }
   bool erase(const T& x) 
   {
      if(empty())
         return false;
      iterator pos = find(x);
      if(pos == end())
         return false;
      return erase(pos);
   }

   iterator find(const T& x)
   {
      if(empty())
         return end();
      iterator i(_root);
      while(true)
      {
         if(i == end())
            return i;
         if(x > *i)
         {
            if(i._node->_right)
               i._node = i._node->_right;
            else
               return end();
         }
         else if(x < *i)
         {
            if(i._node->_left)
               i._node = i._node->_left;
            else
               return end();
         }
         else
            return i;
      }
   }

   void clear() { size_t j = size(); for(size_t i = 0;i < j;i++) pop_front(); } // delete all nodes except for the dummy node 

   void sort() {}

   void print() const { cout << "What the fuxk?" << endl; }

private:
   // [NOTE] DO NOT ADD or REMOVE any data member
   BSTreeNode<T>*  _root;
   BSTreeNode<T>*  _tail;
   // [OPTIONAL TODO] helper functions; called by public member functions
};

#endif // BST_H