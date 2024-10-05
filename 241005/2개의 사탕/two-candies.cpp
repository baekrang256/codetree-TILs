#include<iostream>
#include<array>
#include<queue>

#define MAX_N 10
#define MAX_M 10

using namespace std;

int N, M;
char board[MAX_N][MAX_M];
pair<int, int> red_pos, blue_pos, hole_pos;
pair<int, int> delta[4] = { 
    {-1, 0},
    {1, 0},
    {0, 1},
    {0, -1}
};

bool visited[MAX_N][MAX_M][MAX_N][MAX_M];

int main() {
    cin >> N >> M;

    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < M; ++c) {
            cin >> board[r][c];
            if (board[r][c] == 'R') {
                red_pos = { r, c };
            }
            else if (board[r][c] == 'B') {
                blue_pos = { r, c };
            }
            else if (board[r][c] == 'O') {
                hole_pos = { r, c };
            }
        }
    }

    //utilizing bfs to find the smallest turn
    queue<array<int, 5>> next_pos;
    next_pos.push({ red_pos.first, red_pos.second, 0, blue_pos.first, blue_pos.second });
    visited[red_pos.first][red_pos.second][blue_pos.first][blue_pos.second] = true;

    while (!next_pos.empty()) {
        auto cur_data = next_pos.front();
        next_pos.pop();
        int cur_r = cur_data[0], cur_c = cur_data[1], cur_turn = cur_data[2];
        int cur_b_r = cur_data[3], cur_b_c = cur_data[4];

        //cout << cur_r << " " << cur_c << " " << cur_turn << " " << cur_b_r << " " << cur_b_c << "\n";
        
        if (cur_turn + 1 > 10) {
            break;
        }

        for (int dir = 0; dir < 4; ++dir) {
            bool red_out = false, met_blue = false;

            //first, simulate red until we found a wall or blue
            int new_r = cur_r + delta[dir].first, new_c = cur_c + delta[dir].second;
            while (board[new_r][new_c] != '#' && board[new_r][new_c] != 'O'
                && (new_r != cur_b_r || new_c != cur_b_c)) {
                new_r += delta[dir].first;
                new_c += delta[dir].second;
            }

            //if next was wall, just go back.
            if (board[new_r][new_c] == '#') {
                new_r -= delta[dir].first;
                new_c -= delta[dir].second;
            }
            //if next was hole. get out
            else if (board[new_r][new_c] == 'O') {
                red_out = true;
            }
            //if next was blue, just stop before blue position
            //we also set that we've met blue.
            else if (new_r == cur_b_r && new_c == cur_b_c) {
                new_r -= delta[dir].first;
                new_c -= delta[dir].second;
                met_blue = true;
            }

            //now simulate blue. If red met blue, then move red too.
            int new_b_r = cur_b_r + delta[dir].first, new_b_c = cur_b_c + delta[dir].second;
            if (met_blue) {
                new_r += delta[dir].first;
                new_c += delta[dir].second;
            }
            while (board[new_b_r][new_b_c] != '#' && board[new_b_r][new_b_c] != 'O'
                && (new_b_r != new_r || new_b_c != new_c)) {
                new_b_r += delta[dir].first;
                new_b_c += delta[dir].second;
                if (met_blue) {
                    new_r += delta[dir].first;
                    new_c += delta[dir].second;
                }
            }
            //if blue met a wall, just go back.
            if (board[new_b_r][new_b_c] == '#') {
                new_b_r -= delta[dir].first;
                new_b_c -= delta[dir].second;
                if (met_blue) {
                    new_r -= delta[dir].first;
                    new_c -= delta[dir].second;
                }
            }
            //if blue met a hole, get out, and this move is not valid.
            else if (board[new_b_r][new_b_c] == 'O') continue;
            //if blue met a red, just go back (it's the case when red is at front of blue)
            else if (new_b_r == new_r && new_b_c == new_c) {
                new_b_r -= delta[dir].first;
                new_b_c -= delta[dir].second;
            }

            //if red met a hole and blue didn't met a hole, this is the shortest path.
            if (red_out) {
                cout << cur_turn + 1;
                return 0;
            }
            //if red and blue didn't met a hole, it's a valid turn, but game is not over
            //move it into the queue. but only do that if that position was never met before.
            else if (!visited[new_r][new_c][new_b_r][new_b_c]) {
                next_pos.push({ new_r, new_c, cur_turn + 1, new_b_r, new_b_c });
                visited[new_r][new_c][new_b_r][new_b_c] = true;
            }
        }
    }

    cout << -1 << flush;
    return 0;
}