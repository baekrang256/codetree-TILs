#include <iostream>
#include <array>
#include <queue>

using namespace std;

int answer = 0;
int R, C, K, golem_r, golem_c, exit_dir, golem_num;
pair<int, bool> map[73][71]; // (골렘 존재시 골렘 번호, 없으면 0, is_exit)
bool visited[73][71];
pair<int, int> delta[4] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};

void print_golem_pos() {
    cout << "golem position : " << golem_r << " " << golem_c << "\n\n";
}

void print_map() {
    for (int r = 0; r <= R+2; ++r) {
        for (int c = 1; c <= C; ++c) {
            cout << "{" << map[r][c].first << ", " << map[r][c].second << "} ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void initialize() {
    for (int r = 0; r <= R + 2; ++r) {
        for (int c = 1; c <= C; ++c) {
            map[r][c].first = 0;
            map[r][c].second = false;
        }
    }
}

void visited_initialize() {
    for (int r = 0; r <= R + 2; ++r) {
        for (int c = 1; c <= C; ++c) {
            visited[r][c] = false;
        }
    }
}

//아래 이동이 가능하면 움직이고 true. 불가능하면 false
bool check_bottom_and_move() {
    if (map[golem_r + 2][golem_c].first
        || map[golem_r + 1][golem_c - 1].first
        || map[golem_r + 1][golem_c + 1].first) {
        return false;
    }
    ++golem_r;
    return true;
}
bool check_left_and_move() {
    int ori_golem_c = golem_c;
    int ori_golem_r = golem_r;
    int ori_exit_dir = exit_dir;
    if (golem_c - 2 < 1 || map[golem_r-1][golem_c-1].first
        || map[golem_r][golem_c-2].first || map[golem_r+1][golem_c-1].first) {
        //결국 이동 불가
        return false;
    }
    golem_c -= 1;
    exit_dir -= 1;
    if (exit_dir < 0) exit_dir = 3;
    if (check_bottom_and_move()) {
        //좌회전 후 이동 성공!
        return true;
    }
    else {
        golem_r = ori_golem_r;
        golem_c = ori_golem_c;
        exit_dir = ori_exit_dir;
        return false;
    }
}
bool check_right_and_move() {
    int ori_golem_c = golem_c;
    int ori_golem_r = golem_r;
    int ori_exit_dir = exit_dir;
    if (golem_c + 2 > C || map[golem_r - 1][golem_c + 1].first
        || map[golem_r][golem_c + 2].first || map[golem_r + 1][golem_c + 1].first) {
        //결국 이동 불가
        return false;
    }
    golem_c += 1;
    exit_dir = (exit_dir +1) % 4;
    if (check_bottom_and_move()) {
        //우회전 후 이동 성공!
        return true;
    }
    else {
        golem_r = ori_golem_r;
        golem_c = ori_golem_c;
        exit_dir = ori_exit_dir;
        return false;
    }
}

//숲 진입 movement
bool entry_move() {
    for (int cnt = 0; cnt < 2; ++cnt) {
        if (!check_bottom_and_move() &&
            !check_left_and_move() &&
            !check_right_and_move()) {
            return false;
        }
    }
    return true;
}

//숲 진입 후 movement
void forest_move() {
    while (true) {
        if (golem_r + 2 > R+2) return; //이미 끝까지 왔네
        if (!check_bottom_and_move() &&
            !check_left_and_move() &&
            !check_right_and_move()) {
            return;
        }
    }
}

//골렘 위치 투입
void golem_mark() {
    map[golem_r][golem_c].first = golem_num;
    for (int idx = 0; idx < 4; ++idx) {
        map[golem_r + delta[idx].first][golem_c + delta[idx].second].first = golem_num;
        if (idx == exit_dir) {
            map[golem_r + delta[idx].first][golem_c + delta[idx].second].second = true;
        }
    }
}

//정령 이동
int elemental_move() {
    visited_initialize();
    int fairy_r = golem_r;
    queue<pair<int, int>> q;
    q.push({ golem_r, golem_c });
    visited[golem_r][golem_c] = true;
    while (!q.empty()) {
        int cur_r = q.front().first, cur_c = q.front().second;
        if (cur_r > fairy_r) fairy_r = cur_r;
        bool cur_exit = map[cur_r][cur_c].second;
        int cur_num = map[cur_r][cur_c].first;
        q.pop();

        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;
            //범위 벗어나거나, 골렘이 아니거나, 이미 방문한데면 이동 불가
            if (new_r > R + 2 || new_r < 3 || new_c > C || new_c < 1 
                || !map[new_r][new_c].first || visited[new_r][new_c]) continue;
            if (cur_exit || map[new_r][new_c].first == cur_num) {
                //같은 골렘은 아니지만 현재 출구에 위치하니 이동 가능~
                //혹은 같은 골렘이니 이동 가능~
                q.push({ new_r, new_c });
                visited[new_r][new_c] = true;
            }
        }
    }

    return fairy_r - 2; //행 정보가 요상하니 수정 필요
}

int main() {
    cin >> R >> C >> K;
    initialize();

    //K개의 골렘에 대해서 다음을 반복한다.
    for (int golem_cnt = 0; golem_cnt < K; ++golem_cnt) {
        cin >> golem_c >> exit_dir;
        golem_r = 1;
        ++golem_num;
        
        //일단 숲에 온전히 진입을 해야 한다. 이것이 실패하면 map을 초기화하고 바로 다음 골렘
        if (!entry_move()) {
            initialize();
            //cout << "skipped golme no. " << golem_num << "\n\n";
            continue;
        }
        
        //숲에 진입을 했으면 최대한 아래로 내려간다. 이후 마킹
        forest_move();
        golem_mark();
        //print_golem_pos();
        //print_map();
        //정령의 최대 행을 구하고 이를 answer에 더한다.
        int result = elemental_move();
        //cout << result << "\n\n";
        answer += result;
    }

    cout << answer << flush;

    return 0;
}