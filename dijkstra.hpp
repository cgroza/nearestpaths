#include <handlegraph/handle_graph.hpp>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#ifndef DIJKSTRA_HPP
#define DIJKSTRA_HPP

using namespace std;
using namespace handlegraph;

bool dijkstra(const HandleGraph *g, handle_t start,
              std::function<bool(const handle_t &, size_t)> reached_callback,
              bool traverse_leftward = false, bool prune = false,
              bool cycle_to_start = false);

bool dijkstra(const HandleGraph *g, const std::unordered_set<handle_t> &starts,
              std::function<bool(const handle_t &, size_t)> reached_callback,
              bool traverse_leftward = false, bool prune = false,
              bool cycle_to_start = false);

void for_each_handle_in_shortest_path(const HandleGraph* g, handle_t start, handle_t end, std::function<bool(const handle_t&, size_t)> iteratee);

#endif
