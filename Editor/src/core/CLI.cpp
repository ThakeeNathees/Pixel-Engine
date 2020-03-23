#include "pch.h"
#include "CLI.h"



CLI* CLI::s_instance = nullptr;
std::string CLI::s_exec_path;

void CLI::init()
{
	char pBuf[1024];
#ifdef _WIN32
	int bytes = GetModuleFileName(NULL, pBuf, sizeof pBuf);
	// TODO:  assert bytes > 0
#elif __linux__
	char szTmp[32];
	sprintf(szTmp, "/proc/%d/exe", getpid());
	int bytes = MIN(readlink(szTmp, pBuf, len), len - 1);
	if (bytes >= 0)
		pBuf[bytes] = '\0';
#endif
	PE_CONSOLE_LOG("attempting to import python module os");
	m_py_os = py::module::import("os");
	s_exec_path = m_py_os.attr("path").attr("dirname")(pe::__fixPath(pBuf)).cast<std::string>();
	py::exec(std::string("sys.path.append('").append(s_exec_path).append("')"));
	PE_CONSOLE_LOG("import os success - python interpriter working");

	readPeConfigFile();

	PE_CONSOLE_LOG("attempting to import python module init (at pysrc)");
	m_py_proj_init = py::module::import("init");
	PE_CONSOLE_LOG("import init success\nattempting to import python module assets_updater");
	m_py_assets_updater = py::module::import("assets_updater");
	PE_CONSOLE_LOG("import asset_updater success");
	m_console = new Console();
}

const std::string CLI::getCwd() {
	char buff[1024];
	return getCurrentDir(buff, sizeof buff);
}

void CLI::chDir(const std::string& path) {
	changeDir(path.c_str());
	PE_CONSOLE_LOG("CWD: %s", getCwd().c_str());
}


