#include <iostream>
#include <vector>
#include <array>

using namespace std;

struct Node {
    //int my_id; //내 번호. 하지만 필요 없어 보임.
    int parent_id; //-1이거나 이미 존재하는 노드. -1이면 이 녀석이 전체 root
    int color; //1~5
    int max_depth; //자식까지의 최대 깊이. 본인만 존재시 깊이 1, 바로 밑에 자식이 있으면 2...
};

int Q, cmd;
struct Node tree[100001] = { 0 }; //노드 모음
vector<int> childrens[100001]; //각 노드의 자식 번호. change_color, calc_score 때문에 필요
int score = 0;
int max_node = -1;
vector<int> root_nodes;
int tree_max_depth = 0;

void debug_print_tree() {
    for (int i = 0; i <= max_node; ++i) {
        if (tree[i].parent_id == 0) continue;
        cout << i << " : { parent_id : " <<
            tree[i].parent_id << ", color : " <<
            tree[i].color << ", max_depth : " <<
            tree[i].max_depth << "}\n";
    }
    cout << "\n";
}

void debug_print_childrens() {
    bool no_child = true;
    for (int i = 0; i <= max_node; ++i) {
        if (tree[i].parent_id == 0 || childrens[i].size() == 0) continue;
        no_child = false;
        cout << i << "'s childrens : { ";
        for (int child : childrens[i]) {
            cout << child << " ";
        }
        cout << "}\n";
    }
    if (no_child) cout << "no children yet\n\n";
    else cout << "\n";
}

//새로운 노드를 추가한다. 노드 번호는 무조건 새로운 값이다.
//부모 노드 번호는 -1, 혹은 이미 존재하는 노드 번호다.
//본인의 추가로 특정 상위 node의 max_depth에 모순이 발생 시, 즉시 종료. 
//m_id : 고유번호, p_id : 부모 번호, color : 색깔, max_depth : 최대 깊이
//가치를 실시간으로 바꿔야 한다.
//복잡도 : 최대 O(100) : 최대 높이가 100이라 그렇다.
void add_node(int m_id, int p_id, int color, int max_depth) {
    //본인이 root라면 갱신
    if (p_id == -1) {
        //그런데 root가 이미 있을수도 있다. 이 경우... 하나 더 만든다(...)
        root_nodes.push_back(m_id);
    }

    //일단 상위 노드의 max_depth 조건에 저촉되지 않는지를 확인해야 한다.
    int cur_pid = p_id, cur_max_depth, cur_depth = 1;
    while (cur_pid != -1) {
        cur_depth += 1;
        cur_max_depth = tree[cur_pid].max_depth;
        if (cur_max_depth < cur_depth) return;
        cur_pid = tree[cur_pid].parent_id;
    }
    if (cur_depth > tree_max_depth) tree_max_depth = cur_depth;

    //노드 추가 및 가치 업데이트
    tree[m_id].parent_id = p_id;
    tree[m_id].color = color;
    tree[m_id].max_depth = max_depth;

    //자식 추가
    if (p_id != -1) childrens[p_id].push_back(m_id);

    //debugging
    if (max_node < m_id) max_node = m_id;
}

//m_id 노드를 루트로 하는 서브 트리 내 모든 노드의 색깔을 color로 변경.
//m_id는 무조건 존재하는 노드 번호다.
//가치를 실시간으로 바꿔야 한다.
//복잡도 : 최대 O(100000). 노드 최대 개수가 10만이라 그렇다.
void change_color(int m_id, int color) {
    //자기 색깔 바꾼다. 그리고 하위 색깔 정보를 갱신하고 가치도 갱신.
    tree[m_id].color = color;

    for (int child : childrens[m_id]) {
        change_color(child, color);
    }
}

//m_id 노드가 어떤 색인지 출력
//m_id는 무조건 존재하는 노드 번호다.
//색은 그 색깔을 나타내는 숫자를 출력한다.
//복잡도 : O(1)
void check_color(int m_id) {
    cout << tree[m_id].color << "\n";
}

//모든 노드의 가치의 제곱의 합을 구한다.
// 각 node가 먼저 본인 하위 color 개수를 파악한다. 
//복잡도 : O(1000000)
array<int, 6> calc_score(int m_id) {
    array<int, 6> color_cnt = { 0 };
    color_cnt[tree[m_id].color] += 1;
    for (int child : childrens[m_id]) {
        auto subtree_colors = calc_score(child);
        for (int i = 1; i <= 5; ++i) {
            color_cnt[i] += subtree_colors[i];
        }
    }
    int diff_color_cnt = 0;
    for (int i = 1; i <= 5; ++i) {
        if (color_cnt[i] >= 1) diff_color_cnt += 1;
    }
    score += diff_color_cnt * diff_color_cnt;
    return color_cnt;
}

//가치 제곱의 합을 구하고 출력한다.
void calc_and_print() {
    score = 0;
    for (int root_node : root_nodes) {
        calc_score(root_node);
    }
    cout << score << "\n";
}

int main() {
    cin >> Q;
    int m_id, p_id, color, max_depth;
    for (int test_case = 0; test_case < Q; ++test_case) {
        cin >> cmd;
        switch (cmd) {
        case 100:
            cin >> m_id >> p_id >> color >> max_depth;
            add_node(m_id, p_id, color, max_depth);
            break;
        case 200:
            cin >> m_id >> color;
            change_color(m_id, color);
            break;
        case 300:
            cin >> m_id;
            check_color(m_id);
            break;
        case 400:
            calc_and_print();
            break;
        }
        //debug_print_tree();
        //debug_print_childrens();
    }
    cout << flush;
    return 0;
}