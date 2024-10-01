#include <iostream>
#include <unordered_map>
#include <vector>

using namespace std;

#define BELT_MAX 10
#define PRODUCT_MAX 100000
#define NONE -1

//상품.
struct Product {
    int weight; //본인 무게
    int front_idx; //본인 앞의 상품 index. 고유 번호가 아니라 index다!!!
    int back_idx; //본인 뒤의 상품 index. 고유번호가 아니라 index다!!!.
    int cur_belt; //현재 위치한 벨트.
    bool is_unloaded; //하차 여부.
};

struct Belt {
    int head_idx; //첫부분 index. 고유번호가 아니라 index다!!!
    int tail_idx; //뒷부분 index. 고유번호가 아니라 index다!!!
    bool broken;
};

int q, cmd, n, m;

unordered_map<int, int> idx_map; //상품 번호 -> 관련 idx hashmap
Product products[PRODUCT_MAX];//상품을 모은 배열. 보유 정보는 무게, 하차 여부 (존재 여부), 뒤의 물건, 현재 위치 벨트
Belt belts[BELT_MAX + 1];//벨트 모음. 직접 만든 단일 연결 리스트로 구현할까 싶다.

//디버깅 용도.
void debug_print() {
    cout << "products status\n";
    for (int product = 0; product < n; ++product) {
        cout << "{" << products[product].weight << ", "
            << products[product].front_idx << ", "
            << products[product].back_idx << ", "
            << products[product].cur_belt << ", "
            << products[product].is_unloaded << "}\n";
    }
    cout << "\n";
    cout << "belts status\n";
    for (int belt_num = 1; belt_num <= m; ++belt_num) {
        cout << "{" << belts[belt_num].head_idx << ", "
            << belts[belt_num].tail_idx << ", "
            << belts[belt_num].broken << "}\n";
    }
    cout << "\n";
}

void print_error() {
    cout << -1 << "\n";
}

//100번 명령어
//m개의 벨트에, n/m개의 물건 올리기. n은 m의 배수임이 보장됨.
void make_factory() {
    cin >> n >> m;

    //제품 정보 수집 및 벨트에 넣기.
    //ID들이 먼저 쫙 나온 다음 W가 나온다.
    int idx = 0;
    vector<int> IDs(n), Ws(n);
    for (int num = 0; num < n; ++num) {
        cin >> IDs[num];
    }
    for (int num = 0; num < n; ++num) {
        cin >> Ws[num];
    }
    
    //벨트에 순서대로 넣어야 하니까... 좀 복잡한 이중 for-loop이 되었다.
    for (int belt_num = 1; belt_num <= m; ++belt_num) {
        belts[belt_num].broken = false;
        for (int cnt = 1; cnt <= n / m; ++cnt) {
            int ID = IDs[(belt_num - 1) * (n / m) + (cnt - 1)];
            int W = Ws[(belt_num - 1) * (n / m) + (cnt - 1)];
            //일단 제품 정보를 입력하자.
            products[idx].cur_belt = belt_num;
            products[idx].weight = W;
            products[idx].is_unloaded = false;

            //사실 밑의 초기화 작업은 이것보다 약간 간단하게 할 수 있을 것 같지만...
            //일단 이대로 냅뒀음
            
            //벨트에 아무것도 없으면 head에 넣음
            if (cnt == 1) {
                belts[belt_num].head_idx = idx;
                belts[belt_num].tail_idx = idx;
                products[idx].front_idx = NONE;
                products[idx].back_idx = NONE;
            }
            //벨트에 하나라도 있으면 tail에다가 배치.
            else {
                products[belts[belt_num].tail_idx].back_idx = idx;
                products[idx].front_idx = belts[belt_num].tail_idx;
                products[idx].back_idx = NONE;
                belts[belt_num].tail_idx = idx;
            }

            //맵에다가 집어넣자.
            idx_map.insert({ID, idx++});
        }
    }
}

//200번 명령어
void unload_product() {
    int w_max;
    cin >> w_max;

    int unloaded_weights = 0;

    //각 벨트의 맨 앞에, w_max 이하의 제품이 있는지 확인
    for (int belt_num = 1; belt_num <= m; ++belt_num) {
        if (belts[belt_num].broken) continue; //벨트가 이미 부숴짐
        int head_idx = belts[belt_num].head_idx;
        if (head_idx == NONE) continue; //head가 없음.
        

        if (products[head_idx].weight <= w_max) {
            //이하인 경우 하차 및 벨트에서 제거
            int head_back_idx = products[head_idx].back_idx;
            products[head_idx].is_unloaded = true;
            belts[belt_num].head_idx = head_back_idx;
            if (head_back_idx != NONE) //본인 뒤에 element가 잇는 경우에만 수행.
                products[head_back_idx].front_idx = NONE;
            else // 이 경우 tail도 NONE이 되어야 함.
                belts[belt_num].tail_idx = NONE;

            //출력용 값 갱신
            unloaded_weights += products[head_idx].weight;
        }
        else {
            //초과인 경우 벨트 맨 뒤로 보냄.
            //단, head랑 tail이 같으면 굳이 그럴 이유가 없음.
            if (belts[belt_num].head_idx == belts[belt_num].tail_idx) continue;

            int head_back_idx = products[head_idx].back_idx;
            int prev_tail_idx = belts[belt_num].tail_idx;
            products[head_idx].front_idx = prev_tail_idx;
            products[head_idx].back_idx = NONE;
            products[prev_tail_idx].back_idx = head_idx;
            products[head_back_idx].front_idx = NONE;
            belts[belt_num].head_idx = head_back_idx;
            belts[belt_num].tail_idx = head_idx;
        }
    }

    //출력
    cout << unloaded_weights << "\n";
}

