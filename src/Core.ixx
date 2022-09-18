export module Core;

import <string>;
import <string_view>;
import <vector>;

export struct Core
{
	virtual void ApplyNewSampleRate() = 0;
	virtual void Detach() = 0;
	virtual void DisableAudio() = 0;
	virtual void EnableAudio() = 0;
	virtual std::vector<std::string_view> GetActionNames() = 0;
	virtual unsigned GetNumberOfInputs() = 0;
	virtual void Initialize() = 0;
	virtual bool LoadBios(const std::string& path) = 0;
	virtual bool LoadRom(const std::string& path) = 0;
	virtual void LoadState() {};
	virtual void NotifyNewAxisValue(unsigned player_index, unsigned action_index, int new_axis_value) {};
	virtual void NotifyButtonPressed(unsigned player_index, unsigned action_index) = 0;
	virtual void NotifyButtonReleased(unsigned player_index, unsigned action_index) = 0;
	virtual void Reset() = 0;
	virtual void Run() = 0;
	virtual void SaveState() {};

	void SetupCommunicationWithFrontend();
};