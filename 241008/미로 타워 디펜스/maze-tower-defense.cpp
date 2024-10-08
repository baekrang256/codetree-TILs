#include <iostream>
#include <vector>

#define EMPTY 0

using namespace std;

int N, M;
int explode_cnt[4] = { 0 }; //1~3번 구슬 폭발 개수
int marbles[49][49] = { 0 }; //구슬 모음
pair<int, int> delta[5] = { {0, 0}, {-1, 0}, {1, 0}, {0, -1}, {0, 1} };
int d_idx[4] = { 3, 2, 4, 1 }; //보드 순회시 회전 순서
int tot_marble_cnt = 0;

void debug_print() {
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            cout << marbles[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void explosion(vector<pair<int, int>>& v, int marble_num, int marble_cnt) {
    for (auto pos : v) {
        marbles[pos.first][pos.second] = EMPTY;
    }
    explode_cnt[marble_num] += marble_cnt;
    tot_marble_cnt -= marble_cnt;
}

int main() {
    cin >> N >> M;
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            cin >> marbles[r][c];
            if (marbles[r][c] != EMPTY) tot_marble_cnt++;
        }
    }

    int d, s;
    for (int magic_cnt = 0; magic_cnt < M; ++magic_cnt) {
        //마법 발동 쿠와아
        //cout << "magic no." << magic_cnt + 1 << "\n\n";
        cin >> d >> s;

        int cur_r = (N - 1) / 2, cur_c = (N - 1) / 2;
        int base_len, len, d_idx_idx, cur_marble, cur_marble_cnt; bool change;
        vector<pair<int, int>> v;

        while (s > 0) {
            --s;
            cur_r += delta[d].first;
            cur_c += delta[d].second;
            if (marbles[cur_r][cur_c] != EMPTY) {
                marbles[cur_r][cur_c] = EMPTY;
                tot_marble_cnt--;
            }
        }
        //cout << "started blizzard " << magic_cnt << "\n";
        //debug_print();

        //이동 + 폭발한다! 무한 반복.
        while (true) {
            //이동
            base_len = 1; len = 1; d_idx_idx = 0; change = false;
            cur_r = (N - 1) / 2 + delta[d_idx[d_idx_idx]].first;
            cur_c = (N - 1) / 2 + delta[d_idx[d_idx_idx]].second;
            int empty_cnt = 0;
            v.clear();
            int cur_cnt = 0;
            while (cur_r >= 0 && cur_r < N && cur_c >= 0 && cur_c < N) {
                //구슬 옮기기
                v.push_back({ cur_r, cur_c });

                if (marbles[cur_r][cur_c] == EMPTY) {
                    empty_cnt++;
                }
                else {
                    auto next_pos = v[v.size() - 1 - empty_cnt];
                    marbles[next_pos.first][next_pos.second] = marbles[cur_r][cur_c];
                }

                //잉여 구슬 제거
                if (cur_cnt >= tot_marble_cnt) {
                    marbles[cur_r][cur_c] = EMPTY;
                }
                cur_cnt++;

                //이동
                len--;
                if (len == 0) {
                    if (change) {
                        base_len++;
                    }
                    len = base_len;
                    d_idx_idx = (d_idx_idx + 1) % 4;
                    change = !change;
                }

                cur_r += delta[d_idx[d_idx_idx]].first;
                cur_c += delta[d_idx[d_idx_idx]].second;
            }

            //cout << "after movement" << "\n";
            //debug_print();

            //폭발한다!
            bool exploded = false;
            cur_marble_cnt = 0;
            base_len = 1; len = 1; d_idx_idx = 0; change = false;
            cur_r = (N - 1) / 2 + delta[d_idx[d_idx_idx]].first;
            cur_c = (N - 1) / 2 + delta[d_idx[d_idx_idx]].second;
            cur_marble = -1;
            v.clear();
            while (cur_r >= 0 && cur_r < N && cur_c >= 0 && cur_c < N) {

                if (cur_marble != marbles[cur_r][cur_c]) {
                    //구슬이 바뀜.
                    //일단 폭발 여부를 확인해야 함
                    if (cur_marble_cnt >= 4) {
                        explosion(v, cur_marble, cur_marble_cnt);
                        exploded = true;
                    }
                    //그리고 새로운 구슬로 대체
                    cur_marble_cnt = 1;
                    cur_marble = marbles[cur_r][cur_c];
                    v.clear();
                }
                else { //기존과 동일한 구슬
                    cur_marble_cnt += 1;
                }

                //이동 + 잡다한 조작
                v.push_back({ cur_r, cur_c });
                len--;
                if (len == 0) {
                    if (change) {
                        base_len++;
                    }
                    len = base_len;
                    d_idx_idx = (d_idx_idx + 1) % 4;
                    change = !change;
                }

                cur_r += delta[d_idx[d_idx_idx]].first;
                cur_c += delta[d_idx[d_idx_idx]].second;

            }
            //마지막 iteration에 대한 폭발 여부를 확인해야 함.
            if (cur_marble != 0 && cur_marble_cnt >= 4) {
                explosion(v, cur_marble, cur_marble_cnt);
                exploded = true;
            }

            //폭발이 없으면 탈출.
            if (!exploded) break;

            //cout << "after explosion" << "\n";
            //debug_print();
        }
        //cout << "explosion over" << "\n\n";

        //변화
        base_len = 1; len = 1; d_idx_idx = 0; change = false;
        cur_r = (N - 1) / 2 + delta[d_idx[d_idx_idx]].first;
        cur_c = (N - 1) / 2 + delta[d_idx[d_idx_idx]].second;
        cur_marble = marbles[cur_r][cur_c];
        cur_marble_cnt = 0;
        v.clear();
        //일단 변화 후 추가되는 구슬들 통계를 v에다가 저장하자.
        while (cur_r >= 0 && cur_r < N && cur_c >= 0 && cur_c < N) {
            //변화할게 더이상 없음
            if (marbles[cur_r][cur_c] == EMPTY) {
                if (cur_marble_cnt != 0) {
                    v.push_back({ cur_marble_cnt, cur_marble });
                }
                break;
            }

            if (cur_marble != marbles[cur_r][cur_c]) {
                //구슬이 바뀜. 통계를 내고 v에다가 저장하자.
                v.push_back({ cur_marble_cnt, cur_marble });
                cur_marble_cnt = 1;
                cur_marble = marbles[cur_r][cur_c];
            }
            else {
                cur_marble_cnt += 1;
            }

            //이동
            len--;
            if (len == 0) {
                if (change) {
                    base_len++;
                }
                len = base_len;
                d_idx_idx = (d_idx_idx + 1) % 4;
                change = !change;
            }

            cur_r += delta[d_idx[d_idx_idx]].first;
            cur_c += delta[d_idx[d_idx_idx]].second;

        }

        //이제 변화하는 구슬들을 최대한 집어넣자.
        base_len = 1; len = 1; d_idx_idx = 0; change = false;
        cur_r = (N - 1) / 2 + delta[d_idx[d_idx_idx]].first;
        cur_c = (N - 1) / 2 + delta[d_idx[d_idx_idx]].second;
        tot_marble_cnt = 0;
        int cur_idx = 0;
        bool put_first = true;

        while (cur_r >= 0 && cur_r < N && cur_c >= 0 && cur_c < N) {
            if (cur_idx >= v.size()) {
                marbles[cur_r][cur_c] = EMPTY;
            }
            else if (put_first) {
                ++tot_marble_cnt;
                marbles[cur_r][cur_c] = v[cur_idx].first;
            }
            else {
                ++tot_marble_cnt;
                marbles[cur_r][cur_c] = v[cur_idx].second;
                cur_idx += 1;
            }
            put_first = !put_first;

            //이동
            len--;
            if (len == 0) {
                if (change) {
                    base_len++;
                }
                len = base_len;
                d_idx_idx = (d_idx_idx + 1) % 4;
                change = !change;
            }

            cur_r += delta[d_idx[d_idx_idx]].first;
            cur_c += delta[d_idx[d_idx_idx]].second;

        }

        //cout << "after blizzard " << magic_cnt << "\n";
        //debug_print();
    }

    //결과 발표
    cout << explode_cnt[1] + explode_cnt[2] * 2 + explode_cnt[3] * 3 << flush;

    return 0;
}