int CLI::readTextFile(std::string& out, const std::string& path) {
	std::ifstream in(path);
	if (in.good())
	{
		std::string str((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
		out = str;
		return 0;
	}
	return 1;
}

int CLI::readBinaryFile(std::vector<unsigned char>& buffer, const std::string& path) {
	std::ifstream in(path.c_str(), std::ios::binary );
	if (in.good()) {
		std::vector<unsigned char> new_buffer(std::istreambuf_iterator<char>(in), {});
		buffer = new_buffer;
		return 0;
	}
	return 1;
}

int CLI::readProjFile() {
	m_proj_file_name = PyUtils::getInstance()->getFileUtil().attr("getProjFileName")().cast<std::string>();
	if (m_proj_file_name == std::string("")) {
		PE_LOG("\nERROR: in method Resources::readProjFile : can't find project file (*.peproj)\n");
		return 1;
	}
	else PE_LOG("project file found : %s", m_proj_file_name.c_str());

	int error = updatePeproj();
	return error;
}

int CLI::updatePeproj() {
	int error;
	error = projFileUpdate(false);
	if (error) { return 1; }
	
	pe::FileHandler file;
	error = file.readProject(m_proj_file_name.c_str());
	if (error) { 
		CLI::log("Error: reading project file was failure!", Console::LOGLEVEL_ERROR); 
		return error;
	}
	else { CLI::log("Project file has read"); }
	m_peproj = file.getProject();
	return error;
}

void CLI::parseArgs(int argc, char** argv){
	//	TODO: parse args, update proj, init, ...
}

//////////////////////////////////////////////////////////////////////////

std::pair<std::string, std::vector<std::string>> CLI::getKeyValue(const std::string& line) {
	auto dict = pe::split(line, '=');
	std::vector<std::string> values;
	if (dict.size() < 2) return std::make_pair("",values);
	std::string key = pe::__removeWiteSpace(dict[0]);
	std::vector<std::string> __values = pe::split(dict[1], ',');
	for (int i = 0; i < __values.size(); i++) {
		values.push_back(pe::__getValueString(__values[i]));
	}
	return std::make_pair(key, values);
}

void CLI::save(const std::string& text_to_save, const std::string& file_path)
{
	std::ofstream save_file;
	save_file.open(file_path);
	save_file << text_to_save;
	save_file.close();
}


void CLI::readPeConfigFile() {
	PE_CONSOLE_LOG("\nconfig file reading started");
	std::ifstream init_file(CLI::getExecPath().append("/peconfig.init"));
	if (init_file.is_open()) {
		std::string line;
		while (std::getline(init_file, line)) {
			if (line[0] == '#') continue;
			auto key_value = CLI::getKeyValue(line);

			if (key_value.first == std::string("logo_path")) { Resources::LOGO.loadFromFile(CLI::getExecPath().append(key_value.second[0])); continue; }
			if (key_value.first == std::string("logo_pe_path")) { Resources::LOGO_PE.loadFromFile(CLI::getExecPath().append(key_value.second[0])); continue; }
			if (key_value.first == std::string("png_bg_path")) { 
				Resources::PNG_BG.loadFromFile(CLI::getExecPath().append(key_value.second[0])); 
				Resources::PNG_BG.setRepeated(true);
				continue; 
			}

			if (pe::__removeWiteSpace(line) == std::string("paths:")) {
				while (std::getline(init_file, line)) {
					if (pe::__removeWiteSpace(line) == std::string("end")) break; 
					key_value = CLI::getKeyValue(line);
					if (key_value.first == std::string("py_path")) {
						PE_CONSOLE_LOG(std::string("\tattempting to add py_path : ").append(key_value.second[0]).c_str());
						py::exec(std::string("sys.path.append('").append(
							m_py_os.attr("path").attr("abspath")(CLI::getExecPath().append(key_value.second[0])).attr("replace")("\\","/").cast<std::string>()
						).append("')"));
					}
				}
				continue;
			}

			if (pe::__removeWiteSpace(line) == std::string("fonts:")) {
				while (std::getline(init_file, line)) {
					if (pe::__removeWiteSpace(line) == std::string("end")) break;
					key_value = CLI::getKeyValue(line);
					if (key_value.first != std::string("")) {
						PE_CONSOLE_LOG(std::string("\tattempting to add font : ").append(key_value.second[0]).c_str());
						auto font = ImGui::GetIO().Fonts->AddFontFromFileTTF(CLI::getExecPath().append(key_value.second[0]).c_str(), std::stof(key_value.second[1]));
						ImGui::SFML::UpdateFontTexture();
						Resources::addFont(key_value.first, font, std::stof(key_value.second[2])); continue;
					}
				}
				continue;
			}

			if (pe::__removeWiteSpace(line) == std::string("file_format_icons:")) {
				while (std::getline(init_file, line)) {
					if (pe::__removeWiteSpace(line)== std::string("end")) break;
					key_value = CLI::getKeyValue(line);
					if (key_value.first != std::string("")) {
						PE_CONSOLE_LOG(std::string("\tattempting to add file_format_icon : ").append(key_value.second[0]).c_str());
						sf::Texture tex; tex.loadFromFile(CLI::getExecPath().append(key_value.second[0])); Resources::addFileFormatIcon(key_value.first, tex); continue;
					}
				}
				continue;
			}

			if (pe::__removeWiteSpace(line) == std::string("menu_icons:")) {
				while (std::getline(init_file, line)) {
					if (pe::__removeWiteSpace(line) == std::string("end")) break;
					key_value = CLI::getKeyValue(line);
					if (key_value.first != std::string("")) {
						PE_CONSOLE_LOG(std::string("\tattempting to add menu_icon : ").append(key_value.second[0]).c_str());
						sf::Texture tex; tex.loadFromFile(CLI::getExecPath().append(key_value.second[0])); Resources::addMenuIcon(key_value.first, tex); continue;
					}
				}
				continue;
			}

			if (pe::__removeWiteSpace(line) == std::string("other_icons:")) {
				while (std::getline(init_file, line)) {
					if (pe::__removeWiteSpace(line) == std::string("end")) break;
					key_value = CLI::getKeyValue(line);
					if (key_value.first != std::string("")) {
						PE_CONSOLE_LOG(std::string("\tattempting to add other_icon : ").append(key_value.second[0]).c_str());
						sf::Texture tex; tex.loadFromFile(CLI::getExecPath().append(key_value.second[0])); Resources::addOtherIcon(key_value.first, tex); continue;
					}
				}
				continue;
			}


		}
	}
	PE_CONSOLE_LOG("config file reading success\n");
}