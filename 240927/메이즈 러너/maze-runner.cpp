#include <iostream>
#include <vector>

#define EXIT (-1)

using namespace std;

int N, M, K;
int exit_r, exit_c;
int maze[11][11] = { 0 }; //미로
bool player[11][11][11] = { false }; //해당 위치에 몇번 참가자 있는가. 여러 명이 동시에 있을 수 있어서...
int tmp_maze[11][11]; //회전용
int tmp_player[11][11][11]; //회전용
int move_cnt = 0; //모든 참가자 이동 횟수.
pair<int, int> player_pos[11]; //참가자 위치
bool exited[11] = { false }; //해당 번호 플레이어 탈출 여부
int exit_cnt = 0; //참가자 탈출 명수
pair<int, int> delta[4] = {
    {-1, 0}, {1, 0}, {0, -1}, {0, 1}
};
pair<int, int> candidates[11];
int len[11];

int dist(int r1, int c1, int r2, int c2) {
    return abs(r1 - r2) + abs(c1 - c2);
}

void debug_print_maze() {
    cout << "maze status\n";
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= N; ++c) {
            cout << maze[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void debug_candidates() {
    cout << "candidate status\n";
    for (int p_num = 1; p_num <= M; ++p_num) {
        if (exited[p_num]) continue;
        cout << "{" << candidates[p_num].first << ", " << candidates[p_num].second << "}, ";
    }
    cout << "\n\n";
}

int main() {
    //기본 정보
    cin >> N >> M >> K;

    //미로 정보
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= N; ++c) {
            cin >> maze[r][c];
        }
    }

    //플레이어 정보
    for (int p_num = 1; p_num <= M; ++p_num) {
        int r, c;
        cin >> r >> c;
        player_pos[p_num].first = r;
        player_pos[p_num].second = c;
        player[r][c][p_num] = true;
    }

    //탈출구 정보
    cin >> exit_r >> exit_c;
    maze[exit_r][exit_c] = EXIT;

    //게임시간만큼 다음을 반복
    for (int turn = 0; turn < K; ++turn) {
        //플레이어 일단 지도에 지움.
        for (int p_num = 1; p_num <= M; ++p_num) {
            if (!exited[p_num])
                player[player_pos[p_num].first][player_pos[p_num].second][p_num] = false;
        }

        //플레이어 이동 시뮬레이션.
        for (int p_num = 1; p_num <= M; ++p_num) {
            if (exited[p_num]) continue; //이미 탈출한 애는 따질 필요 없음

            //현재 정보
            int r = player_pos[p_num].first;
            int c = player_pos[p_num].second;
            int cur_dist = dist(r, c, exit_r, exit_c);

            //우선순위대로 각 방향에 대해 다음 수행
            for (int dir_idx = 0; dir_idx < 4; ++dir_idx) {
                int new_r = r + delta[dir_idx].first;
                int new_c = c + delta[dir_idx].second;

                //맵 이탈, 벽 조우 시 문제가 됨.
                if (new_r < 1 || new_r > N || new_c < 1 || new_c > N || maze[new_r][new_c] > 0) {
                    continue;
                }

                //아니면 출구를 만나는 경우일수도? 이 경우 즉시 탈출.
                if (maze[new_r][new_c] == EXIT) {
                    move_cnt += 1;
                    exited[p_num] = true;
                    exit_cnt += 1;
                    break;
                }

                //그게 아니면 거리가 가까워지는지 확인하자.
                //가까워지면 그 즉시 해당 이동 방향을 선택한다. 우선순위는 이미 고려 됨.
                //위치 정보까지 전부 갱신.
                int new_dist = dist(new_r, new_c, exit_r, exit_c);
                if (new_dist < cur_dist) {
                    move_cnt += 1;
                    player_pos[p_num].first = new_r;
                    player_pos[p_num].second = new_c;
                    break;
                }
            }
        }

        //플레이어 전원 탈출 시 즉시 종료.
        if (exit_cnt >= M) break;

        //플레이어 이동 지도에 반영
        for (int p_num = 1; p_num <= M; ++p_num) {
            if (exited[p_num]) continue;
            player[player_pos[p_num].first][player_pos[p_num].second][p_num] = true;
        }

        //debug_print_maze();

        //조건에 맞는 회전용 정사각형 구하기
        //이는 각 플레이어를 포함하는 정사각형 중 가장 작은 정사각형을 선택하는 형태로 구할 것이다.
        //여러 방법을 고려해봤는데 이것이 제일 변수가 없는 것으로 판별 (다른 것은 예외사항이 있을 것 같음)
        for (int p_num = 1; p_num <= M; ++p_num) {
            if (exited[p_num]) continue;
            int cur_r = player_pos[p_num].first;
            int cur_c = player_pos[p_num].second;
            //일단 둘의 가로/세로 길이를 파악한다.
            int h = abs(exit_r - cur_r) + 1;
            int w = abs(exit_c - cur_c) + 1;

            //실제 정사각형 길이는 이 둘의 max값
            len[p_num] = max(h, w);

            //h랑 w가 같은 경우, r이 더 작은 애, c가 더 작은 애로 무조건 pivot을 골라야 한다.
            if (h == w) {
                candidates[p_num].first = min(exit_r, cur_r);
                candidates[p_num].second = min(exit_c, cur_c);
            }
            //h랑 w가 다른 경우가 문제인데 h가 더 작거나 w가 더 작을 것이다.
            //h가 더 작은 경우를 가정하면 일단 h상 더 위에 있는 녀석이 player인지 exit인지 확인
            //그 녀석의 r을 따르고, 이후 계속 r을 줄여본다.위로 더 이상 올리는 것이 불가능하면 거기서 멈춘다.
            //c는 player이랑 exit 중 더 작은 녀석을 따른다.
            else if (h < w) {
                int cand_r = min(exit_r, cur_r);
                int delta_len = w - h;
                while (cand_r > 1 && delta_len > 0) {
                    cand_r -= 1;
                    delta_len -= 1;
                }
                candidates[p_num].first = cand_r;
                candidates[p_num].second = min(exit_c, cur_c);
            }
            //w가 더 작은 경우도 비슷하다. 일단 w상 더 왼쪽에 있는 녀석이 player인지 exit인지 확인
            //그 녀석의 c를 따르고, 이후 계속 c를 줄여본다. 좌측으로 더 이상 가는 것이 불가능하면 거기서 멈춘다.
            //r은 player이랑 exit 중 더 작은 녀석을 따른다.
            else {
                int cand_c = min(exit_c, cur_c);
                int delta_len = h - w;
                while (cand_c > 1 && delta_len > 0) {
                    cand_c -= 1;
                    delta_len -= 1;
                }
                candidates[p_num].first = min(exit_r, cur_r);
                candidates[p_num].second = cand_c;
            }
        }

        //debug_candidates();

        //정해진 후보들 중 가장 길이가 작은 애, 길이가 동일하면 우선순위가 가장 높은 애를 선택.
        int sel_r = 20, sel_c = 20;
        int min_len = 10000000;
        for (int p_num = 1; p_num <= M; ++p_num) {
            if (exited[p_num]) continue;
            if (min_len > len[p_num]
                || (min_len == len[p_num] && sel_r > candidates[p_num].first)
                || (min_len == len[p_num] && sel_r == candidates[p_num].first && sel_c > candidates[p_num].second)) {
                sel_r = candidates[p_num].first;
                sel_c = candidates[p_num].second;
                min_len = len[p_num];
            }
        }

        //정사각형 회전
        //일단 해당 영역 복사.
        for (int r = sel_r; r < sel_r + min_len; ++r) {
            for (int c = sel_c; c < sel_c + min_len; ++c) {
                tmp_maze[r - sel_r][c - sel_c] = maze[r][c];
                for (int p_num = 1; p_num <= M; ++p_num) {
                    tmp_player[r - sel_r][c - sel_c][p_num] = player[r][c][p_num];
                }
            }
        }

        //그리고 이동. 이때 몇몇 정보 갱신 필요.
        //갱신 정보 : 출구 위치, 플레이어 위치, 벽 내구도
        int tmp_r = 0;
        for (int c = sel_c + min_len - 1; c >= sel_c; --c, ++tmp_r) {
            int tmp_c = 0;
            for (int r = sel_r; r < sel_r + min_len; ++r, ++tmp_c) {
                maze[r][c] = tmp_maze[tmp_r][tmp_c];
                if (maze[r][c] == EXIT) {
                    //출구 갱신
                    exit_r = r;
                    exit_c = c;
                }
                else if (maze[r][c] > 0) {
                    //벽 내구도 깎기
                    maze[r][c] -= 1;
                }

                for (int p_num = 1; p_num <= M; ++p_num) {
                    player[r][c][p_num] = tmp_player[tmp_r][tmp_c][p_num];
                    if (tmp_player[tmp_r][tmp_c][p_num]) {
                        player_pos[p_num].first = r;
                        player_pos[p_num].second = c;
                    }
                }
            }
        }

        //debug_print_maze();
    }

    //출력
    cout << move_cnt << "\n" << exit_r << " " << exit_c << flush;

    return 0;
}