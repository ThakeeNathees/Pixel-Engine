#pragma once
#include "..//Console.h"

class CLI
{
public:

	void init();
	void projInit(const std::string& path, const std::string& name ) {
		try {
			m_py_proj_init.attr("init")(name, path);
			PE_LOG("CLI::projInit success");
		}
		catch (const std::exception& e){
			PE_LOG("\nERROR: in mehtod CLI::projInit \n%s\n", e.what());
		}
	}
	void projUpdate(bool include_pe = true, const std::string& proj_name="", const std::string& proj_dir=".") {
		try {
			m_py_proj_init.attr("updateProj")(proj_name, proj_dir, include_pe);
			PE_LOG("CLI::projUpdate success");
		}
		catch (const std::exception&e){
			PE_LOG("\nERROR: in method CLI::projUpdate\n%s\n",e.what());
		}
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

	Console* getConsole() {
		return m_console;
	}

private:
	CLI() {}
	static std::pair<std::string,std::vector<std::string>> getKeyValue(const std::string& line);
	static CLI* s_instance;
	static std::string s_exec_path;

	Console* m_console; // TODO: delete
	void readPeConfigFile();
	py::module m_py_os;
	py::module m_py_proj_init;
};