#pragma once

#include <string>

#include "algochecker/analyzer.h"

namespace algochecker::web_server {

void start_server_stub();
AnalysisReport analyze_request_stub(const std::string& source);

}
