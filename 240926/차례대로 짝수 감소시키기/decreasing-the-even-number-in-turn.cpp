#include <iostream>

using namespace std;

int n;

int main() {
    cin >> n;
    int base = 1000;
    int cnt = 0;
    int total = 0;
    int even_num = 0;
    while (base > n) {
        ++cnt;
        even_num += 2;
        total += even_num;
        base -= even_num;
    }

    cout << cnt << " " << total;

}