#include <spargel/base/unique_ptr.h>
#include <spargel/gpu/gpu3_metal.h>

// metal
#import <Metal/Metal.h>

namespace spargel::gpu {

    namespace {
        
        MTLCullMode translate_cull_mode(cull_mode mode) {
            using enum cull_mode;
            switch (mode) {
            case none: return MTLCullModeNone;
            case front: return MTLCullModeFront;
            case back: return MTLCullModeBack;
            }
        }

        MTLWinding translate_orientation(orientation o) {
            using enum orientation;
            switch (o) {
            case clockwise: return MTLWindingClockwise;
            case counter_clockwise: return MTLWindingCounterClockwise;
            }
        }

        MTLPrimitiveTopologyClass translate_primitive_type(primitive_kind kind) {
            using enum primitive_kind;
            switch (kind) {
            case point: return MTLPrimitiveTopologyClassPoint;
            case line: return MTLPrimitiveTopologyClassLine;
            case triangle: return MTLPrimitiveTopologyClassTriangle;
            }
        }

        MTLVertexStepFunction translate_step_mode(vertex_step_mode mode) {
            using enum vertex_step_mode;
            switch (mode) {
            case vertex: return MTLVertexStepFunctionPerVertex;
            case instance: return MTLVertexStepFunctionPerInstance;
            }
        }

    }

    base::unique_ptr<device> make_device_metal() { return base::make_unique<device_metal>(); }

    device_metal::device_metal() : device(device_kind::metal) {
        _device = MTLCreateSystemDefaultDevice(); // ns_returns_retained
        [_device retain];
    }

    device_metal::~device_metal() {
        [_device release];
    }

    object_ptr<shader_library> device_metal::make_shader_library(shader_library_descriptor const& descriptor) {
        // dispatch create a copy of the data
        // 
        // notes:
        // This object is retained initially. It is your responsibility to release the data object when you are done using it.
        //
        auto library_data = dispatch_data_create(descriptor.bytes, descriptor.size, dispatch_get_main_queue(), DISPATCH_DATA_DESTRUCTOR_DEFAULT);
        NSError* error;
        auto library = [_device newLibraryWithData:library_data error:&error];
        if (library == nil) {
            return nullptr;
        }
        return make_object<shader_library_metal>(library_data, library);
    }

    object_ptr<render_pipeline> device_metal::make_render_pipeline(render_pipeline_descriptor const& descriptor) {
        auto desc = [[MTLRenderPipelineDescriptor alloc] init];
        auto vertex_library = descriptor.vertex_shader.library.cast<shader_library_metal>();
        auto vertex_entry = descriptor.vertex_shader.entry;
        auto vertex_function = [vertex_library->library() newFunctionWithName:[NSString stringWithUTF8String:vertex_entry]];

        auto vertex_descriptor = [MTLVertexDescriptor vertexDescriptor];

        for (ssize i = 0; i < descriptor.vertex_buffers.count(); i++) {
            vertex_descriptor.layouts[i].stepFunction = translate_step_mode(descriptor.vertex_buffers[i].step_mode);
            vertex_descriptor.layouts[i].stride = descriptor.vertex_buffers[i].stride;
        }

        desc.vertexFunction = vertex_function;
        desc.colorAttachments[0].pixelFormat = MTLPixelFormatRGBA8Unorm;
        desc.vertexDescriptor = vertex_descriptor;
        desc.inputPrimitiveTopology = translate_primitive_type(descriptor.primitive);

        NSError* error;
        auto state = [_device newRenderPipelineStateWithDescriptor:desc error:&error];

        [desc release];

        return make_object<render_pipeline_metal>(vertex_function, state);
    }

    void device_metal::destroy_shader_library(object_ptr<shader_library> l) {
        auto library = l.cast<shader_library_metal>();
        destroy_object(library);
    }
    
    void device_metal::destroy_render_pipeline(object_ptr<render_pipeline> p) {
        auto pipeline = p.cast<render_pipeline_metal>();
        destroy_object(pipeline);
    }

    shader_library_metal::shader_library_metal(dispatch_data_t data, id<MTLLibrary> library) : _data{data}, _library{library} {}

    shader_library_metal::~shader_library_metal() {
        NSLog(@"%@", [_library functionNames]);
        [_library release];
        [_data release];
    }

    render_pipeline_metal::render_pipeline_metal(id<MTLFunction> vertex, id<MTLRenderPipelineState> pipeline) : _vertex{vertex}, _pipeline{pipeline} {}

    render_pipeline_metal::~render_pipeline_metal() {
        [_pipeline release];
        [_vertex release];
    }

}  // namespace spargel::gpu
