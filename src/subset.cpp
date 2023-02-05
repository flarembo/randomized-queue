#include "subset.h"

#include "randomized_queue.h"

#include <string>

void subset(unsigned long k, std::istream & in, std::ostream & out)
{
    randomized_queue a = randomized_queue<std::string>();
    std::string line;
    while (std::getline(in, line)) {
        a.enqueue(line);
    }

    while (!a.empty() && k != 0) {
        out << a.dequeue() << std::endl;
        k--;
    }
}
