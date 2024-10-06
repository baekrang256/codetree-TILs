#include <iostream>
#include <queue>

#define MAX_N 20

#define EMPTY 0
#define WALL -1

#define NONE -1

using namespace std;

int n, m, k, c;
int map[MAX_N][MAX_N]; //base map
bool spoiled[MAX_N][MAX_N] = { false }; //is that place spoiled?
int spoil_cnt[MAX_N][MAX_N] = { 0 }; //spoiled area left year cnt
int growth_cnt[MAX_N][MAX_N]; //this year's tree growth
long long int dead_cnt = 0; //dead tree cnt

pair<int, int> par_delta[4] = {
    {1, 0}, {0, 1}, {-1, 0}, {0, -1}
};
pair<int, int> diag_delta[4] = {
    {-1, -1}, {-1, 1}, {1, -1}, {1, 1}
};

void debug_print(int year) {
    cout << "year : " << year << "\n";
    cout << "map\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << map[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "growth_cnt\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << growth_cnt[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "spoil_cnt\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << spoil_cnt[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool out_of_bound(int r, int c) {
    return r < 0 || r >= n || c < 0 || c >= n;
}

//iterate each tree. check nearby trees and update it's growth.
//this only happens for area where tree already existed.
void grow() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (map[r][c] > 0) { //tree exists
                int increment_cnt = 0;
                for (int idx = 0; idx < 4; ++idx) {
                    int new_r = r + par_delta[idx].first;
                    int new_c = c + par_delta[idx].second;
                    if (!out_of_bound(new_r, new_c) && map[new_r][new_c] > 0) {
                        //there's a tree at the other direction.
                        ++increment_cnt;
                    }
                }
                map[r][c] += increment_cnt;
            }
        }
    }
}

void initialize_growth_cnt() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            growth_cnt[r][c] = 0;
        }
    }
}

//reproducing part. algorithm is like below
//1. initialize growth_cnt
//2. for each trees, update it's nearby area of how much trees will it plant. data updated to growth_cnt
//3. update the map according to growth_cnt
void reproduce() {
    //stage 1
    initialize_growth_cnt();

    //stage 2
    vector<pair<int, int>> fill_position;
    int fillable_cnt;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (map[r][c] > 0) { //tree exists
                fillable_cnt = 0;
                fill_position.clear();
                for (int idx = 0; idx < 4; ++idx) {
                    int new_r = r + par_delta[idx].first;
                    int new_c = c + par_delta[idx].second;
                    if (!out_of_bound(new_r, new_c) && map[new_r][new_c] == EMPTY
                        && !spoiled[new_r][new_c]) {
                        //there is an empty space that could be filled in other direction (not spoiled)
                        fillable_cnt += 1;
                        fill_position.push_back({ new_r, new_c });
                    }
                }

                for (auto pos : fill_position) {
                    growth_cnt[pos.first][pos.second] += map[r][c] / fillable_cnt;
                }
            }
        }
    }

    //stage 3
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            map[r][c] += growth_cnt[r][c];
        }
    }
}

//for each position, we try counting how much tress will it spoil
//after calculation, check if it's larger than maximum and select that if it's larger
//final maximum r and c will be used to spoil the trees
// mark the spoiled area at map.
//we also need to update the spoil cnt. this should be 'c+1'. since, we decrement all count on next stage
void spoil() {
    int max_r = 0, max_c = 0; //if we can't destroy trees anywhere, spoil 0, 0.
    int max_spoil_cnt = 0;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (map[r][c] > 0) {
                //spoil is effective only when it's done on tree position.
                int spoil_cnt = map[r][c]; //spoils current position
                for (int idx = 0; idx < 4; ++idx) {
                    for (int len = 1; len <= k; ++len) {
                        int new_r = r + (diag_delta[idx].first * len);
                        int new_c = c + (diag_delta[idx].second * len);

                        //no trees? stop spoiling that direction.
                        if (out_of_bound(new_r, new_c) || map[new_r][new_c] <= 0) {
                            break;
                        }

                        spoil_cnt += map[new_r][new_c];
                    }
                }
                if (spoil_cnt > max_spoil_cnt) {
                    max_r = r;
                    max_c = c;
                    max_spoil_cnt = spoil_cnt;
                }
            }
        }
    }

    //now we actually spoil it.
    if (max_spoil_cnt == 0) {
        //...actually nothing to spoil.
        return;
    }

    dead_cnt += map[max_r][max_c];
    map[max_r][max_c] = EMPTY;
    spoiled[max_r][max_c] = true;
    spoil_cnt[max_r][max_c] = c + 1;

    for (int idx = 0; idx < 4; ++idx) {
        for (int len = 1; len <= k; ++len) {
            int new_r = max_r + (diag_delta[idx].first * len);
            int new_c = max_c + (diag_delta[idx].second * len);

            //out of bound? stop it.
            if (out_of_bound(new_r, new_c)) break;

            //no more trees? spoil it and exit.
            if (map[new_r][new_c] <= 0) {
                spoiled[new_r][new_c] = true;
                spoil_cnt[new_r][new_c] = c + 1;
                break;
            }

            //spoil it.
            dead_cnt += map[new_r][new_c];
            map[new_r][new_c] = EMPTY;
            spoiled[new_r][new_c] = true;
            spoil_cnt[new_r][new_c] = c + 1;
        }
    }
}

//decremnt all spoil cnt by one for all area.
//if it became zero, mark it at map.
void update_spoil_cnt() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (spoil_cnt[r][c] > 0) {
                spoil_cnt[r][c] -= 1;
                if (spoil_cnt[r][c] == 0) {
                    spoiled[r][c] = false;
                }
            }

        }
    }
}

int main() {
    //obtain data
    cin >> n >> m >> k >> c;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cin >> map[r][c];
        }
    }

    //for each year, simulate like below
    for (int year = 1; year <= m; ++year) {
        grow();
        reproduce();
        spoil();
        update_spoil_cnt();
        //debug_print(year);
    }

    cout << dead_cnt << "\n";

    return 0;
}