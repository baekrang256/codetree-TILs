#include <iostream>

using namespace std;

int N, Q, cmd;
int parents[100001];
int powers[100001];
bool notify[100001];
int income_totals[100001][20]; //제발 메모리 초과 나지 마라

void update_income(int c, int power, int increment) {
    if (power >= 19)
        income_totals[c][19] += increment;
    else
        income_totals[c][power] += increment;
}

void debug_print(int turn) {
    cout << "after query " << turn << "\n";
    cout << "parents\n";
    for (int i = 1; i <= N; ++i) {
        cout << parents[i] << " ";
    }
    cout << "\n\n";
    cout << "powers\n";
    for (int i = 1; i <= N; ++i) {
        cout << powers[i] << " ";
    }
    cout << "\n\n";
    cout << "notify\n";
    for (int i = 1; i <= N; ++i) {
        cout << notify[i] << " ";
    }
    cout << "\n\n";
    cout << "income_totals\n";
    for (int i = 1; i <= N; ++i) {
        cout << "{ ";
        for (int power = 0; power < 20; ++power) {
            cout << income_totals[i][power] << " ";
        }
        cout << "}\n";
    }
    cout << "\n\n";
}

//초기화 작업
void initialize() {
    //0번은 부모 없당. ㅎ
    parents[0] = -1;

    //부모 정보. 하는김에 notify도 초기화
    for (int i = 1; i <= N; ++i) {
        cin >> parents[i];
        notify[i] = true;
    }

    //세기 정보
    for (int i = 1; i <= N; ++i) {
        cin >> powers[i];
    }

    //이제 각 node에, 각 세기별 유통이 얼마나 이루어지는지 파악해야 한다.
    //일단 모든 곳에서 유통이 이루어지긴 한다. 왜냐하면 기본이 ON이니까.
    for (int i = 1; i <= N; ++i) {
        //각 node는 본인 위에 있는 녀석들에게, 어느 세기의 메세지가 들어오는지를 알려줘야 함.
        int cur_power = powers[i];
        int cur_parent = parents[i];
        update_income(i, cur_power, 1);
        
        cur_power -= 1;
        while (cur_parent > 0 && cur_power >= 0) {
            //깊이는 20이 최대라 19정도까지만 유용하지만
            //정작 power는 100000까지 가능하다. 그래서 밑과 같이 통계를 낸다.
            update_income(cur_parent, cur_power, 1);
            cur_power -= 1;
            cur_parent = parents[cur_parent];
        }
    }
}

void set_notification() {
    //일단 필요 정보 습득.
    int c;
    cin >> c;

    if (notify[c]) {
        //켜져 있었다. 일단 끈다.
        notify[c] = false;

        //그리고 본인의 위에 있는 node들이 받는 정보에 차이가 생길 것임을 갱신해야 한다.
        //정보가 들어오지 않기 시작한다.
        int height = 0;
        int cur_parent = parents[c];
        while (cur_parent > 0) {
            ++height;
            for (int idx = height; idx < 20; ++idx) {
                income_totals[cur_parent][idx - height] -= income_totals[c][idx];
            }
            //참고로 parent가 또 상위 노드에 원래 정보를 안 주는 경우라면
            //거기서부터는 갱신을 할 이유가 없다.
            if (!notify[cur_parent]) break;
            cur_parent = parents[cur_parent];
        }
    }
    else {
        //꺼져 있었다. 일단 킨다.
        notify[c] = true;

        //그리고 본인의 위에 있는 node들이 받는 정보에 차이가 생길 것임을 갱신해야 한다.
        //정보가 추가로 들어온다.
        int height = 0;
        int cur_parent = parents[c];
        while (cur_parent > 0) {
            ++height;
            for (int idx = height; idx < 20; ++idx) {
                income_totals[cur_parent][idx - height] += income_totals[c][idx];
            }
            //참고로 parent가 상위 노드에 정보를 안 주는 경우라면
            //거기서부터는 갱신을 할 이유가 없다.
            if (!notify[cur_parent]) break;
            cur_parent = parents[cur_parent];
        }
    }
}

