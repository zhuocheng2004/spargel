#pragma once

#include <spargel/base/unique_ptr.h>
#include <spargel/base/vector.h>
#include <spargel/gpu/gpu3.h>

// metal
#import <Metal/Metal.h>
#import <QuartzCore/QuartzCore.h>

namespace spargel::gpu {

    class shader_library_metal : public shader_library {
    public:
        shader_library_metal(dispatch_data_t data, id<MTLLibrary> library);
        ~shader_library_metal();

        auto library() const { return _library; }

    private:
        dispatch_data_t _data;
        id<MTLLibrary> _library;
    };

    class render_pipeline_metal : public render_pipeline {
    public:
        render_pipeline_metal(id<MTLFunction> vertex_function, id<MTLRenderPipelineState> pipeline);
        ~render_pipeline_metal();

        id<MTLRenderPipelineState> pipeline() { return _pipeline; }

    private:
        id<MTLFunction> _vertex;
        id<MTLRenderPipelineState> _pipeline;
    };

    class buffer_metal : public buffer {
    public:
        buffer_metal(id<MTLBuffer> b);
        ~buffer_metal();

        id<MTLBuffer> buffer() { return _buffer; }

    private:
        id<MTLBuffer> _buffer;
    };

    class texture_metal final : public texture {
    public:
        texture_metal(id<MTLTexture> t) : _texture{t} {}
        ~texture_metal() {}

        id<MTLTexture> texture() { return _texture; }

    private:
        id<MTLTexture> _texture;
    };

    class surface_metal final : public surface {
    public:
        surface_metal(CAMetalLayer* layer) : _layer{layer}, _texture(nil) {}
        ~surface_metal() {}

        CAMetalLayer* layer() { return _layer; }

        object_ptr<texture> next_texture() override {
            _drawable = [_layer nextDrawable];
            base::construct_at(&_texture, _drawable.texture);
            return object_ptr<texture>(&_texture);
        }

        id<CAMetalDrawable> drawable() { return _drawable; }

        float width() override { return _layer.drawableSize.width; }
        float height() override { return _layer.drawableSize.height; }

    private:
        CAMetalLayer* _layer;
        id<CAMetalDrawable> _drawable;
        texture_metal _texture;
    };

    class device_metal final : public device {
    public:
        device_metal();
        ~device_metal() override;

        object_ptr<shader_library> make_shader_library(
            shader_library_descriptor const& descriptor) override;
        object_ptr<render_pipeline> make_render_pipeline(
            render_pipeline_descriptor const& descriptor) override;
        object_ptr<buffer> make_buffer_with_bytes(base::span<u8> bytes) override;
        object_ptr<surface> make_surface(ui::window* w) override;

        void begin_render_pass(object_ptr<texture> t) override;
        void set_render_pipeline(object_ptr<render_pipeline> p) override;
        void set_vertex_buffer(object_ptr<buffer> b, vertex_buffer_location const& loc) override;
        void set_viewport(viewport v) override;
        void draw(int vertex_start, int vertex_count) override;
        void draw(int vertex_start, int vertex_count, int instance_start,
                  int instance_count) override;
        void end_render_pass() override;
        void present(object_ptr<surface> s) override;

        void destroy_shader_library(object_ptr<shader_library> library) override;
        void destroy_render_pipeline(object_ptr<render_pipeline> pipeline) override;
        void destroy_buffer(object_ptr<buffer> b) override;

        void execute(task_graph& graph) override;

    private:
        id<MTLDevice> _device;
        id<MTLCommandQueue> _queue;
        id<MTLCommandBuffer> _buffer;
        id<MTLRenderCommandEncoder> _encoder;
        // MTLRenderPassDescriptor* _pass_desc;
    };

}  // namespace spargel::gpu
