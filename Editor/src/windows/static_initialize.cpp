#include "pch.h"

#include "StartWindow.h"
#include "windows/views/TextEditors.h"
#include "windows/views/HexEditors.h"
#include "windows/views/FontViwers.h"
#include "windows/views/ImageViwers.h"

#include "PyInterpriter.h"
#include "ExplorerPopup.h"

StartWindow* StartWindow::s_instance = nullptr;
ExplorerPopup* ExplorerPopup::s_instance = nullptr;
PyInterpreter* PyInterpreter::s_instance = nullptr;

// TODO: remove text editors
std::map<long long, TextEditors::TextEditorData*> TextEditors::s_text_editors;
std::map<long long, HexEditors::HexEditorData*> HexEditors::s_hex_editors;


FontViewer* FontViewer::s_instance;
ImageViewer* ImageViewer::s_instance;
float TextEditors::s_font_scale = 150; // TODO: font scale for default font
std::string TextEditors::s_font = ""; // "" means ImGui default font
long long TextEditors::s_close_conformation = -1;

