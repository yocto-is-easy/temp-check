#include <fstream>
#include <string>
#include <random>
#include <thread>
#include <vector>
#include <iostream>
#include <filesystem>
#include <memory>
#include <lrrp.h>

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

class Logger
{
private:
    string m_logFilePath;
public:
    Logger(string logFilePath = "/dev/kmsg")
        : m_logFilePath(logFilePath) {}
    
    void log(string msg) {
        ofstream logFile;

        logFile.open(m_logFilePath);
        if(logFile.is_open()) {
            logFile << ("temp-check-service: " + msg);
            logFile.close();
            return;
        }
    }
};

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

class temp_handler : public lrrp::handler_base {
public:
    virtual lrrp::response handle(const lrrp::request& req) override {
        auto devices = get_devices<DS18B20>();
        float temperature = 0;
        if(devices.size() > 0) {
            temperature = devices[0].temperature();
        }
        return lrrp::response_builder().set_payload(json({{"temp", temperature}})).build();
    }
};

void start_up_server() {
    lrrp::server s(3660);
    s.add_handler("temp", std::make_unique<temp_handler>());

    s.run_async();
}

const int TEMP_CHECK_INTERVAL_S = 1;

int main(int argc, char** argv) {
    std::thread serv(start_up_server);
    serv.detach();

    Logger logger;

    for(int i = 0; true; i++) {
        auto devices = get_devices<DS18B20>();
        float temperature = 0;
        if(devices.size() > 0) {
            temperature = devices[0].temperature();
            logger.log(to_string(temperature) + " C");
        }

        this_thread::sleep_for(chrono::seconds(TEMP_CHECK_INTERVAL_S));
    }
}
