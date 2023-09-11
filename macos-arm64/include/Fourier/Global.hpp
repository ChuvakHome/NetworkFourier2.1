
#ifndef _GLOBAL_HPP_
#define _GLOBAL_HPP_

#include <Fourier/Defines.hpp>
#include <Fourier/UI/Manager.hpp>

extern UIManager uiManager;

/* getResourcePath(...) and getExternalPath(...) are nessecesary because macOS has
 * special format for Applications. An application on macOS is a directory with specific
 * file hierarchy. Application directory name should end with .app suffix. Executable
 * binary file located in MyApp.app/Contents/MacOS/my_app. Resources locate in
 * MyApp.app/Contents/Resources folder. Applications for Linux and Windows is identical
 * to the binary executable file. Thus, resourcePath for macOS is MyApp.app/Contents/Resources
 * and externalPath is the path of parent directory of MyApp.app. For Linux and Windows
 * resourcePath and externalPath are the same things
*/

std::string& getResourcePath(void);

std::string getResourcePath(const std::string &path);

std::string& getExternalPath(void);

std::string getExternalPath(const std::string &path);

std::string& getApplicationPath(void);

void init_global(int argc, char **argv, char **env);

#endif
