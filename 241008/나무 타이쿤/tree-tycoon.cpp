#include <iostream>
#include <vector>

#define MAX_N 15

using namespace std;

int n, m;
int field[MAX_N + 1][MAX_N + 1]; //libro field
bool is_medical[MAX_N + 1][MAX_N + 1]; //have earned grower
int grow_cnt[MAX_N + 1][MAX_N + 1]; //amount to grow for each position in each year.
vector<pair<int, int>> med_pos; //medicine positions
vector<pair<int, int>> move_order; //first : dir, second : move_cnt (p)

pair<int, int> delta[9] = {
    {0, 0}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1},
    {0, -1}, {1, -1}, {1, 0}, {1, 1}
};

void debug_med_pos() {
    cout << "medicine positions\n";
    for (auto pos : med_pos) {
        cout << "{" << pos.first << " " << pos.second << "}, ";
    }
    cout << "\n\n";
}

void debug_grow_cnt() {
    cout << "grow_cnt\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << grow_cnt[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void debug_field() {
    cout << "field\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << field[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void obtain_data_and_setup() {
    //obtain data
    cin >> n >> m;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cin >> field[r][c];
        }
    }
    move_order.resize(m + 1);
    for (int year = 1; year <= m; ++year) {
        cin >> move_order[year].first >> move_order[year].second;
    }

    //initial medicine setup
    med_pos.push_back({ n - 1, 0 });
    med_pos.push_back({ n - 1, 1 });
    med_pos.push_back({ n - 2, 0 });
    med_pos.push_back({ n - 2, 1 });
}

//initializes medical and grow_cnt
void initialize() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            is_medical[r][c] = false;
            grow_cnt[r][c] = 0;
        }
    }
}

int move_handler(int pos, int delta, int cnt) {
    pos += delta * cnt;
    if (pos >= n) pos %= n;
    if (pos < 0) pos += n;
    return pos;
}

void move_medicine(int year) {
    for (int idx = 0; idx < med_pos.size(); ++idx) {
        int dir = move_order[year].first;
        int cnt = move_order[year].second;
        med_pos[idx].first = move_handler(med_pos[idx].first, delta[dir].first, cnt);
        med_pos[idx].second = move_handler(med_pos[idx].second, delta[dir].second, cnt);
    }
}

bool out_of_bound(int r, int c) {
    return r < 0 || r >= n || c < 0 || c >= n;
}

void grow_trees_on_pos() {
    for (auto pos : med_pos) {
        field[pos.first][pos.second] += 1;
        is_medical[pos.first][pos.second] = true;
    }
}

void calc_grow_cnt() {
    for (auto pos : med_pos) {
        int cnt = 0;
        for (int dir = 2; dir <= 8; dir += 2) {
            int diag_r = pos.first + delta[dir].first;
            int diag_c = pos.second + delta[dir].second;
            if (out_of_bound(diag_r, diag_c)) continue;
            if (field[diag_r][diag_c] >= 1) ++cnt;
        }
        grow_cnt[pos.first][pos.second] += cnt;
    }
}

void grow_trees_diag() {
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            field[r][c] += grow_cnt[r][c];
        }
    }
}

void obtain_new_medicines() {
    med_pos.clear();
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (field[r][c] >= 2 && !is_medical[r][c]) {
                field[r][c] -= 2;
                med_pos.push_back({ r, c });
            }
        }
    }
}

int obtain_height() {
    int heights = 0;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            heights += field[r][c];
        }
    }

    return heights;
}

int main() {
    //obtain data and setup
    obtain_data_and_setup();

    //for each turn
    for (int year = 1; year <= m; ++year) {
        //initialize
        initialize();
        //move medicine
        move_medicine(year);
        //debug_med_pos();
        grow_trees_on_pos();
        calc_grow_cnt();
        //debug_grow_cnt();
        //grow trees
        grow_trees_diag();
        //debug_field();
        //obtain new medicines
        obtain_new_medicines();
        //debug_field();
        //debug_med_pos();
    }

    //obtain height sum
    cout << obtain_height() << flush;

    return 0;
}