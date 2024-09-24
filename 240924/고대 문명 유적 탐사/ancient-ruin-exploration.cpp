#include <iostream>
#include <queue>
#include <algorithm>

#define ARB 0
#define REAL 1

#define EMPTY 0

using namespace std;

int K, M;
int ruin[2][5][5]; //유적지. 0 : 가짜(임시) 1 : 진짜
int scores[10]; //각 턴 별 모음 유물 가치
int wall_piece[300]; //추후 채워지는 유물 조각
int piece_idx = 0; //wall_piece 추적 index
vector<int> box(8); //회전용 상자
bool visited[5][5]; //bfs 용도
vector<pair<int, int>> path; //유물 제거 용도
pair<int, int> delta[4] = {
    {0, -1}, {0, 1}, {-1, 0}, {1, 0}
};

void box_debug_print() {
    for (int i = 0; i < 8; ++i) {
        cout << box[i] << " ";
    }
    cout << "\n";
}

void ruin_debug_print(int map_type) {
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            cout << ruin[map_type][r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}


void fill_box(int r, int c) {
    box[0] = ruin[REAL][r - 1][c - 1];
    box[1] = ruin[REAL][r - 1][c];
    box[2] = ruin[REAL][r - 1][c + 1];
    box[3] = ruin[REAL][r][c + 1];
    box[4] = ruin[REAL][r + 1][c + 1];
    box[5] = ruin[REAL][r + 1][c];
    box[6] = ruin[REAL][r + 1][c - 1];
    box[7] = ruin[REAL][r][c - 1];
}

//real map 내용물 arb map으로 옮기고 회전한거 반영하기.
void initialize_arb_map(int box_r, int box_c) {
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            ruin[ARB][r][c] = ruin[REAL][r][c];
        }
    }

    ruin[ARB][box_r - 1][box_c - 1] = box[0];
    ruin[ARB][box_r - 1][box_c] = box[1];
    ruin[ARB][box_r - 1][box_c + 1] = box[2];
    ruin[ARB][box_r][box_c + 1] = box[3];
    ruin[ARB][box_r + 1][box_c + 1] = box[4];
    ruin[ARB][box_r + 1][box_c] = box[5];
    ruin[ARB][box_r + 1][box_c - 1] = box[6];
    ruin[ARB][box_r][box_c - 1] = box[7];
}

void initialize_real_map(int box_r, int box_c, int box_rotate_cnt) {
    if (box_rotate_cnt == 0) return;
    fill_box(box_r, box_c);
    for (int rotate_cnt = 0; rotate_cnt < box_rotate_cnt; ++rotate_cnt) {
        rotate(box.begin(), box.begin() + 2, box.end());
    }

    ruin[REAL][box_r - 1][box_c - 1] = box[0];
    ruin[REAL][box_r - 1][box_c] = box[1];
    ruin[REAL][box_r - 1][box_c + 1] = box[2];
    ruin[REAL][box_r][box_c + 1] = box[3];
    ruin[REAL][box_r + 1][box_c + 1] = box[4];
    ruin[REAL][box_r + 1][box_c] = box[5];
    ruin[REAL][box_r + 1][box_c - 1] = box[6];
    ruin[REAL][box_r][box_c - 1] = box[7];
}

void initialize_visited() {
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            visited[r][c] = false;
        }
    }
}

//각 위치별 점수 계산
int bfs(int start_r, int start_c, bool map_type) {
    int score = 1;
    int val = ruin[map_type][start_r][start_c];
    visited[start_r][start_c] = true;
    path.clear();
    queue<pair<int, int>> q;
    q.push({ start_r, start_c });
    path.push_back({ start_r, start_c });
    while (!q.empty()) {
        int cur_r = q.front().first;
        int cur_c = q.front().second;
        q.pop();
        for (int i = 0; i < 4; ++i) {
            int new_r = cur_r + delta[i].first;
            int new_c = cur_c + delta[i].second;
            if (new_r < 0 || new_r >= 5 || new_c < 0 || new_c >= 5 ||
                visited[new_r][new_c] || ruin[map_type][new_r][new_c] != val)
                continue;
            q.push({ new_r, new_c });
            path.push_back({ new_r, new_c });
            visited[new_r][new_c] = true;
            ++score;
        }
    }
    if (score >= 3) {
        if (map_type == REAL) {
            for (auto pos : path) {
                ruin[map_type][pos.first][pos.second] = EMPTY;
            }
        }
        return score;
    }
    else {
        return 0;
    }
}

int calc_score(int box_r, int box_c, int map_type, int box_rotate_cnt = 0) {
    int score = 0;
    if (map_type == ARB) initialize_arb_map(box_r, box_c);
    else initialize_real_map(box_r, box_c, box_rotate_cnt);
    initialize_visited();
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            if (!visited[r][c]) {
                score += bfs(r, c, map_type);
            }
        }
    }
    return score;
}

void fill_pieces() {
    for (int c = 0; c < 5; ++c) {
        for (int r = 4; r >= 0; --r) {
            if (ruin[REAL][r][c] == EMPTY) {
                ruin[REAL][r][c] = wall_piece[piece_idx];
                ++piece_idx;
            }
        }
    }
}

int main() {
    //기본 셋업
    cin >> K >> M;
    for (int r = 0; r < 5; ++r) {
        for (int c = 0; c < 5; ++c) {
            cin >> ruin[REAL][r][c];
        }
    }
    for (int i = 0; i < M; ++i) {
        cin >> wall_piece[i];
    }

    //매 턴마다 다음을 수행
    for (int turn = 0; turn < K; ++turn) {
        //초기 탐사 진행
        int max_score = 0, max_r = -1, max_c = -1, max_rotate_cnt = 0;

        for (int c = 3; c >= 1; --c) {
            for (int r = 3; r >= 1; --r) {
                //벡터 만들고, 한번씩 회전을 하면서 점수 계산을 하자.
                fill_box(r, c);
                for (int rotate_cnt = 0; rotate_cnt < 3; ++rotate_cnt) {
                    rotate(box.begin(), box.begin() + 2, box.end());
                    //box_debug_print();
                    int score = calc_score(r, c, ARB);
                    //cout << "r, c, rotate_cnt, score : " << r << " " << c << " " << rotate_cnt << " " << score << "\n";
                    if (score == 0) continue;
                    if (max_score < score || (max_score == score && max_rotate_cnt <= rotate_cnt)) {
                        max_score = score;
                        max_r = r;
                        max_c = c;
                        max_rotate_cnt = rotate_cnt + 1;
                    }
                }
            }
        }
      //  cout << "\n";

        //cout << max_r << " " << max_c << " " << max_rotate_cnt << " " << max_score << "\n";
        if (max_score == 0) break;

        //맵 갱신

        int score = calc_score(max_r, max_c, REAL, max_rotate_cnt);
        int next_score = 0;

        //조각 채우기, 유물 재획득을 무한 반복.
        while (true) {
            fill_pieces();
            //ruin_debug_print(REAL);
            next_score = calc_score(-1, -1, REAL);
            if (next_score == 0) break;
            score += next_score;
        }
        scores[turn] = score;
    }

    //출력
    for (int turn = 0; turn < K; ++turn) {
        if (scores[turn] == 0) break;
        cout << scores[turn] << " ";
    }
    cout << flush;

    return 0;
}