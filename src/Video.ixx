export module Video;

import Types;

import <SDL.h>;

import <algorithm>;
import <cassert>;
import <chrono>;
import <format>;

namespace Video
{
	export
	{
		enum class PixelFormat {
			ABGR8888,
			INDEX1LSB,
			BGR888,
			RGB888,
			RGBA8888,
		};

		void DisableFullscreen();
		void DisableRendering();
		void EnableFullscreen();
		void EnableRendering();
		bool Initialize(SDL_Renderer* renderer, SDL_Window* window);
		void NotifyNewGameFrameReady();
		void RenderGame();
		void SetFramebufferHeight(uint height);
		void SetFramebufferPtr(u8* ptr);
		void SetFramebufferSize(uint width, uint height);
		void SetFramebufferWidth(uint width);
		void SetPixelFormat(PixelFormat format);
		void SetGameRenderAreaOffsetX(uint offset);
		void SetGameRenderAreaOffsetY(uint offset);
		void SetGameRenderAreaSize(uint width, uint height);
		void SetWindowSize(uint width, uint height);
	}

	void EvaluateWindowProperties();
	void RecreateTexture();
	void UpdateWindowsFpsLabel(f32 new_fps);

	struct Framebuffer
	{
		u8* ptr;
		uint width, height, pitch;
		uint bytes_per_pixel;
		uint pixel_format;
	} framebuffer;

	struct Window
	{
		uint width, height; /* the dimensions of the sdl window */
		uint game_width, game_height; /* the dimensions of the game render area */
		uint game_offset_x, game_offset_y; /* offset of game render area on window */
		uint game_inner_render_offset_x, game_inner_render_offset_y;
		uint scale; /* scale of game render area in relation to the base core resolution. */
	} window;

	bool rendering_is_enabled;

	uint frame_counter;

	SDL_Rect dstrect;
	SDL_Renderer* sdl_renderer;
	SDL_Texture* sdl_texture;
	SDL_Window* sdl_window;

	std::chrono::time_point time_now = std::chrono::steady_clock::now();
}