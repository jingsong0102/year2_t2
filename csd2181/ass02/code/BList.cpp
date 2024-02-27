/*!
@file BList.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignent 2
@date 2/17/2024
@brief This file contains the definition of the BList class and its related classes and structures.
*/
/**
 * @brief Returns the size of each node in the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @return size_t Size of each node.
 */
template <typename T, unsigned Size>
size_t BList<T, Size>::nodesize(void)
{
  return sizeof(BNode);
}
/**
 * @brief Returns a constant pointer to the head node of the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @return const typename BList<T, Size>::BNode* Constant pointer to the head node.
 */
template <typename T, unsigned Size>
const typename BList<T, Size>::BNode *BList<T, Size>::GetHead() const
{
  return head_;
}
/**
 * @brief Default constructor for BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 */
template <typename T, unsigned Size>
BList<T, Size>::BList()
{
  try
  {
    head_ = new BNode;
    tail_ = head_;
  }
  catch (const std::exception &e)
  {
    throw(BListException(BListException::E_NO_MEMORY, e.what()));
  }
  stats_ = BListStats(sizeof(BNode), 0, Size, 0);
  stats_.NodeCount++;
}
/**
 * @brief Copy constructor for BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param rhs Reference to the BList object to be copied.
 */
template <typename T, unsigned Size>
BList<T, Size>::BList(const BList &rhs)
{
  BNode *rhs_ = rhs.head_;
  BNode *lhs{};
  BNode *prev{};

  while (rhs_ != nullptr)
  {
    try
    {
      lhs = new BNode();
    }
    catch (const std::exception &e)
    {
      throw(BListException(BListException::E_NO_MEMORY, e.what()));
    }
    lhs->prev = prev;
    if (prev != nullptr)
    {
      prev->next = lhs;
    }
    else
    {
      head_ = lhs;
    }
    lhs->count = rhs_->count;
    for (unsigned int i = 0; i < lhs->count; i++)
    {
      lhs->values[i] = rhs_->values[i];
    }
    prev = lhs;
    rhs_ = rhs_->next;
  }
  // update accounting info
  stats_.ArraySize = rhs.GetStats().ArraySize;
  stats_.ItemCount = rhs.GetStats().ItemCount;
  stats_.NodeCount = rhs.GetStats().NodeCount;
  stats_.NodeSize = rhs.GetStats().NodeSize;
  tail_ = lhs;
}
/**
 * @brief Destructor for BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 */
template <typename T, unsigned Size>
BList<T, Size>::~BList()
{
  clear();
}
/**
 * @brief Assignment operator for BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param rhs Reference to the BList object to be assigned.
 * @return BList<T, Size>& Reference to the current BList object after assignment.
 */
