#ifndef TreeDecomposition_h
#define TreeDecomposition_h

#include <vector>
#include <ostream>
#include <sstream>
#include <string>
#include <algorithm>

#include "PermutationStrategy.h"
#include "Graph.h"
#include "Bag.h"

//Class implementing a greedy tree decomposition
class TreeDecomposition{
private:
  Graph& graph;
  PermutationStrategy& strategy;
  std::vector<Bag> bags;
  std::unordered_map<unsigned long, unsigned long> bag_ids;
  unsigned long treewidth = 0;
  std::stringstream stat;
public:
  TreeDecomposition(Graph& gr, PermutationStrategy& str):\
    graph(gr), strategy(str) {}
  
  //Computes decomposition
  void decompose(unsigned long partial_degree=0){
    //building the first permutation
    strategy.init_permutation(graph);
    unsigned long max_bag = 0;
    unsigned long bag_id = 0;
    //looping greedily through the permutation
    //we stop when the maximum bag has the same width as the remaining graph
    //or when we achive the partial decomposition condition
    while(max_bag<graph.number_nodes()&&!strategy.empty()){
      //getting the next node
      unsigned long node = strategy.get_next();
      std::unordered_set<unsigned long> neigh = graph.get_neighbours(node);
      unsigned long width = neigh.size();
      unsigned long prev_max_bag = max_bag;
      unsigned long new_max_bag = std::max(width,max_bag);
      //we stop as soon as we find a bigger bag, in case of partial decomposition
      if(partial_degree!=0&&new_max_bag>partial_degree) break;
      if(new_max_bag!=prev_max_bag){
        stat << prev_max_bag << "\t" << graph.number_nodes() <<"\t"\
          << graph.number_edges() << std::endl;
      }
      max_bag=new_max_bag; 
      //removing the node from the graph
      graph.remove_node(node);
      //filling missing edges between the neighbours and recomputing statistics
      //  for relevant nodes in the graph (the neighbours, most of the time)
      graph.fill(neigh);
      strategy.recompute(neigh, graph);
      //adding the bag
      neigh.insert(node);
      Bag bag = Bag(bag_id, neigh);
      bags.push_back(bag);
      bag_ids[node] = bag_id;
      bag_id++;
    }
    stat << max_bag << "\t" << graph.number_nodes() <<"\t"\
      << graph.number_edges() << std::endl;
    //adding the remaining graph to the decomposition
    Bag bag = Bag(bag_id, graph.get_nodes());
    bags.push_back(bag);
    treewidth = std::max(max_bag,graph.number_nodes()-1);
  }

  //Computes the decomposition tree by choosing, for each bag, the neighbour
  //  that is closest in the permutation
  void build_tree(){
    for(unsigned long i=0;i<bags.size()-1;i++){
      std::vector<unsigned long> nodes = bags[i].get_nodes();
      unsigned long min_bag = bags.size()-1;
      for(unsigned long n=0;n<nodes.size()-1;n++){
        if(bag_ids.find(nodes[n])!=bag_ids.end()&&bag_ids[nodes[n]]!=i)
          min_bag = std::min(bag_ids[nodes[n]], min_bag);
      }
      bags[i].set_parent(min_bag);
      bags[min_bag].add_to_children(i);
    }
  }
  
  //Returns a string cotaining the statistics about the decomposition process
  std::string get_stat(){
    return stat.str();
  }

  friend std::ostream& operator<<(std::ostream& out, TreeDecomposition& dec);
};

std::ostream& operator<<(std::ostream& out, TreeDecomposition& dec){
  out << dec.treewidth << std::endl;
  out << dec.bags.size() << std::endl;
  for(Bag bag:dec.bags) out << bag;
  return out;
}

#endif /* TreeDecomposition_h */
