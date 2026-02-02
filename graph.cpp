#include <vector>
#include <random>
#include <set>
#include <algorithm>
#include <emscripten/bind.h>

using namespace std;
using namespace emscripten;

class Graph {
public:
    int V;
    vector<set<int>> adj; // 중복 간선 방지용 set

    // 생성자: 노드만 생성
    Graph(int V) : V(V) {
        adj.resize(V + 1);
    }

    // 수동 연결
    void connection(int u, int v) {
        if (u < 1 || v < 1 || u > V || v > V || u == v) return;
        adj[u].insert(v);
        adj[v].insert(u);
    }

    // 랜덤 엣지 추가 (기존 그래프에 E개 추가)
    void addRandomEdges(int count) {
        random_device rd;
        mt19937 gen(rd());
        uniform_int_distribution<int> dis(1, V);

        int added = 0;
        int attempts = 0;
        // 무한루프 방지: 시도가 너무 많으면 중단 (Full graph일 경우 등)
        while (added < count && attempts < count * 10) {
            int u = dis(gen);
            int v = dis(gen);
            if (u != v && adj[u].find(v) == adj[u].end()) {
                connection(u, v);
                added++;
            }
            attempts++;
        }
    }

    // [중요] 시각화를 위해 현재 상태를 2차원 배열로 반환
    // JS는 set을 바로 이해하기 힘들어서 vector로 변환해줍니다.
    vector<vector<int>> getAdjacencyList() {
        vector<vector<int>> result;
        // 0번 인덱스는 더미
        result.push_back({}); 

        for (int i = 1; i <= V; i++) {
            vector<int> neighbors(adj[i].begin(), adj[i].end());
            result.push_back(neighbors);
        }
        return result;
    }
};

// 바인딩 코드
EMSCRIPTEN_BINDINGS(graph_module) {
    register_vector<int>("VectorInt");
    register_vector<vector<int>>("MatrixInt");

    class_<Graph>("Graph")
        .constructor<int>()
        .function("connection", &Graph::connection)
        .function("addRandomEdges", &Graph::addRandomEdges)
        .function("getAdjacencyList", &Graph::getAdjacencyList);
}