template <typename T, unsigned Size>
BList<T, Size> &BList<T, Size>::operator=(const BList &rhs)
{
  if (this != &rhs)
  {
    clear();
    BNode *lhs{};
    BNode *rhs_ = rhs.head_;
    BNode *prev{};

    while (rhs_ != nullptr)
    {
      try
      {
        lhs = new BNode();
      }
      catch (const std::exception &e)
      {
        throw(BListException(BListException::E_NO_MEMORY, e.what()));
      }
      lhs->prev = prev;
      if (prev != nullptr)
      {
        prev->next = lhs;
      }
      else
      {
        head_ = lhs;
      }
      lhs->count = rhs_->count;
      for (unsigned int i = 0; i < lhs->count; i++)
      {
        lhs->values[i] = rhs_->values[i];
      }
      prev = lhs;
      rhs_ = rhs_->next;
    }
    // update accounting info
    stats_.ArraySize = rhs.GetStats().ArraySize;
    stats_.ItemCount = rhs.GetStats().ItemCount;
    stats_.NodeCount = rhs.GetStats().NodeCount;
    stats_.NodeSize = rhs.GetStats().NodeSize;
    tail_ = lhs;
  }
  return *this;
}
/**
 * @brief Adds an element to the end of the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param value The value to be added to the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::push_back(const T &value)
{
  BNode *prev{};
  BNode *node = head_;
  bool found{false};
  while (!found)
  {
    if (node->count < Size)
    {
      node->values[node->count] = value;
      node->count++;
      stats_.ItemCount++;
      found = true;
    }
    else if (node->next == nullptr && node->count == Size)
    {
      prev = node;
      node = node->prev;
      BNode *newNode{};
      try
      {
        newNode = new BNode();
      }
      catch (const std::exception &e)
      {
        throw(BListException(BListException::E_NO_MEMORY, e.what()));
      }
      node = newNode;
      node->prev = prev;
      prev->next = node;
      tail_ = node;
      stats_.NodeCount++;
    }
    else if (node->count < Size)
    {
      insertNode(node, value, node->count);
    }
    else
    {
      node = node->next;
    }
  }
}
/**
 * @brief Adds an element to the beginning of the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param value The value to be added to the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::push_front(const T &value)
{
  BNode *prev{};
  BNode *node = head_;
  bool found{false};
  while (!found)
  {
    if (node->count < Size)
    {
      for (int i = node->count - 1; i >= 0; i--)
      {
        node->values[i + 1] = node->values[i];
      }
      node->values[0] = value;
      node->count++;
      stats_.ItemCount++;
      found = true;
    }
    else
    {
      BNode *newNode{};
      try
      {
        newNode = new BNode();
      }
      catch (const std::exception &e)
      {
        throw(BListException(BListException::E_NO_MEMORY, e.what()));
      }
      prev = node;
      node = newNode;
      prev->prev = newNode;
      newNode->next = prev;
      head_ = newNode;
      stats_.NodeCount++;
    }
  }
}
/**
 * @brief Inserts an element into the BList while maintaining sorted order.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param value The value to be inserted into the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insert(const T &value)
{
  BNode *node = head_;
  while (node != nullptr)
  {
    for (unsigned int i = 0; i < Size; i++)
    {
      if (stats_.ItemCount == 0)
      {
        push_front(value);
        return;
      }
      if (value < node->values[i] && node->count == Size)
      {
        if (node->prev != nullptr)
        {
          if (node->prev->count == Size && i == 0 && Size != 1)
          {
            splitNode(node->prev);
            node = node->prev;
            continue;
          }
          else
          {
            splitNode(node);
          }
        }
        else
          splitNode(node);
      }
      if (value < node->values[i] && i < node->count)
      {
        if (Size == 1)
        {
          insertNode(node->next, node->values[0], 0);
          node->values[0] = static_cast<T>(0);
          node->count--;
          stats_.ItemCount--;
          insertNode(node, value, i);
          return;
        }
        else
        {
          insertNode(node, value, i);
          return;
        }
      }

      if (node->next == nullptr && i == node->count - 1)
      {
        if (node->count >= Size)
        {
          splitNode(node);
          if (Size > 1)
            insertNode(node->next, value, node->next->count);
          else if (Size == 1)
            insertNode(node->next, value, 0);
          return;
        }
        else
        {
          insertNode(node, value, node->count);
          return;
        }
      }
      if (i >= node->count && value < node->next->values[0])
      {
        insertNode(node, value, i);
        return;
      }
    }
    node = node->next;
  }
}
/**
 * @brief Removes an element from the BList at the specified index.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param index The index of the element to be removed.
 */
template <typename T, unsigned Size>
void BList<T, Size>::remove(int index)
{
  if (index < 0 || index > static_cast<int>(stats_.ItemCount + 1))
  {
    throw(BListException(BListException::E_BAD_INDEX, "Index out of bounds"));
  }

  unsigned int node_pos = static_cast<unsigned int>(index + 1);
  BNode *node = head_;
  while (node_pos > node->count)
  {
    node_pos -= node->count;
    node = node->next;
  }
  for (unsigned int i = node_pos - 1; i < node->count - 1; i++)
  {
    node->values[i] = node->values[i + 1];
  }
  stats_.ItemCount--;
  if (--node->count == 0)
  {
    if (node->prev)
      node->prev->next = node->next;
    else
      head_ = node->next;
    if (node->next)
      node->next->prev = node->prev;
    else
      tail_ = node->prev;
    delete node;
    stats_.NodeCount--;
  }
}
/**
 * @brief Removes all elements from the BList that match the specified value.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param value The value to be removed from the BList.
 */
template <typename T, unsigned Size>
void BList<T, Size>::remove_by_value(const T &value)
{
  BNode *node = head_;
  while (node != nullptr)
  {
    for (unsigned int i = 0; i < node->count; i++)
    {
      if (node->values[i] == value)
      {
        for (unsigned int j = i; j < node->count - 1; j++)
        {
          node->values[j] = node->values[j + 1];
        }
        stats_.ItemCount--;
        BNode *temp = node->prev;

        if (--node->count == 0)
        {
          if (node->prev)
            node->prev->next = node->next;
          else
            head_ = node->next;
          if (node->next)
            node->next->prev = node->prev;
          else
            tail_ = node->prev;
          delete node;
          stats_.NodeCount--;
        }
        node = temp;
        break;
      }
    }
    if (node != nullptr)
      node = node->next;
    else
      node = head_;
  }
}
/**
 * @brief Returns the index of the first element in the BList that matches the specified value.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param value The value to be found in the BList.
 * @return int The index of the first element that matches the specified value.
 */
