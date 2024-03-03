/*!
@file BSTree.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 3
@date 3/2/2024
@brief This file contains the definition of the BSTree class and its related classes and structures.
*/
#include "BSTree.h"
/**
 * @brief Constructs a new BSTree object.
 * @param oa Pointer to an ObjectAllocator for memory management.
 * @param ShareOA Flag indicating whether to share the ObjectAllocator.
 */
template <typename T>
BSTree<T>::BSTree(ObjectAllocator *oa, bool ShareOA) : root_(nullptr), oa_(oa), share_oa_(ShareOA)
{
    if (oa_ == nullptr)
    {
        OAConfig config(true);
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
    }
}
/**
 * @brief Copy constructor.
 * @param rhs Reference to the BSTree to be copied.
 */
template <typename T>
BSTree<T>::BSTree(const BSTree &rhs) : root_(nullptr), share_oa_(rhs.share_oa_)
{
    if (share_oa_)
    {
        oa_ = rhs.oa_;
    }
    else
    {
        OAConfig config(true);
        oa_ = new ObjectAllocator(sizeof(BinTreeNode), config);
    }
    copy_tree(root_, rhs.root_);
}
/**
 * @brief Destructor.
 */
template <typename T>
BSTree<T>::~BSTree()
{
    clear();
    delete oa_;
    oa_ = nullptr;
}
/**
 * @brief Assignment operator.
 * @param rhs Reference to the BSTree to be assigned.
 * @return Reference to the modified BSTree.
 */
template <typename T>
BSTree<T> &BSTree<T>::operator=(const BSTree &rhs)
{
    if (share_oa_)
    {
        delete oa_;
        oa_ = rhs.oa_;
    }

    clear();
    copy_tree(root_, rhs.root_);
    return *this;
}
/**
 * @brief Subscript operator for r-values.
 * @param index The index of the node to be retrieved.
 * @return Pointer to the node at the specified index.
 */
template <typename T>
const typename BSTree<T>::BinTreeNode *BSTree<T>::operator[](int index) const
{
    return find_index(root_, index);
}
/**
 * @brief Inserts a new node into the BST.
 * @param value The value to be inserted.
 */
template <typename T>
void BSTree<T>::insert(const T &value)
{
    root_ = insert_node(root_, value);
}
/**
 * @brief Removes a node from the BST.
 * @param value The value to be removed.
 */
template <typename T>
void BSTree<T>::remove(const T &value)
{
    root_ = remove_node(root_, value);
}
/**
 * @brief Clears the BST.
 */
template <typename T>
void BSTree<T>::clear()
{
    clear_tree(root_);
    root_ = nullptr;
}
/**
 * @brief Searches for a value in the BST.
 * @param value The value to be searched for.
 * @param compares The number of comparisons made during the search.
 * @return true if the value is found, false otherwise.
 */
template <typename T>
bool BSTree<T>::find(const T &value, unsigned &compares) const
{
    compares = 0;
    return find_node(root_, value, compares);
}
/**
 * @brief Checks if the BST is empty.
 * @return true if the BST is empty, false otherwise.
 */
template <typename T>
bool BSTree<T>::empty() const
{
    return root_ == nullptr;
}
/**
 * @brief Returns the number of nodes in the BST.
 * @return The number of nodes in the BST.
 */
template <typename T>
unsigned int BSTree<T>::size() const
{
    return root_ == nullptr ? 0 : root_->count;
}
/**
 * @brief Returns the height of the BST.
 * @return The height of the BST.
 */
template <typename T>
int BSTree<T>::height() const
{
    return tree_height(root_);
}
/**
 * @brief Returns the root of the BST.
 * @return The root of the BST.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::root() const
{
    return root_;
}
/**
 * @brief Returns the root of the BST.
 * @return The root of the BST.
 */
template <typename T>
typename BSTree<T>::BinTree &BSTree<T>::get_root()
{
    return root_;
}
/**
 * @brief Creates a new node.
 * @param value The value to be stored in the new node.
 * @return Pointer to the new node.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::make_node(const T &value) const
{
    try
    {
        BinTree node = static_cast<BinTree>(oa_->Allocate());
        node = new (node) BinTreeNode(value);
        return node;
    }
    catch (const OAException &e)
    {
        throw BSTException{BSTException::BST_EXCEPTION::E_NO_MEMORY, "No memory"};
    }
}
/**
 * @brief Frees a node.
 * @param node Pointer to the node to be freed.
 */
