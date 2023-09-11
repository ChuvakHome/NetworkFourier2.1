
#include <iostream>

#include <Fourier/Global.hpp>
#include <Fourier/Model/Fourier.hpp>
#include <Fourier/View.hpp>

#ifdef SFML_SYSTEM_WINDOWS
    #include <Windows.h>
#endif

int main(int argc, char **argv, char **env) {
    (void) argc;
    (void) argv;
    (void) env;

    #ifdef SFML_SYSTEM_WINDOWS
        SetConsoleCP(1251);
        SetConsoleOutputCP(1251);
    #endif

    std::cout << "Engine Demo" << std::endl;

    FourierEngine::init();
    init_global(argc, argv, env);
    init_view();

    return 0;
}