template <typename T, unsigned Size>
int BList<T, Size>::find(const T &value) const
{
  BNode *node = head_;
  int index = 0;
  while (node != nullptr)
  {
    for (unsigned int i = 0; i < node->count; i++)
    {
      if (node->values[i] == value)
      {
        return index;
      }
      index++;
    }
    node = node->next;
  }
  return -1;
}
/**
 * @brief Returns a reference to the element at the specified index in the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param index The index of the element to be returned.
 * @return T& Reference to the element at the specified index.
 */
template <typename T, unsigned Size>
T &BList<T, Size>::operator[](int index)
{
  if (index > stats_.ItemCount)
  {
    throw(BListException(BListException::E_BAD_INDEX, "Index out of bounds"));
  }
  unsigned int node_pos = static_cast<unsigned int>(index) + 1;
  BNode *node = head_;

  while (node_pos > node->count)
  {
    node_pos -= node->count;
    node = node->next;
  }
  return node->values[node_pos - 1];
}
/**
 * @brief Returns a constant reference to the element at the specified index in the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param index The index of the element to be returned.
 * @return const T& Constant reference to the element at the specified index.
 */
template <typename T, unsigned Size>
const T &BList<T, Size>::operator[](int index) const
{
  if (index > stats_.ItemCount)
  {
    throw(BListException(BListException::E_BAD_INDEX, "Index out of bounds"));
  }
  unsigned int node_pos = static_cast<unsigned int>(index) + 1;
  BNode *node = head_;

  while (node_pos > node->count)
  {
    node_pos -= node->count;
    node = node->next;
  }
  return node->values[node_pos - 1];
}
/**
 * @brief Returns the number of elements in the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @return size_t Number of elements in the BList.
 */
template <typename T, unsigned Size>
size_t BList<T, Size>::size() const
{
  return stats_.ItemCount;
}
/**
 * @brief Removes all elements from the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 */
template <typename T, unsigned Size>
void BList<T, Size>::clear()
{
  while (head_ != nullptr)
  {
    BNode *node = head_;
    head_ = head_->next;
    delete node;
  }
  head_ = nullptr;
  tail_ = nullptr;
  stats_.ItemCount = 0;
  stats_.NodeCount = 0;
}
/**
 * @brief Returns the statistics of the BList.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @return BListStats Statistics of the BList.
 */
template <typename T, unsigned Size>
BListStats BList<T, Size>::GetStats() const
{
  return stats_;
}
/**
 * @brief Inserts an element into the specified node at the specified index.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param node Pointer to the node to insert the element into.
 * @param value The value to be inserted into the node.
 * @param index The index to insert the value at.
 */
template <typename T, unsigned Size>
void BList<T, Size>::insertNode(BNode *node, const T &value, int index)
{
  if (node->count < Size)
  {
    for (int i = static_cast<int>(node->count - 1); i >= index; i--)
    {
      node->values[i + 1] = node->values[i];
    }
    node->values[index] = value;
    node->count++;
    stats_.ItemCount++;
    return;
  }
}
/**
 * @brief Copies the elements from one array to another.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param src Pointer to the source array.
 * @param dest Pointer to the destination array.
 * @param count Number of elements to copy.
 */
template <typename T, unsigned Size>
void BList<T, Size>::copyList(T *src, T *dest, const unsigned int count)
{
  for (unsigned int i = 0; i < count; i++)
  {
    dest[i] = src[i];
  }
}
/**
 * @brief Splits the specified node into two nodes.
 * 
 * @tparam T Type of elements stored in the BList.
 * @tparam Size Maximum number of elements in each node.
 * @param node Pointer to the node to be split.
 */
template <typename T, unsigned Size>
void BList<T, Size>::splitNode(BNode *node)
{
  if (node != nullptr)
  {
    T lhs[Size];
    T rhs[Size];
    unsigned int mid = static_cast<unsigned int>(node->count / 2 - 0.5);
    unsigned int lhsIndex{};
    unsigned int rhsIndex{};
    for (unsigned int i = 0; i < node->count; i++)
    {
      if (i <= mid)
      {
        lhs[lhsIndex++] = node->values[i];
      }
      else if (i > mid)
      {
        rhs[rhsIndex++] = node->values[i];
      }
    }

    BNode *newNode = new BNode();

    newNode->next = node->next;
    newNode->prev = node;
    if (node->next != nullptr)
    {
      node->next->prev = newNode;
    }
    node->next = newNode;

    node->count = lhsIndex;
    newNode->count = rhsIndex;

    copyList(lhs, node->values, Size);
    copyList(rhs, newNode->values, Size);
    stats_.NodeCount++;
  }
}