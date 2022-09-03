#include <fstream>
#include <string>
#include <random>
#include <thread>
#include <iostream>
#include <lrrp.h>

using namespace std;

class TempGen
{
private:
    int m_tmp = 0;

    const int MAX_RANGE = 5;
public:
    TempGen() {
        srand(time(NULL));
    }

    int getNext() {
        m_tmp += (rand() % MAX_RANGE - (MAX_RANGE / 2));
        return m_tmp;
    }
};

class TempLogger
{
private:
    string m_logFilePath;
public:
    TempLogger(string logFilePath = "/dev/kmsg")
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

const int TEMP_CHECK_INTERVAL_S = 1;
int currTemp = 0;

class temp_handler : public lrrp::handler_base {
public:
    virtual lrrp::response handle(const lrrp::request& req) override {
        return lrrp::response_builder().set_payload(json({{"temp", currTemp}})).build();
    }
};

void start_up_server() {
    lrrp::server s(3660);
    s.add_handler("temp", std::make_unique<temp_handler>());

    s.run_async();
}

int main(int argc, char** argv) {
    std::thread serv(start_up_server);
    serv.detach();

    TempGen  tempGen;
    TempLogger tempLogger;

    for(int i = 0; true; i++) {
        currTemp = tempGen.getNext();
        tempLogger.log(to_string(currTemp) + " C");
        cout << "Checked temperature (" << i << ") times" << endl;

        this_thread::sleep_for(chrono::seconds(TEMP_CHECK_INTERVAL_S));
    }
}
