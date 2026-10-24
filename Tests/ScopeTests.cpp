#include "Core/ScopeFifo.h"
#include <iostream>
#include <stdexcept>
#include <thread>
int main() {
    da::ScopeFifo fifo;
    da::ScopeFrame f{1, 2, 3, 4}, out;
    for (size_t i = 0; i < da::ScopeFifo::capacity - 1; ++i)
        if (fifo.push(&f, 1) != 1)
            return 1;
    if (fifo.push(&f, 1) != 0)
        return 2; // full audio queue drops immediately
    for (size_t i = 0; i < da::ScopeFifo::capacity - 1; ++i)
        if (fifo.pop(&out, 1) != 1 || out.right != 4)
            return 3;
    if (fifo.pop(&out, 1) != 0)
        return 4;
    constexpr size_t total = 200000;
    bool valid = true;
    std::thread producer([&] {
        for (size_t i = 0; i < total; ++i) {
            da::ScopeFrame frame{float(i), float(i) * 2, 0, 0};
            while (fifo.push(&frame, 1) == 0)
                std::this_thread::yield();
        }
    });
    for (size_t i = 0; i < total; ++i) {
        while (fifo.pop(&out, 1) == 0)
            std::this_thread::yield();
        if (out.input != float(i) || out.output != float(i) * 2)
            valid = false;
    }
    producer.join();
    if (!valid)
        return 5;
    std::cout << "PASS scope FIFO: full/empty/wrap/200000 concurrent ordered frames\n";
}
