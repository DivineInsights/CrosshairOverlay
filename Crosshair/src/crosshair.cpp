#include "stdafx.h"

mINI::INIFile _file("Crosshair.ini");
mINI::INIStructure _ini;

Crosshair _crosshair;
inline const char* BoolToString(bool b) {
	return b ? "true" : "false";
}

inline bool StringToBool(std::string s) {
	return s != "false";
}

int tick_count = 0;
void Crosshair::Render() {
	if (tick_count + 100 < GetTickCount()) {
		if (GetAsyncKeyState(VK_INSERT) & 0x8000) {
			this->open = !this->open;
			tick_count = GetTickCount();
		}
	}

	// create overlay window for settings etc circle, x, outline, no outline

	// crosshair
	if (this->settings.crosshair) {
		if (this->settings.shape == this->circle) {
			ImGui::GetBackgroundDrawList()->AddCircleFilled({ this->screen_center.x, this->screen_center.y }, this->settings.shape_size.y, this->settings.color);
		}

		if (this->settings.shape == this->cross) {
			// top 
			ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ this->screen_center.x - (this->settings.shape_size.x / 2), (this->screen_center.y - this->settings.shape_size.y) - (this->settings.gap ? this->settings.gap_size : 0) },
				{ this->screen_center.x + (this->settings.shape_size.x / 2), this->screen_center.y - (this->settings.gap ? this->settings.gap_size : 0) },
				this->settings.color
			);

			// bottom
			ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ this->screen_center.x - (this->settings.shape_size.x / 2), this->screen_center.y + (this->settings.gap ? this->settings.gap_size : 0) },
				{ this->screen_center.x + (this->settings.shape_size.x / 2), (this->screen_center.y + this->settings.shape_size.y) + (this->settings.gap ? this->settings.gap_size : 0) },
				this->settings.color
			);

			// left
			ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ (this->screen_center.x - this->settings.shape_size.y) - (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y - (this->settings.shape_size.x / 2) },
				{ this->screen_center.x - (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y + (this->settings.shape_size.x / 2) },
				this->settings.color
			);

			// right
			ImGui::GetBackgroundDrawList()->AddRectFilled(
				{ this->screen_center.x + (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y - (this->settings.shape_size.x / 2) },
				{ (this->screen_center.x + this->settings.shape_size.y) + (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y + (this->settings.shape_size.x / 2) },
				this->settings.color
			);

			if (this->settings.outline) {
				//// top 
				//ImGui::GetBackgroundDrawList()->AddRectFilled(
				//	{ this->screen_center.x - (this->settings.shape_size.x / 2), (this->screen_center.y - this->settings.shape_size.y) - (this->settings.gap ? this->settings.gap_size : 0) },
				//	{ this->screen_center.x + (this->settings.shape_size.x / 2), this->screen_center.y - (this->settings.gap ? this->settings.gap_size : 0) },
				//	this->settings.color
				//);

				//// bottom
				//ImGui::GetBackgroundDrawList()->AddRectFilled(
				//	{ this->screen_center.x - (this->settings.shape_size.x / 2), this->screen_center.y + (this->settings.gap ? this->settings.gap_size : 0) },
				//	{ this->screen_center.x + (this->settings.shape_size.x / 2), (this->screen_center.y + this->settings.shape_size.y) + (this->settings.gap ? this->settings.gap_size : 0) },
				//	this->settings.color
				//);

				//// left
				//ImGui::GetBackgroundDrawList()->AddRectFilled(
				//	{ (this->screen_center.x - this->settings.shape_size.y) - (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y - (this->settings.shape_size.x / 2) },
				//	{ this->screen_center.x - (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y + (this->settings.shape_size.x / 2) },
				//	this->settings.color
				//);

				//// right
				//ImGui::GetBackgroundDrawList()->AddRectFilled(
				//	{ this->screen_center.x + (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y - (this->settings.shape_size.x / 2) },
				//	{ (this->screen_center.x + this->settings.shape_size.y) + (this->settings.gap ? this->settings.gap_size : 0), this->screen_center.y + (this->settings.shape_size.x / 2) },
				//	this->settings.color
				//);
			}
		}
	}

	if (this->open) {
		// make sure we can click the window
		if (this->clickable) {
			long style = GetWindowLong(this->window, GWL_EXSTYLE);
			style &= ~WS_EX_LAYERED;
			SetWindowLong(window, GWL_EXSTYLE, style);
			SetForegroundWindow(window);

			this->clickable = false;
		}
	} else {
		// make sure we can't click the window
		if (!this->clickable) {
			long style = GetWindowLong(this->window, GWL_EXSTYLE);
			style |= WS_EX_LAYERED;
			SetWindowLong(this->window, GWL_EXSTYLE, style);

			this->clickable = true;
		}
	}

	if (open) {
		if (!ImGui::Begin("Crosshair Overlay - Developed By Divine", &open)) {
			ImGui::End();
			return;
		}

		ImGui::Checkbox((this->settings.crosshair ? "Disable Crosshair" : "Enable Crosshair"), &this->settings.crosshair);

		const char* shapes[] = { "circle", "cross" };
		static const char* current_shape = shapes[this->settings.shape];

		if (ImGui::BeginCombo("Shapes", current_shape)) {
			for (int i = 0; i < IM_ARRAYSIZE(shapes); i++) {
				bool is_selected = (current_shape == shapes[i]);
				
				if (ImGui::Selectable(shapes[i], is_selected)) {
					current_shape = shapes[i];
					this->settings.shape = i;
				}
				
				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}

			ImGui::EndCombo();
		}
		
		if (this->settings.shape == this->circle)
			ImGui::DragFloat("Size", &this->settings.shape_size.y, 1.0F, 1, 10);

		// if cross is selected show options for cross only
		if (this->settings.shape == this->cross) {
			ImGui::DragFloat("Width", &this->settings.shape_size.x, 0.5F, 1, 10);
			ImGui::DragFloat("Length", &this->settings.shape_size.y, 0.5F, 1, 10);
			ImGui::Checkbox("Gap", &this->settings.gap);
			ImGui::DragFloat("Gap Size", &this->settings.gap_size, 1.0F, 0.1, 10);
			ImGui::DragFloat("Gap Size", &this->settings.gap_size, 1.0F, 0.1, 10);
		}

		//ImGui::Checkbox("Outline", &this->settings.outline);
		ImGui::ColorEdit4("Color", (float*)&this->settings.color, 0);
		//ImGui::ColorEdit4("Outline Color", (float*)&this->settings.outline_color, 0);
		
		if (ImGui::Button("Save")) {
			_ini["DEFAULT"]["Crosshair"] = BoolToString(this->settings.crosshair);
			_ini["DEFAULT"]["Shape"] = std::to_string(this->settings.shape);
			_ini["DEFAULT"]["Shape_Size_X"] = std::to_string(this->settings.shape_size.x);
			_ini["DEFAULT"]["Shape_size_Y"] = std::to_string(this->settings.shape_size.y);
			_ini["DEFAULT"]["Gap"] = BoolToString(this->settings.gap);
			_ini["DEFAULT"]["Gap_Size"] = std::to_string(this->settings.gap_size);
			_ini["DEFAULT"]["Outline"] = BoolToString(this->settings.outline);
			_ini["DEFAULT"]["Color_R"] = std::to_string(this->settings.color.Value.x);
			_ini["DEFAULT"]["Color_G"] = std::to_string(this->settings.color.Value.y);
			_ini["DEFAULT"]["Color_B"] = std::to_string(this->settings.color.Value.z);
			_ini["DEFAULT"]["Color_A"] = std::to_string(this->settings.color.Value.w);
			_ini["DEFAULT"]["Outline_Color_R"] = std::to_string(this->settings.outline_color.Value.x);
			_ini["DEFAULT"]["Outline_Color_G"] = std::to_string(this->settings.outline_color.Value.y);
			_ini["DEFAULT"]["Outline_Color_B"] = std::to_string(this->settings.outline_color.Value.z);
			_ini["DEFAULT"]["Outline_Color_A"] = std::to_string(this->settings.outline_color.Value.w);
			_file.write(_ini);
		}

		ImGui::End();
	}
}

