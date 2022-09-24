#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

module Frontend;

import Audio;
import Emulator;
import Input;
import UserMessage;
import Video;

namespace Frontend
{
	float GetImGuiMenuBarHeight()
	{
		// TODO
		return 19;
	}


	bool Initialize(std::shared_ptr<Core> core)
	{
		/* Setup SDL */
		SDL_SetMainReady();
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			std::cerr << SDL_GetError();
			return false;
		}
		sdl_window = SDL_CreateWindow(
			"Emulator",
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			500,
			500,
			SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS | SDL_WINDOW_RESIZABLE
		);
		if (!sdl_window) {
			std::cerr << SDL_GetError();
			return false;
		}
		UserMessage::SetWindow(sdl_window);
		sdl_renderer = SDL_CreateRenderer(sdl_window, 0, SDL_RENDERER_ACCELERATED);
		if (!sdl_renderer) {
			UserMessage::Show(SDL_GetError(), UserMessage::Type::Fatal);
			return false;
		}

		/* Setup ImGui */
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui::StyleColorsDark();
		if (!ImGui_ImplSDL2_InitForSDLRenderer(sdl_window, sdl_renderer)) {
			UserMessage::Show("Failure when initializing ImGui in call to \"ImGui_ImplSDL2_InitForSDLRenderer\"", 
				UserMessage::Type::Fatal);
			return false;
		}
		if (!ImGui_ImplSDLRenderer_Init(sdl_renderer)) {
			UserMessage::Show("Failure when initializing ImGui in call to \"ImGui_ImplOpenGL3_Init\"", 
				UserMessage::Type::Fatal);
			return false;
		}
		auto menubar_height = GetImGuiMenuBarHeight();

		Emulator::SetCore(core);
		core->Initialize();
		core->SetupCommunicationWithFrontend();

		/* Setup Audio/Video/Input */
		if (!Audio::Initialize()) {
			UserMessage::Show("Failed to initialize audio.", UserMessage::Type::Fatal);
			return false;
		}
		if (!Input::Initialize()) {
			UserMessage::Show("Failed to initialize input.", UserMessage::Type::Fatal);
			return false;
		}
		if (!Video::Initialize(sdl_renderer, sdl_window)) {
			UserMessage::Show("Failed to initialize video.", UserMessage::Type::Fatal);
			return false;
		}
		Video::SetGameRenderAreaSize(500, 500);
		Video::SetGameRenderAreaOffsetX(0);
		Video::SetGameRenderAreaOffsetY(19);

		input_window_button_pressed = false;
		menu_enable_audio = true;
		menu_fullscreen = false;
		menu_lock_framerate = true;
		menu_pause_emulation = false;
		quit = false;
		show_gui = true;
		show_input_bindings_window = false;

		core_action_names = Input::GetCoreActionNames();

