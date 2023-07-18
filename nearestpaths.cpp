#include <handlegraph/algorithms/dijkstra.hpp>
#include <handlegraph/handle_graph.hpp>
#include <bdsg/packed_graph.hpp>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

const size_t max_distance = 100;

int main(int argc, char **argv) {
  std::vector<std::string> paths;

  std::ifstream paths_file("test/chr21_subset.paths");

  std::string p;
  while(!paths_file.eof()) {
    std::getline(paths_file, p);
    paths.push_back(p);
  }

  bdsg::PackedGraph graph;
  graph.deserialize("test/chr21_subset.pack");

  for (auto &s : paths) {
    std::cout << s << std::endl;
    if (!graph.has_path(s)) {
      std::cout << "No path " << s << std::endl;
      continue;
    }
    bdsg::path_handle_t path = graph.get_path_handle(s);
    bdsg::handle_t start_node = graph.get_handle_of_step(graph.path_begin(path));
    handlegraph::algorithms::dijkstra(
        &graph, start_node, [&](const handlegraph::handle_t h, size_t d) {
          std::cout << "Reached " << d << std::endl;
          if(d > max_distance)
            return false;
          return true;
        });
  }

  return 0;
}
