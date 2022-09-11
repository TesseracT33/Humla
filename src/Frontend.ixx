export module Frontend;

import Core;

import <SDL.h>;

import <chrono>;
import <iostream>;
import <memory>;
import <string>;
import <string_view>;
import <thread>;
import <utility>;
import <vector>;

namespace Frontend
{
	export
	{
		bool Initialize(std::shared_ptr<Core> core);
		void LoadGame(std::string rom_path);
		void RunGui(bool boot_game_immediately = false);
		void Shutdown();
	}

	float GetImGuiMenuBarHeight();
	void OnCtrlKeyPress(SDL_Keycode keycode);
	void OnMenuConfigureBindings();
	void OnMenuEnableAudio();
	void OnMenuFullscreen();
	void OnMenuLoadState();
	void OnMenuLockFramerate();
	void OnMenuOpen();
	void OnMenuOpenBios();
	void OnMenuOpenRecent();
	void OnMenuPause();
	void OnMenuQuit();
	void OnMenuReset();
	void OnMenuSaveState();
	void OnMenuStop();
	void OnMenuWindowScale();
	void RenderGui();
	void RenderInputBindingsWindow();
	void ScheduleEmuThread(void(*function)());
	void StartGame();
	void StopGame();

	bool input_window_button_pressed;
	bool menu_enable_audio;
	bool menu_fullscreen;
	bool menu_lock_framerate;
	bool menu_pause_emulation;
	bool quit;
	bool show_gui;
	bool show_input_bindings_window;

	std::string prev_core_action_binding;

	std::jthread emu_thread;

	std::vector<std::string_view> core_action_names;
	std::vector<std::string_view> core_action_bindings;

	SDL_Renderer* sdl_renderer;
	SDL_Window* sdl_window;
}