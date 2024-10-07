#include <iostream>
#include <queue>
#include <array>

#define NONE 0
#define EAST 1
#define WEST 2
#define NORTH 3
#define SOUTH 4

#define UP 0
#define THROUGH 1
#define DOWN 2

using namespace std;

int R, C, K, W;
int temperature[21][21] = { 0 };
int modification[21][21] = { 0 };
bool walls[21][21][5] = { false }; //방향은 매크로 참고
bool incremented[21][21];
vector<array<int, 3>> warmers;
vector<pair<int, int>> search_pos;

//퍼질 때 사용할 delta다. 각 온풍기 방향마다 다름.
pair<int, int> tmp_delta[5][3]{
    {{0, 0},{0, 0},{0,0}},
    {{-1, 1},{0, 1},{1, 1}},
    {{-1, -1},{0, -1},{1, -1}},
    {{-1, -1},{-1, 0},{-1, 1}},
    {{1, -1},{1, 0},{1, 1}}
};

pair<int, int> nearby_delta[5] = {
    {0, 0},{0, 1},{0, -1},{-1, 0},{1, 0}
};

//퍼질 때 벽이 없어야 하는 위치다. 각 온풍기 방향마다 다름.
//첫번째 elem은 시작 기준 벽이 없어야하는 곳
//두번째 elem은 도착 기준 벽이 없어야 하는 곳이다. 연산 편의를 위해 좀 복잡하게 구성.
pair<int, int> no_wall[5][3]{
    {{0,0},{0,0},{0,0}},
    {{NORTH, WEST}, {EAST, NONE}, {SOUTH, WEST}},
    {{NORTH, EAST}, {WEST, NONE}, {SOUTH, EAST}},
    {{WEST, SOUTH}, {NORTH, NONE}, {EAST, SOUTH}},
    {{WEST, NORTH}, {SOUTH, NONE}, {EAST, NORTH}}
};

void initialize_incremented() {
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            incremented[r][c] = false;
        }
    }
}

void initialize_modification() {
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            modification[r][c] = 0;
        }
    }
}

bool out_of_bound(int r, int c) {
    return r < 1 || r > R || c < 1 || c > C;
}

void blow(int r, int c, int type) {
    initialize_incremented();
    queue<array<int, 3>> q;
    //먼저 초기 바람을 수행해야 한다.
    //조건에 따라 벽이 있거나, 칸이 없는 경우는 없음.
    int new_r = r + tmp_delta[type][THROUGH].first;
    int new_c = c + tmp_delta[type][THROUGH].second;
    temperature[new_r][new_c] += 5;
    q.push({ new_r, new_c, 4 });

    while (!q.empty()) {
        auto cur_r = q.front()[0], cur_c = q.front()[1], cur_inc = q.front()[2];
        q.pop();
        for (int idx = 0; idx < 3; ++idx) {
            //새로운 위치를 구한다.
            int new_r = cur_r + tmp_delta[type][idx].first;
            int new_c = cur_c + tmp_delta[type][idx].second;

            //유효한 위치인지 확인한다.
            if (out_of_bound(new_r, new_c)) continue;

            //이미 온도가 증가했는지 확인한다.
            if (incremented[new_r][new_c]) continue;

            //벽이 있는지 확인한다.
            if (walls[cur_r][cur_c][no_wall[type][idx].first] 
                || walls[new_r][new_c][no_wall[type][idx].second]) {
                continue;
            }

            //전부 통과. 온도를 높이고 이것저것 조작
            temperature[new_r][new_c] += cur_inc;
            incremented[new_r][new_c] = true;
            if (cur_inc > 1) {
                q.push({ new_r, new_c, cur_inc - 1 });
            }
        }
    }
}

void balance() {
    initialize_modification();
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            int my_temp = temperature[r][c];
            for (int idx = 1; idx <= 4; ++idx) {
                if (walls[r][c][idx]) continue; //벽이 있넹

                int new_r = r + nearby_delta[idx].first;
                int new_c = c + nearby_delta[idx].second;
                if (out_of_bound(new_r, new_c)) continue; //범위를 벗어나넹

                int other_temp = temperature[new_r][new_c];
                int inc_val = abs(my_temp - other_temp) / 4;
                if (inc_val <= 0) continue; //변화가 없넹

                //높은 곳에서 낮은 곳으로 이동.
                if (my_temp > other_temp) {
                    modification[r][c] -= inc_val;
                    modification[new_r][new_c] += inc_val;
                }
                else if (my_temp < other_temp) {
                    modification[r][c] += inc_val;
                    modification[new_r][new_c] -= inc_val;
                }
            }
        }
    }
}

void update() {
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            //중복해서 계산이 되니 2로 나눌 필요가 있음.
            temperature[r][c] += (modification[r][c]) / 2;
        }
    }
}

void decrementing() {
    for (int r = 1; r <= R; ++r) {
        if (r != 1 && r != R) {
            if (temperature[r][1] != 0)
                temperature[r][1] -= 1;
            if (temperature[r][C] != 0)
                temperature[r][C] -= 1;
        }
        else {
            for (int c = 1; c <= C; ++c) {
                if (temperature[r][c] != 0)
                    temperature[r][c] -= 1;
            }
        }
    }
}

bool all_warmed() {
    for (auto pos : search_pos) {
        if (temperature[pos.first][pos.second] < K)
            return false;
    }
    return true;
}

int main() {
    //온풍기 + 조사 위치 구하기.
    cin >> R >> C >> K;
    for (int r = 1; r <= R; ++r) {
        for (int c = 1; c <= C; ++c) {
            int val;
            cin >> val;
            if (val == 5) {
                search_pos.push_back({ r, c });
            }
            else if (val != 0) {
                warmers.push_back({ r, c, val });
            }
        }
    }

    //벽 위치 구하기
    cin >> W;
    for (int wall = 0; wall < W; ++wall) {
        int x, y, t;
        cin >> x >> y >> t;
        if (t == 0) {
            walls[x][y][NORTH] = true;
            walls[x - 1][y][SOUTH] = true;
        }
        else {
            walls[x][y][EAST] = true;
            walls[x][y + 1][WEST] = true;
        }
    }

    int choco_cnt = 0;

    while (choco_cnt <= 100) {
        //바람 불기
        for (auto warmer_data : warmers) {
            int warmer_r = warmer_data[0];
            int warmer_c = warmer_data[1];
            int warmer_type = warmer_data[2];
            blow(warmer_r, warmer_c, warmer_type);
        }

        //온도 조절하기
        balance();
        update();

        //바깥쪽 칸들 온도 감소시키기
        decrementing();

        //초콜릿 먹기
        ++choco_cnt;

        //조사하기
        if (all_warmed()) break;
    }

    cout << choco_cnt << flush;

    return 0;
}