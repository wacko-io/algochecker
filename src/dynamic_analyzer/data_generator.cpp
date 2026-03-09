#include "dynamic_analyzer/data_generator.h"

namespace algochecker::dynamic_analyzer {

std::vector<int> generate_test_data(int size) {
    if (size < 0) {
        size = 0;
    }
    return std::vector<int>(static_cast<std::size_t>(size), 0);
}

}
