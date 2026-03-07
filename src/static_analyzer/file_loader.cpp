#include "static_analyzer/file_loader.h"
#include <fstream>
#include <string>

using namespace std;

namespace algochecker::static_analyzer {

    string load_source_file(const string& path)
    {
        ifstream file(path);

        if (!file.is_open())
        {
            return "";
        }

        string line;
        string code = "";

        while (getline(file, line))
        {
            code += line;
            code += "\n";
        }

        file.close();

        return code;
    }

} // namespace algochecker::static_analyzer