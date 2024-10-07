#include <iostream>
#include <climits>

using namespace std;

int N, K;
int line[100];
int square[10][25] = { 0 }; //1단계, 2단계를 전부 고려하면 배열은 이렇게 만들어야 함.
int diff[10][25] = { 0 }; //물고기 변화량 측정용
int base_len, square_w, square_h;
pair<int, int> delta[4] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};
//square과 diff 초기화
void initialize() {
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 25; ++c) {
            square[r][c] = 0;
            diff[r][c] = 0;
        }
    }
}

//차이 계산
int calc_diff() {
    int max_val = 0, min_val = INT_MAX;
    for (int i = 0; i < N; ++i) {
        max_val = max(line[i], max_val);
        min_val = min(line[i], min_val);
    }
    return max_val - min_val;
}

//최솟값 계산
int find_min() {
    int min_val = INT_MAX;
    for (int i = 0; i < N; ++i) {
        min_val = min(line[i], min_val);
    }
    return min_val;
}

//물고기 더하기
void add_fish(int min_fish) {
    for (int i = 0; i < N; ++i) {
        if (line[i] == min_fish) line[i] += 1;
    }
}

//1차 공중부양에서 사용할 base_len, square_len 설정용
void calc_base_len() {
    square_w = 1;
    square_h = 0;
    int incr_w = false;
    while (true) {
        if (incr_w) ++square_w;
        else ++square_h;

        if (N - square_w * square_h < square_w) {
            if (incr_w) --square_w;
            else --square_h;
            break;
        }
        incr_w = !incr_w;
    }
    
    base_len = N - square_w * square_h;

    //cout << base_len << " " << square_w << " " << square_h << "\n";
}

//1차 공중부양
void modify_one() {
    //일단 밑변 길이의 마지막 녀석들은 번호가 그대로 있음.
    //그 이후에 과거 상어 토네이도와 유사한 형식으로 말아오르기가 이루어지면 1차는 완료다.
    int line_idx = N - base_len;
    int r = 9, c = 0;
    int delta_idx = 0;

    for (int idx = 0; idx < base_len; ++idx) {
        square[r][idx] = line[line_idx + idx];
    }
    line_idx -= 1;
    int len = 0;
    int base_len = square_h;
    int start_len = 0;
    bool base_is_h = true;
    bool change_start_len = true;
    while (line_idx >= 0) {
        r += delta[delta_idx].first;
        c += delta[delta_idx].second;
        square[r][c] = line[line_idx];
        ++len;
        if (len >= base_len) {
            //change direction
            delta_idx = (delta_idx + 1) % 4;

            //update length provision.
            if (base_is_h) {
                base_len = square_w;
            }
            else {
                base_len = square_h;
            }

            if (change_start_len) {
                start_len += 1;
            }
            len = start_len;
            change_start_len = !change_start_len;
            base_is_h = !base_is_h;
        }
        line_idx -= 1;
    }
}

//2차 공중부양
void modify_two() {
    //2차는 좀 더 간단하다.
    int base_idx = N - (N / 4);
    for (int idx = 0; idx < N/4; ++idx) {
        square[9][idx] = line[base_idx + idx];
    }
    base_idx = N / 4 - 1;
    for (int idx = 0; idx < N / 4; ++idx) {
        square[8][idx] = line[base_idx - idx];
    }
    base_idx = N - (3 * N / 4);
    for (int idx = 0; idx < N / 4; ++idx) {
        square[7][idx] = line[base_idx + idx];
    }
    base_idx = 3 * N / 4 - 1;
    for (int idx = 0; idx < N / 4; ++idx) {
        square[6][idx] = line[base_idx - idx];
    }
}

//정리
void clean() {
    //square을 기준으로 한다.
    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 25; ++c) {
            if (square[r][c] == 0) continue;
            for (int idx = 0; idx < 4; ++idx) {
                int new_r = r + delta[idx].first;
                int new_c = c + delta[idx].second;
                //이탈, 혹은 어항이 아닌 곳에다가 물고기를 옮기면 안된다.
                if (new_r < 0 || new_r >= 10 || new_c < 0 || new_c >= 25 || square[new_r][new_c] == 0) continue;
                int move_cnt = abs(square[r][c] - square[new_r][new_c]) / 5;
                if (move_cnt > 0) {
                    if (square[r][c] > square[new_r][new_c]) {
                        diff[r][c] -= move_cnt;
                        diff[new_r][new_c] += move_cnt;
                    }
                    else {
                        diff[r][c] += move_cnt;
                        diff[new_r][new_c] -= move_cnt;
                    }
                }
            }
        }
    }

    for (int r = 0; r < 10; ++r) {
        for (int c = 0; c < 25; ++c) {
            square[r][c] += diff[r][c] / 2;
        }
    }
}

//배치
void recover() {
    //재배치는 말아오르기 없이 특정 순서로 하면 되어서 쉽다.
    int line_idx = 0;
    for (int c = 0; c < 25; ++c) {
        for (int r = 9; r >= 0; --r) {
            if (square[r][c] == 0) continue;
            line[line_idx++] = square[r][c];
            if (line_idx >= N) return;
        }
    }
}

int main() {
    cin >> N >> K;
    for (int i = 0; i < N; ++i) {
        cin >> line[i];
    }
    int cnt = 0;
    int diff = calc_diff();
    calc_base_len();
    while (diff > K) {
        ++cnt;
        int min_val = find_min();
        add_fish(min_val);
        initialize();
        modify_one();
        clean();
        recover();
        initialize();
        modify_two();
        clean();
        recover();
        diff = calc_diff();
    }

    cout << cnt << flush;;

    return 0;
}