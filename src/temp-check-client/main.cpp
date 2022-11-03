#include <iostream>

#include "temp_check_interface.hpp"

using namespace std;

int main() {
    std::cout
        << "Current temperature is: "
        << temp_sensor_get_temperature()
        << " C" << std::endl;
}
