#include <iostream>
#include <queue>
#include <climits>

#define MAX_N 10
#define MAX_M 10
#define DESTROYED 0
#define MAX_ATTACK INT_MAX
#define MAX_TURN 1000
#define DUMMY_ATK_R 0
#define DUMMY_ATK_C 0
#define DUMMY_TAR_R 0
#define DUMMY_TAR_C 1
#define OVER_RC -1

using namespace std;
int N, M, K;

int attack[MAX_N + 1][MAX_M + 1] = { 0 }; //현재 포탑들 상태
int last_attack[MAX_N + 1][MAX_M + 1] = { 0 }; //포탑 마지막 공격 시점
pair<int, int> prev_node[MAX_N + 1][MAX_M + 1]; //BFS 경로 역추적 용도.
bool visited[MAX_N + 1][MAX_M + 1]; //방문 여부 추적 용도.
bool attacked[MAX_N + 1][MAX_M + 1]; //공격 당함 여부. 공격 수행 시뮬레이션 용도.
pair<int, int> delta[4] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
}; //이동 방향. 우선순위 고려.
pair<int, int> bombard[8] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}, {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
}; //폭탄 퍼지기 방향

int cur_turn = 0; //현재 턴
int attacker_r, attacker_c; //공격자 위치
int target_r, target_c; //타겟 위치
int non_broken_cnt = 0; //안 부서진 포탑 수

//정보 수집
void obtain_data() {
    cin >> N >> M >> K;
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            cin >> attack[r][c];
            if (attack[r][c] != DESTROYED) non_broken_cnt += 1;
        }
    }
    attack[DUMMY_ATK_R][DUMMY_ATK_C] = MAX_ATTACK;
    attack[DUMMY_TAR_R][DUMMY_TAR_C] = 0;
}

//턴 시작 전 정보 업데이트
void initialize() {
    cur_turn++;
    attacker_r = DUMMY_ATK_R, attacker_c = DUMMY_ATK_C;
    target_r = DUMMY_TAR_R, target_c = DUMMY_TAR_C;
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            visited[r][c] = false;
            attacked[r][c] = false;
            prev_node[r][c] = { 0, 0 };
        }
    }
}

bool should_be_attacker(int r, int c, int cmp_r, int cmp_c) {
    int cur_attack = attack[r][c];
    int cur_last_attack = last_attack[r][c];
    int cmp_attack = attack[cmp_r][cmp_c];
    int cmp_last_attack = last_attack[cmp_r][cmp_c];
    if (cmp_attack > cur_attack) return true; //조건 1
    if (cmp_attack == cur_attack &&
        cmp_last_attack < cur_last_attack) return true; //조건 2
    if (cmp_attack == cur_attack &&
        cmp_last_attack == cur_last_attack &&
        cmp_r + cmp_c < r + c) return true; //조건 3
    if (cmp_attack == cur_attack &&
        cmp_last_attack == cur_last_attack &&
        cmp_r + cmp_c == r + c &&
        cmp_c > c) return true; //조건 4
    return false;
    
}

void increase_attack() {
    attack[attacker_r][attacker_c] += N + M;
}

//공격자 선택
void select_attacker() {
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            int cur_attack = attack[r][c];
            //부서진 포탑은 무시
            if (cur_attack == DESTROYED) continue;
            if (should_be_attacker(r, c, attacker_r, attacker_c)) {
                attacker_r = r;
                attacker_c = c;
            }
        }
    }

    //공격력 증가
    increase_attack();

    //공격 시간 갱신
    last_attack[attacker_r][attacker_c] = cur_turn;
}

bool should_be_target(int r, int c, int cmp_r, int cmp_c) {
    int cur_attack = attack[r][c];
    int cur_last_attack = last_attack[r][c];
    int cmp_attack = attack[cmp_r][cmp_c];
    int cmp_last_attack = last_attack[cmp_r][cmp_c];
    if (cmp_attack < cur_attack) return true; //조건 1
    if (cmp_attack == cur_attack &&
        cmp_last_attack > cur_last_attack) return true; //조건 2
    if (cmp_attack == cur_attack &&
        cmp_last_attack == cur_last_attack &&
        cmp_r + cmp_c > r + c) return true; //조건 3
    if (cmp_attack == cur_attack &&
        cmp_last_attack == cur_last_attack &&
        cmp_r + cmp_c == r + c &&
        cmp_c < c) return true; //조건 4
    return false;

}

