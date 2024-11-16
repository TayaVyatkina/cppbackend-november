#include "file_system.h"

namespace userFileSystem{

using namespace std::literals;


bool IsSubPath(std::filesystem::path path, std::filesystem::path base) {
    path = std::filesystem::weakly_canonical(path);
    base = std::filesystem::weakly_canonical(base);

    
    for (auto b = base.begin(), p = path.begin(); b != base.end(); ++b, ++p) {
        if (p == path.end() || *p != *b) {
            return false;
        }
    }
    return true;
}

}