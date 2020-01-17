#pragma once

#include "core/CLI.h"
#include "windows/views/TextEditors.h"
#include "windows/views/HexEditors.h"
#include "windows/views/FontViwers.h"
#include "windows/views/ImageViwers.h"

class FileTree
{
private:
	FileTree(const std::string& title)
		: m_title(title)
	{
		auto m = py::module::import("file_tree");
		m_py_filetree = m.attr("FileTree")(CLI::getCwd());
		m_object_reader = py::module::import("object_reader");
		m_scene_reader = py::module::import("scene_reader");
	}

	static FileTree* s_instance;
	std::string m_title;
	py::object m_py_filetree;
	py::module m_object_reader;
	py::module m_scene_reader;
	std::map<long long, py::object> m_objects;
	std::map<long long, py::object> m_scenes;

	long long m_selected_id = -1;
	long long m_selected_menu_id = -1;

	bool m_open_popup			= false;
	bool m_open_popup_sprite	= false;
	bool m_open_popup_area		= false;

	long long m_delete_conform_obj_id = -1;

	std::string m_delete_path = "";
	bool m_is_deletepathdir = false;


public:

	std::map<long long, py::object>& getObjectTags() {
		return m_objects;
	}

	std::map<long long, py::object>& getSceneTags() {
		return m_scenes;
	}

	bool m_open = true; // main menu bar need &m_open
	static FileTree* getInstance() {
		if (!s_instance) s_instance = new FileTree("Project-Explorer");
		return s_instance;
	}

	void reload() {
		auto m = py::module::import("file_tree");
		m_py_filetree = m.attr("FileTree")(CLI::getCwd());
		reloadObjectTags();
	}

	/*
	void reloadObjects() {
		for (auto path : m_py_filetree.attr("object_paths").cast<std::vector<std::string>>()) {
			long long id = PyUtils::getInstance()->getMathUtil().attr("md5Hash")(path, "long").cast<long long>();
			if (m_objects.find(id) == m_objects.end()) {
				auto obj_tag = m_object_reader.attr("ObjectTag")(path);
				m_objects[id] = obj_tag;
			}
		}
	}
	*/

	void reloadScenes() {
		for (auto path : m_py_filetree.attr("scene_paths").cast<std::vector<std::string>>()) {
			long long id = PyUtils::getInstance()->getMathUtil().attr("md5Hash")(path, "long").cast<long long>();
			if (m_scenes.find(id) == m_scenes.end()) {
				// TODO:
			}
		}
	}
	
	void render() {
		if (m_open) {
			ImGui::Begin(m_title.c_str(), &m_open);
			renderTreeRecursive(m_py_filetree, true);
			renderPopup();
			ImGui::End();
		}
	}


	py::object& getPyFileTree() {
		return m_py_filetree;
	}

	//py::object& getObjectTag_objId(int id);
	std::map<long long, py::object>& getObjects();

	////////////////////////////////////////////////////////////////////////////////////////////////////////
private:
	void reloadObjectTags();

	void renderTreeRecursive(py::object& tree, bool next_item_open = false);
	void renderAssetsTree(const std::string& path);
	void renderObjectTree(const std::string& path);
	void renderSceneTree(const std::string& path);

	void renderRightMouseMenu(const std::string& path);
	void renderRightMouseMenuTexture(int texture_id);
	void renderRightMouseMenuFont(int font_id);
	void renderRightMouseMenuAssets(const std::string& path, long long id);
	void renderRightMouseMenuObject(const std::string& path, long long id);
	void renderRightMouseMenuScene(const std::string& path, long long id);
	void renderRightMouseMenuSprite(long long id);
	void renderRightMouseMenuArea(long long id);

	void renderPopup();
	void nodeClickedEvent(const std::string& title, const std::string& path, long long id=-1);

};