//300번 명령어
void remove_product() {
    int r_id;
    cin >> r_id;

    auto r_idx_it = idx_map.find(r_id);
    //r_id에 해당하는 제품이 아예 없던 경우.
    if (r_idx_it == idx_map.end()) {
        print_error();
        return;
    }

    int r_idx = r_idx_it->second;
    //r_id에 해당하는 제품이 있었긴 한데 하차한 경우
    if (products[r_idx].is_unloaded) {
        print_error();
        return;
    }

    //여기까지 오면 있는 경우다. 일단 물건 제거
    products[r_idx].is_unloaded = true;
    
    //이후 벨트를 갱신한다.
    int front_idx = products[r_idx].front_idx;
    int back_idx = products[r_idx].back_idx;
    int belt_num = products[r_idx].cur_belt;

    if (front_idx != NONE) {
        products[front_idx].back_idx = back_idx;
    }
    else { //이 경우 삭제되는 애가 head이었던 것이다. 갱신한다.
        belts[belt_num].head_idx = back_idx;
    }
    if (back_idx != NONE) {
        products[back_idx].front_idx = front_idx;
    }
    else { //이 경우 삭제되는 애가 tail이었던 것이다. 갱신한다.
        belts[belt_num].tail_idx = front_idx;
    }

    //출력
    cout << r_id << "\n";
}

//400번 명령어
void check_product() {
    int f_id;
    cin >> f_id;

    auto f_idx_it = idx_map.find(f_id);
    //f_id에 해당하는 제품이 아예 없던 경우.
    if (f_idx_it == idx_map.end()) {
        print_error();
        return;
    }

    int f_idx = f_idx_it->second;
    //f_id에 해당하는 제품이 있었긴 한데 하차한 경우
    if (products[f_idx].is_unloaded) {
        print_error();
        return;
    }

    //여기까지 오면 있는 경우다.
    int front_idx = products[f_idx].front_idx;
    int belt_num = products[f_idx].cur_belt;
    int prev_head_idx = belts[belt_num].head_idx;
    int prev_tail_idx = belts[belt_num].tail_idx;

    //일단 벨트 번호를 출력하자.
    cout << belt_num << "\n";

    //이후 벨트를 갱신할건데...
    //만일 head가 본인이랑 같으면 아무것도 할 필요 없다.
    if (prev_head_idx == f_idx) return;

    //다른 경우, 본인과 본인 뒤의 노드들을 전부 앞으로 끌어모아야 한다.
    belts[belt_num].head_idx = f_idx;
    products[f_idx].front_idx = NONE;
    products[prev_tail_idx].back_idx = prev_head_idx;
    products[prev_head_idx].front_idx = prev_tail_idx;
    products[front_idx].back_idx = NONE;
    belts[belt_num].tail_idx = front_idx;

}

//500번 명령어
void belt_broken() {
    int b_num;
    cin >> b_num;

    //이미 고장난 경우.
    if (belts[b_num].broken) {
        print_error();
        return;
    }

    //고장이 안났으면 고장내고, 새로운 번호를 찾아보자.
    //조건에 따르면 무조건 성공해야 한다.
    //출력도 미리 해버리자.
    cout << b_num << "\n";
    belts[b_num].broken = true;
    int new_b_num = b_num;
    while (belts[new_b_num].broken) {
        new_b_num += 1;
        if (new_b_num > m) new_b_num = 1;
    }

    //일단 b_num에 있는 것들 번호를 전부 갱신하자.
    int cur_idx = belts[b_num].head_idx;
    while (cur_idx != NONE) {
        products[cur_idx].cur_belt = new_b_num;
        cur_idx = products[cur_idx].back_idx;
    }

    //옮긴다. 그런데 해당 벨트 비어있을수도 있다.
    //이 경우 head랑 tail을 그냥 교체하면 된다
    if (belts[new_b_num].head_idx == NONE) {
        belts[new_b_num].head_idx = belts[b_num].head_idx;
        belts[new_b_num].tail_idx = belts[b_num].tail_idx;
        return;
    }

    //만약 아니라면
    //1. tail을 바꾸고.
    //2. 과거 new_b_num의 tail과 과거 b_num의 head를 갱신해야 함.
    //단 이건 element가 존재하는 경우에나 해당되는 소리라 일단 그걸 확인한다.
    if (belts[b_num].head_idx == NONE) return;

    int new_prev_tail_idx = belts[new_b_num].tail_idx;
    int prev_head_idx = belts[b_num].head_idx;
    int prev_tail_idx = belts[b_num].tail_idx;

    belts[new_b_num].tail_idx = prev_tail_idx;
    products[new_prev_tail_idx].back_idx = prev_head_idx;
    products[prev_head_idx].front_idx = new_prev_tail_idx;
}

int main() {
    cin >> q;
    for (int query = 1; query <= q; ++query) {
        cin >> cmd;
        switch (cmd) {
        case 100:
            make_factory();
            break;
        case 200:
            unload_product();
            break;
        case 300:
            remove_product();
            break;
        case 400:
            check_product();
            break;
        case 500:
            belt_broken();
            break;
        }
        //debug_print();
    }

    cout << flush;

    return 0;
}