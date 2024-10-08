#include <iostream>
#include <tuple>

#define MAX_N 100

using namespace std;

int n, k;

pair<int, bool> u[MAX_N];
pair<int, bool> d[MAX_N];

void Shift() {
    pair<int, bool> temp = u[n - 1];
    for(int i = n - 1; i >= 1; i--)
        u[i] = u[i - 1];
    u[0] = d[n - 1];
    
    for(int i = n - 1; i >= 1; i--)
        d[i] = d[i - 1];
    d[0] = temp;
}

bool CanGo(int idx) {
    // 밖으로 나가는 것은 항상 가능합니다.
    if(idx == n)
        return true;
    
    // 안정성이 0보다 크면서 사람이 없는 경우에만
    // 이동이 가능합니다.
    int stability; bool occupied;
    tie(stability, occupied) = u[idx];
    return stability > 0 && !occupied; 
}

void Move(int idx) {
    // 현재 위치에 있던 사람은 사라집니다.
    int curr_stability;
    tie(curr_stability, ignore) = u[idx];
    u[idx] = make_pair(curr_stability, false);
    
    // 밖으로 나가는 것이 아니라면,
    // 안정성이 1 감소하며, 사람이 추가됩니다.
    if(idx + 1 < n) {
        int next_stability;
        tie(next_stability, ignore) = u[idx + 1];
        u[idx + 1] = make_pair(next_stability - 1, true);
    }
}

void MoveAll() {
    // 현재 위치에 사람이 있고, 그 다음 위치로 이동이
    // 가능한 경우에만 움직입니다.
    for(int i = n - 1; i >= 0; i--) {
        bool occupied;
        tie(ignore, occupied) = u[i];
        if(occupied && CanGo(i + 1))
            Move(i);
    }
}

void Add() {
    // 안정성이 0보다 크고 사람이 없는 경우에만
    // 첫 번째 위치에 사람을 올려놓습니다.
    int stability; bool occupied;
    tie(stability, occupied) = u[0];
    if(stability > 0 && !occupied)
        u[0] = make_pair(stability - 1, true);
}

void Simulate() {
    // Step1. 무빙워크를 한 칸 회전시킵니다.
    Shift();
    
    // Step2. 사람들을 한 칸씩 앞으로 이동시킵니다.
    MoveAll();
    
    // Step3. 사람을 새로 올립니다.
    Add();
    
    // Step4. n번 칸 위치에 사람이 있다면, 즉시 내려줍니다.
    bool occupied;
    tie(ignore, occupied) = u[n - 1];
    if(occupied)
        Move(n - 1);
}

bool Done() {
    int unstable_cnt = 0;
    for(int i = 0; i < n; i++) {
        int stability;
        tie(stability, ignore) = u[i];
        if(!stability) unstable_cnt++;
    }
    for(int i = 0; i < n; i++) {
        int stability;
        tie(stability, ignore) = d[i];
        if(!stability) unstable_cnt++;
    }
    
    return unstable_cnt >= k;
}

int main() {
    cin >> n >> k;
    
    for(int i = 0; i < n; i++) {
        int stability;
        cin >> stability;
        u[i] = make_pair(stability, false);
    }
    for(int i = 0; i < n; i++) {
        int stability;
        cin >> stability;
        d[i] = make_pair(stability, false);
    }
    
    int trial = 0;
    
    while(!Done()) {
        Simulate();
        trial++;
    }
    
    cout << trial;
    return 0;
}