#include <iostream>
#include <set>
#include <vector>
#include <unordered_map>
#include <climits>

using namespace std;

struct Rabbit {
    int pid;
    int r_pos;
    int c_pos;
    int jump_cnt;
};

//경주에서 토끼 우선순위 결정하는 비교 함수.
struct compare {
    bool operator() (const Rabbit& r1, const Rabbit& r2) const {
        if (r1.jump_cnt != r2.jump_cnt) return r1.jump_cnt < r2.jump_cnt;
        int r1_sum = r1.r_pos + r1.c_pos, r2_sum = r2.r_pos + r2.c_pos;
        if (r1_sum != r2_sum) return r1_sum < r2_sum;
        if (r1.r_pos != r2.r_pos) return r1.r_pos < r2.r_pos;
        if (r1.c_pos != r2.c_pos) return r1.c_pos < r2.c_pos;
        return r1.pid < r2.pid;
    }
};

int Q, cmd, N, M, P;
long long int cumu_score = 0;
set<Rabbit, compare> rabbits; //토끼들 모음. 우선순위에 따라 정렬
vector<bool> chosen; //특정 경주에서 이 토끼가 선택되었는지 여부
vector<int> dist; //특정 토끼의 한 턴 당 이동 거리.
vector<long long int> score; //특정 토끼의 점수.
unordered_map<int, int> rabbit_idx; //거리, 선택 여부, 점수 구할 때 사용될 index 찾는 용도. key는 pid.
pair<int, int> delta[4] = {
    {-1, 0}, {1, 0}, {0, 1}, {0, -1}
};

//디버깅 용도
void debug_print() {
    cout << "rabbit infos\n";
    for (const auto rabbit : rabbits) {
        cout << "{" << rabbit.pid << ", "
            << rabbit.r_pos << ", "
            << rabbit.c_pos << ", "
            << rabbit.jump_cnt << "}\n";
    }
    cout << "\n";

    cout << "scores for each rabbit in above order\n";
    for (const auto rabbit : rabbits) {
        int idx = rabbit_idx[rabbit.pid];
        cout << score[idx] << ", ";
    }
    cout << "\n\n";
}

//토끼 준비 과정
void prepare() {
    cin >> N >> M >> P;
    chosen.resize(P);
    dist.resize(P);
    score.resize(P);
    fill(score.begin(), score.end(), 0L);
    int idx = 0;
    for (int rabbit_cnt = 0; rabbit_cnt < P; ++rabbit_cnt) {
        int pid, d;
        cin >> pid >> d;
        rabbits.insert({ pid, 1, 1, 0 });
        rabbit_idx.insert({pid, idx});
        dist[idx++] = d;
    }
}

void race() {
    //정보 수집 및 초기화
    int K, S;
    cin >> K >> S;
    fill(chosen.begin(), chosen.end(), false);

    //경기 시작
    for (int turn = 0; turn < K; ++turn) {
        //가장 앞에 있는 놈 선택.
        auto it = rabbits.begin();
        Rabbit rabbit = *it;
        rabbits.erase(it);

        //선택 된 놈 표기
        int cur_idx = rabbit_idx[rabbit.pid];
        chosen[cur_idx] = true;
        //cout << rabbit.pid << " chosen\n";

        //상하좌우 네 방향으로 d만큼 이동했을 때의 위치를 기반으로 새 위치 우선순위 결정
        int rabbit_dist = dist[cur_idx];
        int sel_r = 0, sel_c = 0, sel_sum = 0;
        for (int idx = 0; idx < 4; ++idx) {
            int new_r = rabbit.r_pos + (rabbit_dist % (2 * (N-1))) * delta[idx].first;
            int new_c = rabbit.c_pos + (rabbit_dist % (2 * (M-1))) * delta[idx].second;
            while (new_r > N || new_r < 1) {
                if (new_r > N) new_r -= 2 * abs(new_r - N);
                else if (new_r < 1) new_r += 2 * abs(new_r - 1);
            }
            while (new_c > M || new_c < 1) {
                if (new_c > M) new_c -= 2 * abs(new_c - M);
                else if (new_c < 1) new_c += 2 * abs(new_c - 1);
            }
            //cout << new_r << ", " << new_c << "\n";
            int new_sum = new_r + new_c;
            if (sel_sum < new_sum
                || (sel_sum == new_sum && sel_r < new_r)
                || (sel_sum == new_sum && sel_r == new_r && sel_c < new_c)) {
                sel_r = new_r;
                sel_c = new_c;
                sel_sum = sel_r + sel_c;
            }
        }

        //cout << sel_r << ", " << sel_c << "\n";

        //정해졌다. 이제 점수를 갱신해야 하는데...
        //일일이 더하는것보다 현재만 빼고 누적 점수를 더하는게 더 효율적.
        cumu_score += sel_sum;
        score[cur_idx] -= sel_sum;

        //이후 토끼 정보 갱신 후 다시 집어넣는다.
        rabbit.jump_cnt += 1;
        rabbit.r_pos = sel_r;
        rabbit.c_pos = sel_c;
        rabbits.insert(rabbit);
    }

    //마지막에는 위치를 기준으로 가장 우선순위가 높은 토끼를 골라서 S를 더해야 하는데
    //선택된 토끼만 가능하기 때문에 그냥 전부 iterate 하면서 일일이 확인.
    int sel_r = 0, sel_c = 0, sel_sum = 0, sel_idx = -1, sel_pid = -1;
    for (auto it = rabbits.begin(); it != rabbits.end(); ++it) {
        int cur_idx = rabbit_idx[it->pid];
        if (!chosen[cur_idx]) continue;
        int cur_r = it->r_pos, cur_c = it->c_pos;
        int cur_sum = cur_r + cur_c;
        if (cur_sum > sel_sum
            || (cur_sum == sel_sum && cur_r > sel_r)
            || (cur_sum == sel_sum && cur_r == sel_r && cur_c > sel_c)
            || (cur_sum == sel_sum && cur_r == sel_r && cur_c == sel_c && it->pid > sel_pid)) {
            sel_sum = cur_sum;
            sel_r = cur_r;
            sel_c = cur_c;
            sel_pid = it->pid;
            sel_idx = cur_idx;
        }
    }
    score[sel_idx] += S;
}

//이동 거리 변화. 곱해야 한다.
void change_len() {
    int pid_t, L;
    cin >> pid_t >> L;
    dist[rabbit_idx[pid_t]] *= L;
}

//최고 점수 출력
void award() {
    long long int max_val = LLONG_MIN;
    for (int idx = 0; idx < P; ++idx) {
        if (max_val < score[idx]) {
            max_val = score[idx];
        }
    }
    cout << max_val + cumu_score << "\n";
}

int main() {
    cin >> Q;
    for (int query = 0; query < Q; ++query) {
        //cout << "query " << query << "\n";
        cin >> cmd;
        switch (cmd) {
        case 100:
            prepare();
            break;
        case 200:
            race();
            break;
        case 300:
            change_len();
            break;
        case 400:
            award();
            break;
        }
        //debug_print();
    }

    cout << flush;

    return 0;
}