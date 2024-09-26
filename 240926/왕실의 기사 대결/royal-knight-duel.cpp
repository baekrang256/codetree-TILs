#include <iostream>
#include <vector>

#define EMPTY 0
#define TRAP 1
#define WALL 2

using namespace std;

struct Knight {
    int r, c; //왼쪽 위 꼭짓점 위치
    int h, w; //방패 높이, 너비
    int k; //현재 체력
};

int L, N, Q;
int chess_board[42][42]; //0, 41 부분은 벽이다.
int knight_status[42][42]; //기사 위치는 별도로 저장
Knight knights[31]; //기사 모음
int original_hp[31]; //기사들 원래 체력
bool moved[31]; //해당 기사가 이번에 움직였는가?
bool dead[31] = { false };
pair<int, int> delta[4] = {
    {-1, 0}, {0, 1}, {1, 0}, {0, -1}
};

void board_print() {
    cout << "board\n";
    for (int r = 1; r <= L; ++r) {
        for (int c = 1; c <= L; ++c) {
            cout << chess_board[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
}

void debug_print() {
    cout << "knight status" << "\n";
    for (int r = 1; r <= L; ++r) {
        for (int c = 1; c <= L; ++c) {
            cout << knight_status[r][c] << " ";
        }
        cout << "\n";
    }
    cout << "\n";
    cout << "knight infos\n";
    for (int i = 1; i <= N; ++i) {
        cout << knights[i].r << " "
            << knights[i].c << " "
            << knights[i].h << " "
            << knights[i].w << " "
            << knights[i].k << "\n";
    }
    cout << "\n";
}

//knight_num번 기사가 죽었는가 살았는가
bool knight_dead(int knight_num) {
    return knights[knight_num].k <= 0;
}

int knight_r(int knight_num) {
    return knights[knight_num].r;
}

int knight_c(int knight_num) {
    return knights[knight_num].c;
}

int knight_h(int knight_num) {
    return knights[knight_num].h;
}

int knight_w(int knight_num) {
    return knights[knight_num].w;
}

//moved 초기화
void initialize_moved() {
    for (int i = 1; i <= N; ++i) {
        moved[i] = false;
    }
}

int main() {
    //데이터 수집
    cin >> L >> N >> Q;
    
    //지도 정보 수집
    for (int r = 1; r <= L; ++r) {
        for (int c = 1; c <= L; ++c) {
            cin >> chess_board[r][c];
        }
    }

    //지도에 벽도 채워야 한다.
    for (int r = 0; r <= L + 1; ++r) {
        if (r == 0 || r == L + 1) {
            for (int c = 0; c <= L + 1; ++c) {
                chess_board[r][c] = WALL;
            }
        }
        else {
            chess_board[r][0] = WALL;
            chess_board[r][L + 1] = WALL;
        }
    }

    //기사 정보 수집 및 배치
    for (int knight_num = 1; knight_num <= N; ++knight_num) {
        cin >> knights[knight_num].r
            >> knights[knight_num].c
            >> knights[knight_num].h
            >> knights[knight_num].w
            >> knights[knight_num].k;

        original_hp[knight_num] = knights[knight_num].k;
        for (int r = knight_r(knight_num); r < knight_r(knight_num) + knight_h(knight_num); ++r) {
            for (int c = knight_c(knight_num); c < knight_c(knight_num) + knight_w(knight_num); ++c) {
                knight_status[r][c] = knight_num;
            }
        }
    }
    //board_print();

    //쿼리 수행
    int i, d;
    vector<int> moved_knights; //한 쿼리에서 이동할 기사들.
    vector<int> dead_knights; //죽은 기사들 모음
    for (int query = 1; query <= Q; ++query) {
        //초기화
        moved_knights.clear();
        dead_knights.clear();
        initialize_moved();

        //debug_print();
        
        //정보 수집
        cin >> i >> d;

        //죽었으면 이동 안하고, 안 죽었으면 이동 시도
        if (dead[i]) continue;
        moved_knights.push_back(i);

        bool wall_smashed = false; //이번 이동에서 벽에 부딪히는 기사가 있는가?
        int idx = 0; //moved_knight 기준 idx가 될 것이다.
        while (idx < moved_knights.size()) { //새로 이동할 기사가 없을때까지 진행한다.
            int cur_knight = moved_knights[idx];
            //해당 기사는 밑의 위치로 이동할 것이다.
            int new_r = knight_r(cur_knight) + delta[d].first;
            int new_c = knight_c(cur_knight) + delta[d].second;
            ++idx;

            //그렇게 이동한 위치의 범위 내에...
            for (int r = new_r; r < new_r + knight_h(cur_knight); ++r) {
                for (int c = new_c; c < new_c + knight_w(cur_knight); ++c) {
                    //벽이 존재한다면 이동은 안 이루어진다.
                    if (chess_board[r][c] == WALL) {
                        wall_smashed = true;
                        break;
                    }
                    //기사가 존재한다면 그 기사도 이동한다.
                    //단 자기 자신을 다른 기사로 착각해서는 안된다.
                    //또 이미 이동한 기사를 움직이려고 시도하진 말자.
                    else if (knight_status[r][c] != cur_knight && knight_status[r][c] != EMPTY && !moved[knight_status[r][c]]) {
                        moved_knights.push_back(knight_status[r][c]);
                        moved[knight_status[r][c]] = true;
                    }
                }
                if (wall_smashed) break; //벽 부딪힘
            }

            if (wall_smashed) break; //벽을 만남. 이동이 더이상 계산될 이유 없음.
        }

        if (wall_smashed) {
            //cout << "smashed wall" << "\n";
            continue;
        }

        //벽에 안 부딪혔으면 이동을 전부 한다.
        //일단 본인들의 이전 위치들을 전부 다 0으로 만든다.
        for (int knight_num : moved_knights) {
            for (int r = knight_r(knight_num); r < knight_r(knight_num) + knight_h(knight_num); ++r) {
                for (int c = knight_c(knight_num); c < knight_c(knight_num) + knight_w(knight_num); ++c) {
                    knight_status[r][c] = 0;
                }
            }
        }

        //그 후 새로운 위치로 정보 업데이트를 한다.
        //지도 업데이트랑 본인 기사 정보 업데이트를 해야 한다.
        //함정을 밟게 되었는지도 여기서 확인하며, 이 경우 체력을 깎는다.
        //함정을 밟고 체력이 0이 되는 순간에 그 기사는 죽는다.
        for (int knight_num : moved_knights) {
            bool cur_dead = false;
            int new_r = knight_r(knight_num) + delta[d].first;
            int new_c = knight_c(knight_num) + delta[d].second;
            knights[knight_num].r = new_r;
            knights[knight_num].c = new_c;
            for (int r = new_r;  r < new_r + knight_h(knight_num); ++r) {
                for (int c = new_c; c < new_c + knight_w(knight_num); ++c) {
                    knight_status[r][c] = knight_num;
                    if (knight_num != i && chess_board[r][c] == TRAP) {
                        knights[knight_num].k -= 1;
                        if (knights[knight_num].k <= 0) {
                            cur_dead = true;
                            dead_knights.push_back(knight_num);
                            dead[knight_num] = true;
                            break;
                        }
                    }
                }
                if (cur_dead) break;
            }
        }

        //죽은 기사 존재 시, 해당 영역을 전부 영으로 만들어야 한다.
        for (int knight_num : dead_knights) {
            for (int r = knight_r(knight_num); r < knight_r(knight_num) + knight_h(knight_num); ++r) {
                for (int c = knight_c(knight_num); c < knight_c(knight_num) + knight_w(knight_num); ++c) {
                    knight_status[r][c] = 0;
                }
            }
        }
    }

    //debug_print();

    int total = 0;
    for (int i = 1; i <= N; ++i) {
        if (dead[i]) continue;
        total += (original_hp[i] - knights[i].k);
    }
    cout << total << flush;

    return 0;
}