#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

#define MAX_N 20
#define MAX_M 5

#define CLOCK 0
#define CNT_CLOCK 1

#define EMPTY 0
#define HEAD 1
#define MATE 2
#define TAIL 3
#define ROUTE 4

using namespace std;

int n, m, k;
bool visited[MAX_N][MAX_N];
int base_map[MAX_N][MAX_N], 
    route_map[MAX_N][MAX_N],
    mates_map[MAX_N][MAX_N]; //base map, each team's route map, each team's mates map.

pair<int, int> delta[4] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
};

vector<pair<int, int>> teams[MAX_M + 1]; //position of each team's teammates
int scores = 0; //total scores


void debug_print(int turn) {
    cout << "after turn " << turn << "\n";
    //cout << "base map\n";
    //for (int r = 0; r < n; ++r) {
    //    for (int c = 0; c < n; ++c) {
    //        cout << base_map[r][c] << " ";
    //    }
    //    cout << "\n";
    //}
    //cout << "\n";
    //cout << "route_map\n";
    //for (int r = 0; r < n; ++r) {
    //    for (int c = 0; c < n; ++c) {
    //        cout << route_map[r][c] << " ";
    //    }
    //    cout << "\n";
    //}
    //cout << "\n";
    cout << "mates_map\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << mates_map[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool out_of_bound(int r, int c) {
    return r < 0 || r >= n || c < 0 || c >= n;
}

void initialize_visited() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            visited[r][c] = false;
        }
    }
}

//mark specific team's route 
void bfs_mark(int r, int c, int route_num) {
    initialize_visited();
    queue<pair<int, int>> bfs_queue;
    bfs_queue.push({ r, c });
    visited[r][c] = true;

    while (!bfs_queue.empty()) {
        int cur_r = bfs_queue.front().first;
        int cur_c = bfs_queue.front().second;
        bfs_queue.pop();
        route_map[cur_r][cur_c] = route_num;

        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;
            if (out_of_bound(new_r, new_c) || visited[new_r][new_c]) continue;
            if (base_map[new_r][new_c] != EMPTY) {
                visited[new_r][new_c] = true;
                bfs_queue.push({ new_r, new_c });
            }
        }
    }
}

//obtain information of every teammate in each team
//we also calculate the direction of team here.
void bfs_team(int r, int c, int team_num) {

    int tail_r, tail_c; //record and push it at last resort.

    initialize_visited();
    queue<pair<int, int>> bfs_queue;
    bfs_queue.push({ r, c });
    teams[team_num].push_back({ r, c });
    visited[r][c] = true;

    while (!bfs_queue.empty()) {
        int cur_r = bfs_queue.front().first;
        int cur_c = bfs_queue.front().second;
        bfs_queue.pop();
        mates_map[cur_r][cur_c] = team_num;

        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;
            if (out_of_bound(new_r, new_c) || visited[new_r][new_c]) continue;
            if (base_map[new_r][new_c] == MATE) {
                visited[new_r][new_c] = true;
                bfs_queue.push({ new_r, new_c });
                teams[team_num].push_back({ new_r, new_c });
            }
            else if (base_map[new_r][new_c] == TAIL) {
                visited[new_r][new_c] = true;
                bfs_queue.push({ new_r, new_c });
                tail_r = new_r, tail_c = new_c;
            }
        }
    }

    teams[team_num].push_back({tail_r, tail_c});
}

//calculate score, add it to team score
//then, switch direction by reversing the vector
void score_and_simulate(int r, int c) {
    auto &team = teams[mates_map[r][c]];
    for (int idx = 0; idx < team.size(); ++idx) {
        if (r == team[idx].first && c == team[idx].second) {
            scores += (idx + 1) * (idx + 1);
            reverse(team.begin(), team.end());
            return;
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    //obtain data
    cin >> n >> m >> k;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cin >> base_map[r][c];
        }
    }

    //obtain each team's route and save it to route map
    int route_cnt = 1;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (base_map[r][c] != EMPTY && route_map[r][c] == EMPTY) {
                bfs_mark(r, c, route_cnt++);
            }
        }
    }

    //obtain each team's teammate and find out it's position and direction
    //save it at mates_map and separate data structure
    int team_cnt = 1;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (base_map[r][c] == HEAD) {
                bfs_team(r, c, team_cnt++);
            }
        }
    }

    //debug_print(-1);

    for (int turn = 0; turn < k; ++turn) {
        //1. move. For tail, we erase it's previous position
        // then all except head follows it's front position
        // then head choose the position not occupied, while it's in the route.
        for (int team_num = 1; team_num <= m; ++team_num) {
            int prev_tail_r = teams[team_num].rbegin()->first, prev_tail_c = teams[team_num].rbegin()->second;
            for (int mate_idx = teams[team_num].size() - 1; mate_idx >= 1; --mate_idx) {
                teams[team_num][mate_idx] = teams[team_num][mate_idx - 1];
            }
            mates_map[prev_tail_r][prev_tail_c] = EMPTY;
            int prev_head_r = teams[team_num].begin()->first;
            int prev_head_c = teams[team_num].begin()->second;
            for (int idx = 0; idx < 4; ++idx) {
                int new_r = prev_head_r + delta[idx].first;
                int new_c = prev_head_c + delta[idx].second;
                if (!out_of_bound(new_r, new_c) && route_map[new_r][new_c] != EMPTY
                    && mates_map[new_r][new_c] == EMPTY) {
                    teams[team_num][0].first = new_r;
                    teams[team_num][0].second = new_c;
                    mates_map[new_r][new_c] = team_num;
                    break;
                }
            }

        }

        //2. throw ball, calculate score, switch direction
        int throw_idx = turn % (4 * n);
        if (throw_idx < n) {
            int r = throw_idx;
            for (int c = 0; c < n; ++c) {
                if (mates_map[r][c] != EMPTY) {
                    score_and_simulate(r, c);
                    break;
                }
            }
        }
        else if (throw_idx < 2 * n) {
            int c = throw_idx % n;
            for (int r = n - 1; r >= 0; --r) {
                if (mates_map[r][c] != EMPTY) {
                    score_and_simulate(r, c);
                    break;
                }
            }
        }
        else if (throw_idx < 3 * n) {
            int r = (n-1) - (throw_idx % n);
            for (int c = n - 1; c >= 0; --c) {
                if (mates_map[r][c] != EMPTY) {
                    score_and_simulate(r, c);
                    break;
                }
            }
        }
        else {
            int c = (n-1) - (throw_idx % n);
            for (int r = 0; r < n; ++r) {
                if (mates_map[r][c] != EMPTY) {
                    score_and_simulate(r, c);
                    break;
                }
            }
        }

        //debug_print(turn);
    }

    //print each team's score.
    cout << scores << "\n";

    return 0;
}