#include "dynamic_analyzer/visualizer.h"

#include <fstream>

namespace algochecker::dynamic_analyzer {

bool generate_gnuplot_script(const std::string& data_path, const std::string& script_path) {
    std::ofstream out(script_path);
    if (!out.is_open()) {
        return false;
    }

    out << "set datafile separator ','\n";
    out << "set terminal png size 1024,768\n";
    out << "set output 'benchmark.png'\n";
    out << "set title 'Algorithm Runtime'\n";
    out << "set xlabel 'N'\n";
    out << "set ylabel 'Time (ns)'\n";
    out << "plot '" << data_path << "' using 1:2 with linespoints title 'T(N)'\n";

    return true;
}

}
