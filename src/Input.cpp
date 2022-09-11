module Input;

import Frontend;
import UserMessage;

namespace Input
{
	SDL_Event event;


	void Await()
	{
		while (true) {
			while (SDL_PollEvent(&event)) {
				switch (event.type) {

				case SDL_CONTROLLERAXISMOTION:
					if (MatchInput<HostInputType::ControllerAxis>(event.caxis.axis, event.caxis.which, event.caxis.value)) {
						return;
					}
					break;

				case SDL_CONTROLLERBUTTONDOWN:
					if (MatchInput<HostInputType::ControllerButton, ButtonEvent::Press>(event.cbutton.button, event.cbutton.which)) {
						return;
					}
					break;

				case SDL_CONTROLLERBUTTONUP:
					if (MatchInput<HostInputType::ControllerButton, ButtonEvent::Release>(event.cbutton.button, event.cbutton.which)) {
						return;
					}
					break;

				case SDL_CONTROLLERDEVICEADDED:
				case SDL_CONTROLLERDEVICEREMOVED:
					OpenGameControllers();
					break;

				case SDL_KEYDOWN:
					if (MatchInput<HostInputType::Key, ButtonEvent::Press>(event.key.keysym.sym)) {
						return;
					}
					break;

				case SDL_KEYUP:
					if (MatchInput<HostInputType::Key, ButtonEvent::Release>(event.key.keysym.sym)) {
						return;
					}
					break;

				case SDL_MOUSEBUTTONDOWN:
					if (MatchInput<HostInputType::MouseButton, ButtonEvent::Press>(event.button.button)) {
						return;
					}
					break;

				case SDL_MOUSEBUTTONUP:
					if (MatchInput<HostInputType::MouseButton, ButtonEvent::Release>(event.button.button)) {
						return;
					}
					break;

				default:
					break;
				}
			}
		}
	}


	void ClearAllBindings()
	{
		for (uint player = 0; player < max_players; ++player) {
			ClearBindings(player);
		}
	}


	void ClearBindings(uint player_index)
	{
		std::ranges::fill(players.at(player_index).core_bindings, unbound_host_input);
	}


	std::vector<std::string_view> GetCoreActionNames()
	{
		return core_action_names;
	}


	bool Initialize()
	{
		for (Player& player : players) {
			player.active = true;
			player.core_bindings.resize(num_core_inputs);
			for (HostInputBinding& binding : player.core_bindings) {
				binding = unbound_host_input;
			}
		}
		OpenGameControllers();
		LoadBindings();
		return true;
	}


	std::string JoystickIdToGuid(SDL_JoystickID joystick_id)
	{
		SDL_Joystick* joystick = SDL_JoystickFromInstanceID(joystick_id);
		SDL_JoystickGUID joystick_guid = SDL_JoystickGetGUID(joystick); /* typedef struct { Uint8 data[16]; } */
		/* SDL_joystick.h: "You should supply at least 33 bytes [for the buffer] [for SDL_JoystickGetGUIDString]" */
		std::string joystick_guid_str;
		joystick_guid_str.reserve(33);
		SDL_JoystickGetGUIDString(joystick_guid, joystick_guid_str.data(), 33);
		return joystick_guid_str;
	}


	void LoadBindings()
	{
		//if (!std::filesystem::exists(bindings_file_path)) {
		//	return;
		//}
		//SerializationStream stream{ SerializationMode::Read, bindings_file_path };
		//if (stream.HasError()) {
		//	UserMessage::Show("Could not load input bindings.", UserMessage::Type::Warning);
		//	return;
		//}
		// TODO
	}


	void OpenGameControllers()
	{
		controllers.clear();
		for (int i = 0; i < SDL_NumJoysticks(); ++i) {
			if (SDL_IsGameController(i)) {
				SDL_GameController* controller = SDL_GameControllerOpen(i);
				if (controller) {
					controllers.push_back(controller);
				}
			}
		}
	}


	void ProcessEvent(SDL_Event event)
	{
		switch (event.type) {

		case SDL_CONTROLLERAXISMOTION:
			MatchInput<HostInputType::ControllerAxis>(event.caxis.axis, event.caxis.which, event.caxis.value);
			break;

		case SDL_CONTROLLERBUTTONDOWN:
			MatchInput<HostInputType::ControllerButton, ButtonEvent::Press>(event.cbutton.button, event.cbutton.which);
			break;

		case SDL_CONTROLLERBUTTONUP:
			MatchInput<HostInputType::ControllerButton, ButtonEvent::Release>(event.cbutton.button, event.cbutton.which);
			break;

		case SDL_CONTROLLERDEVICEADDED:
		case SDL_CONTROLLERDEVICEREMOVED:
			OpenGameControllers();
			break;

		case SDL_KEYDOWN: {
			MatchInput<HostInputType::Key, ButtonEvent::Press>(event.key.keysym.sym);
			break;
		}

		case SDL_KEYUP:
			MatchInput<HostInputType::Key, ButtonEvent::Release>(event.key.keysym.sym);
			break;

		case SDL_MOUSEBUTTONDOWN:
			MatchInput<HostInputType::MouseButton, ButtonEvent::Press>(event.button.button);
			break;

		case SDL_MOUSEBUTTONUP:
			MatchInput<HostInputType::MouseButton, ButtonEvent::Release>(event.button.button);
			break;

		default:
			break;
		}
	}


	void SaveBindings()
	{
		/*SerializationStream stream{ SerializationMode::Write, bindings_file_path };
		if (stream.HasError()) {
			UserMessage::Show("Could not save input bindings.", UserMessage::Type::Warning);
			return;
		}*/
		// TODO
	}


	void SetCoreActionNames(std::vector<std::string_view> names)
	{
		core_action_names = names;
		num_core_inputs = (uint)names.size();
	}


	void SetDefaultBindings()
	{
		// TODO
	}


	void SetPlayerActive(uint player_index)
	{
		players.at(player_index).active = true;
	}


	void SetPlayerInactive(uint player_index)
	{
		players.at(player_index).active = false;
	}
}