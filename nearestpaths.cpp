// #include <handlegraph/algorithms/dijkstra.hpp>
#include <bdsg/packed_graph.hpp>
#include <fstream>
#include <handlegraph/handle_graph.hpp>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>
#include <unistd.h>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
#include "dijkstra.hpp"

// ambiguous definition to order of include files

using namespace handlegraph;

namespace opt {
std::string packed_graph_path;
std::string graph_paths_path;
size_t max_distance = 1000;
size_t num_threads = 1;
}


void usage() {
  std::cerr << "Usage nearestpaths:" << std::endl;
  std::cerr << "-i : path to PackedGraph." << std::endl;
  std::cerr << "-p : list of graph paths to consider, one per line." << std::endl;
  std::cerr << "-d : radius of the search in bp (1000)." << std::endl;
  std::cerr << "-t : number of threads to use (1)." << std::endl;
}

int main(int argc, char **argv) {

  opterr = 0;
  int c;
  while ((c = getopt(argc, argv, "i:p:t:d:")) != -1)
    switch (c) {
    case 't':
        try {
            opt::num_threads = std::stoi(optarg);
        }
        catch(std::invalid_argument){
            std::cerr << "Number of threads must be positive integer!" << std::endl;
            usage();
            return -1;
        }
        break;
    case 'd':
        try {
            opt::max_distance = std::stoi(optarg);
        }
        catch(std::invalid_argument) {
            std::cerr << "Maximum search distance must be positive integer!" << std::endl;
            usage();
            return -1;
        }
      break;
    case 'i':
      opt::packed_graph_path = optarg;
      break;
    case 'p':
      opt::graph_paths_path = optarg;
      break;
    }

  if(opt::packed_graph_path.empty()) {
    std::cerr << "Path to a PackedGraph required." << std::endl;
    usage();
    return -1;
  }
  if (opt::graph_paths_path.empty()) {
    std::cerr << "Path to a a list of paths in the graph required." << std::endl;
    usage();
    return -1;
  }

  std::unordered_set<std::string> paths;
  std::unordered_map<std::string, std::vector<pair<std::string, size_t>>>
      neighbor_paths;

  std::ifstream paths_file(opt::graph_paths_path);
  if(!paths_file.is_open()) {
    std::cerr << "Could not open file " << opt::graph_paths_path << std::endl;
    usage();
    return -1;
  }

  std::string p;
  while (!paths_file.eof()) {
    std::getline(paths_file, p);
    paths.insert(p);
    neighbor_paths.emplace(p, std::vector<pair<std::string, size_t>>());
  }

  bdsg::PackedGraph graph;
  graph.deserialize(opt::packed_graph_path);

  for (auto &p : paths) {
    if (!graph.has_path(p)) {
      std::cerr << "No path " << p << std::endl;
      continue;
    }
    // current path, looking for other nearby paths near it
    bdsg::path_handle_t path = graph.get_path_handle(p);
    // we only need one node for this path as a start site (maybe should pick middle node?)
    bdsg::handle_t start_node = graph.get_handle_of_step(graph.path_begin(path));
    // to not double count
    std::unordered_set<std::string> seen_paths;

    // callback function that keeps track of encountered paths X distance away
    std::function<bool(const handle_t h, size_t d)> find_nearest_paths =
        [&](const handle_t h, size_t d) {
          if (d > opt::max_distance)
            return false;

          std::vector<step_handle_t> steps = graph.steps_of_handle(h);
          for (step_handle_t s : steps) {
            path_handle_t sp = graph.get_path_handle_of_step(s);
            std::string path_name = graph.get_path_name(sp);
            // path must be a peak, not a reference
            if (!paths.contains(path_name))
              continue;
            // if it is the same path
            if (path_name == p || seen_paths.contains(path_name)) {
              continue;
            }
            neighbor_paths[p].push_back(make_pair(path_name, d));
            seen_paths.insert(path_name);
          }
          return true;
        };
    // search rightward
    dijkstra(&graph, start_node, find_nearest_paths);

    // search graph leftward
    dijkstra(&graph, start_node, find_nearest_paths, true);
  }

  // output data
  for (auto &it : neighbor_paths) {
    std::cout << it.first << "\t";
    for (auto &jt : it.second) {
      std::cout << jt.first << "@" << jt.second << ",";
    }
    std::cout << std::endl;
  }

  return 0;
}
