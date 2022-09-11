module Video;

import UserMessage;

namespace Video
{
	void DisableFullscreen()
	{
		// TODO
		SDL_SetWindowFullscreen(sdl_window, 0);
	}


	void DisableRendering()
	{
		rendering_is_enabled = false;
	}


	void EnableFullscreen()
	{
		SDL_SetWindowFullscreen(sdl_window, SDL_WINDOW_FULLSCREEN_DESKTOP);
		SDL_DisplayMode display_mode;
		SDL_GetCurrentDisplayMode(0, &display_mode);
		window.width = window.game_width = display_mode.w;
		window.height = window.game_height = display_mode.h;
		EvaluateWindowProperties();
	}


	void EnableRendering()
	{
		rendering_is_enabled = true;
	}


	void EvaluateWindowProperties()
	{
		window.scale = std::min(window.game_width / framebuffer.width, window.game_height / framebuffer.height);
		window.game_inner_render_offset_x = (window.game_width - window.scale * framebuffer.width) / 2;
		window.game_inner_render_offset_y = (window.game_height - window.scale * framebuffer.height) / 2;
		dstrect.w = window.scale * framebuffer.width;
		dstrect.h = window.scale * framebuffer.height;
		dstrect.x = window.game_offset_x + window.game_inner_render_offset_x;
		dstrect.y = window.game_offset_y + window.game_inner_render_offset_y;
	}


	bool Initialize(SDL_Renderer* renderer, SDL_Window* window)
	{
		if (!renderer) {
			UserMessage::Show("SDL_Renderer* was null", UserMessage::Type::Fatal);
			return false;
		}
		if (!window) {
			UserMessage::Show("SDL_Window* was null", UserMessage::Type::Fatal);
			return false;
		}
		Video::sdl_renderer = renderer;
		Video::sdl_window = window;
		rendering_is_enabled = true;
		RecreateTexture();
		return true;
	}


	void NotifyNewGameFrameReady()
	{
		if (++frame_counter == 60) {
			auto microsecs_to_render_60_frames = std::chrono::duration_cast<std::chrono::microseconds>(
				std::chrono::steady_clock::now() - time_now).count();
			f32 fps = 60.0f * 1'000'000.0f / f32(microsecs_to_render_60_frames);
			UpdateWindowsFpsLabel(fps);
			time_now = std::chrono::steady_clock::now();
			frame_counter = 0;
		}
	}


	void RecreateTexture()
	{
		SDL_DestroyTexture(sdl_texture);
		sdl_texture = SDL_CreateTexture(
			sdl_renderer,
			framebuffer.pixel_format,
			SDL_TEXTUREACCESS_STREAMING,
			framebuffer.width,
			framebuffer.height
		);
	}


	void RenderGame()
	{
		if (!rendering_is_enabled) {
			return;
		}

		void* locked_pixels = nullptr;
		int locked_pixels_pitch;
		SDL_LockTexture(sdl_texture, nullptr, &locked_pixels, &locked_pixels_pitch);

		SDL_ConvertPixels(
			framebuffer.width,        /* framebuffer width  */
			framebuffer.height,       /* framebuffer height */
			framebuffer.pixel_format, /* source format      */
			framebuffer.ptr,          /* source             */
			framebuffer.pitch,        /* source pitch       */
			framebuffer.pixel_format, /* destination format */
			locked_pixels,            /* destination        */
			framebuffer.pitch         /* destination pitch  */
		);

		SDL_UnlockTexture(sdl_texture);
		SDL_RenderCopy(sdl_renderer, sdl_texture, nullptr, &dstrect);
	}


	void SetFramebufferHeight(uint height)
	{
		framebuffer.height = height;
		EvaluateWindowProperties();
		RecreateTexture();
	}


	void SetFramebufferPtr(u8* ptr)
	{
		if (!ptr) {
			UserMessage::Show("Fatal: framebuffer pointer was set to null.", UserMessage::Type::Fatal);
			exit(1);
		}
		framebuffer.ptr = ptr;
	}


	void SetFramebufferSize(uint width, uint height)
	{
		framebuffer.width = width;
		framebuffer.height = height;
		framebuffer.pitch = width * framebuffer.bytes_per_pixel;
		EvaluateWindowProperties();
		RecreateTexture();
	}


	void SetFramebufferWidth(uint width)
	{
		framebuffer.width = width;
		framebuffer.pitch = width * framebuffer.bytes_per_pixel;
		EvaluateWindowProperties();
		RecreateTexture();
	}


	void SetGameRenderAreaOffsetX(uint offset)
	{
		window.game_offset_x = offset;
		EvaluateWindowProperties();
	}


	void SetGameRenderAreaOffsetY(uint offset)
	{
		window.game_offset_y = offset;
		EvaluateWindowProperties();
	}


	void SetGameRenderAreaSize(uint width, uint height)
	{
		window.game_width = width;
		window.game_height = height;
		EvaluateWindowProperties();
	}


	void SetPixelFormat(PixelFormat format)
	{
		/* This is not ideal, but it's meant to decouple the cores from SDL completely */
		framebuffer.pixel_format = [&] {
			using enum PixelFormat;
			switch (format) {
			case ABGR8888:
				framebuffer.bytes_per_pixel = 4;
				return SDL_PIXELFORMAT_ABGR8888;

			case BGR888:
				framebuffer.bytes_per_pixel = 3;
				return SDL_PIXELFORMAT_BGR24;

			case RGB888:
				framebuffer.bytes_per_pixel = 3;
				return SDL_PIXELFORMAT_RGB24;

			case RGBA8888:
				framebuffer.bytes_per_pixel = 4;
				return SDL_PIXELFORMAT_RGBA8888;

			default:
				assert(false);
				return SDL_PIXELFORMAT_RGBA8888;
			}
		}();
		framebuffer.pitch = framebuffer.width * framebuffer.bytes_per_pixel;
		RecreateTexture();
	}


	void SetWindowSize(uint width, uint height)
	{
		window.width = width;
		window.height = height;
		EvaluateWindowProperties();
	}


	void UpdateWindowsFpsLabel(f32 new_fps)
	{
		std::string label = std::format("FPS: {}", new_fps);
		SDL_SetWindowTitle(sdl_window, label.data());
	}
}