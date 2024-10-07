#include <iostream>
#include <queue>

#define MAX_N 20

//dir idx
#define RIGHT 0
#define DOWN 1
#define LEFT 2
#define UP 3

//face idx
#define BELOW_FACE 0
#define RIGHT_FACE 1
#define FRONT_FACE 2

using namespace std;

int n, m;
int map[MAX_N][MAX_N]; //original map
int scores[MAX_N][MAX_N]; //score obtained at arrival
bool visited[MAX_N][MAX_N] = { false }; //for bfs when calculating score.
int tot_score = 0;

int dice_dir = RIGHT;
int dice_r = 0, dice_c = 0;
int faces[3] = {6, 3, 2};

pair<int, int> delta[4] = {
    {0, 1}, {1, 0}, {0, -1}, {-1, 0}
};

void debug_print() {
    cout << "scores\n";
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cout << scores[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

bool out_of_bound(int r, int c) {
    return r < 0 || r >= n || c < 0 || c >= n;
}

void bfs(int r, int c) {
    queue<pair<int, int>> q;
    vector<pair<int, int>> route;
    q.push({ r, c });
    visited[r][c] = true;
    int num = map[r][c];

    //bfs
    while (!q.empty()) {
        auto pos = q.front();
        q.pop();
        route.push_back(pos);
        int cur_r = pos.first;
        int cur_c = pos.second;

        for (int idx = 0; idx < 4; ++idx) {
            int new_r = cur_r + delta[idx].first;
            int new_c = cur_c + delta[idx].second;
            if (out_of_bound(new_r, new_c) || visited[new_r][new_c]
                || map[new_r][new_c] != num) continue;

            visited[new_r][new_c] = true;
            q.push({ new_r, new_c });
        }
    }

    //look through the route, and mark the score;
    int score = route.size() * num;
    for (auto pos : route) {
        scores[pos.first][pos.second] = score;
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL); cout.tie(NULL);

    cin >> n >> m;
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            cin >> map[r][c];
        }
    }

    //calculate score obtained when arriving at each area
    for (int r = 0; r < n; ++r) {
        for (int c = 0; c < n; ++c) {
            if (!visited[r][c]) {
                bfs(r, c);
            }
        }
    }

    //debug_print();

    //roll for m turn
    for (int turn = 1; turn <= m; ++turn) {
        //choose direction according to below
        if (turn != 1) {
            if (faces[BELOW_FACE] > map[dice_r][dice_c]) {
                dice_dir = (dice_dir + 1) % 4;
            }
            else if (faces[BELOW_FACE] < map[dice_r][dice_c]) {
                dice_dir -= 1;
                if (dice_dir < 0) dice_dir = UP;
            }
        }

        //roll
        int new_r = dice_r + delta[dice_dir].first;
        int new_c = dice_c + delta[dice_dir].second;

        //out of bound, needs to change direction and move according to that position.
        if (out_of_bound(new_r, new_c)) {
            dice_dir = (dice_dir + 2) % 4;
            new_r = dice_r + delta[dice_dir].first;
            new_c = dice_c + delta[dice_dir].second;
        }

        dice_r = new_r; dice_c = new_c;

        //update faces
        int new_below, new_front, new_right;
        if (dice_dir == RIGHT) {
            new_below = faces[RIGHT_FACE];
            new_front = faces[FRONT_FACE];
            new_right = 7 - faces[BELOW_FACE];
        }
        else if (dice_dir == DOWN) {
            new_below = faces[FRONT_FACE];
            new_front = 7 - faces[BELOW_FACE];
            new_right = faces[RIGHT_FACE];
        }
        else if (dice_dir == LEFT) {
            new_below = 7 - faces[RIGHT_FACE];
            new_front = faces[FRONT_FACE];
            new_right = faces[BELOW_FACE];
        }
        else if (dice_dir == UP) {
            new_below = 7 - faces[FRONT_FACE];
            new_front = faces[BELOW_FACE];
            new_right = faces[RIGHT_FACE];
        }
        faces[BELOW_FACE] = new_below;
        faces[FRONT_FACE] = new_front;
        faces[RIGHT_FACE] = new_right;

        //score
        tot_score += scores[dice_r][dice_c];
    }

    cout << tot_score << flush;

    return 0;
}