#include <iostream>
#include <set>

#define MAX_N 20
#define MAX_M 30
#define NO_PLAYER 0
#define NO_GUN 0

using namespace std;

int n, m, k;
int player_map[MAX_N + 1][MAX_N + 1];// 지도 (현재 플레이어 위치)
multiset<int, greater<int>> gun_map[MAX_N + 1][MAX_N + 1];// 지도2 (현재 해당 위치에 있는 총들.) 크기 순으로 정렬.
int scores[MAX_M + 1] = { 0 };// 플레이어 점수 (m)
pair<int, int> positions[MAX_M + 1];// 플레이어 위치 (m)
int directions[MAX_M + 1]; //플레이어 방향 (m)
int initial_stat[MAX_M + 1];// 플레이어 초기 능력치 (m)
int guns[MAX_M + 1] = { 0 };// 플레이어 보유 총 (m)
pair<int, int> delta[4] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};// 방향

void debug_print(int turn) {
    cout << "turn : " << turn << "\n\n";
    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            if (!gun_map[r][c].empty()) {
                cout << "gun at " << r << ", " << c << " : ";
                for (int val : gun_map[r][c]) {
                    cout << val << " ";
                }
                cout << "\n";
            }
        }
    }
    cout << "\n";
    cout << "player guns\n";
    for (int player_num = 1; player_num <= m; ++player_num) {
        cout << guns[player_num] << " ";
    }
    cout << "\n\n";
    cout << "player map\n";
    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            cout << player_map[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

//정보 수집
void obtain_data() {
    cin >> n >> m >> k;

    //지도 정보 (총 정보) 수집
    for (int r = 1; r <= n; ++r) {
        for (int c = 1; c <= n; ++c) {
            int elem;
            cin >> elem;
            if (elem != 0) {
                gun_map[r][c].insert(elem);
            }
        }
    }

    //플레이어 정보 수집
    int x, y, d, s;
    for (int player_num = 1; player_num <= m; ++player_num) {
        cin >> x >> y >> d >> s;
        positions[player_num].first = x;
        positions[player_num].second = y;
        directions[player_num] = d;
        initial_stat[player_num] = s;
        player_map[x][y] = player_num;
    }
}

//현 위치가 경계를 벗어나는가?
//do_turn 보조 함수
bool out_of_bound(int r, int c) {
    return r < 1 || r > n || c < 1 || c > n;
}

//가장 큰 총 선택.
//do_turn 보조 함수
//반환 값은 해당 위치, 본인 보유 총 포함 가장 강한 총.
//집합 관리도 알아서 해준다.
int max_gun(int r, int c, int cur_gun) {
    auto max_gun_it = gun_map[r][c].begin();
    int max_gun = *max_gun_it;
    if (max_gun > cur_gun) {
        gun_map[r][c].erase(max_gun_it);
        if (cur_gun != NO_GUN) gun_map[r][c].insert(cur_gun);
        cur_gun = max_gun;
    }
    return cur_gun;
}

//왼쪽이 이기는가?
//do_turn 보조 함수
bool left_wins(int num1, int num2) {
    int num1_stat = guns[num1] + initial_stat[num1];
    int num2_stat = guns[num2] + initial_stat[num2];
    if (num1_stat != num2_stat) return num1_stat > num2_stat;
    else return initial_stat[num1] > initial_stat[num2];
}

//왼쪽이 이긴다 가정 시 점수 계산
//do_turn 보조함수
int earned_score(int num1, int num2) {
    int num1_stat = guns[num1] + initial_stat[num1];
    int num2_stat = guns[num2] + initial_stat[num2];
    return num1_stat - num2_stat;
}

//진 사람 시뮬레이션
//do_turn 보조 함수
//현재 진행 중인 애가 아닐 수 있어서 정보 업데이트 즉시 필요.
void loser_simulation(int p_num) {
    int cur_r = positions[p_num].first;
    int cur_c = positions[p_num].second;
    int cur_dir = directions[p_num];

    //0. 본인 위치에서 본인을 제거한다. 의미없을 수도 있지만 해야 한다.
    player_map[cur_r][cur_c] = NO_PLAYER;

    //1. 본인 총을 격자에 내려놓는다.
    if (guns[p_num] != NO_GUN) {
        gun_map[cur_r][cur_c].insert(guns[p_num]);
        guns[p_num] = NO_GUN;
    }

    //2. 해당 플레이어가 원래 가지고 있던 방향으로 이동한다.
    //단 거기에 다른 플레이어가 있거나 격자 범위 밖이면 오른쪽으로 90도 회전.
    //한 곳은 무조건 비어 있는것이 보장된다 (생각해보셈)
    int new_r, new_c;
    while (true) {
        new_r = cur_r + delta[cur_dir].first;
        new_c = cur_c + delta[cur_dir].second;
        if (!out_of_bound(new_r, new_c) && player_map[new_r][new_c] == NO_PLAYER) 
            break;
        cur_dir = (cur_dir + 1) % 4;
    }
    
    //3. 만약 새로운 칸에 총이 있으면 거기서 공격력이 가장 높은 총을 획득한다.
    if (!gun_map[new_r][new_c].empty()) {
        guns[p_num] = max_gun(new_r, new_c, NO_GUN);
    }

    //4. 정보 갱신. 총 정보는 이미 갱신했고...
    //본인 위치, 본인 방향, 지도상의 본인 위치
    player_map[new_r][new_c] = p_num;
    positions[p_num].first = new_r;
    positions[p_num].second = new_c;
    directions[p_num] = cur_dir;
}

//이긴 사람 시뮬레이션
//do_turn 보조 함수
//현재 진행 중인 애가 아닐 수 있어서 정보 업데이트 즉시 필요
void winner_simulation(int p_num) {
    int cur_r = positions[p_num].first;
    int cur_c = positions[p_num].second;

    //1. 승리한 칸에 떨어져있는 총들과 원래 들고 있떤 총 중 가장 공격력이 높은 총 획득
    if (!gun_map[cur_r][cur_c].empty()) {
        guns[p_num] = max_gun(cur_r, cur_c, guns[p_num]);
    }

    //2. 정보 갱신. 총 정보는 이미 갱신했고...
    //지도상의 본인 위치. 본인 위치랑 방향은 그대로기 때문.
    player_map[cur_r][cur_c] = p_num;
}

// 턴 시뮬레이션
void do_turn() {
    //각 플레이어마다 다음을 수행
    for (int player_num = 1; player_num <= m; ++player_num) {
        //먼저 시뮬레이션에 필요한 본인 관련 정보들을 전부 습득을 해놓는다.
        int cur_r = positions[player_num].first;
        int cur_c = positions[player_num].second;
        int dir = directions[player_num];
        int cur_gun = guns[player_num];

        //0. 본인 현재 위치에서 본인을 제거한다.
        //연산 꼬임 방지 용도 (전투 후 진 사람 이동의 경우라든가)
        player_map[cur_r][cur_c] = NO_PLAYER;

        //1. 본인 방향으로 이동.
        //만일 격자를 벗어나면 정반대 방향으로 방향을 바꾸어 이동.
        int new_r = cur_r + delta[dir].first;
        int new_c = cur_c + delta[dir].second;
        if (out_of_bound(new_r, new_c)) {
            dir = (dir + 2) % 4;
            new_r = cur_r + delta[dir].first;
            new_c = cur_c + delta[dir].second;
        }

        //일단 본인 위치랑 방향을 업데이트 한다.
        positions[player_num].first = new_r;
        positions[player_num].second = new_c;
        directions[player_num] = dir;

        //2-1. 이동한 방향에 플레이어가 없는 경우
        if (player_map[new_r][new_c] == NO_PLAYER) {
            //총이 있는 경우 총을 획득한다.
            //단, 본인이 총을 보유하고 있는 경우, 공격력이 가장 쎈 총을 선택한다.
            //총이 바꿔졌으면 보유했던 총을 집어넣어라.
            if (!gun_map[new_r][new_c].empty()) {
                cur_gun = max_gun(new_r, new_c, cur_gun);
            }

            //변동된 정보들을 전부 업데이트 해야 한다. 이는 밑과 같다.
            //총, 지도 상의 위치
            guns[player_num] = cur_gun;
            player_map[new_r][new_c] = player_num;
        }
        //2-2. 이동한 방향에 플레이어가 있는 경우
        //각자의 초기 능력치와 총의 공격력의 합을 비교해 더 큰 플레이어가 이기도록 한다.
        else {
            int opponent_num = player_map[new_r][new_c];
            if (left_wins(player_num, opponent_num)) {
                scores[player_num] += earned_score(player_num, opponent_num);
                loser_simulation(opponent_num);
                winner_simulation(player_num);
            }
            else {
                scores[opponent_num] += earned_score(opponent_num, player_num);
                loser_simulation(player_num);
                winner_simulation(opponent_num);
            }

            //여기 정보는 후처리 함수가 알아서 다 업데이트 함.
        }
    }
}

//점수 출력
void print_scores() {
    for (int player_num = 1; player_num <= m; ++player_num) {
        cout << scores[player_num] << " ";
    }
}

int main() {

    obtain_data();
    for (int turn = 1; turn <= k; ++turn) {
        do_turn();
        //debug_print(turn);
    }
    print_scores();

    cout << flush;
    return 0;
}