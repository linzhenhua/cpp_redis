#include "cpp_redis/cpp_redis.hpp"

#include <signal.h>
#include <iostream>

bool should_exit = false;
cpp_redis::redis_subscriber sub;

void
sigint_handler(int) {
    std::cout << "disconnected (sigint handler)" << std::endl;
    sub.disconnect();
}

int
main(void) {
    sub.set_disconnection_handler([] (cpp_redis::redis_subscriber&) {
        std::cout << "sub disconnected (disconnection handler)" << std::endl;
        should_exit = true;
    });

    try {
        sub.connect();
    }
    catch (const cpp_redis::redis_error& e) {
        std::cerr << e.what() << std::endl;
        return -1;
    }

    std::cout << "Connected" << std::endl;

    sub.subscribe("some_chan", [] (const std::string& chan, const std::string& msg) {
        std::cout << "MESSAGE " << chan << ": " << msg << std::endl;
    });
    sub.psubscribe("*", [] (const std::string& chan, const std::string& msg) {
        std::cout << "PMESSAGE " << chan << ": " << msg << std::endl;
    });

    signal(SIGINT, &sigint_handler);
    while (not should_exit);

    return 0;
}