#include <exception>
#include "application.hpp"

int main() {
    try {
        SetTraceLogLevel(LOG_WARNING);
        auto app = std::make_unique<meow::Application>();
        app->run();
    } catch (const raylib::RaylibException &e) {
        std::cerr << "[raylib error]:\t" << e.what() << std::endl;
        return 1;
    } catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "don't throw that at me :c" << std::endl;
        return 1;
    }
}
