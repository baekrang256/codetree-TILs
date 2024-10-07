#include <iostream>
#include <vector>

#define MAP_LEN 4

using namespace std;

int m, t;

//ghost related
struct Ghost {
    int r;
    int c;
    int dir;
};
vector<int> map[MAP_LEN + 1][MAP_LEN + 1];
vector<int> replicates[MAP_LEN + 1][MAP_LEN + 1]; //eggs waiting to hatch. has dir entry.
int corpse_cnt[MAP_LEN + 1][MAP_LEN + 1] = { 0 }; //corpse turn left count.
vector<Ghost> moving_ghosts; //container to hold moving ghosts


//pacman related
pair<int, int> pacman_pos;
vector<pair<int, int>> arb_route(3); //arbitrary route for route selection.
int max_ghost_eaten; //used for choosing route : route with most eaten ghost
vector<pair<int, int>> route(3); //chosen route to move
bool visited[MAP_LEN + 1][MAP_LEN + 1];

pair<int, int> delta[9] = {
    {0, 0},{-1, 0},{-1, -1},{0, -1},{1, -1},{1, 0},{1, 1},{0, 1},{-1, 1}
};

void debug_print(int turn) {
    cout << "turn : " << turn << "\n";
    cout << "map\n";
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            cout << "{ ";
            for (int dir : map[r][c]) {
                cout << dir << " ";
            }
            cout << "} ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << pacman_pos.first << " " << pacman_pos.second << "\n";
}

bool out_of_bound(int r, int c) {
    return r < 1 || r > MAP_LEN || c < 1 || c > MAP_LEN;
}

void replicate_trial() {
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            replicates[r][c].clear();
            if (!map[r][c].empty()) {
                for (int ghost : map[r][c]) {
                    replicates[r][c].push_back(ghost);
                }
            }
        }
    }
}

void monster_move() {
    moving_ghosts.clear();

    //obtain movable ghosts and initialize map
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            if (!map[r][c].empty()) {
                for (int ghost : map[r][c]) {
                    moving_ghosts.push_back({ r, c, ghost });
                }
                map[r][c].clear();
            }
        }
    }

    //now move each ghost
    for (Ghost ghost : moving_ghosts) {
        bool moved = false;
        for (int idx = 0; idx < 8; ++idx) {
            int dir = ghost.dir + idx;
            if (dir > 8) dir -= 8;

            int new_r = ghost.r + delta[dir].first;
            int new_c = ghost.c + delta[dir].second;

            //not out of bound, no pacman, no corpse => ok
            if (!out_of_bound(new_r, new_c) && (new_r != pacman_pos.first || new_c != pacman_pos.second)
                && (corpse_cnt[new_r][new_c] <= 0)) {
                map[new_r][new_c].push_back(dir);
                moved = true;
                break;
            }
        }

        if (!moved) {
            map[ghost.r][ghost.c].push_back(ghost.dir);
        }
    }
}

void initialize_visited() {
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            visited[r][c] = false;
        }
    }
}

int solve_route() {
    int cnt = 0;
    initialize_visited();
    for (auto pos : arb_route) {
        if (!map[pos.first][pos.second].empty() && !visited[pos.first][pos.second]) {
            cnt += map[pos.first][pos.second].size();
            visited[pos.first][pos.second] = true;
        }
    }
    return cnt;
}

void choose_and_select(int turn, int cur_r, int cur_c) {
    if (turn >= 3) {
        int cnt = solve_route();
        if (cnt > max_ghost_eaten) {
            for (int idx = 0; idx < 3; ++idx) {
                route[idx] = arb_route[idx];
            }
            max_ghost_eaten = cnt;
        }
        return;
    }

    for (int idx = 0; idx < 4; ++idx) {
        int new_r = cur_r + delta[idx * 2 + 1].first;
        int new_c = cur_c + delta[idx * 2 + 1].second;
        if (out_of_bound(new_r, new_c)) continue;
        arb_route[turn].first = new_r;
        arb_route[turn].second = new_c;
        choose_and_select(turn + 1, new_r, new_c);
    }
}

void pacman_move() {
    max_ghost_eaten = -1;
    choose_and_select(0, pacman_pos.first, pacman_pos.second);

    for (auto pos : route) {
        //cout << pos.first << " " << pos.second << "\n";
        if (!map[pos.first][pos.second].empty()) {
            map[pos.first][pos.second].clear();
            corpse_cnt[pos.first][pos.second] = 3;
        }
    }

    pacman_pos.first = route[2].first;
    pacman_pos.second = route[2].second;
}

void corpse_vanish() {
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            if (corpse_cnt[r][c] >= 1) {
                corpse_cnt[r][c] -= 1;
            }
        }
    }
}

void replicate_complete() {
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            if (!replicates[r][c].empty()) {
                for (int ghost : replicates[r][c]) {
                    map[r][c].push_back(ghost);
                }
            }
        }
    }
}

int count_ghosts() {
    int cnt = 0;
    for (int r = 1; r <= MAP_LEN; ++r) {
        for (int c = 1; c <= MAP_LEN; ++c) {
            cnt += map[r][c].size();
        }
    }
    return cnt;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    //obtain data
    cin >> m >> t;
    cin >> pacman_pos.first >> pacman_pos.second;
    for (int ghost = 0; ghost < m; ++ghost) {
        int r, c, d;
        cin >> r >> c >> d;
        map[r][c].push_back(d);
    }

    //debug_print(0);

    //simulate
    for (int turn = 1; turn <= t; ++turn) {
        replicate_trial();
        monster_move();
        //debug_print(turn);
        pacman_move();
        //debug_print(turn);
        corpse_vanish();
        replicate_complete();
        //debug_print(turn);
    }

    //print left ghost count.
    cout << count_ghosts() << flush;

    return 0;
}