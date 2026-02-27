#include "dynamic_analyzer/exporter.h"

#include <fstream>

namespace algochecker::dynamic_analyzer {

bool export_measurements_csv(const std::string& output_path, const std::vector<long long>& measurements) {
    std::ofstream out(output_path);
    if (!out.is_open()) {
        return false;
    }

    out << "n,time_ns\n";
    for (std::size_t i = 0; i < measurements.size(); ++i) {
        out << i << ',' << measurements[i] << '\n';
    }

    return true;
}

} // namespace algochecker::dynamic_analyzer