void set_power() {
    //일단 필요 정보 습득.
    int c, power;
    cin >> c >> power;

    //먼저 본인 정보를 갱신
    int old_power = powers[c];
    powers[c] = power;
    update_income(c, old_power, -1);
    update_income(c, power, 1);

    //그 다음 본인 부모 정보들도 전부 갱신을 해야 한다.
    //단! 자식이 켜져 있어야 부모가 업데이트된다는 점 유의.
    int cur_parent = parents[c];
    int cur_child = c;
    power -= 1;

    //notify[cur_child]를 통해 현 채널이 위의 채널에 전달을 하고 있는지 판단한다.
    while (cur_parent > 0 && power >= 0 && notify[cur_child]) {
        update_income(cur_parent, power, 1);
        power -= 1;
        cur_child = cur_parent;
        cur_parent = parents[cur_parent];
    }

    //이를 원래 power에 대해서도 똑같이 수행
    cur_parent = parents[c];
    cur_child = c;
    old_power -= 1;

    while (cur_parent > 0 && old_power >= 0 && notify[cur_child]) {
        update_income(cur_parent, old_power, -1);
        old_power -= 1;
        cur_child = cur_parent;
        cur_parent = parents[cur_parent];
    }
}

void exchange_parent() {
    //일단 필요 정보 습득
    int c1, c2;
    cin >> c1 >> c2;
    int c1_parent = parents[c1];
    int c2_parent = parents[c2];

    //일단 부모가 같을 수 있다. 이에 해당되면 그냥 스킵.
    if (c1_parent == c2_parent) return;

    //부모가 다르면 본격적인 교환 진행.
    //먼저 본인 정보를 갱신한다.
    parents[c1] = c2_parent;
    parents[c2] = c1_parent;

    //각 부모에 대해서 데이터 갱신을 해야 한다.
    //위에 전달하지 않는 경우일 때까지, 혹은 부모가 0이 될 때까지 계속 업데이트를 한다.
    int height = 0;
    int old_parent = c1_parent; //'과거' c1 parent
    int cur_parent = c2_parent; //'현재' c1 parent
    int cur_child = c1;
    while (notify[cur_child] && old_parent > 0) {
        ++height;
        for (int idx = height; idx < 20; ++idx) {
            income_totals[old_parent][idx - height] -= income_totals[c1][idx];
        }
        cur_child = old_parent;
        old_parent = parents[old_parent];
    }
    height = 0;
    cur_child = c1;
    while (notify[cur_child] && cur_parent > 0) {
        ++height;
        for (int idx = height; idx < 20; ++idx) {
            income_totals[cur_parent][idx - height] += income_totals[c1][idx];
        }
        cur_child = cur_parent;
        cur_parent = parents[cur_parent];
    }
    
    height = 0;
    old_parent = c2_parent;
    cur_parent = c1_parent;
    cur_child = c2;
    while (notify[cur_child] && old_parent > 0) {
        ++height;
        for (int idx = height; idx < 20; ++idx) {
            income_totals[old_parent][idx - height] -= income_totals[c2][idx];
        }
        cur_child = old_parent;
        old_parent = parents[old_parent];
    }
    height = 0;
    cur_child = c2;
    while (notify[cur_child] && cur_parent > 0) {
        ++height;
        for (int idx = height; idx < 20; ++idx) {
            income_totals[cur_parent][idx - height] += income_totals[c2][idx];
        }
        cur_child = cur_parent;
        cur_parent = parents[cur_parent];
    }    
}

void cnt_rooms() {
    //일단 필요 정보 습득.
    int c;
    cin >> c;

    //본인 방을 제외하고, 각 세기별 income이 얼마인지 계산한다.
    int total = -1;
    for (int power = 0; power < 20; ++power) {
        total += income_totals[c][power];
    }
    
    //출력
    cout << total << "\n";
}

int main() {
    //정보 수집
    cin >> N >> Q;

    //쿼리 처리
    for (int query = 1; query <= Q; ++query) {
        //명령어 수집
        cin >> cmd;
        switch (cmd) {
        case 100:
            initialize();
            break;
        case 200:
            set_notification();
            break;
        case 300:
            set_power();
            break;
        case 400:
            exchange_parent();
            break;
        case 500:
            cnt_rooms();
            break;
        }

        //debug_print(query);
    }

    cout << flush;
}