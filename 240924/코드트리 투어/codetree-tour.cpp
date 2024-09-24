#include <iostream>
#include <set>
#include <vector>
#include <queue>

#define INF 1000000000

using namespace std;

int Q, cmd, n, m;
vector<pair<int, int>> edges[2000]; //엣지들 모음. first : cost, second : dest
int cost[2000]; //특정 시작지점으로부터의 도착 지점 비용 모음. 다익스트라가 갱신
bool visited[2000]; //다익스트라 활용 방문 여부
pair<int, int> info[30000]; //id에 따른 product 정보. first : revenue, second :dest

int dest(int id) {
    return info[id].second;
}

int profit(int id) {
    return info[id].first - cost[dest(id)];
}

struct compare {
    bool operator() (const int& a, const int& b) const {
        if (a == b) return false;
        int a_val = profit(a);
        int b_val = profit(b);
        if (a_val != b_val) return a_val > b_val;
        return a < b;
    }
};

set<int, compare> products; //product를 우선순위에 따라 배치해놓음

void debug_print_cost() {
    cout << "costs\n";
    for (int i = 0; i < n; ++i) {
        cout << cost[i] << " ";
    }
    cout << "\n\n";
}

void debug_print_product_order() {
    cout << "order\n";
    for (int id : products) {
        cout << id << " ";
    }
    cout << "\n\n";
}

void initialize_dijkstra() {
    for (int i = 0; i < n; ++i) {
        cost[i] = INF;
        visited[i] = false;
    }
}

void dijkstra(int start) {
    initialize_dijkstra();
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    cost[start] = 0;
    pq.push({ 0, start });
    for (int cnt = 0; cnt < n; ++cnt) {
        pair<int, int> cur_info;
        while (true) {
            if (pq.empty()) {
                cur_info.second = -1;
                break;
            }
            cur_info = pq.top();
            pq.pop();
            if (!visited[cur_info.second]) break;
        }
        if (cur_info.second == -1) break;
        int cur_cost = cur_info.first;
        int cur_pos = cur_info.second;
        visited[cur_pos] = true;
        for (auto edge : edges[cur_pos]) {
            int edge_cost = edge.first;
            int dest = edge.second;
            if (visited[dest]) continue;
            if (edge_cost + cur_cost < cost[dest]) {
                cost[dest] = edge_cost + cur_cost;
                pq.push({ cost[dest], dest });
            }
        }
        //if (!pq.empty()) cout << pq.top().first << " " << pq.top().second << "\n";
    }
    //debug_print_cost();
}

//1번
//V*(ElogV) 복잡도 (대략)
void build_land() {
    cin >> n >> m;
    int v, w, u;
    for (int edge_cnt = 0; edge_cnt < m; ++edge_cnt) {
        cin >> v >> u >> w;
        edges[v].push_back({ w, u });
        edges[u].push_back({ w, v });
    }

    //미리 비용 전부 계산
    dijkstra(0);
}

//2번
//log(N) 복잡도 (N은 상품 개수)
void make_product() {
    int id, revenue, dest;
    cin >> id >> revenue >> dest;
    info[id].first = revenue;
    info[id].second = dest;
    products.insert(id);
    //debug_print_product_order();
}

//3번
//log(N) 복잡도 (N은 상품 개수)
void cancel_product() {
    int id;
    cin >> id;
    products.erase(id);
}

//4번
void sell_optimized_product() {
    auto it = products.begin();
    if (it == products.end()) {
        //no existance
        cout << -1 << "\n";
        return;
    }
    int id = *it;
    if (cost[dest(id)] == INF || profit(id) < 0) {
        cout << -1 << "\n";
        return;
    }
    products.erase(it);
    cout << id << "\n";
}

//5번
//V*(ElogV) 복잡도 + NlogN
void change_product_start() {
    int s;
    cin >> s;
    int id_cnt = products.size();
    vector<int> v(id_cnt);
    auto it = products.begin();
    for (int i = 0; i < id_cnt; ++i) {
        v[i] = *it;
        ++it;
    }
    products.clear();
    dijkstra(s);
    for (int i = 0; i < id_cnt; ++i) {
        products.insert(v[i]);
    }
}

//명령 수행
int main() {
    cin >> Q;
    for (int query_cnt = 0; query_cnt < Q; ++query_cnt) {
        cin >> cmd;
        switch(cmd) {
            case 100:
                build_land();
                break;
            case 200:
                make_product();
                break;
            case 300:
                cancel_product();
                break;
            case 400:
                sell_optimized_product();
                break;
            case 500:
                change_product_start();
                break;
        }
    }

    return 0;
}