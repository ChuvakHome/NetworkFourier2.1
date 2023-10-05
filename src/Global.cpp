
#include <Fourier/Defines.hpp>
#include <Fourier/UI/Manager.hpp>

#if defined(SFML_SYSTEM_MACOS)
    #include <mach-o/dyld.h>
    #include <libgen.h>

    #define EXECUTABLE_PATH_BUFFER_SIZE 4096
#endif

#include <cstring>

UIManager uiManager;

static std::string resourcePath;
static std::string externalPath;

std::string& getResourcePath(void) {
    return resourcePath;
}

std::string getResourcePath(const std::string &path) {
    return resourcePath + path;
}

std::string& getExternalPath(void) {
    return externalPath;
}

std::string getExternalPath(const std::string &path) {
    return externalPath + path;
}

void init_global(int argc, char **argv, char **env) {
    (void) argc;
    (void) argv;
    (void) env;

    #if defined(SFML_SYSTEM_MACOS)
        if (argc > 1) {
            resourcePath = "";
            externalPath = dirname(argv[0]);
        }
        else {
            char pathbuf[EXECUTABLE_PATH_BUFFER_SIZE];
            std::uint32_t bufsize = EXECUTABLE_PATH_BUFFER_SIZE;

            _NSGetExecutablePath(pathbuf, &bufsize);
            char *parentdir = dirname(pathbuf);

            resourcePath = std::string(parentdir) + "/../Resources/";
            externalPath = std::string(parentdir) + "/../../../";
        }
    #else
        resourcePath = "";
        externalPath = argc > 0 ? argv[0] : "";
    #endif

    uiManager.initManager();
}
