#include "src/application/config/config.h"

namespace ui {

Config& Config::GetInstance() {
    static Config instance;
    return instance;
};

}