		return true;
	}


	bool LoadBios(std::string bios_path)
	{
		if (!Emulator::LoadBios(bios_path)) {
			UserMessage::Show(std::format("Could not load bios at path \"{}\"", bios_path),
				UserMessage::Type::Warning);
			return false;
		}
		return true;
	}


	bool LoadGame(std::string rom_path)
	{
		if (!Emulator::LoadRom(rom_path)) {
			UserMessage::Show(std::format("Could not load rom at path \"{}\"", rom_path),
				UserMessage::Type::Warning);
			return false;
		}
		return true;
	}


	void OnCtrlKeyPress(SDL_Keycode keycode)
	{
		switch (keycode) {
		case SDLK_a:
			menu_enable_audio = !menu_enable_audio;
			OnMenuEnableAudio();
			break;

		case SDLK_f:
			menu_lock_framerate = !menu_lock_framerate;
			OnMenuLockFramerate();
			break;

		case SDLK_l:
			OnMenuLoadState();
			break;

		case SDLK_m:
			show_gui = !show_gui;
			Video::SetGameRenderAreaOffsetY(show_gui ? 19 : 0); // TODO: make non-hacky
			break;

		case SDLK_o:
			OnMenuOpen();
			break;

		case SDLK_p:
			menu_pause_emulation = !menu_pause_emulation;
			OnMenuPause();
			break;

		case SDLK_q:
			OnMenuQuit();
			break;

		case SDLK_r:
			OnMenuReset();
			break;

		case SDLK_RETURN:
			menu_fullscreen = !menu_fullscreen;
			OnMenuFullscreen();
			break;

		case SDLK_s:
			OnMenuSaveState();
			break;

		case SDLK_x:
			OnMenuStop();
			break;

		default:
			break;
		}
	}


	void OnMenuConfigureBindings()
	{
		show_input_bindings_window = !show_input_bindings_window;
	}


	void OnMenuEnableAudio()
	{
		menu_enable_audio ? Emulator::EnableAudio() : Emulator::DisableAudio();
	}


	void OnMenuFullscreen()
	{
		menu_fullscreen ? Video::EnableFullscreen() : Video::DisableFullscreen();
	}


	void OnMenuLoadState()
	{
		Emulator::LoadState();
	}


	void OnMenuLockFramerate()
	{
		menu_lock_framerate ? Emulator::LockFramerate() : Emulator::UnlockFramerate();
	}


	void OnMenuOpen()
	{
		// TODO
	}


	void OnMenuOpenBios()
	{
		// TODO
	}


	void OnMenuOpenRecent()
	{
		// TODO
	}


	void OnMenuPause()
	{
		menu_pause_emulation ? Emulator::Pause() : ScheduleEmuThread(Emulator::Resume);
	}


	void OnMenuQuit()
	{
		Emulator::Stop();
		if (emu_thread.joinable()) {
			emu_thread.join();
		}
		quit = true;
	}


	void OnMenuReset()
	{
		ScheduleEmuThread(Emulator::Reset);
	}


	void OnMenuSaveState()
	{
		Emulator::SaveState();
	}


	void OnMenuStop()
	{
		Emulator::Stop();
	}


	void OnMenuWindowScale()
	{
		// TODO
	}


	void RenderGui()
	{
		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::BeginMenu("File")) {
				if (ImGui::MenuItem("Open", "Ctrl+O")) {
					OnMenuOpen();
				}
				if (ImGui::MenuItem("Open recent")) {
					OnMenuOpenRecent();
				}
				if (ImGui::MenuItem("Open BIOS")) {
					OnMenuOpenBios();
				}
				if (ImGui::MenuItem("Load state", "Ctrl+L")) {
					OnMenuLoadState();
				}
				if (ImGui::MenuItem("Save state", "Ctrl+S")) {
					OnMenuSaveState();
				}
				if (ImGui::MenuItem("Quit", "Ctrl+Q")) {
					OnMenuQuit();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Emulation")) {
				if (ImGui::MenuItem("Pause", "Ctrl+P", &menu_pause_emulation, true)) {
					OnMenuPause();
				}
				if (ImGui::MenuItem("Reset", "Ctrl+R")) {
					OnMenuReset();
				}
				if (ImGui::MenuItem("Stop", "Ctrl+X")) {
					OnMenuStop();
				}
				if (ImGui::MenuItem("Lock framerate", "Ctrl+F", &menu_lock_framerate, true)) {
					OnMenuLockFramerate();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Audio")) {
				if (ImGui::MenuItem("Enable", "Ctrl+A", &menu_enable_audio, true)) {
					OnMenuEnableAudio();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Video")) {
				if (ImGui::MenuItem("Set window scale")) {
					OnMenuWindowScale();
				}
				if (ImGui::MenuItem("Fullscreen", "Ctrl+Enter", &menu_fullscreen, true)) {
					OnMenuFullscreen();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Input")) {
				if (ImGui::MenuItem("Configure bindings")) {
					OnMenuConfigureBindings();
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Debug")) {
				// TODO
				ImGui::EndMenu();
			}
			ImGui::EndMainMenuBar();
		}

		if (show_input_bindings_window) {
			RenderInputBindingsWindow();
		}
	}


	void RenderInputBindingsWindow()
	{
		// TODO
		if (ImGui::Begin("Input bindings", &show_input_bindings_window)) {
			for (std::string_view& action_binding : core_action_bindings) {
				if (ImGui::Button(action_binding.data())) {

				}
				ImGui::SameLine();
				ImGui::Text("ASD");
			}
		}
		ImGui::End();
	}


	void RunGui(bool boot_game_immediately)
	{
		if (boot_game_immediately) {
			StartGame();
		}

		SDL_Event event;
		while (!quit) {
			while (SDL_PollEvent(&event)) {
				ImGui_ImplSDL2_ProcessEvent(&event);
				if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT
					&& event.window.event == SDL_WINDOWEVENT_CLOSE
					&& event.window.windowID == SDL_GetWindowID(sdl_window)) {
					quit = true;
				}
				else if (event.type == SDL_KEYDOWN) {
					SDL_Keycode keycode = event.key.keysym.sym;
					if ((SDL_GetModState() & SDL_Keymod::KMOD_CTRL) != 0 && keycode != SDLK_LCTRL && keycode != SDLK_RCTRL) { /* LCTRL/RCTRL is held */
						OnCtrlKeyPress(keycode);
					}
					else {
						Input::ProcessEvent(event);
					}
				}
				else {
					Input::ProcessEvent(event);
				}
			}

			SDL_RenderClear(sdl_renderer);
			ImGui_ImplSDLRenderer_NewFrame();
			ImGui_ImplSDL2_NewFrame(sdl_window);
			ImGui::NewFrame();
			Video::RenderGame();
			if (show_gui) {
				RenderGui();
			}
			ImGui::Render();
			ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
			SDL_RenderPresent(sdl_renderer);
			SDL_GL_SwapWindow(sdl_window);

			/* SDL will automatically block so that the number of frames rendered per second is
			   equal to the display's refresh rate. */
		}
	}


	void ScheduleEmuThread(void(*function)())
	{
		Emulator::Stop();
		if (emu_thread.joinable()) {
			emu_thread.join();
		}
		emu_thread = std::jthread{ function };
	}


	void Shutdown()
	{
		Emulator::Stop();
		ImGui_ImplSDLRenderer_Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
		SDL_DestroyRenderer(sdl_renderer);
		SDL_DestroyWindow(sdl_window);
		SDL_Quit();
	}


	void StartGame()
	{
		ScheduleEmuThread(Emulator::StartGame);
	}


	void StopGame()
	{
		Emulator::Stop();
	}
}