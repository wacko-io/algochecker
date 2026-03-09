#pragma once

#include <string>
#include <vector>

namespace algochecker::dynamic_analyzer {

bool export_measurements_csv(const std::string& output_path, const std::vector<long long>& measurements);

}
