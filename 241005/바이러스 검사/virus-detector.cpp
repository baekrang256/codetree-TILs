#include <iostream>

#define MAX_N 1000000

using namespace std;

long long int customers[MAX_N];
long long int manager_cnt, employee_cnt;

int main() {
    int n; long long int answer = 0;
    cin >> n;
    for (int restaurant_idx = 0; restaurant_idx < n; ++restaurant_idx) {
        cin >> customers[restaurant_idx];
    }
    cin >> manager_cnt >> employee_cnt;

    for (int restaurant_idx = 0; restaurant_idx < n; ++restaurant_idx) {
        ++answer;
        customers[restaurant_idx] -= manager_cnt;
        if (customers[restaurant_idx] > 0) {
            answer += customers[restaurant_idx] / employee_cnt + (customers[restaurant_idx] % employee_cnt != 0 ? 1 : 0);
        }
    }

    cout << answer << flush;

    return 0;
}