//타겟 선정
void select_target() {
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            int cur_attack = attack[r][c];
            //부서진 포탑은 무시
            if (cur_attack == DESTROYED) continue;
            //공격자 무시
            if (r == attacker_r && c == attacker_c) continue;
            
            if (should_be_target(r, c, target_r, target_c)) {
                target_r = r;
                target_c = c;
            }
        }
    }
}

//레이저 공격이 가능한지 판별.
bool laser_possible() {
    //bfs로 확인을 한다. 경로 저장 필요.
    bool possible = false;
    queue<pair<int, int>> q;
    q.push({ attacker_r, attacker_c });
    visited[attacker_r][attacker_c] = true;
    prev_node[attacker_r][attacker_c] = { OVER_RC, OVER_RC };

    while (!q.empty()) {
        int cur_r = q.front().first, cur_c = q.front().second;
        q.pop();
        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;

            //위치 수정
            if (new_r > N) new_r = 1;
            else if (new_r < 1) new_r = N;
            if (new_c > M) new_c = 1;
            else if (new_c < 1) new_c = M;

            //파괴 안되어있어야 함. 방문 안되어 있어야 함.
            if (attack[new_r][new_c] != DESTROYED && !visited[new_r][new_c]) {
                q.push({ new_r, new_c });
                visited[new_r][new_c] = true;
                prev_node[new_r][new_c] = { cur_r, cur_c };
                if (new_r == target_r && new_c == target_c) {
                    possible = true;
                    break;
                }
            }
        }
        if (possible) break;
    }

    return possible;
}

void do_attack(int r, int c, int damage) {
    attacked[r][c] = true;
    attack[r][c] -= damage;
    if (attack[r][c] < 0) attack[r][c] = 0;
    if (attack[r][c] == 0) non_broken_cnt -= 1;
}

//레이저 공격 수행. 이전 경로 탐색 활용.
void do_laser() {
    int cur_r = target_r;
    int cur_c = target_c;
    int attacker_atk = attack[attacker_r][attacker_c];

    do_attack(cur_r, cur_c, attacker_atk);
    auto val = prev_node[cur_r][cur_c];
    cur_r = val.first;
    cur_c = val.second;
    attacker_atk /= 2;

    while (cur_r != attacker_r || cur_c != attacker_c) {
        do_attack(cur_r, cur_c, attacker_atk);
        auto val = prev_node[cur_r][cur_c];
        cur_r = val.first;
        cur_c = val.second;
    }
}

//폭탄 공격 수행.
void do_bomb() {
    int attacker_atk = attack[attacker_r][attacker_c];
    do_attack(target_r, target_c, attacker_atk);
    attacker_atk /= 2;

    for (int idx = 0; idx < 8; ++idx) {
        int new_r = target_r + bombard[idx].first;
        int new_c = target_c + bombard[idx].second;

        //위치 수정
        if (new_r > N) new_r = 1;
        else if (new_r < 1) new_r = N;
        if (new_c > M) new_c = 1;
        else if (new_c < 1) new_c = M;

        //파괴 안되어있어야 함. 본인 아니어야 함.
        if (attack[new_r][new_c] != DESTROYED && (new_r != attacker_r || new_c != attacker_c)) {
            do_attack(new_r, new_c, attacker_atk);
        }
    }
}

//공격
void do_attack() {
    select_target();
    if (laser_possible()) {
        do_laser();
    }
    else {
        do_bomb();
    }
}

//공격 당하지 않은 곳 수리
void repair() {
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            //공격자 아니어야함, 공격당한 곳 아니어야함, 부서진 곳 아니어야 함.
            if ((r != attacker_r || c != attacker_c) && attack[r][c] != DESTROYED && !attacked[r][c]) {
                attack[r][c] += 1;
            }
        }
    }
}

int find_max() {
    int max_val = 0;
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= M; ++c) {
            max_val = max(attack[r][c], max_val);
        }
    }

    return max_val;
}

bool should_exit() {
    return non_broken_cnt <= 1;
}

int main() {
    obtain_data();
    for (int turn = 0; turn < K; ++turn) {
        initialize();
        select_attacker();
        do_attack();
        repair();
        if (should_exit()) break;
    }
    cout << find_max() << flush;

    return 0;
}