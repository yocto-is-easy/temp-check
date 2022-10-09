#include <iostream>
#include <lrrp.h>

using namespace std;

int main() {
    lrrp::client c("127.0.0.1", 3660);
    lrrp::request req = lrrp::request_builder("temp")
            .build();

    lrrp::response res = c.send(req);
    std::cout
        << "Current temperature is: "
        << res.get_payload()["temp"]
        << " C" << std::endl;
}
