#include <iostream>

#define MAX_N 100000
#define MAX_M 100000
#define NONE 0

using namespace std;

//doubly linked list를 활용하면 매우 편함.
struct Present_info {
    int prev_idx; //본인 이전 선물 번호. 없으면 -1
    int next_idx; //본인 이후 선물 번호. 없으면 -1
};

struct Belt_info {
    int head_idx; //본인 벨트의 가장 앞에 있는 선물 번호. 없으면 -1
    int tail_idx; //본인 벨트의 가장 뒤에 있는 선물 번호. 없으면 -1
    int present_cnt; //본인 벨트에 있는 선물 번호
};

int q, cmd, n, m;
Present_info presents[MAX_M+1]; //각 번호별 선물 관련 정보
Belt_info belts[MAX_N + 1]; //각 번호별 벨트 관련 정보

//디버깅 용도
void debug_print() {
    cout << "presents info\n";
    for (int i = 1; i <= m; ++i) {
        cout << "{" << presents[i].prev_idx << ", " << presents[i].next_idx << "}\n";
    }
    cout << "\n";
    cout << "belts info\n";
    for (int i = 1; i <= n; ++i) {
        cout << "{" << belts[i].head_idx << ", " << belts[i].tail_idx << ", "
            << belts[i].present_cnt << "}\n";
    }
    cout << "\n";
}

void print_present_cnt(int belt_num) {
    cout << belts[belt_num].present_cnt << "\n";
}

//리스트 조작 관련
//벨트의 끝에다가 집어넣기.
void add_end(int belt_num, int present_idx) {
    //첫 element인 경우 본인이 head이자 tail이 된다.
    if (belts[belt_num].head_idx == NONE) {
        belts[belt_num].head_idx = present_idx;
        belts[belt_num].tail_idx = present_idx;
        presents[present_idx].prev_idx = NONE;
        presents[present_idx].next_idx = NONE;
    }
    //2번째 이후의 element인 경우 본인이 새로운 tail이 된다.
    //기존 노드 정보도 갱신해야 하는 점 유의.
    else {
        int tail_present_idx = belts[belt_num].tail_idx;
        presents[present_idx].prev_idx = tail_present_idx;
        presents[present_idx].next_idx = NONE;
        presents[tail_present_idx].next_idx = present_idx;
        belts[belt_num].tail_idx = present_idx;
    }
    
    //이후 벨트 선물 개수 추가
    belts[belt_num].present_cnt += 1;
}

//서로 연결되는 새로운 두 노드를 설정해주기.
//NONE이어도 크게 상관은 없다.
void new_prev_next(int prev_idx, int next_idx) {
    presents[prev_idx].next_idx = next_idx;
    presents[next_idx].prev_idx = prev_idx;
}

//head를 다음 노드의 녀석으로 바꾸기
//다음 head가 NONE일 수도 있다.
void change_head_next(int belt_num) {
    int prev_head = belts[belt_num].head_idx;
    int new_head = presents[prev_head].next_idx;
    belts[belt_num].head_idx = new_head;
    if (new_head == NONE) {
        belts[belt_num].tail_idx = NONE; //tail도 없어짐.
    }
    belts[belt_num].present_cnt -= 1;
    presents[new_head].prev_idx = NONE;
}

//복잡도 : O(N)
//100번 명령어 
void construct_factory() {
    cin >> n >> m;
    int belt_num;
    for (int present_idx = 1; present_idx <= m; ++present_idx) {
        cin >> belt_num;
        add_end(belt_num, present_idx);
    }
}

//복잡도 : O(1)
//200번 명령어
void move_all_products() {
    int m_src, m_dst;
    cin >> m_src >> m_dst;

    //m_src에 선물이 없으면 스킵
    if (belts[m_src].present_cnt == 0) {
        print_present_cnt(m_dst);
        return;
    }
    
    //먼저, m_dst의 tail present의 정보를 갱신을 한다.
    //이때 m_dst의 head가 NONE일수도 있다.
    int m_dst_prev_head = belts[m_dst].head_idx;
    int m_src_prev_tail = belts[m_src].tail_idx;
    new_prev_next(m_src_prev_tail, m_dst_prev_head);

    //다음, m_dst의 정보를 갱신한다.
    //유의할게, NONE인 경우 tail_idx를 갱신해야 한다.
    if (belts[m_dst].tail_idx == NONE)
        belts[m_dst].tail_idx = belts[m_src].tail_idx;
    belts[m_dst].head_idx = belts[m_src].head_idx;
    belts[m_dst].present_cnt += belts[m_src].present_cnt;

    //다음, m_src의 정보를 갱신한다.
    belts[m_src].head_idx = NONE;
    belts[m_src].tail_idx = NONE;
    belts[m_src].present_cnt = 0;

    //m_dst 선물 개수 출력.
    print_present_cnt(m_dst);
}

