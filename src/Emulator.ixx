export module Emulator;

import Core;

import <atomic>;
import <cassert>;
import <chrono>;
import <filesystem>;
import <memory>;
import <string>;
import <string_view>;

namespace Emulator
{
	export
	{
		void DisableAudio();
		void EnableAudio();
		std::shared_ptr<Core> GetCore();
		bool LoadRom(const std::string& rom_path);
		void LoadState();
		void LockFramerate();
		void Pause();
		void Reset();
		void Resume();
		void SaveState();
		void SetCore(std::shared_ptr<Core> core);
		void StartGame();
		void Stop();
		void TogglePaused();
		void UnlockFramerate();
	}

	std::shared_ptr<Core> core;

	std::string GetSaveStatePath();
	void Loop();

	std::atomic<bool> is_paused;
	std::atomic<bool> is_running;

	std::string current_rom_name;
	std::string current_rom_path;
}