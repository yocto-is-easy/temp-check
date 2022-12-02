#include <fstream>
#include <string>
#include <random>
#include <thread>
#include <vector>
#include <iostream>
#include <filesystem>
#include <memory>

#include <supervisor.hpp>
#include <logger.hpp>
#include "temp_check_interface.hpp"

using namespace std;
namespace fs = std::filesystem;

class DS18B20
{
    fs::path m_path;
public:
    static const string id;

    DS18B20(fs::path path) : m_path(path) {}

    float temperature() {
        ifstream tempFile((m_path/"temperature").string());
        int temp = 0;
        if(tempFile.is_open()) {
            tempFile >> temp;
        }
        tempFile.close();
        return (float)temp / 1000.0;
    }
};

const string DS18B20::id = "28";

template <typename Device>
vector<Device> get_devices() {
    vector<Device> ret;

    fs::path devices("/sys/bus/w1/devices");
    for(auto& el : fs::directory_iterator(devices)) {
        if(el.is_directory() && el.path().filename().string().starts_with(Device::id)) {
            ret.push_back(Device(fs::absolute(el.path())));
        }
    }

    return ret;
}

INIT_MIDF_SERVER(temp_check);

MIDF_IMPL_FUNC(float, temp_check, get_temperature) () {
    auto devices = get_devices<DS18B20>();
    float temperature = 0;
    if(devices.size() > 0) {
        temperature = devices[0].temperature();
    }
    return temperature;
}

void start_up_server() {
    START_MIDF_SERVER(temp_check);
}

INIT_MIDF_SERVER(test_service);

MIDF_IMPL_FUNC(int, test_service, sum, int, int) (int a, int b) {
    return a + b;
}

void start_up_test_service() {
    START_MIDF_SERVER(test_service);
}

const int TEMP_CHECK_INTERVAL_S = 1;

int main(int argc, char** argv) {
    std::thread serv(start_up_server);
    serv.detach();

    std::thread test_service(start_up_test_service);
    test_service.detach();

    for(int i = 0; true; i++) {
        auto devices = get_devices<DS18B20>();
        float temperature = 0;
        if(devices.size() > 0) {
            temperature = devices[0].temperature();
            logger::log("temp-check-service", to_string(temperature) + " C");
        }

        this_thread::sleep_for(chrono::seconds(TEMP_CHECK_INTERVAL_S));
    }
}
