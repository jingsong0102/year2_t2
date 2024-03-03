/*!
@file AVLTree.h
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 3
@date 3/2/2024
@brief This file contains the declaration of the AVLTree class and its related classes and structures.
*/
//---------------------------------------------------------------------------
#ifndef AVLTREE
#define AVLTREE
//---------------------------------------------------------------------------
#include <stack>
#include "BSTree.h"

/*!
  Definition for the AVL Tree
*/
template <typename T>
class AVLTree : public BSTree<T>
{
public:
  /**
   * @brief Constructs a new AVLTree object.
   * @param oa Pointer to an ObjectAllocator for memory management.
   * @param ShareOA Flag indicating whether to share the ObjectAllocator.
   */
  AVLTree(ObjectAllocator *oa = 0, bool ShareOA = false);
  virtual ~AVLTree() = default; // DO NOT IMPLEMENT
  /**
   * @brief Inserts a value into the AVL tree.
   * @param value Value to be inserted.
   */
  virtual void insert(const T &value) override;
  /**
   * @brief Removes a value from the AVL tree.
   * @param value Value to be removed.
   */
  virtual void remove(const T &value) override;

  // Returns true if efficiency implemented
  /**
   * @brief Returns true if efficiency implemented.
   * @return True if efficiency implemented.
   */
  static bool ImplementedBalanceFactor(void);

private:
  // private stuff
  /**
   * @brief Inserts a node into the AVL tree.
   * @param tree Reference to the root node of the tree.
   * @param value Value to be inserted.
   * @return Pointer to the root node of the modified tree.
   */
  typename BSTree<T>::BinTree insert_node(typename BSTree<T>::BinTree &tree, const T &value);
  /**
   * @brief Removes a node from the AVL tree.
   * @param tree Reference to the root node of the tree.
   * @param value Value to be removed.
   * @return Pointer to the root node of the modified tree.
   */
  typename BSTree<T>::BinTree remove_node(typename BSTree<T>::BinTree &tree, const T &value);
  /**
   * @brief Rotates the tree to the left.
   * @param tree Reference to the root node of the tree.
   * @return Pointer to the root node of the modified tree.
   */
  typename BSTree<T>::BinTree rotate_left(typename BSTree<T>::BinTree &tree);
  /**
   * @brief Rotates the tree to the right.
   * @param tree Reference to the root node of the tree.
   * @return Pointer to the root node of the modified tree.
   */
  typename BSTree<T>::BinTree rotate_right(typename BSTree<T>::BinTree &tree);
  /**
   * @brief Calculates the balance factor of a tree.
   * @param tree Reference to the root node of the tree.
   * @return The balance factor of the tree.
   */
  int balance_factor(typename BSTree<T>::BinTree tree);
};

#include "AVLTree.cpp"

#endif /* AVLTREE */
//---------------------------------------------------------------------------
