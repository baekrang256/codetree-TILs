#include <iostream>
#include <list>
#include <queue>
#include <climits>

#define MAX_N 15
#define MAX_M 30
#define EMPTY 0
#define BASE_CAMP 1
#define INPASSABLE 2 //편의점 도착 후 막히는 칸 표기용

using namespace std;

int n, m;
int turn = 0; //현재 시간
int map[MAX_N + 1][MAX_N + 1]; //지도. 

pair<int, int> dest[MAX_M + 1]; //각 인원 목적지
pair<int, int> cur_pos[MAX_M + 1]; //각 인원 현재 위치
bool arrive[MAX_M + 1] = { false }; //각 인원 도착 여부
int arrival_cnt = 0; //도착한 인원 수

list<pair<int, int>> base_camp_pos_list; // 각 베이스 캠프 위치

bool visited[MAX_N + 1][MAX_N + 1]; //bfs 방문용
pair<int, int> prev_node[MAX_N + 1][MAX_N + 1]; //bfs 경로용
pair<int, int> delta[4] = {
    {-1, 0}, {0, -1}, {0, 1}, {1, 0}
}; //방향별 delta. 1번 우선순위에 맞췄음

//디버깅용
void debug_map() {
    cout << "map status\n";

    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            cout << map[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";

    cout << "person positions\n";
    for (int cnt = 1; cnt <= m; ++cnt) {
        cout << "{" << cur_pos[cnt].first << ", " << cur_pos[cnt].second << "},";
    }
    cout << "\n";
}

//디버깅용2
void debug_prev() {
    cout << "prev_node status\n";

    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            cout << "{" <<  prev_node[r][c].first << ", " << prev_node[r][c].second << "}, ";
        }
        cout << "\n";
    }
    cout << "\n";
}

//bfs 용도
//visited 초기화
void initialize_visited() {
    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            visited[r][c] = false;
        }
    }
}

bool out_of_bound(int r, int c) {
    return r < 1 || r > n || c < 1 || c > n;
}

//1단계, 3단계용도
//bfs.
int bfs(int r, int c, int dest_r, int dest_c) {
    initialize_visited();
    queue<pair<pair<int, int>, int>> q; //bfs 구현용 {{r, c}, dist}
    vector<int> path;
    q.push({ {r, c}, 0 });
    visited[r][c] = true;
    while (!q.empty()) {
        auto cur_pos = q.front();
        int cur_r = cur_pos.first.first, cur_c = cur_pos.first.second;
        int dist = cur_pos.second;
        q.pop();
        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;
            //cout << new_r << " " << new_c << "\n";

            //경계를 벗어나거나, 막힌 곳이거나, 이미 방문했으면 넘어가자.
            if (out_of_bound(new_r, new_c) || map[new_r][new_c] == INPASSABLE || visited[new_r][new_c]) {
                continue;
            }

            //경로 파악용 업데이트.
            prev_node[new_r][new_c] = { cur_r, cur_c };

            //도착 했으면 바로 탈출.
            //그 위치까지의 거리를 반환하며, 경로 탐색용 이전 위치들이 전부 저장됨.
            if (new_r == dest_r && new_c == dest_c) {
                return dist + 1;
            }

            //도착한게 아니면 queue에 추가, 방문 표시.
            q.push({ { new_r, new_c }, dist + 1 });
            visited[new_r][new_c] = true;
        }
    }

    //문제 조건에 따르면 도착을 하지 못할 일은 없기 때문에, 여기에 오는 것은 문제가 있음...
    //다만 그래도 도달하는 것이 가능은 하다. 모든 베이스캠프가 도달할 필요는 없거든.
    //이 경우 최댓값 반환 필요.
    //cout << "what?\n";
    return INT_MAX;
}

//bfs이후, 시작지점에서 최단거리로 가기 위해 선택한 첫 방향이 뭔지 파악하는 용도
pair<int, int> find_next(int dest_r, int dest_c, int start_r, int start_c) {
    int cur_r = dest_r, cur_c = dest_c;
    while (prev_node[cur_r][cur_c].first != start_r
        || prev_node[cur_r][cur_c].second != start_c) {
        auto val = prev_node[cur_r][cur_c];
        cur_r = val.first;
        cur_c = val.second;
    }
    
    return { cur_r, cur_c };
}