//복잡도 : O(1)
void switch_only_front() {
    int m_src, m_dst;
    cin >> m_src >> m_dst;
    int m_src_prev_head = belts[m_src].head_idx;
    int m_dst_prev_head = belts[m_dst].head_idx;

    //둘 다 head가 없으면 그냥 무시한다.
    if (m_src_prev_head == NONE && m_dst_prev_head == NONE) {
        print_present_cnt(m_dst);
        return;
    }

    //둘 중 한쪽에만 head가 없으면 head를 옮긴다.
    if (m_src_prev_head == NONE) {
        change_head_next(m_dst);
        add_end(m_src, m_dst_prev_head);
        print_present_cnt(m_dst);
        return;
    }
    else if (m_dst_prev_head == NONE) {
        change_head_next(m_src);
        add_end(m_dst, m_src_prev_head);
        print_present_cnt(m_dst);
        return;
    }

    //둘 다 head가 있으면 head를 서로 바꾼다.
    int m_src_prev_head_next = presents[m_src_prev_head].next_idx;
    int m_dst_prev_head_next = presents[m_dst_prev_head].next_idx;
    belts[m_src].head_idx = m_dst_prev_head;
    belts[m_dst].head_idx = m_src_prev_head;
    presents[m_dst_prev_head].next_idx = m_src_prev_head_next;
    presents[m_src_prev_head].next_idx = m_dst_prev_head_next;
    presents[m_src_prev_head_next].prev_idx = m_dst_prev_head;
    presents[m_dst_prev_head_next].prev_idx = m_src_prev_head;

    //head를 바꿨는데 그게 tail이 되어야 할 수도 있다.
    if (belts[m_src].present_cnt == 1) {
        belts[m_src].tail_idx = m_dst_prev_head;
    }
    if (belts[m_dst].present_cnt == 1) {
        belts[m_dst].tail_idx = m_src_prev_head;
    }

    print_present_cnt(m_dst);
}

//복잡도 : O(floor(n/2)). 100번만 하니까 괜찮을거다. 응.
void distribute_products() {
    int m_src, m_dst;
    cin >> m_src >> m_dst;
    int cnt = belts[m_src].present_cnt / 2;
    if (cnt == 0) {
        print_present_cnt(m_dst);
        return; //선물 전달할게 없음.
    }

    int head_idx = belts[m_src].head_idx;
    int cur_idx = head_idx;
    int iter_cnt = cnt - 1;
    while (iter_cnt-- > 0) {
        cur_idx = presents[cur_idx].next_idx;
    }

    //m_src head는 cur_idx의 next가 된다.
    //만약 그게 NONE인 경우, tail도 NONE이 된다.
    //m_src의 선물 개수는 cnt만큼 사라진다.
    int cur_idx_next = presents[cur_idx].next_idx;
    presents[cur_idx_next].prev_idx = NONE;
    belts[m_src].head_idx = cur_idx_next;
    if (cur_idx_next == NONE) {
        belts[m_src].tail_idx = NONE;
    }
    belts[m_src].present_cnt -= cnt;

    //head_idx는 dst의 새로운 head가 된다.
    //cur_idx는 dst의 기존 head의 앞에 들어가게 된다.
    //m_dst 선물 개수는 cnt만큼 추가된다.
    int m_dst_prev_head = belts[m_dst].head_idx;
    presents[m_dst_prev_head].prev_idx = cur_idx;
    presents[cur_idx].next_idx = m_dst_prev_head;
    belts[m_dst].head_idx = head_idx;
    if (belts[m_dst].present_cnt == 0) {
        belts[m_dst].tail_idx = cur_idx;
    }
    belts[m_dst].present_cnt += cnt;

    print_present_cnt(m_dst);
}

//복잡도 : O(1)
void obtain_present_info() {
    int p_num;
    cin >> p_num;
    int prev = presents[p_num].prev_idx == NONE ? -1 : presents[p_num].prev_idx;
    int next = presents[p_num].next_idx == NONE ? -1 : presents[p_num].next_idx;
    cout << prev + 2 * next << "\n";
}

//복잡도 : O(1)
void obtain_belt_info() {
    int b_num;
    cin >> b_num;
    int head = belts[b_num].head_idx == NONE ? -1 : belts[b_num].head_idx;
    int tail = belts[b_num].tail_idx == NONE ? -1 : belts[b_num].tail_idx;
    int present_cnt = belts[b_num].present_cnt;
    cout << head + 2 * tail + 3 * present_cnt << "\n";
}

int main() {
    cin >> q;
    for (int query = 0; query < q; ++query) {
        //cout << "query : " << query + 1 << "\n";
        cin >> cmd;
        switch (cmd) {
        case 100:
            construct_factory();
            break;
        case 200:
            move_all_products();
            break;
        case 300:
            switch_only_front();
            break;
        case 400:
            distribute_products();
            break;
        case 500:
            obtain_present_info();
            break;
        case 600:
            obtain_belt_info();
            break;
        }
        //debug_print();
    }

    cout << flush;

    return 0;
}