export module Audio;

import Types;

import <SDL.h>;
import <SDL_mixer.h>;

import <chrono>;
import <cmath>;
import <format>;
import <string_view>;
import <thread>;
import <vector>;

namespace Audio
{
	export
	{
		void CloseFile();
		void EnqueueSample(f32 sample);
		void Exit();
		uint GetSampleRate();
		bool Initialize();
		void OpenFileForPlaying(std::string_view path);
		void PlayFile();
		void PlayFile(std::string_view path);
		void SetNumberOfOutputChannels(uint num_channels);
		void SetSampleBufferSizePerChannel(uint buffer_size);
		void SetSampleRate(uint sample_rate);
	}

	uint microsecs_per_audio_enqueue;
	uint num_output_channels;
	uint sample_buffer_index;
	uint sample_buffer_size_per_channel;
	uint sample_rate;

	Mix_Chunk* mixer_last_opened_file = nullptr;

	SDL_AudioDeviceID audio_device_id;

	std::vector<f32> sample_buffer;

	std::chrono::steady_clock::time_point last_audio_enqueue_time_point;
}