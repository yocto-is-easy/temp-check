#include <iostream>

#include "temp_check_interface.hpp"

using namespace std;

int main() {
    std::cout
        << "Current temperature is: "
        << temp_check::get_temperature()
        << " C"
        << string(' ', 10) << "sum:" << test_service::sum(1, 2) << endl;
}
