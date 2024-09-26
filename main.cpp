#include "threadpool.hpp"
#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
using namespace std;
using namespace literals::chrono_literals;

int main() {
    ThreadPool thread_pool(5);

    auto task1 = [](size_t nums) -> int {

        cout << "I'm task 1" << endl;
        for(size_t i = 0; i < nums; ++i) {
            cout << "times past " << i << " seconds" << endl;

            
            this_thread::sleep_for(1s);
        }
        return 42;
    };

    auto task2 = [](const vector<int> &arrays) -> int {
        for(auto it = arrays.begin(); it != arrays.end(); ++it) {
            cout << *it << " ";
        }
        cout << endl;

        return 38;
    };

    vector<int> vec = {2, 2 ,5 ,6,9,6,4,2,31,28,35,54};
    auto res1 = thread_pool.submit_task(task1, 10);
    auto res2 = thread_pool.submit_task(task2, vec);
    cout << res2.get() << endl;
    cout << res1.get() << endl;

}