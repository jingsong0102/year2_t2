/*!
@file AVLTree.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 3
@date 3/2/2024
@brief This file contains the definition of the AVLTree class and its related classes and structures.
*/
#include "AVLTree.h"
/**
 * @brief Constructs a new AVLTree object.
 * @param oa Pointer to an ObjectAllocator for memory management.
 * @param ShareOA Flag indicating whether to share the ObjectAllocator.
 */
template <typename T>
AVLTree<T>::AVLTree(ObjectAllocator *oa, bool ShareOA) : BSTree<T>(oa, ShareOA)
{
}
/**
 * @brief Inserts a value into the AVL tree.
 * @param value Value to be inserted.
 */
template <typename T>
void AVLTree<T>::insert(const T &value)
{
    this->get_root() = insert_node(this->get_root(), value);
}
/**
 * @brief Removes a value from the AVL tree.
 * @param value Value to be removed.
 */
template <typename T>
void AVLTree<T>::remove(const T &value)
{
    this->get_root() = remove_node(this->get_root(), value);
}
/**
 * @brief Returns true if efficiency implemented.
 * @return True if efficiency implemented.
 */
template <typename T>
bool AVLTree<T>::ImplementedBalanceFactor(void)
{
    return true;
}
/**
 * @brief Inserts a node into the AVL tree.
 * @param tree Reference to the root node of the tree.
 * @param value Value to be inserted.
 * @return Pointer to the root node of the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree AVLTree<T>::insert_node(typename BSTree<T>::BinTree &tree, const T &value)
{
    if (tree == nullptr)
    {
        return this->make_node(value);
    }
    if (value < tree->data)
    {
        tree->count++;
        tree->left = insert_node(tree->left, value);
    }
    else if (value > tree->data)
    {
        tree->count++;
        tree->right = insert_node(tree->right, value);
    }
    else
        return tree;
    tree->balance_factor = balance_factor(tree);
    // right rotation
    if (tree->balance_factor > 1)
    {
        if (value < tree->left->data)
        {
            return rotate_right(tree);
        }
        else
        {
            tree->left = rotate_left(tree->left);
            return rotate_right(tree);
        }
    }
    // left rotation
    if (tree->balance_factor < -1)
    {
        if (value > tree->right->data)
        {
            return rotate_left(tree);
        }
        else
        {
            tree->right = rotate_right(tree->right);
            return rotate_left(tree);
        }
    }
    return tree;
}
/**
 * @brief Removes a node from the AVL tree.
 * @param tree Reference to the root node of the tree.
 * @param value Value to be removed.
 * @return Pointer to the root node of the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree AVLTree<T>::remove_node(typename BSTree<T>::BinTree &tree, const T &value)
{
    if (tree == nullptr)
    {
        return tree;
    }
    if (value < tree->data)
    {
        tree->count--;
        tree->left = remove_node(tree->left, value);
    }
    else if (value > tree->data)
    {
        tree->count--;
        tree->right = remove_node(tree->right, value);
    }
    else
    {
        if (tree->left == nullptr || tree->right == nullptr)
        {
            typename AVLTree<T>::BinTree temp = tree->left ? tree->left : tree->right;
            if (temp == nullptr)
            {
                temp = tree;
                tree = nullptr;
            }
            else
                *tree = *temp;
            this->free_node(temp);
        }
        else
        {
            typename AVLTree<T>::BinTree temp = tree->right;
            while (temp->left != nullptr)
            {
                temp = temp->left;
            }
            tree->data = temp->data;
            tree->count--;
            tree->right = remove_node(tree->right, temp->data);
        }
    }
    if (tree == nullptr)
    {
        return tree;
    }
    tree->balance_factor = balance_factor(tree);
    // right rotation
    if (tree->balance_factor > 1)
    {
        if (balance_factor(tree->left) >= 0)
        {
            return rotate_right(tree);
        }
        else
        {
            tree->left = rotate_left(tree->left);
            return rotate_right(tree);
        }
    }
    // left rotation
    if (tree->balance_factor < -1)
    {
        if (balance_factor(tree->right) <= 0)
        {
            return rotate_left(tree);
        }
        else
        {
            tree->right = rotate_right(tree->right);
            return rotate_left(tree);
        }
    }
    return tree;
}
/**
 * @brief Rotates the tree to the left.
 * @param tree Reference to the root node of the tree.
 * @return Pointer to the root node of the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree AVLTree<T>::rotate_left(typename BSTree<T>::BinTree &tree)
{
    typename BSTree<T>::BinTree temp = tree->right;
    tree->right = temp->left;
    temp->left = tree;

    tree->count = 1 + (tree->left ? tree->left->count : 0) + (tree->right ? tree->right->count : 0);
    temp->count = 1 + (temp->left ? temp->left->count : 0) + (temp->right ? temp->right->count : 0);
    return temp;
}
/**
 * @brief Rotates the tree to the right.
 * @param tree Reference to the root node of the tree.
 * @return Pointer to the root node of the modified tree.
 */
template <typename T>
typename BSTree<T>::BinTree AVLTree<T>::rotate_right(typename BSTree<T>::BinTree &tree)
{
    typename BSTree<T>::BinTree temp = tree->left;
    tree->left = temp->right;
    temp->right = tree;

    tree->count = 1 + (tree->left ? tree->left->count : 0) + (tree->right ? tree->right->count : 0);
    temp->count = 1 + (temp->left ? temp->left->count : 0) + (temp->right ? temp->right->count : 0);
    return temp;
}
/**
 * @brief Calculates the balance factor of a tree.
 * @param tree Reference to the root node of the tree.
 * @return The balance factor of the tree.
 */
template <typename T>
int AVLTree<T>::balance_factor(typename BSTree<T>::BinTree tree)
{
    if (tree == nullptr)
    {
        return 0;
    }
    return this->tree_height(tree->left) - this->tree_height(tree->right);
}