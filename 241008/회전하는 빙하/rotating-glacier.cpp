#include <iostream>
#include <tuple>
#include <queue>
#include <vector>

#define MAX_N 6
#define MAX_Q 1000
#define MAX_GRID_SIZE 64
#define DIR_NUM 4

using namespace std;

int n, q;
int grid_size;

int grid[MAX_GRID_SIZE][MAX_GRID_SIZE];
int next_grid[MAX_GRID_SIZE][MAX_GRID_SIZE];

queue<pair<int, int> > bfs_q;
bool visited[MAX_GRID_SIZE][MAX_GRID_SIZE];

// 방향은 편의상 오른쪽, 아래, 위, 왼쪽 순서대로 정의합니다. 
int dx[DIR_NUM] = {0, 1, -1, 0};
int dy[DIR_NUM] = {1, 0, 0, -1};

bool InRange(int x, int y) {
    return 0 <= x && x < grid_size && 0 <= y && y < grid_size;
}

bool CanGo(int x, int y) {
    return InRange(x, y) && !visited[x][y] && grid[x][y];
}

// BFS를 진행한 이후 해당 그룹의 크기를 반환합니다.
int BFS() {
    int group_size = 0;
    
    // BFS 탐색을 수행합니다.
    while(!bfs_q.empty()) {
        pair<int, int> curr_pos = bfs_q.front();
        int curr_x, curr_y;
        tie(curr_x, curr_y) = curr_pos;
        group_size++;
        bfs_q.pop();

        for(int i = 0; i < DIR_NUM; i++) {
            int new_x = curr_x + dx[i];
            int new_y = curr_y + dy[i];

            if(CanGo(new_x, new_y)) {
                bfs_q.push(make_pair(new_x, new_y));
                visited[new_x][new_y] = true;
            }
        }
    }
    
    return group_size;
}

// 남아있는 빙하의 총 양을 계산합니다.
int GetNumOfIces() {
    int cnt = 0;
    for(int i = 0; i < grid_size; i++) 
        for(int j = 0; j < grid_size; j++)
            cnt += grid[i][j];
    
    return cnt;
}

// 얼음 군집 중 최대 크기를 구합니다.
int GetBiggestSize() {
    int max_size = 0;
    for(int i = 0; i < grid_size; i++) 
        for(int j = 0; j < grid_size; j++)
            if(grid[i][j] && !visited[i][j]) {
                // 시작 위치를 queue에 넣고 BFS를 진행합니다.
                // bfs 진행 이후 나온 그룹의 크기 중 최댓값을 찾습니다.
                visited[i][j] = true;
                bfs_q.push(make_pair(i, j));
                max_size = max(max_size, BFS());
            }
    
    return max_size;
}

// (start_row, start_col)에서 half_size 크기의 격자를 
// move_dir 방향으로 이동합니다.
void Move(int start_row, int start_col, int half_size, int move_dir) {
    for(int row = start_row; row < start_row + half_size; row++)
        for(int col = start_col; col < start_col + half_size; col++) {
            int next_row = row + dx[move_dir] * half_size;
            int next_col = col + dy[move_dir] * half_size;
            
            next_grid[next_row][next_col] = grid[row][col]; 
        }
}

void Rotate(int level) {
    // Step1.
    // rotate 이후의 상태를 저장할
    // 배열을 0으로 초기화합니다.
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            next_grid[i][j] = 0;
    
    int box_size = (1 << level);
    int half_size = box_size / 2;
    
    // Step2. 조건에 맞게 회전을 진행합니다.
    
    // Step2-1. 회전할 2^L * 2^L 크기 격자의 왼쪽 위 모서리 위치를 잡습니다.
    for(int i = 0; i < grid_size; i += box_size) 
        for(int j = 0; j < grid_size; j += box_size) {
            // Step2-2. 움직여야하는 2^(L - 1) * 2^(L - 1) 크기 격자의
            //          왼쪽 위 모서리를 각각 잡아
            //          알맞은 방향으로 이동시킵니다.
            Move(i, j, half_size, 0);
            Move(i, j + half_size, half_size, 1);
            Move(i + half_size, j, half_size, 2);
            Move(i + half_size, j + half_size, half_size, 3);
        }
    
    // Step3.
    // rotate 이후의 결과를 다시
    // grid 배열로 가져옵니다.
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j] = next_grid[i][j];
}

// 인접한 곳에 있는 얼음의 수를 셉니다.
int GetNeighborNums(int curr_x, int curr_y) {
    int cnt = 0;
    for(int i = 0; i < DIR_NUM; i++) {
        int new_x = curr_x + dx[i];
        int new_y = curr_y + dy[i];

        if(InRange(new_x, new_y) && grid[new_x][new_y])
            cnt++;
    }
    
    return cnt;
}

void Melt() {
    // Step1.
    // 녹은 이후의 상태를 저장할
    // 배열을 0으로 초기화합니다.
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            next_grid[i][j] = 0;
    
    // Step2.
    // 인접한 칸의 수가 3개 이하인 곳의 얼음을 
    // 찾아 1씩 녹입니다.
    
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++) {
            int cnt = GetNeighborNums(i, j);
            // Step2-1. 녹는경우에는 1을 빼서 넣어줍니다.
            if(grid[i][j] && cnt < 3)
                next_grid[i][j] = grid[i][j] - 1;
            // Step2-2. 녹지 않는 경우에는 그대로 넣어줍니다.
            else
                next_grid[i][j] = grid[i][j];
        }
    
    // Step3.
    // 녹은 이후의 결과를 다시
    // grid 배열로 가져옵니다.
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++)
            grid[i][j] = next_grid[i][j];
}

int main() {
    cin >> n >> q;
    grid_size = (1 << n);
    
    for(int i = 0; i < grid_size; i++)
        for(int j = 0; j < grid_size; j++) 
            cin >> grid[i][j];
    
    // q번에 걸쳐 회전과 녹는 과정을 진행합니다.
    while(q--) {
        int level;
        cin >> level;
        
        if(level)
            Rotate(level);

        Melt();
    }
    
    cout << GetNumOfIces() << endl;
    cout << GetBiggestSize();
    return 0;
}