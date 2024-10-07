#include <iostream>
#include <list>

#define CLOCK 1
#define CNT_CLOCK -1
#define MAX_N 99
#define MAX_M (99*99)
#define LEFT_RIGHT 1
#define UP_DOWN 2
#define UP 0
#define RIGHT 1
#define DOWN 2
#define LEFT 3

using namespace std;

//basic
int n, m, h, k;
pair<int, int> delta[4] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};

//runner related
pair<int, int> runner_pos[MAX_M];
int runner_dir[MAX_M];
bool runner_out[MAX_M] = { false };
int runner_cnt;

//tree related
bool tree_map[MAX_N + 1][MAX_N + 1] = { false };

//catcher related
int catcher_dir = UP;
int catcher_rot = CLOCK;
int catcher_len = 1;
int catcher_moved = 0;
bool first_turn;
int catcher_r, catcher_c;
int score = 0;

void debug_print(int turn) {
    cout << "turn : " << turn << "\n";
    cout << "runner_pos\n";
    for (int idx = 0; idx < m; ++idx) {
        cout << "{";
        cout << runner_pos[idx].first << " " << runner_pos[idx].second;
        cout << "}\n";
    }
    cout << "\n";
    //cout << "catcher_pos : ";
    //cout << catcher_r << " " << catcher_c << "\n";
    //cout << catcher_len << " " << catcher_dir << "\n\n";
}

int dist(int x1, int y1, int x2, int y2) {
    return abs(x1 - x2) + abs(y1 - y2);
}

bool out_of_bound(int r, int c) {
    return r < 1 || r > n || c < 1 || c > n;
}

//actually, a more efficient way to simulate runners
//is to make 3x3 inner runners to move... but it's quite annoying to implement
//since the question condition also allows us to check all runners, I'll do that instead
void runners_move() {
    int cur_cnt = 0;
    for (int runner = 0; runner < m; ++runner) {
        if (runner_out[runner]) continue; //already dead
        
        //move runners with distance less or equal to 3
        int r = runner_pos[runner].first;
        int c = runner_pos[runner].second;
        if (dist(r, c, catcher_r, catcher_c) > 3) continue;

        //calculate new position
        int dir = runner_dir[runner];
        int new_r = r + delta[dir].first;
        int new_c = c + delta[dir].second;

        //out of bound. change direction and calculate new position
        //changed direction is updated right away.
        if (out_of_bound(new_r, new_c)) {
            dir = (dir + 2) % 4;
            new_r = r + delta[dir].first;
            new_c = c + delta[dir].second;
            runner_dir[runner] = dir;
        }

        if (new_r == catcher_r && new_c == catcher_c) continue; //same as catcher. don't move.

        //update position
        runner_pos[runner].first = new_r;
        runner_pos[runner].second = new_c;

        ++cur_cnt;
        if (cur_cnt >= runner_cnt) break;
    }
}

//move and change direction.
//note that when we arrived to position where we have to change direction, WE CHANGE THE DIRECTION RIGHT AWAY
void catcher_move() {
    int new_r = catcher_r + delta[catcher_dir].first;
    int new_c = catcher_c + delta[catcher_dir].second;

    //arrived position is where we need to setup reverse position
    if ((new_r == 1 && new_c == 1) || (new_r ==  (n / 2 + 1) && new_c == (n / 2 + 1))) {
        catcher_dir = (catcher_dir + 2) % 4;
        if (new_r == 1 && new_c == 1) {
            catcher_len = catcher_moved + 1;
            catcher_moved = -1;
        }
        else {
            catcher_len = 1;
            catcher_moved = -1;
        }
        catcher_rot = -catcher_rot;
        if (catcher_rot == CNT_CLOCK) first_turn = true;
    }

    catcher_r = new_r;
    catcher_c = new_c;
    catcher_moved += 1;

    //arrived position is where we need to setup direction change
    if (catcher_moved >= catcher_len) {
        catcher_moved = 0;

        if (catcher_rot == CLOCK) {
            catcher_dir = (catcher_dir + 1) % 4;
            if (catcher_dir % 2 == 0) {
                catcher_len += 1;
            }
        }
        else {
            catcher_dir -= 1;
            if (catcher_dir < 0) catcher_dir = LEFT;
            if (catcher_dir % 2 == 1) {
                if (first_turn) first_turn = false;
                else catcher_len -= 1;
            }
        }
    }
}

//catch 
void catcher_catch(int turn) {
    pair<int, int> catch_positions[3] = {
        {catcher_r, catcher_c},
        {catcher_r + delta[catcher_dir].first, catcher_c + delta[catcher_dir].second},
        {catcher_r + (delta[catcher_dir].first * 2), catcher_c + (delta[catcher_dir].second * 2)}
    };
    int cur_cnt = 0;
    int catch_cnt = 0;

    for (int runner = 0; runner < m; ++runner) {
        if (runner_out[runner]) continue;

        auto pos = runner_pos[runner];
        if (tree_map[pos.first][pos.second]) continue;

        if (pos == catch_positions[0] || pos == catch_positions[1] || pos == catch_positions[2]) {
            catch_cnt++;
            runner_cnt--;
            runner_out[runner] = true;
        }
        else {
            cur_cnt++;
        }
        
        if (cur_cnt >= runner_cnt) break;
    }

    score += turn * catch_cnt;
}

int main() {

    //obtain basic infos
    cin >> n >> m >> h >> k;
    catcher_r = n / 2 + 1; catcher_c = n / 2 + 1;
    runner_cnt = m;

    //obtain runner infos
    for (int runner = 0; runner < m; ++runner) {
        int r, c, d;
        cin >> r >> c >> d;
        runner_pos[runner].first = r;
        runner_pos[runner].second = c;
        if (d == LEFT_RIGHT) {
            runner_dir[runner] = RIGHT;
        }
        else {
            runner_dir[runner] = DOWN;
        }
    }

    //obtain tree infos
    for (int tree = 0; tree < h; ++tree) {
        int r, c;
        cin >> r >> c;
        tree_map[r][c] = true;
    }
    //debug_print(0);

    //simulate
    for (int turn = 1; turn <= k; ++turn) {
        runners_move();
        catcher_move();
        catcher_catch(turn);
        //debug_print(turn);
        if (runner_cnt <= 0) break;
    }

    cout << score << flush;

    return 0;
}