//정보 수집
void obtain_data() {
    //기본 정보
    cin >> n >> m;

    //지도 정보 + 베이스 캠프 정보
    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            cin >> map[r][c];
            if (map[r][c] == BASE_CAMP) {
                //미리 베이스캠프 선택 우선순위에 맞춰가지고 집어넣기.
                base_camp_pos_list.push_back({ r, c });
            }
        }
    }

    //편의점 정보
    for (int cnt = 1; cnt <= m; ++cnt) {
        int r, c;
        cin >> r >> c;
        dest[cnt].first = r;
        dest[cnt].second = c;
    }
}

//사람 이동.
void move_people() {
    int max_idx = min(m, turn - 1);

    for (int person_idx = 1; person_idx <= max_idx; ++person_idx) {
        //이미 도착한 사람이면 넘어간다.
        if (arrive[person_idx]) continue;

        //현 위치에서 목적지로 bfs를 일단 수행한다.
        int cur_r = cur_pos[person_idx].first;
        int cur_c = cur_pos[person_idx].second;
        int dest_r = dest[person_idx].first;
        int dest_c = dest[person_idx].second;
        bfs(cur_r, cur_c, dest_r, dest_c);

        //선택된 다음 노드를 파악한다.
        //debug_prev();
        auto sel_delta = find_next(dest_r, dest_c, cur_r, cur_c);
        
        //그 방향으로 이동을 한다. 만약 도착한 경우에 길을 막는 것은 2번째 단계에서 한다.
        cur_pos[person_idx].first = sel_delta.first;
        cur_pos[person_idx].second = sel_delta.second;
    }
}

//편의점 칸 지나가지 못하게 하는 것 업데이트
void update_arrival() {
    int max_idx = min(m, turn - 1);

    for (int person_idx = 1; person_idx <= max_idx; ++person_idx) {
        //이미 도착한 사람이면 넘어간다.
        if (arrive[person_idx]) continue;

        //도착한 사람이 아니고, 이번에 도착을 했으면 업데이트
        //도착한 사람 수 증가, 도착 여부 true 변경,
        //해당 지도 위치는 더이상 지나가는 것이 불가능.
        if (dest[person_idx].first == cur_pos[person_idx].first
            && dest[person_idx].second == cur_pos[person_idx].second) {
            ++arrival_cnt;
            arrive[person_idx] = true;
            map[dest[person_idx].first][dest[person_idx].second] = INPASSABLE;
        }
    }
}

//time이 m이하일시 새로운 사람 추가
void new_mover() {
    if (turn > m) return; //더이상 추가될 사람이 없음.

    //turn번 사람이 추가가 된다.
    //남아있는 베이스캠프들 중에서 본인 목적지랑 가장 가까운 곳을 확인한다.
    auto sel_it = base_camp_pos_list.end();
    int min_dist = INT_MAX;
    int dest_r = dest[turn].first, dest_c = dest[turn].second;
    //cout << "destination : " << dest_r << " " << dest_c << "\n";
    for (auto it = base_camp_pos_list.begin(); it != base_camp_pos_list.end(); ++it) {
        //cout << "basecamp pos : " << it->first << " " << it->second << "\n";
        int dist = bfs(it->first, it->second, dest_r, dest_c);
        if (dist < min_dist) {
            sel_it = it;
            min_dist = dist;
        }
    }

    //해당 베이스캠프 이치가 본인 위치가 된다.
    //해당 베이스캠프는 더이상 지나가는 것이 불가능하다.
    cur_pos[turn].first = sel_it->first;
    cur_pos[turn].second = sel_it->second;
    //cout << cur_pos[turn].first << " " << cur_pos[turn].second << "\n";
    map[sel_it->first][sel_it->second] = INPASSABLE;

    //해당 베이스캠프는 더이상 사용이 불가능하다.
    base_camp_pos_list.erase(sel_it);
}

int main() {
    obtain_data();

    //전부 도착할 때까지 다음 과정을 반복
    while (arrival_cnt < m) {
        ++turn; //턴 진행
        move_people();
        update_arrival();
        new_mover();
        //debug_map();
    }

    cout << turn << flush;

    return 0;
}