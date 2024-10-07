#include <iostream>
#include <vector>
#include <queue>

#define MAX_N 29

using namespace std;

//related to basic and scoring
int n;
int art[MAX_N][MAX_N]; //original map
int area_map[MAX_N][MAX_N]; //map with area numbers
int arb_map[MAX_N][MAX_N]; //arb map used for rotating
int tot_score = 0; //total score of initial/1/2/3
vector<int> areas_num; //from area 0, what is the number of that area?
vector<int> areas_size; //from area 0, how many blocks does that area possess?
vector<vector<int>> nearby_edge_cnt; //from area 0, what is the number of nearby edge?

//related to bfs
bool visited[MAX_N][MAX_N];
pair<int, int> delta[4] = {
    {-1, 0}, {0, -1}, {1, 0}, {0, 1}
};

void debug_print() {
    cout << "new print\n";
    cout << "art\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << art[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "area_map" << "\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << area_map[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "area_size" << "\n";
    for (int cnt : areas_size) {
        cout << cnt << " ";
    }
    cout << "\n\n";
    cout << "nearby_edge_cnt" << "\n";
    for (int area = 0; area < nearby_edge_cnt.size(); ++area) {
        cout << "{";
        for (int area2 = 0; area2 < nearby_edge_cnt.size(); ++area2) {
            cout << nearby_edge_cnt[area][area2] << ",";
        }
        cout << "}\n";
    }
    cout << "\n";
}

void initialize_visited() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            visited[r][c] = false;
        }
    }
}

void initialize_areas() {
    areas_num.clear();
    areas_size.clear();
    nearby_edge_cnt.clear();
}

bool out_of_bound(int r, int c) {
    return r < 0 || r >= n || c < 0 || c >= n;
}

void bfs_area(int r, int c, int area_num) {
    visited[r][c] = true;
    queue<pair<int, int>> bfs_queue;
    bfs_queue.push({ r, c });
    int art_num = art[r][c];
    areas_num.push_back(art_num);
    areas_size.push_back(0);

    while (!bfs_queue.empty()) {
        int cur_r = bfs_queue.front().first;
        int cur_c = bfs_queue.front().second;
        bfs_queue.pop();
        area_map[cur_r][cur_c] = area_num;
        areas_size[area_num] += 1;
        for (int dir = 0; dir < 4; ++dir) {
            int new_r = cur_r + delta[dir].first;
            int new_c = cur_c + delta[dir].second;
            if (!out_of_bound(new_r, new_c) && !visited[new_r][new_c]
                && art[new_r][new_c] == art_num) {
                bfs_queue.push({ new_r, new_c });
                visited[new_r][new_c] = true;
            }
        }
    }

}

void bfs_near_edge(int r, int c) {
    visited[r][c] = true;
    queue<pair<int, int>> bfs_queue;
    bfs_queue.push({ r, c });
    int area_num = area_map[r][c];

    while (!bfs_queue.empty()) {
        int cur_r = bfs_queue.front().first;
        int cur_c = bfs_queue.front().second;
        bfs_queue.pop();
        for (int dir = 0; dir < 4; ++dir) {
            int new_r = cur_r + delta[dir].first;
            int new_c = cur_c + delta[dir].second;
            if (out_of_bound(new_r, new_c)) continue;

            int oth_area_num = area_map[new_r][new_c];
            if (oth_area_num != area_num) {
                nearby_edge_cnt[area_num][oth_area_num] += 1;
            }
            else if (!visited[new_r][new_c]) {
                bfs_queue.push({ new_r, new_c });
                visited[new_r][new_c] = true;
            }
        }
    }
    
}

//find areas, and record them
//then, we calculate the score based on that info.
int calc_score() {
    //check areas
    initialize_visited();
    initialize_areas();
    int area_cnt = 0;

    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!visited[r][c]) {
                bfs_area(r, c, area_cnt++);
            }
        }
    }

    //now check nearby edges between all areas
    nearby_edge_cnt.resize(area_cnt);
    for (int area_idx = 0; area_idx < area_cnt; ++area_idx) {
        nearby_edge_cnt[area_idx].resize(area_cnt);
    }

    initialize_visited();
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!visited[r][c]) {
                bfs_near_edge(r, c);
            }
        }
    }

    //now calculate score
    int score = 0;
    for (int area_idx = 0; area_idx < area_cnt; ++area_idx) {
        for (int oth_area_idx = area_idx + 1; oth_area_idx < area_cnt; ++oth_area_idx) {
            score +=
                (areas_size[area_idx] + areas_size[oth_area_idx])
                * areas_num[area_idx] * areas_num[oth_area_idx] * nearby_edge_cnt[area_idx][oth_area_idx];
        }
    }

    return score;
}

void rotate_cross(int c_r, int c_c) {
    //rotate cross
    for (int dir = 0; dir < 4; ++dir) {
        int oth_dir = (dir - 1);
        if (oth_dir < 0) oth_dir = 3;

        int new_r = c_r + delta[dir].first;
        int new_c = c_c + delta[dir].second;
        int oth_new_r = c_r + delta[oth_dir].first;
        int oth_new_c = c_c + delta[oth_dir].second;

        while (!out_of_bound(new_r, new_c)) {
            arb_map[new_r][new_c] = art[oth_new_r][oth_new_c];
            new_r += delta[dir].first;
            new_c += delta[dir].second;
            oth_new_r += delta[oth_dir].first;
            oth_new_c += delta[oth_dir].second;
        }
    }

    //copy it to original
    for (int dir = 0; dir < 4; ++dir) {
        int new_r = c_r + delta[dir].first;
        int new_c = c_c + delta[dir].second;
        while (!out_of_bound(new_r, new_c)) {
            art[new_r][new_c] = arb_map[new_r][new_c];
            new_r += delta[dir].first;
            new_c += delta[dir].second;
        }
    }
}

void rotate_square(int pivot_r, int pivot_c, int len) {
    //rotate square
    for (int r = 0; r < len; ++r) {
        for (int c = 0; c < len; ++c) {
            arb_map[r][c] = art[pivot_r + len - 1 - c][pivot_c + r];
        }
    }

    //copy it to original
    for (int r = 0; r < len; ++r) {
        for (int c = 0; c < len; ++c) {
            art[pivot_r + r][pivot_c + c] = arb_map[r][c];
        }
    }
}

//rotate the map
void rotate() {
    //1. rotate center cross
    rotate_cross(n / 2, n / 2);
    //2. rotate four squares
    rotate_square(0, 0, n / 2);
    rotate_square(0, n / 2 + 1, n / 2);
    rotate_square(n / 2 + 1, 0, n / 2);
    rotate_square(n / 2 + 1, n / 2 + 1, n / 2);
}

int main() {
    cin >> n;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cin >> art[r][c];
        }
    }

    //obtain initial score
    tot_score += calc_score();
    //debug_print();

    for (int cnt = 1; cnt <= 3; ++cnt) {
        rotate();
        tot_score += calc_score();
        //debug_print();
    }

    cout << tot_score;
}