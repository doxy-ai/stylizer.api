export module stylizer.graphics.current_backend;

#if STYLIZER_CURRENT_BACKEND == 1 // TODO: Replace with platform decision logic
	export import stylizer.graphics.webgpu.common;
import stylizer.graphics.webgpu;

	export namespace stylizer::graphics::current_backend {
		using webgpu::surface;
		using webgpu::texture_view;
		using webgpu::texture;
		using webgpu::buffer;
		using webgpu::shader;
		using webgpu::pipeline;
		using webgpu::command_buffer;
		using webgpu::bind_group;
		using webgpu::compute_pipeline;
		using webgpu::command_encoder;
		using webgpu::render_pass;
		using webgpu::render_pipeline;
		using webgpu::device;
		// using namespace webgpu;
	}
#endif

	namespace stylizer::graphics::current_backend {
		export namespace compute {
			using pass = command_encoder;
			using pipeline = compute_pipeline;
		}

		export namespace render {
			using pass = render_pass;
			using pipeline = render_pipeline;
		}
	}