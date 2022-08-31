#include <fstream>
#include <string>
#include <random>
#include <thread>

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

int main(int argc, char** argv) {
    TempGen  tempGen;
    TempLogger tempLogger;

    while(true) {
        tempLogger.log(to_string(tempGen.getNext()) + " C");

        this_thread::sleep_for(chrono::seconds(TEMP_CHECK_INTERVAL_S));
    }
}
