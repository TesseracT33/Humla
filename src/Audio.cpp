module Audio;

import Emulator;
import UserMessage;

namespace Audio
{
	void EnqueueSample(f32 sample)
	{
		//// TODO: in the future, we may have to support samples other than f32
		//sample_buffer[sample_buffer_index++] = sample;
		//if (sample_buffer_index == sample_buffer.size()) {
		//	//auto microsecs_until_next_audio_enqueue =
		//	//	std::chrono::duration_cast<std::chrono::microseconds>(
		//	//		std::chrono::steady_clock::now() - last_audio_enqueue_time_point);
		//	//// TODO: handle this in a better way
		//	//static constexpr auto time_until_no_sleep = std::chrono::microseconds(1500);
		//	//if (microsecs_until_next_audio_enqueue > time_until_no_sleep) {
		//	//	std::this_thread::sleep_for(microsecs_until_next_audio_enqueue - time_until_no_sleep);
		//	//}
		//	while (std::chrono::duration_cast<std::chrono::microseconds>(
		//		std::chrono::steady_clock::now() - last_audio_enqueue_time_point).count() < microsecs_per_audio_enqueue);
		//	SDL_QueueAudio(audio_device_id, sample_buffer.data(), uint(sample_buffer.size() * sizeof(f32)));
		//	sample_buffer_index = 0;
		//	last_audio_enqueue_time_point = std::chrono::steady_clock::now();
		//}
	}


	void CloseFile()
	{
		if (mixer_last_opened_file != nullptr) {
			Mix_FreeChunk(mixer_last_opened_file);
			mixer_last_opened_file = nullptr;
		}
	}


	void Exit()
	{
		SDL_CloseAudioDevice(audio_device_id);
		Mix_CloseAudio();
	}


	uint GetSampleRate()
	{
		return sample_rate;
	}


	bool Initialize()
	{
		if (SDL_Init(SDL_INIT_AUDIO) != 0) {
			UserMessage::Show(SDL_GetError(), UserMessage::Type::Error);
			return false;
		}

		static constexpr uint default_sample_rate = 44100;
		static constexpr uint default_num_output_channels = 2;
		static constexpr uint default_sample_buffer_size_per_channel = 512;

		SDL_AudioSpec desired_spec;
		SDL_zero(desired_spec);
		desired_spec.freq = default_sample_rate;
		desired_spec.format = AUDIO_F32;
		desired_spec.channels = default_num_output_channels;
		desired_spec.samples = default_sample_buffer_size_per_channel;
		desired_spec.callback = nullptr;

		SDL_AudioSpec obtained_spec;
		audio_device_id = SDL_OpenAudioDevice(nullptr, 0, &desired_spec, &obtained_spec, 0);
		if (audio_device_id == 0) {
			const char* error_msg = SDL_GetError();
			UserMessage::Show(std::format("Could not open an audio device; {}", error_msg), UserMessage::Type::Warning);
			return false;
		}

		SetSampleRate(obtained_spec.freq);
		num_output_channels = obtained_spec.channels;
		sample_buffer_size_per_channel = obtained_spec.samples;
		microsecs_per_audio_enqueue = std::lround(f64(sample_buffer_size_per_channel) / f64(sample_rate) * 1e6);
		sample_buffer.resize(sample_buffer_size_per_channel * num_output_channels);

		SDL_PauseAudioDevice(audio_device_id, 0);

		if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
			UserMessage::Show(std::format("Failed to initialize audio; {}", Mix_GetError()),
				UserMessage::Type::Warning);
			return false;
		}

		return true;
	}


	void OpenFileForPlaying(std::string_view path)
	{
		if (mixer_last_opened_file != nullptr) {
			CloseFile();
		}
		mixer_last_opened_file = Mix_LoadWAV(path.data());
		if (mixer_last_opened_file == nullptr) {
			UserMessage::Show(std::format("Failed to open audio file; {}", Mix_GetError()),
				UserMessage::Type::Warning);
		}
	}


	void PlayFile()
	{
		if (mixer_last_opened_file == nullptr) {
			UserMessage::Show("Cannot play audio file that has not been loaded yet", UserMessage::Type::Error);
		}
		else {
			if (Mix_PlayChannel(-1, mixer_last_opened_file, 0) == -1) {
				UserMessage::Show(std::format("Failed to play audio file; {}", Mix_GetError()),
					UserMessage::Type::Warning);
			}
		}
	}


	void PlayFile(std::string_view path)
	{
		OpenFileForPlaying(path);
		PlayFile();
	}


	void SetNumberOfOutputChannels(uint num_channels)
	{
		num_output_channels = num_channels;
		sample_buffer.resize(sample_buffer_size_per_channel * num_output_channels);
		sample_buffer_index = 0;
	}
	
	
	void SetSampleBufferSizePerChannel(uint buffer_size)
	{
		sample_buffer_size_per_channel = buffer_size;
		sample_buffer.resize(sample_buffer_size_per_channel * num_output_channels);
		sample_buffer_index = 0;
		microsecs_per_audio_enqueue = std::lround(f64(sample_buffer_size_per_channel) / f64(sample_rate) * 1e6);
	}
	
	
	void SetSampleRate(uint sample_rate)
	{
		Audio::sample_rate = sample_rate;
		microsecs_per_audio_enqueue = std::lround(f64(sample_buffer_size_per_channel) / f64(sample_rate) * 1e6);
		Emulator::GetCore()->ApplyNewSampleRate();
	}
}