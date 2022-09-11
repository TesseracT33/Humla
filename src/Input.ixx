export module Input;

import Core;
import Emulator;
import Types;

import <SDL.h>;

import <algorithm>;
import <array>;
import <cassert>;
import <filesystem>;
import <string>;
import <string_view>;
import <type_traits>;
import <utility>;
import <variant>;
import <vector>;

namespace Input
{
	export
	{
		constexpr SDL_JoystickID default_joystick_id = -1;

		enum class HostInputType { /* As of SDL2.0.22: */
			ControllerAxis,   /* SDL_ControllerAxisEvent; axis enumeration accessed from event.caxis.axis; typedef of Uint8. */
			ControllerButton, /* SDL_ControllerButtonEvent; button enumeration accessed from event.cbutton.button; typedef of Uint8. */
			Key,              /* SDL_KeyboardEvent; SDL_Keycode accessed from event.key.keysym.sym; typedef of Sint32. */
			MouseButton       /* SDL_MouseButtonEvent; button enumeration accessed from event.button.button; typedef of Uint8. */
		};

		void AddBinding(uint player_index, auto core_action, HostInputType host_action, s32 host_value, SDL_JoystickID joystick_id = default_joystick_id);
		void Await();
		void ClearAllBindings();
		void ClearBindings(uint player_index);
		std::vector<std::string_view> GetCoreActionNames();
		bool Initialize();
		std::string JoystickIdToGuid(SDL_JoystickID joystick_id);
		void LoadBindings();
		void OpenGameControllers();
		void ProcessEvent(SDL_Event event);
		void RemoveBinding(uint player_index, auto core_action);
		void SaveBindings();
		void SetCoreActionNames(std::vector<std::string_view> names);
		void SetDefaultBindings();
		void SetPlayerActive(uint player_index);
		void SetPlayerInactive(uint player_index);
	}

	enum class ButtonEvent {
		Press, Release
	};

	struct HostInputBinding
	{
		HostInputType type;
		s32 value;
		std::string joystick_guid;
	};

	struct Player
	{
		bool active;
		std::vector<HostInputBinding> core_bindings;
	};

	template<HostInputType host_input_type, ButtonEvent button_event = ButtonEvent::Press>
	bool MatchInput(s32 value, s16 axis_value = 0, SDL_JoystickID joystick_id = default_joystick_id);

	constexpr uint max_players = 4;

	constexpr s32 unbound_host_action_value = -1;

	const HostInputBinding unbound_host_input = {
		.type = {},
		.value = unbound_host_action_value,
		.joystick_guid = {}
	};

	uint num_core_inputs;

	std::array<Player, max_players> players;

	std::vector<SDL_GameController*> controllers;

	std::vector<std::string_view> core_action_names;

	/// Template definitions ////////////////////////////
	void AddBinding(uint player_index, auto core_action, HostInputType host_input_type, s32 host_value, SDL_JoystickID joystick_id)
	{
		auto core_action_index = std::to_underlying(core_action);
		std::vector<HostInputBinding> input_set = players.at(player_index).core_bindings;
		input_set[core_action_index] = {
			.type = host_input_type,
			.value = host_value,
			.joystick_guid = JoystickIdToGuid(joystick_id)
		};
	}


	template<HostInputType host_input_type, ButtonEvent button_event>
	bool MatchInput(s32 value, s16 axis_value, SDL_JoystickID joystick_id)
	{
		static constexpr bool controller_input = 
			host_input_type == HostInputType::ControllerAxis || host_input_type == HostInputType::ControllerButton;

		std::string_view joystick_guid;
		if constexpr (controller_input) {
			joystick_guid = JoystickIdToGuid(joystick_id);
		}

		auto core = Emulator::GetCore();
		uint core_action_index;
		uint player_index = 0;
		for ( ; player_index < max_players; ++player_index) {
			const Player& player = players[player_index];
			if (!player.active) {
				continue;
			}
			const std::vector<HostInputBinding> input_set = player.core_bindings;
			auto it_core_action_index = std::ranges::find_if(input_set, [&](HostInputBinding binding) {
				return binding.type == host_input_type
					&& binding.value == value
					&& [&] {
						if constexpr (controller_input) {
							return binding.joystick_guid.compare(joystick_guid) == 0;
						}
						else {
							return true;
						}
					}();
				});
			if (it_core_action_index != input_set.end()) {
				core_action_index = (uint)std::distance(input_set.begin(), it_core_action_index);
				break;
			}
		}

		bool binding_found = player_index < max_players;
		if (binding_found) {
			if constexpr (host_input_type == HostInputType::ControllerAxis) {
				core->NotifyNewAxisValue(player_index, core_action_index, axis_value);
			}
			else { /* ControllerButton, Key, or MouseButton */
				if constexpr (button_event == ButtonEvent::Press) {
					core->NotifyButtonPressed(player_index, core_action_index);
				}
				else if constexpr (button_event == ButtonEvent::Release) {
					core->NotifyButtonReleased(player_index, core_action_index);
				}
				else {
					static_assert(AlwaysFalse<host_input_type>);
				}
			}
			return true;
		}
		else {
			return false;
		}
	}


	void RemoveBinding(uint player_index, auto core_action /* enum */)
	{
		auto core_action_index = std::to_underlying(core_action);
		std::vector<HostInputBinding> input_set = players.at(player_index).core_bindings;
		input_set[core_action_index] = unbound_host_input;
	}
}