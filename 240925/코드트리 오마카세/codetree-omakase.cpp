#include <iostream>
#include <string>
#include <list>
#include <vector>
#include <unordered_map>

using namespace std;

int L, Q, cmd, customer_cnt = 0, sushi_cnt = 0;
int glob_customer_idx = 0;
int glob_sushi_idx = 0;
int turn = 0;
pair<int, int> customers[15000]; //각 손님 위치 및 먹은 개수 정보 (이름 별로 분류 되어 있음) first : 위치, second : 개수
list<int> sushi[15000]; //각 초밥 위치 정보. (이름 별로 분류되어 있음) list의 각 elment가 해당 이름 초밥 현재 위치.
unordered_map<string, int> customer_map; //index map
unordered_map<string, int> sushi_map; //index map

void handle_spin() {
    for (auto sushi_info : sushi_map) {
        int sushi_idx = sushi_info.second;
        auto it = sushi[sushi_idx].begin();
        while (it != sushi[sushi_idx].end()) {
            //cout << *it << "\n";
            *it += 1;
            *it %= L;
            it++;
        }
    }
}

void handle_eating() {
    vector<string> v;
    for (auto customer_info : customer_map) {
        const string customer_name = customer_info.first;
        const int customer_idx = customer_info.second;

        int customer_pos = customers[customer_idx].first;
        int customer_left_cnt = customers[customer_idx].second;
        auto sushi_it = sushi_map.find(customer_name);
        if (sushi_it == sushi_map.end()) continue; //초밥이 아직 없넹
        const int sushi_idx = sushi_it->second;
        auto it = sushi[sushi_idx].begin();
        while (it != sushi[sushi_idx].end()) {
            if (*it == customer_pos) {
                it = sushi[sushi_idx].erase(it);
                customer_left_cnt -= 1;
                sushi_cnt -= 1;
            }
            else {
                ++it;
            }
        }

        if (customer_left_cnt <= 0) {
            v.push_back(customer_name);
            customer_cnt -= 1;
        }
        else {
            customers[customer_idx].second = customer_left_cnt;
        }
    }

    for (string erased_name : v) {
        customer_map.erase(erased_name);
        sushi_map.erase(erased_name);
    }
}

void turn_calc(int cur_turn) {
    while (true) {
        //일단 누가 호출 했든 간에 모든 초밥이 회전을 해야 한다.
        handle_spin();
        //이후 턴을 증가시킨다.
        ++turn;
        if (turn >= cur_turn) break;
        handle_eating();
    }
}

void make_sushi() {
    int t, x; string name;
    cin >> t >> x >> name;
    turn_calc(t);
    //초밥 만들기. 일단 해당 이름이 존재하는지 확인
    //있으면 그 index에다가 새로 삽입을 하고 없으면 새로운 매핑 정보를 집어 넣음
    int sushi_idx;
    if (sushi_map.find(name) != sushi_map.end()) {
        sushi_idx = sushi_map[name];
    }
    else {
        sushi_idx = glob_sushi_idx++;
        sushi_map.insert({ name, sushi_idx });
    }
    //이후 해당 초밥을 집어넣음
    sushi[sushi_idx].push_back(x);
    sushi_cnt += 1;

    //먹는 것 까지 처리
    handle_eating();
}

void enter_guest() {
    int t, x, n; string name;
    cin >> t >> x >> name >> n;
    turn_calc(t);
    //새로 들어오고
    int customer_idx = glob_customer_idx++;
    customer_cnt += 1;
    customer_map[name] = customer_idx;
    customers[customer_idx].first = x;
    customers[customer_idx].second = n;

    //먹는 것 까지 처리
    handle_eating();
}

void take_photo() {
    int t;
    cin >> t;
    turn_calc(t);
    //먹는 것 처리하고
    handle_eating();
    //사진 찍기
    cout << customer_cnt << " " << sushi_cnt << "\n";
}

int main() {
    cin >> L >> Q;
    
    for (int query_cnt = 0; query_cnt < Q; ++query_cnt) {
        cin >> cmd;
        switch (cmd) {
        case 100:
            make_sushi();
            break;
        case 200:
            enter_guest();
            break;
        case 300:
            take_photo();
            break;
        }
    }

    return 0;
}