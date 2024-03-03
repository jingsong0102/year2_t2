/*!
@file BSTree.h
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 3
@date 3/2/2024
@brief This file contains the declaration of the BSTree class and its related classes and structures.
*/
//---------------------------------------------------------------------------
#ifndef BSTREE_H
#define BSTREE_H
//---------------------------------------------------------------------------
#include <string>    // std::string
#include <stdexcept> // std::exception

#include "ObjectAllocator.h"

/*!
  The exception class for the AVL/BST classes
*/
class BSTException : public std::exception
{
public:
  /*!
    Non-default constructor

    \param ErrCode
      The kind of exception (only one currently)

    \param Message
      The human-readable reason for the exception.
  */
  BSTException(int ErrCode, const std::string &Message) : error_code_(ErrCode), message_(Message){};

  /*!
    Retrieve the exception code.

    \return
      E_NO_MEMORY
  */
  virtual int code() const
  {
    return error_code_;
  }

  /*!
    Retrieve the message string

    \return
      The human-readable message.
  */
  virtual const char *what() const throw()
  {
    return message_.c_str();
  }

  //! Destructor
  virtual ~BSTException() {}

  //! The kinds of exceptions (only one currently)
  enum BST_EXCEPTION
  {
    E_NO_MEMORY
  };

private:
  int error_code_;      //!< The code of the exception
  std::string message_; //!< Readable message text
};

/*!
  The definition of the BST
*/
template <typename T>
class BSTree
{
public:
  //! The node structure
  struct BinTreeNode
  {
    BinTreeNode *left;  //!< The left child
    BinTreeNode *right; //!< The right child
    T data;             //!< The data
    int balance_factor; //!< optional for efficient balancing
    unsigned count;     //!< nodes in this subtree for efficient indexing

    //! Default constructor
    BinTreeNode() : left(0), right(0), data(0), balance_factor(0), count(1){};

    //! Conversion constructor
    BinTreeNode(const T &value) : left(0), right(0), data(value), balance_factor(0), count(1){};
  };

  //! shorthand
  typedef BinTreeNode *BinTree;
  /**
   * @brief Constructs a new BSTree object.
   * @param oa Pointer to an ObjectAllocator for memory management.
   * @param ShareOA Flag indicating whether to share the ObjectAllocator.
   */
  BSTree(ObjectAllocator *oa = 0, bool ShareOA = false);
  /**
   * @brief Copy constructor.
   * @param rhs Reference to the BSTree to be copied.
   */
  BSTree(const BSTree &rhs);
  /**
   * @brief Destructor.
   */
  virtual ~BSTree();
  /**
   * @brief Assignment operator.
   * @param rhs Reference to the BSTree to be assigned.
   * @return Reference to the modified BSTree.
   */
  BSTree &operator=(const BSTree &rhs);
  /**
   * @brief Subscript operator for r-values.
   * @param index The index of the node to be retrieved.
   * @return Pointer to the node at the specified index.
   */
  const BinTreeNode *operator[](int index) const; // for r-values (Extra Credit)
  /**
   * @brief Inserts a new node into the BST.
   * @param value The value to be inserted.
   */
  virtual void insert(const T &value);
  /**
   * @brief Removes a node from the BST.
   * @param value The value to be removed.
   */
  virtual void remove(const T &value);
  /**
   * @brief Clears the BST.
   */
  void clear();
  /**
   * @brief Searches for a value in the BST.
   * @param value The value to be searched for.
   * @param compares The number of comparisons made during the search.
   * @return true if the value is found, false otherwise.
   */
  bool find(const T &value, unsigned &compares) const;
  /**
   * @brief Checks if the BST is empty.
   * @return true if the BST is empty, false otherwise.
   */
  bool empty() const;
  /**
   * @brief Returns the number of nodes in the BST.
   * @return The number of nodes in the BST.
   */
  unsigned int size() const;
  /**
   * @brief Returns the height of the BST.
   * @return The height of the BST.
   */
  int height() const;
  /**
   * @brief Returns the root of the BST.
   * @return The root of the BST.
   */
  BinTree root() const;

protected:
  /**
   * @brief Returns the root of the BST.
   * @return The root of the BST.
   */
  BinTree &get_root();
  /**
   * @brief Creates a new node.
   * @param value The value to be stored in the new node.
   * @return Pointer to the new node.
   */
  BinTree make_node(const T &value) const;
  /**
   * @brief Frees a node.
   * @param node Pointer to the node to be freed.
   */
  void free_node(BinTree node);
  /**
   * @brief Returns the height of a tree.
   * @param tree Pointer to the root of the tree.
   * @return The height of the tree.
   */
  int tree_height(BinTree tree) const;
  /**
   * @brief Finds the predecessor of a node.
   * @param tree Pointer to the node.
   * @param predecessor Reference to the pointer to the predecessor.
   */
  void find_predecessor(BinTree tree, BinTree &predecessor) const;

private:
  // private stuff...
  BinTree root_;
  ObjectAllocator *oa_;
  bool share_oa_;
  /**
   * @brief Copies a tree.
   * @param dest Reference to the pointer to the destination tree.
   * @param src Pointer to the source tree.
   */
  void copy_tree(BinTree &dest, BinTree src);
  /**
   * @brief Clears a tree.
   * @param tree Pointer to the tree to be cleared.
   */
  void clear_tree(BinTree &tree);
  /**
   * @brief Inserts a new node into a tree.
   * @param tree Reference to the pointer to the tree.
   * @param value The value to be inserted.
   * @return Pointer to the modified tree.
   */
  BinTree insert_node(BinTree &tree, const T &value);
  /**
   * @brief Removes a node from a tree.
   * @param tree Reference to the pointer to the tree.
   * @param value The value to be removed.
   * @return Pointer to the modified tree.
   */
  BinTree remove_node(BinTree &tree, const T &value);
  /**
   * @brief Finds the node at a specified index.
   * @param tree Pointer to the tree.
   * @param index The index of the node to be found.
   * @return Pointer to the node at the specified index.
   */
  const BinTreeNode *find_index(BinTree tree, int index) const;
  /**
   * @brief Finds a node in a tree.
   * @param tree Pointer to the tree.
   * @param value The value to be found.
   * @param compares Reference to the number of comparisons made during the search.
   * @return true if the value is found, false otherwise.
   */
  bool find_node(BinTree tree, const T &value, unsigned &compares) const;
};

#include "BSTree.cpp"

#endif
//---------------------------------------------------------------------------
