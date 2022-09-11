module Emulator;

import Audio;
import Input;
import UserMessage;
import Video;

namespace Emulator
{
	void DisableAudio()
	{
		core->DisableAudio();
	}


	void EnableAudio()
	{
		core->EnableAudio();
	}


	std::shared_ptr<Core> GetCore()
	{
		return core;
	}


	std::string GetSaveStatePath()
	{
		// TODO
		return {};
	}


	bool LoadRom(const std::string& rom_path)
	{
		return core->LoadRom(rom_path);
	}


	void LockFramerate()
	{

	}


	void Loop()
	{
		is_running = true;
		is_paused = false;
		while (is_running && !is_paused) {
			// Run the core for "some amount of time".
			// The core itself should be telling the audio and video frontends what to do.
			core->Run();
		}
	}


	void LoadState()
	{
		if (!is_running) {
			return;
		}
		core->LoadState();
	}


	void Pause()
	{
		is_paused = true;
	}


	void Reset()
	{
		if (is_running) {
			core->Reset();
			Loop();
		}
	}


	void Resume()
	{
		if (is_running) {
			Loop();
		}
	}


	void SaveState()
	{
		if (!is_running) {
			return;
		}
		core->SaveState();
	}


	void SetCore(std::shared_ptr<Core> core)
	{
		assert(core != nullptr);
		Emulator::core = std::move(core);
	}


	void StartGame()
	{
		Loop();
	}


	void Stop()
	{
		is_running = false;
	}


	void TogglePaused()
	{
		if (is_running) {
			is_paused = !is_paused;
		}
	}


	void UnlockFramerate()
	{

	}
}
