#include <iostream>
#include <climits>

using namespace std;

int N, M, P, C, D;
int board[51][51] = { 0 }; //루돌프랑 산타 번호가 입력되어 있음. (루돌프 번호는 필요 없을지도)
bool santa_out[31] = { false }; //산타가 보드 밖으로 나갔는가?
int santa_alive_cnt; //산타 몇 명이 여전히 플레이 가능한가?
bool santa_stunned[31] = { false }; //산타가 기절했는가?
int santa_stun_turn[31] = { 0 }; //산타가 기절한 턴
int santa_score[31] = { 0 }; //산타의 현재 점수
pair<int, int> santa_pos[31]; //산타 위치 정보
int rudolf_r, rudolf_c; //루돌프 위치 정보
int turn = 0; //현재 턴

pair<int, int> santa_delta[4] = {
    {0, -1}, {1, 0}, {0, 1}, {-1, 0}
};

void debug_print() {
    cout << "board status\n";
    for (int r = 1; r <= N; ++r) {
        for (int c = 1; c <= N; ++c) {
            cout << board[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "santa stun status\n";
    for (int i = 1; i <= P; ++i) {
        cout << santa_stunned[i] << " ";
    }
    cout << "\n\n";
    cout << "santa out status\n";
    for (int i = 1; i <= P; ++i) {
        cout << santa_out[i] << " ";
    }
    cout << "\n\n";
    cout << "santa stun turn status\n";
    for (int i = 1; i <= P; ++i) {
        cout << santa_stun_turn[i] << " ";
    }
    cout << "\n\n";
    cout << "santa score status\n";
    for (int i = 1; i <= P; ++i) {
        cout << santa_score[i] << " ";
    }
    cout << "\n\n";
    cout << "santa position status\n";
    for (int i = 1; i <= P; ++i) {
        cout << "{" << santa_pos[i].first << ", " << santa_pos[i].second << "} ";
    }
    cout << "\n\n";
    cout << "rudolf position status\n";
    cout << rudolf_r << " " << rudolf_c;
    cout << "\n\n";
    cout << "santa alive count\n";
    cout << santa_alive_cnt << "\n\n";
}

//거리 계산
int dist(int r1, int c1, int r2, int c2) {
    int val1 = abs(r1 - r2);
    int val2 = abs(c1 - c2);
    return val1 * val1 + val2 * val2;
}

//산타 우선순위 계산 용
bool higher_prior_santa(int min_r, int min_c, int r, int c) {
    if (min_r != r) {
        return min_r < r;
    }
    return min_c < c;
}

//맵 이탈 여부 확인
bool out_of_bound(int r, int c) {
    return r < 1 || r > N || c < 1 || c > N;
}

void collision_handle(int r, int c, int delta_r, int delta_c, int is_rudolf, int cur_santa) {
    int score = is_rudolf ? C : D;
    board[r][c] = 0;
    r += delta_r * score;
    c += delta_c * score;
    santa_score[cur_santa] += score;
    santa_stunned[cur_santa] = true;
    santa_stun_turn[cur_santa] = turn;
    if (out_of_bound(r, c)) {
        //충돌 후 이탈
        santa_out[cur_santa] = true;
        santa_alive_cnt -= 1;
        return;
    }

    int past_santa = board[r][c];
    board[r][c] = cur_santa;
    santa_pos[cur_santa].first = r;
    santa_pos[cur_santa].second= c; 
    while (past_santa != 0) {
        //이탈은 안했는데 거기에 산타가 있네
        cur_santa = past_santa;
        r += delta_r;
        c += delta_c;
        //cout << r << " " << c << "\n";
        if (out_of_bound(r, c)) {
            santa_out[cur_santa] = true;
            santa_alive_cnt -= 1;
            return;
        }
        past_santa = board[r][c];
        board[r][c] = cur_santa;
        santa_pos[cur_santa].first = r;
        santa_pos[cur_santa].second = c;
    }
}

//정보 수집
void initialize() {
    cin >> N >> M >> P >> C >> D;
    cin >> rudolf_r >> rudolf_c;
    for (int santa_idx = 1; santa_idx <= P; ++santa_idx) {
        int santa_num;
        cin >> santa_num;
        cin >> santa_pos[santa_num].first >> santa_pos[santa_num].second;
        board[santa_pos[santa_num].first][santa_pos[santa_num].second] = santa_num;
    }
    santa_alive_cnt = P;
}

void rudolf_turn() {
    //먼저, 가장 가까운 산타가 누구인지를 파악한다.
    int min_dist = INT_MAX;
    int min_dist_santa = -1, min_santa_r = -1, min_santa_c = -1;
    for (int santa_num = 1; santa_num <= P; ++santa_num) {
        if (santa_out[santa_num]) continue; //이탈한 산타는 고려 안 함
        int cur_santa_r = santa_pos[santa_num].first, cur_santa_c = santa_pos[santa_num].second;
        int santa_dist = dist(rudolf_r, rudolf_c, cur_santa_r, cur_santa_c);
        if (santa_dist < min_dist ||
           (min_dist == santa_dist && higher_prior_santa(min_santa_r, min_santa_c, cur_santa_r, cur_santa_c))) {
            min_dist = santa_dist;
            min_dist_santa = santa_num;
            min_santa_r = cur_santa_r;
            min_santa_c = cur_santa_c;
        }
    }

    //이후 해당 산타 방향으로 이동
    int delta_r = 0, delta_c = 0;
    if (rudolf_r < min_santa_r) delta_r = 1;
    else if (rudolf_r > min_santa_r) delta_r = -1;
    if (rudolf_c < min_santa_c) delta_c = 1;
    else if (rudolf_c > min_santa_c) delta_c = -1;

    rudolf_r += delta_r;
    rudolf_c += delta_c;

    //산타랑 충돌시 관련 지도 업데이트
    if (rudolf_r == min_santa_r && rudolf_c == min_santa_c) {
        collision_handle(min_santa_r, min_santa_c, delta_r, delta_c, true, min_dist_santa);
    }
}

void santa_turn() {
    for (int santa_num = 1; santa_num <= P; ++santa_num) {
        if (santa_stunned[santa_num] || santa_out[santa_num]) continue;
        int chosen_idx = -1;
        int santa_r = santa_pos[santa_num].first;
        int santa_c = santa_pos[santa_num].second;
        int rudolf_dist = dist(rudolf_r, rudolf_c, santa_r, santa_c);
        int min_dist = INT_MAX;
        for (int idx = 0; idx < 4; ++idx) {
            int new_r = santa_r + santa_delta[idx].first;
            int new_c = santa_c + santa_delta[idx].second;
            if (out_of_bound(new_r, new_c) || board[new_r][new_c] != 0) continue;
            int new_dist = dist(new_r, new_c, rudolf_r, rudolf_c);
            if (new_dist >= rudolf_dist) continue;
            if (min_dist >= new_dist) {
                chosen_idx = idx;
                min_dist = new_dist;
            }
        }
        if (chosen_idx == -1) continue; //정해진 것이 없음

        //일단 한 번 이동.
        board[santa_r][santa_c] = 0;
        santa_r += santa_delta[chosen_idx].first;
        santa_c += santa_delta[chosen_idx].second;
        //cout << santa_delta[chosen_idx].first << " " << santa_delta[chosen_idx].second << "\n";

        if (rudolf_r == santa_r && rudolf_c == santa_c) {//그런데 거기에 루돌프가 있네
            collision_handle(santa_r, santa_c, -santa_delta[chosen_idx].first, -santa_delta[chosen_idx].second, false, santa_num);
        }
        else { //그런데 거기에 루돌프가 없네
            board[santa_r][santa_c] = santa_num;
            santa_pos[santa_num].first = santa_r;
            santa_pos[santa_num].second = santa_c;
        }
    }
}

bool no_more_santa() {
    return santa_alive_cnt <= 0;
}

void stun_handle() {
    for (int i = 1; i <= P; ++i) {
        if (!santa_out[i] && santa_stunned[i] && (turn > santa_stun_turn[i])) {
            santa_stunned[i] = false;
        }
    }
}

void santa_score_handle() {
    for (int i = 1; i <= P; ++i) {
        if (!santa_out[i]) {
            santa_score[i] += 1;
        }
    }
}

int main() {
    initialize();
    while (turn++ < M) {
        //cout << "turn " << turn << "\n";
        rudolf_turn();
        if (no_more_santa()) break;
        //cout << "####################\nafter rudolf\n####################\n";
        //debug_print();
        santa_turn();
        if (no_more_santa()) break;
        //cout << "####################\nafter santa\n####################\n";
        //debug_print();
        stun_handle();
        santa_score_handle();
        //cout << "####################\nafter handling\n####################\n";
        //debug_print();
    }
    for (int santa_num = 1; santa_num <= P; ++santa_num) {
        cout << santa_score[santa_num] << " ";
    }
    cout << flush;

    return 0;
}