#include "dynamic_analyzer/benchmarker.h"

namespace algochecker::dynamic_analyzer {

long long benchmark_stub(const std::vector<int>& data) {
    return static_cast<long long>(data.size());
}

}
