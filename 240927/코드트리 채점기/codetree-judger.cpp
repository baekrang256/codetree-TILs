#include <iostream>
#include <queue>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <string>

using namespace std;

struct problem {
    string url;
    string domain;
    int priority;
    int assigned_time;
};

struct compare {
    bool operator() (const problem& p1, const problem& p2) const {
        if (p1.priority != p2.priority) return p1.priority < p2.priority;
        return p1.assigned_time < p2.assigned_time;
    }
};

int N, Q, cmd;
bool on_work[50001] = { false }; //worker 동작 여부.
string assigned_work[50001]; //일 하고 있는 worker의 대상 domain. 이는 나중에 domain 채점 시간 및 종료 시간 추적에 필요.
priority_queue<int, vector<int>, greater<int>> pending_worker; //일 안하고 있는 worker들 모음.
set<problem, compare> pending_jobs; //채점 대기에 있는 job 모음. 
unordered_set<string> pending_urls; //채점 대기에 있는 url들 모음. 이는 채점 요청에서 채점 대기 큐에 넣을지 판단하는데 사용.
unordered_map<string, pair<int, int>> start_end_time; //각 도메인의 최근 진행 채점 시작 시간 및 종료 시간. 만약 채점 시작 시간이 종료 시간보다 크면 아직 채점 중이라는 뜻. first가 시작, second가 종료

void debug_print(int query) {
    cout << "current query : " << query << "\n";
    cout << "pending jobs in order\n";
    for (auto val : pending_jobs) {
        cout << val.url << " " << val.domain << " " << val.priority << " " << val.assigned_time << "\n";
    }
    cout << "\n";
}

string extract(string& url) {
    return url.substr(0, url.find("/"));
}

//initialize 작업이라고 볼 수 있다.
void prepare() {
    string u0;
    cin >> N >> u0;
    for (int i = 1; i <= N; ++i) {
        pending_worker.push(i);
    }
    pending_urls.insert(u0);
    string domain = extract(u0);
    pending_jobs.insert({ u0, domain, 1, 0 });
}

//채점 요청
void request() {
    int t, p; string u;
    cin >> t >> p >> u;

    //이미 정확히 똑같은 url이 대기 중이면 그냥 넘어가야 한다.
    if (pending_urls.find(u) != pending_urls.end()) return;

    //아니라면 집어넣는다.
    pending_urls.insert(u);
    string domain = extract(u);
    pending_jobs.insert({ u, domain, p, t });
}

//채점 시도
void trial() {
    int t;
    cin >> t;

    //일단 쉬고 있는 채점기가 있는가? 없으면 넘어간다.
    if (pending_worker.size() == 0) return;

    //다음으로 수행할 수 있는 작업이 있긴 한가?
    auto it = pending_jobs.begin();
    auto start_end_time_it = start_end_time.end();
    while (it != pending_jobs.end()) {
        start_end_time_it = start_end_time.find(it->domain);
        //아직 한번도 시작한 적이 없으면 풀어야죠?
        if (start_end_time_it == start_end_time.end()) {
            break;
        }
        int start_time = start_end_time_it->second.first;
        int end_time = start_end_time_it->second.second;
        //만약 진행 중이면 풀지 말아야죠?
        if (start_time > end_time) {
            ++it;
            continue;
        }
        //만약 진행 중은 아닌데 부적절한 채점이면 수행하면 안됨.
        if (start_time + 3 * (end_time - start_time) > t) {
            ++it;
            continue;
        }
        //전부 통과. 괜찮다.
        break;
    }

    //할 수 있는 작업이 없으면 넘어간다.
    if (it == pending_jobs.end()) return;

    //할 수 있는 작업이면 즉시 수행한다.
    //먼저 worker을 정한고 수행한다.
    int worker = pending_worker.top();
    pending_worker.pop();
    on_work[worker] = true;
    assigned_work[worker] = it->domain;

    //다음으로 시작시간을 업데이트해야 하는데, 없으면 삽입을 한다.
    if (start_end_time_it == start_end_time.end()) {
        start_end_time.insert({ it->domain, {t, 0} }); //시작은 t, 종료는 0으로 설정
    }
    else {//있으면 시작 시간만 변경한다.
        start_end_time_it->second.first = t;
    }

    //이제 대기 중이지 않으니 갱신한다. url과 job를 갱신
    pending_urls.erase(it->url);
    pending_jobs.erase(it);
}

//작업 종료
void over() {
    int t, J_id;
    cin >> t >> J_id;

    //애초에 작업 중이 아니면 무시한다.
    if (!on_work[J_id]) return;

    //일단 작업 종료. 대기 중으로 다시 넣어야 한다.
    on_work[J_id] = false;
    pending_worker.push(J_id);

    //시작 종료 시간 갱신 필요
    auto it = start_end_time.find(assigned_work[J_id]);
    it->second.second = t;
}

//pending_jobs 사이즈 출력
void snoop() {
    int t;
    cin >> t;
    cout << pending_jobs.size() << "\n";
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    cout.tie(NULL);
    cin >> Q;

    for (int query = 1; query <= Q; ++query) {
        cin >> cmd;
        switch (cmd) {
        case 100:
            prepare();
            break;
        case 200:
            request();
            break;
        case 300:
            trial();
            break;
        case 400:
            over();
            break;
        case 500:
            snoop();
            break;
        }

        //debug_print(query);
    }

    cout << flush;
}