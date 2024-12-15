#pragma once

#include <spargel/base/unique_ptr.h>
#include <spargel/base/vector.h>
#include <spargel/gpu/gpu3.h>

// metal
#import <Metal/Metal.h>

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

    private:
        id<MTLFunction> _vertex;
        id<MTLRenderPipelineState> _pipeline;
    };

    class device_metal final : public device {
    public:
        device_metal();
        ~device_metal() override;

        object_ptr<shader_library> make_shader_library(shader_library_descriptor const& descriptor) override;
        object_ptr<render_pipeline> make_render_pipeline(render_pipeline_descriptor const& descriptor) override;

        void destroy_shader_library(object_ptr<shader_library> library) override;
        void destroy_render_pipeline(object_ptr<render_pipeline> pipeline) override;

    private:
        id<MTLDevice> _device;
    };

}  // namespace spargel::gpu
