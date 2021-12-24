#include "find_path.h"

std::vector<std::pair<char, char>> find_min_path(Weighted &graph, char start_point, char finish_point)
{
    auto vertices = graph.GetVertices();
    auto edges = graph.GetEdges();
    auto weights = graph.GetWeights();

    std::vector<std::pair<char, char>> path;
    std::vector<char> visited;
    std::map<char, int> length;

    for (auto v: vertices) {
        if (v != start_point) {
            length[v] = INF;
        } else {
            length[v] = 0;
        }
    }

    visited.push_back(start_point);
    char cur = start_point;
    auto all = vertices.size();
    int min_path = INF;
    while (visited.size() < all) {
        min_path = INF;
        char possible = cur;
        auto len = edges.size();
        for (int i = 0; i < len; i++) {
            if (edges[i].first == cur) {
                auto p = std::find(visited.begin(), visited.end(), edges[i].second);
                if (p == visited.end()) { //if we didn't visit this point
                    if (length[cur] + weights[i] < length[edges[i].second]) {
                        length[edges[i].second] = length[cur] + weights[i];
                    }
                    if (weights[i] < min_path) {
                        possible = edges[i].second;
                        min_path = weights[i];
                    }
                }
            }
            if (edges[i].second == cur) {
                auto p = std::find(visited.begin(), visited.end(), edges[i].first);
                if (p == visited.end()) {
                    if (length[cur] + weights[i] < length[edges[i].first]) {
                        length[edges[i].first] = length[cur] + weights[i];
                    }
                    if (weights[i] < min_path) {
                        possible = edges[i].first;
                        min_path = weights[i];
                    } 
                }
            }
        }
        if (possible == cur && visited.size() != all) {
            throw std::logic_error("Graph isn't connected");
        }
        visited.push_back(possible);
        cur = possible;
    }

    auto cur_length = length[finish_point];
    char next;
    cur = finish_point;
    while (cur != start_point) {
        auto len = edges.size();
        for (int i = 0; i < len; i++) {
            if (edges[i].first == cur) {
                if (length[cur] - weights[i] == length[edges[i].second]) {
                    next = edges[i].second;
                    break;
                }
            }
            if (edges[i].second == cur) {
                if (length[cur] - weights[i] == length[edges[i].first]) {
                    next = edges[i].first;
                    break;
                }
            }
        }
        path.push_back(std::make_pair(next, cur));
        cur = next;
    }
    std::reverse(path.begin(), path.end());
    return path;
}