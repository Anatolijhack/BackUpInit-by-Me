#include <iostream>
#include <memory>
#include "ComandDispatcher.h"
#include "RunCommand.h"
#include "InitCommand.h"
#include "StatusComand.h"

int main(int argc, char* argv[]) {
    CommandDispatcher dispatcher;

    // Регистрируем команды
    dispatcher.registerCommand("run", std::make_unique<RunCommand>());
    dispatcher.registerCommand("init", std::make_unique<InitCommand>());
    dispatcher.registerCommand("status", std::make_unique<StatusCommand>());

    try {
        dispatcher.run(argc, argv);
    }
    catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << "\n";
        return 1;
    }
    catch (...) {
        std::cerr << "Unknown fatal error\n";
        return 1;
    }

    return 0;
}