void Crosshair::Init() {
	RECT desktop;
	const HWND hDesktop = GetDesktopWindow();
	GetWindowRect(hDesktop, &desktop);

	this->screen_size.y = desktop.bottom;
	this->screen_size.x = desktop.right;

	this->screen_center.y = desktop.bottom / 2;
	this->screen_center.x = desktop.right / 2;

	this->clickable = false;

	// read ini
	_file = mINI::INIFile("Crosshair.ini");
	_file.read(_ini);

	this->settings.crosshair = StringToBool(_ini["DEFAULT"]["Crosshair"].c_str());
	this->settings.shape = atoi(_ini["DEFAULT"]["Shape"].c_str());
	this->settings.shape_size.x = std::stof(_ini["DEFAULT"]["Shape_Size_X"].c_str());
	this->settings.shape_size.y = std::stof(_ini["DEFAULT"]["Shape_size_Y"].c_str());
	this->settings.gap = StringToBool(_ini["DEFAULT"]["Gap"].c_str());
	this->settings.gap_size = std::stof(_ini["DEFAULT"]["Gap_Size"].c_str());
	this->settings.outline = StringToBool(_ini["DEFAULT"]["Outline"].c_str());
	this->settings.color.Value.x = atoi(_ini["DEFAULT"]["Color_R"].c_str());
	this->settings.color.Value.y = atoi(_ini["DEFAULT"]["Color_G"].c_str());
	this->settings.color.Value.z = atoi(_ini["DEFAULT"]["Color_B"].c_str());
	this->settings.color.Value.w = atoi(_ini["DEFAULT"]["Color_A"].c_str());
	this->settings.outline_color.Value.x = atoi(_ini["DEFAULT"]["Outline_Color_R"].c_str());
	this->settings.outline_color.Value.y = atoi(_ini["DEFAULT"]["Outline_Color_G"].c_str());
	this->settings.outline_color.Value.z = atoi(_ini["DEFAULT"]["Outline_Color_B"].c_str());
	this->settings.outline_color.Value.w = atoi(_ini["DEFAULT"]["Outline_Color_A"].c_str());
}