

/*
 * David Gleich
 * Copyright, Stanford University, 2006-2007
 */
 

/**
 * @file spanning_trees.cc
 * Implement the BGL spanning tree wrappers.
 */ 

/*
 * 20 April 2006
 * Initial version
 * 
 * 10 November 2006: Fixed bug with incorrect number of edges returned,
 * when the input graph has multiple components.
 * The nedges output parameter is now set correctly for all algorithms.
 * Although, it depends on a somewhat dubious "hack" to detect
 * unused portions of the output iterator.
 * 
 * 9 July 2007
 * Switched to simple_csr_matrix graph type
 * Switched to kruskal mst from boost mod to fix bug with output iterator
 */

#include "include/matlab_bgl.h"

#include <yasmic/simple_csr_matrix_as_graph.hpp>
#include <yasmic/iterator_utility.hpp>

#include <yasmic/boost_mod/kruskal_min_spanning_tree.hpp>
#include <boost/graph/prim_minimum_spanning_tree.hpp>

#include <vector>

/*template <class Graph, class Edge>
class spanning_tree_insert_iterator
    : public boost::iterator_facade<
        spanning_tree_insert_iterator<Edge>
      , Edge
      , boost::forward_traversal_tag
    >
{
public:
    int* i; int* j; double* val;
    Graph& g;

    spanning_tree_insert_iterator(int* _i, int* _j, double* _val)
        : i(_i), j(_j), val(_val)
    {}

private:
    friend class boost::iterator_core_access;

    void increment()
    {
        i++;
        j++;
        val++;
    }

    bool equal(spanning_tree_insert_iterator const& other)
    {
        return (i == other.i && j == other.j && val == other.val);
    }

    Edge


};*/

template <class Graph, class EdgeWeightPropMap, class Iterator>
int copy_to_ijval(Graph& g, EdgeWeightPropMap ewpm, Iterator oi, 
                   Iterator oi_end, mbglIndex* i, mbglIndex* j, double* val)
{
    using namespace boost;

    int ei = 0;
    for (; oi != oi_end; ++oi)
    {
        typename graph_traits<Graph>::edge_descriptor e =
            *oi;

        // unnecessary as of 9 July 2007
        // source = target indicates that this edge is not
        // in the graph.
        // if (source(e,g) == target(e,g))
        // { 
        //     continue;
        // }

        i[ei] = source(e,g);
        j[ei] = target(e,g);
        val[ei] = ewpm[e];
        ++ei;
    }

    return (ei);
}

#include <iostream>

int kruskal_mst(
    mbglIndex nverts, mbglIndex *ja, mbglIndex *ia, double *weight, /* connectivity params */
    mbglIndex* i, mbglIndex* j, double* val /* tree output */,
    mbglIndex* nedges)
{
    using namespace yasmic;
    using namespace boost;

    // create the graph g
    typedef simple_csr_matrix<mbglIndex,double> crs_weighted_graph;
    crs_weighted_graph g(nverts, nverts, ia[nverts], ia, ja, weight); 

    //
    // warning, this code assumes that the default constructor for 
    // an edges has source == target, otherwise we will detect
    // incorrect edges in the next step.  
    //
    std::vector<graph_traits<crs_weighted_graph>::edge_descriptor> 
        oi(nverts-1);

    std::vector<graph_traits<crs_weighted_graph>::edge_descriptor>::iterator
        oi_end = kruskal_minimum_spanning_tree(g,oi.begin());


    //*nedges = nverts-1;
    //*nedges = (int)(oi_end - oi.begin());
    
    // warning, this code assumes that the default constructor for 
    // an edges has source == target, otherwise we will detect
    // incorrect edges in the next step.  
    *nedges = copy_to_ijval(g,get(edge_weight,g), 
        oi.begin(), oi_end, i, j, val);

    return (0);
}

int prim_mst(
    mbglIndex nverts, mbglIndex *ja, mbglIndex *ia, double *weight, /* connectivity params */
    mbglIndex* i, mbglIndex* j, double* val, mbglIndex *nedges /* tree output */)
{
    using namespace yasmic;
    using namespace boost;

    typedef simple_csr_matrix<mbglIndex,double> crs_weighted_graph;
    crs_weighted_graph g(nverts, nverts, ia[nverts], ia, ja, weight); 

    std::vector<mbglIndex> pred(nverts);

    prim_minimum_spanning_tree(g,
        make_iterator_property_map(
           pred.begin(), get(vertex_index,g)));

    mbglIndex edge_num = 0;
    for (mbglIndex pi = 0; pi < nverts; pi++)
    {
        if (pred[pi] == pi) 
        {
            // this edge isn't present
        }
        else
        {
            i[edge_num] = pi;
            j[edge_num] = pred[pi];
            val[edge_num] = 0.0;
            
            for (mbglIndex k=ia[pred[pi]]; k < ia[pred[pi]+1]; k++)
            {
                if (ja[k] == pi) { val[edge_num] = weight[k]; break; }
            }

            edge_num++;
        }
    }

    *nedges = edge_num;

    return (0);
}

