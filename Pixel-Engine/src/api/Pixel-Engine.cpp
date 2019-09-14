#include "pch.h"
#include "Pixel-Engine.h"

#include <pybind11/embed.h>
namespace py = pybind11;

#ifdef _WIN32
	#include <direct.h>
	#define getCurrentDir _getcwd
	#define changeDir _chdir
#else
	#include "unistd.h"
	#define getCurrentDir getcwd
	#define changeDir chdir
#endif

PIXEL_ENGINE_API void pe_mainLoop(const char* project_name, int argc, char** argv)
{
	//changeDir("../../__test_env/pe_test/SandBox/");
	//char buf[4096];
	//std::cout << "CWD: " << getCurrentDir(buf, sizeof buf) << std::endl;

	std::ifstream chcwd_file("chcwd");
	char buf[4096];
	if (chcwd_file.is_open()) {
		std::string path;
		std::getline(chcwd_file, path);
		chcwd_file.close();
		changeDir(path.c_str());
	}
	PE_LOG("cwd: " << getCurrentDir(buf, sizeof buf) );

	py::scoped_interpreter intp;
	pe::Application app(  std::string(project_name).append(".peproj.xml").c_str() );
	app.update();
}
