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

const int TEMP_CHECK_INTERVAL_S = 5;

int main(int argc, char** argv) {
    logger::alog("temp-check-service", "Initializing the threads for MIDF services...");
    std::thread serv(start_up_server);
    serv.detach();

    std::thread test_service(start_up_test_service);
    test_service.detach();
    logger::alog("temp-check-service", "MIDF services has been inited");

    logger::alog("temp-check-service", "Getting the devices...");
    auto devices = get_devices<DS18B20>();
    logger::alog("temp-check-service", "Devices are received");

    for(int i = 0; true; i++) {
        float temperature = 0;
        if(devices.size() > 0) {
            temperature = devices[0].temperature();
            logger::alog("temp-check-service", "Measured temperature is: " + to_string(temperature) + " C");
        } else {
            logger::alog("temp-check-service", "Error: No available temperature sensor devices");
        }

        this_thread::sleep_for(chrono::seconds(TEMP_CHECK_INTERVAL_S));
    }
}
