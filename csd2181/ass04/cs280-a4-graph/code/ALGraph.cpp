/*!
@file ALGraph.cpp
@author Wei Jingsong (jingsong.wei@digipen.edu)
@SIT id 2200646
@course csd2183
@section A
@assignment 4
@date 3/20/2024
@brief This file contains the definition of the ALGraph class and its related classes and structures.
*/
#include "ALGraph.h"
#include <queue>
#include <algorithm>
/**
 * @brief Constructor.
 * @param size The size of the graph.
 */
ALGraph::ALGraph(unsigned size) : size_(size), adj_list_(size)
{
}
/**
 * @brief Destructor.
 */
ALGraph::~ALGraph(void)
{
}
/**
 * @brief Adds a directed edge to the graph.
 * @param source The source node.
 * @param destination The destination node.
 * @param weight The weight of the edge.
 */
void ALGraph::AddDEdge(unsigned source, unsigned destination, unsigned weight)
{
    --source;
    AdjInfo adj_info{destination, weight};
    adj_list_[source].emplace_back(adj_info);
    // Sort the adjacency list for the source node.
    std::sort(adj_list_[source].begin(), adj_list_[source].end(), [](const AdjInfo &lhs, const AdjInfo &rhs) -> bool
              { if(lhs.cost < rhs.cost)
                {
                  return true;
                }
                else if(lhs.cost == rhs.cost && lhs.des < rhs.des)
                {
                  return true;
                }
                
                  return false; });
}
/**
 * @brief Adds an undirected edge to the graph.
 * @param node1 The first node.
 * @param node2 The second node.
 * @param weight The weight of the edge.
 */
void ALGraph::AddUEdge(unsigned node1, unsigned node2, unsigned weight)
{
    AddDEdge(node1, node2, weight);
    AddDEdge(node2, node1, weight);
}
/**
 * @brief Performs Dijkstra's algorithm on the graph.
 * @param start_node The starting node for Dijkstra's algorithm.
 * @return A vector containing Dijkstra's algorithm information for each node.
 */
std::vector<DijkstraInfo> ALGraph::Dijkstra(unsigned start_node) const
{
    //cost of infinity
    std::vector<DijkstraInfo> dijkstra_info(size_);
    std::priority_queue<AdjInfo, std::vector<AdjInfo>, std::greater<AdjInfo>> pq;
    AdjInfo adj_info{start_node - 1, 0};
    pq.emplace(adj_info);

    for (unsigned i = 0; i < size_; ++i)
    {
        dijkstra_info[i].cost = (i == start_node - 1) ? 0 : INFINITY_;
    }
    //visited nodes
    std::vector<bool> visited(size_, false);
    while (!pq.empty())
    {
        
        AdjInfo current = pq.top();
        pq.pop();
        if (!visited[current.des])
        {
            visited[current.des] = true;
        }
        else
        {
            continue;
        }
        //if the cost is less than the current cost
        //update the cost and path
        for (auto it = adj_list_[current.des].begin(); it != adj_list_[current.des].end(); ++it)
        {
            if (dijkstra_info[it->des - 1].cost > (it->cost + (dijkstra_info[current.des].cost == INFINITY_ ? 0 : dijkstra_info[current.des].cost)))
            {
                dijkstra_info[it->des - 1].cost = it->cost + (dijkstra_info[current.des].cost == INFINITY_ ? 0 : dijkstra_info[current.des].cost);
                dijkstra_info[it->des - 1].path = dijkstra_info[current.des].path;
                dijkstra_info[it->des - 1].path.emplace_back(it->des);
                AdjInfo next{it->des - 1, dijkstra_info[it->des - 1].cost};
                pq.emplace(next);
            }
        }
    }
    //insert the start node to the path
    for (auto &d : dijkstra_info)
    {
        if (d.cost == INFINITY_)
        {
            continue;
        }
        d.path.insert(d.path.begin(), start_node);
    }
    return dijkstra_info;
}
/**
 * @brief Gets the adjacency list representation of the graph.
 * @return The adjacency list representation.
 */
ALIST ALGraph::GetAList(void) const
{
    ALIST alist{size_};
    for (unsigned i = 0; i < size_; ++i)
    {
        for (const AdjInfo &adj_info : adj_list_[i])
        {
            AdjacencyInfo adjacency_info{adj_info.des, adj_info.cost};
            alist[i].emplace_back(adjacency_info);
        }
    }
    return alist;
}
/**
 * @brief Default constructor.
 */
ALGraph::AdjInfo::AdjInfo() : des(0), cost(0)
{
}
/**
 * @brief Parameterized constructor.
 * @param dst Destination node.
 * @param weight Weight of the edge.
 */
ALGraph::AdjInfo::AdjInfo(unsigned dst, unsigned weight) : des(dst), cost(weight)
{
}
/**
 * @brief Less than operator for comparing AdjInfo objects.
 * @param rhs The right-hand side object.
 * @return True if this object is less than the right-hand side object, false otherwise.
 */
bool ALGraph::AdjInfo::operator<(const AdjInfo &rhs) const
{
    if (cost < rhs.cost)
    {
        return true;
    }
    else if (cost == rhs.cost && des < rhs.des)
    {
        return true;
    }
    else
    {
        return false;
    }
}
/**
 * @brief Greater than operator for comparing AdjInfo objects.
 * @param rhs The right-hand side object.
 * @return True if this object is greater than the right-hand side object, false otherwise.
 */
bool ALGraph::AdjInfo::operator>(const AdjInfo &rhs) const
{
    if (cost > rhs.cost)
    {
        return true;
    }
    else if (cost == rhs.cost && des > rhs.des)
    {
        return true;
    }
    else
    {
        return false;
    }
}