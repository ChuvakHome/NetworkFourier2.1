
#include <iostream>

#include <Fourier/Global.hpp>
#include <Fourier/Model/Fourier.hpp>
#include <Fourier/View.hpp>

int main(int argc, char **argv, char **env) {
    (void) argc;
    (void) argv;
    (void) env;

    std::cout << "Engine Demo" << std::endl;

    FourierEngine::init();
    init_global(argc, argv, env);
    init_view();

    return 0;
}