template <typename T>
void BSTree<T>::free_node(BinTree node)
{
    try
    {
        oa_->Free(node);
    }
    catch (const OAException &e)
    {
        throw BSTException{BSTException::BST_EXCEPTION::E_NO_MEMORY, "No memory"};
    }
}
/**
 * @brief Returns the height of a tree.
 * @param tree Pointer to the root of the tree.
 * @return The height of the tree.
 */
template <typename T>
int BSTree<T>::tree_height(BinTree tree) const
{
    return tree == nullptr ? -1 : std::max(tree_height(tree->left), tree_height(tree->right)) + 1;
}
/**
 * @brief Finds the predecessor of a node.
 * @param tree Pointer to the node.
 * @param predecessor Reference to the pointer to the predecessor.
 */
template <typename T>
void BSTree<T>::find_predecessor(BinTree tree, BinTree &predecessor) const
{
    predecessor = tree;
    while (predecessor->right != nullptr)
    {
        predecessor = predecessor->right;
    }
}
/**
 * @brief Copies a tree.
 * @param dest Reference to the pointer to the destination tree.
 * @param src Pointer to the source tree.
 */
template <typename T>
void BSTree<T>::copy_tree(BinTree &dest, BinTree src)
{
    if (src == nullptr)
        return;
    else
    {
        dest = make_node(src->data);
        dest->count = src->count;
        copy_tree(dest->left, src->left);
        copy_tree(dest->right, src->right);
    }
}
/**
 * @brief Clears a tree.
 * @param tree Pointer to the tree to be cleared.
 */
template <typename T>
void BSTree<T>::clear_tree(BinTree &tree)
{
    if (tree != nullptr)
    {
        clear_tree(tree->left);
        clear_tree(tree->right);
        free_node(tree);
    }
}
/**
 * @brief Inserts a new node into a tree.
 * @param tree Reference to the pointer to the tree.
 * @param value The value to be inserted.
 * @return Pointer to the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::insert_node(BinTree &tree, const T &value)
{
    if (tree == nullptr)
    {
        tree = make_node(value);
        return tree;
    }
    // smaller value
    else if (value < tree->data)
    {
        tree->left = insert_node(tree->left, value);
    }
    // larger value
    else if (value > tree->data)
    {
        tree->right = insert_node(tree->right, value);
    }
    tree->count++;
    return tree;
}
/**
 * @brief Removes a node from a tree.
 * @param tree Reference to the pointer to the tree.
 * @param value The value to be removed.
 * @return Pointer to the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree BSTree<T>::remove_node(BinTree &tree, const T &value)
{
    if (tree == nullptr)
    {
        return tree;
    }
    // smaller value
    else if (value < tree->data)
    {
        tree->left = remove_node(tree->left, value);
    }
    // larger value
    else if (value > tree->data)
    {
        tree->right = remove_node(tree->right, value);
    }
    else
    {
        if (tree->left == nullptr)
        {
            BinTree temp = tree->right;
            free_node(tree);
            return temp;
        }
        else if (tree->right == nullptr)
        {
            BinTree temp = tree->left;
            free_node(tree);
            return temp;
        }
        BinTree temp;
        find_predecessor(tree->left, temp);
        tree->data = temp->data;
        tree->left = remove_node(tree->left, temp->data);
    }
    tree->count--;
    return tree;
}
/**
 * @brief Finds the node at a specified index.
 * @param tree Pointer to the tree.
 * @param index The index of the node to be found.
 * @return Pointer to the node at the specified index.
 */
template <typename T>
const typename BSTree<T>::BinTreeNode *BSTree<T>::find_index(BinTree tree, int index) const
{
    if (index < 0 || index >= static_cast<int>(size()))
    {
        return nullptr;
    }
    BinTree node = tree;
    while (node != nullptr)
    {
        int left_count = (node->left == nullptr) ? 0 : node->left->count;
        if (index < left_count)
        {
            node = node->left;
        }
        else if (index > left_count)
        {
            index -= (left_count + 1);
            node = node->right;
        }
        else
        {
            return node;
        }
    }
    return nullptr;
}
/**
 * @brief Finds a node in a tree.
 * @param tree Pointer to the tree.
 * @param value The value to be found.
 * @param compares Reference to the number of comparisons made during the search.
 * @return true if the value is found, false otherwise.
 */
template <typename T>
bool BSTree<T>::find_node(BinTree tree, const T &value, unsigned &compares) const
{
    compares++;
    if (tree == nullptr)
    {
        return false;
    }
    else if (value < tree->data)
    {
        return find_node(tree->left, value, compares);
    }
    else if (value > tree->data)
    {
        return find_node(tree->right, value, compares);
    }
    else
    {
        return true;
    }
}
