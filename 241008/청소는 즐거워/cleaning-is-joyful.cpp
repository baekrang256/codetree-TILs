#include <iostream>

#define DIR_NUM 4
#define MAX_N 499

using namespace std;

// 전역 변수 선언:
int n;

int curr_x, curr_y;
int move_dir, move_num;

int ans;

int grid[MAX_N][MAX_N];

int dust_ratio[DIR_NUM][5][5] = {
    {
        {0,  0, 2, 0, 0},
        {0, 10, 7, 1, 0},
        {5,  0, 0, 0, 0},
        {0, 10, 7, 1, 0},
        {0,  0, 2, 0, 0},
    },
    {
        {0,  0, 0,  0, 0},
        {0,  1, 0,  1, 0},
        {2,  7, 0,  7, 2},
        {0, 10, 0, 10, 0},
        {0,  0, 5,  0, 0},
    },
    {
        {0, 0, 2,  0, 0},
        {0, 1, 7, 10, 0},
        {0, 0, 0,  0, 5},
        {0, 1, 7, 10, 0},
        {0, 0, 2,  0, 0},
    },
    {
        {0,  0, 5,  0, 0},
        {0, 10, 0, 10, 0},
        {2,  7, 0,  7, 2},
        {0,  1, 0,  1, 0},
        {0,  0, 0,  0, 0},
    }
};

bool InRange(int x, int y) {
    return 0 <= x && x < n && 0 <= y && y < n;
}

// (x, y) 위치에 dust 만큼의 먼지를 추가합니다.
void AddDust(int x, int y, int dust) {
    // 격자 범위를 벗어난다면 답에 더해줍니다.
    if(!InRange(x, y))
        ans += dust;
    // 격자 범위 안이라면, 해당 칸에 더해줍니다.
    else
        grid[x][y] += dust;
}

// 한 칸 움직이며 청소를 진행합니다.
void Move() {
    // 문제에서 원하는 진행 순서대로 
    // 왼쪽 아래 오른쪽 위 방향이 되도록 정의합니다.
    int dx[DIR_NUM] = {0, 1, 0, -1};
    int dy[DIR_NUM] = {-1, 0, 1, 0};
    
    // curr 위치를 계산합니다.
    curr_x += dx[move_dir]; curr_y += dy[move_dir];
    
    // 현재 위치를 기준으로 각 위치에 먼지를 더해줍니다.
    int added_dust = 0;
    for(int i = 0; i < 5; i++)
        for(int j = 0; j < 5; j++) {
            int dust = grid[curr_x][curr_y] * dust_ratio[move_dir][i][j] / 100;
            AddDust(curr_x + i - 2, curr_y + j - 2, dust);
            
            added_dust += dust;
        }
    
    // a% 자리에 먼지를 추가합니다.
    AddDust(curr_x + dx[move_dir], curr_y + dy[move_dir], 
            grid[curr_x][curr_y] - added_dust);
}

bool End() {
    return !curr_x && !curr_y;
}

int main() {
    // 입력:
    cin >> n;
    
    for(int i = 0; i < n; i++)
        for(int j = 0; j < n; j++)
            cin >> grid[i][j];
    
    // 시작 위치와 방향, 
    // 해당 방향으로 이동할 횟수를 설정합니다. 
    curr_x = n / 2; curr_y = n / 2;
    move_dir = 0; move_num = 1;

    while(!End()) {
        // move_num 만큼 이동합니다.
        for(int i = 0; i < move_num; i++) {
            Move();
            
            // 이동하는 도중 (0, 0)으로 오게 되면,
            // 움직이는 것을 종료합니다.
            if(End())
                break;
        }
        
        // 방향을 바꿉니다.
        move_dir = (move_dir + 1) % 4;
        // 만약 현재 방향이 왼쪽 혹은 오른쪽이 된 경우에는
        // 특정 방향으로 움직여야 할 횟수를 1 증가시킵니다.
        if(move_dir == 0 || move_dir == 2)
            move_num++;
    }
    
    // 출력:
    cout << ans;

    return 0;
}