#pragma once

struct SizeStruct {
	float x;
	float y;
};

struct CrosshairSettings {
	bool crosshair;
	int shape; // 0 - circle, 1 - cross
	SizeStruct shape_size;
	bool gap;
	float gap_size;
	bool outline;
	ImColor color;
	ImColor outline_color;
};

class Crosshair {
public:
	enum CrosshairShape {
		circle,
		cross
	};

	SizeStruct GetScreenSize() {
		return this->screen_size;
	}

	SizeStruct GetScreenCenter() {
		return this->screen_center;
	}

	void SetWindow(HWND window) {
		this->window = window;
	}

	HWND GetWindow() {
		return this->window;
	}

	void Render();
	void Init();

private:
	SizeStruct screen_size;
	SizeStruct screen_center;
	CrosshairSettings settings;
	bool open;
	bool clickable;
	HWND window;
}; extern Crosshair _crosshair;