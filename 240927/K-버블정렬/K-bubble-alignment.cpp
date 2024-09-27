#include <iostream>
using namespace std;
int n, k;
int arr[20];

int main() {
    cin >> n >> k;
    for (int idx = 0; idx < n; ++idx) {
        cin >> arr[idx];
    }

    for (int turn = 0; turn < k; ++turn) {
        for (int idx = 1; idx < n; ++idx) {
            if (arr[idx] < arr[idx-1]) {
                int tmp = arr[idx-1];
                arr[idx-1] = arr[idx];
                arr[idx] = tmp;
            }
        }
    }

    for (int idx = 0; idx < n; ++idx) {
        cout << arr[idx] << " ";
    }
    cout << flush;

    return 0;
}