#pragma once

#include "pybind11/embed.h"
namespace py = pybind11;

class CLI
{
public:

	void init();
	void projInit(const std::string& path, const std::string& name = "proj_test") {
		m_py_proj_init.attr("init")(name, path);
	}

	static void parseArgs(int argc, char** argv);
	static CLI* getInstance() {
		if (!s_instance) s_instance = new CLI();
		return s_instance;
	}
	static void save(const std::string& text_to_save, const std::string& file_path);
	static std::string getExecPath() { return s_exec_path; }
	static const std::string getCwd();
	static void chDir(const std::string& path);
	static int readTextFile(std::string& out, const std::string& path);
	static int readBinaryFile(std::vector<unsigned char>& buffer, const std::string& path);

private:
	CLI() {}
	static std::pair<std::string,std::vector<std::string>> getKeyValue(const std::string& line);
	static CLI* s_instance;
	static std::string s_exec_path;

	void readPeConfigFile();
	py::module m_py_os;
	py::module m_py_proj_init;
};
