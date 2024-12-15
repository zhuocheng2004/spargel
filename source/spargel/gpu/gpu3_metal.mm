#include <spargel/base/base.h>
#include <spargel/base/unique_ptr.h>
#include <spargel/gpu/gpu3_metal.h>
#include <spargel/ui/ui.h>

// metal
#import <Metal/Metal.h>

namespace spargel::gpu {

    namespace {

        MTLCullMode translate_cull_mode(cull_mode mode) {
            using enum cull_mode;
            switch (mode) {
            case none:
                return MTLCullModeNone;
            case front:
                return MTLCullModeFront;
            case back:
                return MTLCullModeBack;
            }
        }

        MTLWinding translate_orientation(orientation o) {
            using enum orientation;
            switch (o) {
            case clockwise:
                return MTLWindingClockwise;
            case counter_clockwise:
                return MTLWindingCounterClockwise;
            }
        }

        MTLPrimitiveTopologyClass translate_primitive_type(primitive_kind kind) {
            using enum primitive_kind;
            switch (kind) {
            case point:
                return MTLPrimitiveTopologyClassPoint;
            case line:
                return MTLPrimitiveTopologyClassLine;
            case triangle:
                return MTLPrimitiveTopologyClassTriangle;
            }
        }

        MTLVertexStepFunction translate_step_mode(vertex_step_mode mode) {
            using enum vertex_step_mode;
            switch (mode) {
            case vertex:
                return MTLVertexStepFunctionPerVertex;
            case instance:
                return MTLVertexStepFunctionPerInstance;
            }
        }

        MTLVertexFormat translate_vertex_format(vertex_attribute_format format) {
            using enum vertex_attribute_format;
            switch (format) {
            case float32x2:
                return MTLVertexFormatFloat2;
            case float32x4:
                return MTLVertexFormatFloat4;
            default:
                spargel_panic_here();
            }
        }

    }  // namespace

    base::unique_ptr<device> make_device_metal() { return base::make_unique<device_metal>(); }

    device_metal::device_metal() : device(device_kind::metal) {
        _device = MTLCreateSystemDefaultDevice();  // ns_returns_retained
        [_device retain];
        _queue = [_device newCommandQueue];
        // _pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];
    }

    device_metal::~device_metal() {
        [_queue release];
        [_device release];
    }

    object_ptr<shader_library> device_metal::make_shader_library(
        shader_library_descriptor const& descriptor) {
        // dispatch create a copy of the data
        //
        // notes:
        // This object is retained initially. It is your responsibility to release the data object
        // when you are done using it.
        //
        auto library_data =
            dispatch_data_create(descriptor.bytes.data(), descriptor.bytes.count(),
                                 dispatch_get_main_queue(), DISPATCH_DATA_DESTRUCTOR_DEFAULT);
        NSError* error;
        auto library = [_device newLibraryWithData:library_data error:&error];
        if (library == nil) {
            return nullptr;
        }
        return make_object<shader_library_metal>(library_data, library);
    }

    object_ptr<render_pipeline> device_metal::make_render_pipeline(
        render_pipeline_descriptor const& descriptor) {
        auto desc = [[MTLRenderPipelineDescriptor alloc] init];

        auto vertex_library = descriptor.vertex_shader.library.cast<shader_library_metal>();
        auto vertex_entry = descriptor.vertex_shader.entry;

        auto vertex_function = [vertex_library->library()
            newFunctionWithName:[NSString stringWithUTF8String:vertex_entry]];

        for (MTLAttribute* attr in vertex_function.stageInputAttributes) {
            NSLog(@"%@", attr);
        }

        auto vertex_descriptor = [MTLVertexDescriptor vertexDescriptor];

        for (ssize i = 0; i < descriptor.vertex_buffers.count(); i++) {
            vertex_descriptor.layouts[i].stepFunction =
                translate_step_mode(descriptor.vertex_buffers[i].step_mode);
            vertex_descriptor.layouts[i].stride = descriptor.vertex_buffers[i].stride;
        }

        for (ssize i = 0; i < descriptor.vertex_attributes.count(); i++) {
            vertex_descriptor.attributes[i].format =
                translate_vertex_format(descriptor.vertex_attributes[i].format);
            vertex_descriptor.attributes[i].offset = descriptor.vertex_attributes[i].offset;
            // TODO: fix
            vertex_descriptor.attributes[i].bufferIndex = descriptor.vertex_attributes[i].buffer;
        }

        desc.vertexFunction = vertex_function;
        desc.colorAttachments[0].pixelFormat = MTLPixelFormatBGRA8Unorm;
        desc.vertexDescriptor = vertex_descriptor;
        desc.inputPrimitiveTopology = translate_primitive_type(descriptor.primitive);

        auto frag_library = descriptor.fragment_shader.library.cast<shader_library_metal>();
        auto frag_entry = descriptor.fragment_shader.entry;

        auto frag_function = [frag_library->library()
            newFunctionWithName:[NSString stringWithUTF8String:frag_entry]];

        for (MTLAttribute* attr in frag_function.stageInputAttributes) {
            NSLog(@"%@", attr);
        }

        desc.fragmentFunction = frag_function;

        NSError* error;

        auto state = [_device newRenderPipelineStateWithDescriptor:desc error:&error];

        [desc release];

        return make_object<render_pipeline_metal>(vertex_function, state);
    }

    object_ptr<buffer> device_metal::make_buffer_with_bytes(base::span<u8> bytes) {
        auto buf = [_device newBufferWithBytes:bytes.data()
                                        length:bytes.count()
                                       options:MTLResourceStorageModeShared];
        return make_object<buffer_metal>(buf);
    }

    object_ptr<surface> device_metal::make_surface(ui::window* w) {
        CAMetalLayer* l = (CAMetalLayer*)w->handle().apple.layer;
        l.device = _device;
        return make_object<surface_metal>(l);
    }

    void device_metal::destroy_shader_library(object_ptr<shader_library> l) {
        auto library = l.cast<shader_library_metal>();
        destroy_object(library);
    }

    void device_metal::destroy_render_pipeline(object_ptr<render_pipeline> p) {
        auto pipeline = p.cast<render_pipeline_metal>();
        destroy_object(pipeline);
    }

    void device_metal::destroy_buffer(object_ptr<buffer> b) {
        auto buf = b.cast<buffer_metal>();
        destroy_object(buf);
    }

    void device_metal::begin_render_pass(object_ptr<texture> t) {
        // todo
        _buffer = [_queue commandBuffer];

        auto _pass_desc = [MTLRenderPassDescriptor renderPassDescriptor];
        _pass_desc.colorAttachments[0].loadAction = MTLLoadActionClear;
        _pass_desc.colorAttachments[0].storeAction = MTLStoreActionStore;
        _pass_desc.colorAttachments[0].clearColor = MTLClearColorMake(0.0, 1.0, 1.0, 1.0);
        _pass_desc.colorAttachments[0].texture = t.cast<texture_metal>()->texture();
        _encoder = [_buffer renderCommandEncoderWithDescriptor:_pass_desc];
    }
    void device_metal::set_render_pipeline(object_ptr<render_pipeline> p) {
        [_encoder setRenderPipelineState:p.cast<render_pipeline_metal>()->pipeline()];
        // [_encoder setViewport:{0.0, 0.0, 500.0, 500.0, 0.0, 1.0}];
    }
    void device_metal::set_vertex_buffer(object_ptr<buffer> b, vertex_buffer_location const& loc) {
        [_encoder setVertexBuffer:b.cast<buffer_metal>()->buffer()
                           offset:0
                          atIndex:loc.apple.buffer_index];
    }
    void device_metal::set_viewport(viewport v) {
        [_encoder setViewport:{v.x, v.y, v.width, v.height, v.z_near, v.z_far}];
    }
    void device_metal::draw(int vertex_start, int vertex_count) {
        [_encoder drawPrimitives:MTLPrimitiveTypeTriangle
                     vertexStart:vertex_start
                     vertexCount:vertex_count];
    }
    void device_metal::draw(int vertex_start, int vertex_count, int instance_start,
                            int instance_count) {
        [_encoder drawPrimitives:MTLPrimitiveTypeTriangle
                     vertexStart:vertex_start
                     vertexCount:vertex_count
                   instanceCount:instance_count
                    baseInstance:instance_start];
    }
    void device_metal::end_render_pass() { [_encoder endEncoding]; }
    void device_metal::present(object_ptr<surface> s) {
        [_buffer presentDrawable:s.cast<surface_metal>()->drawable()];
        [_buffer commit];
        [_buffer waitUntilCompleted];
    }

    void device_metal::execute(task_graph& graph) {}

    shader_library_metal::shader_library_metal(dispatch_data_t data, id<MTLLibrary> library)
        : _data{data}, _library{library} {}

    shader_library_metal::~shader_library_metal() {
        [_library release];
        [_data release];
    }

    render_pipeline_metal::render_pipeline_metal(id<MTLFunction> vertex,
                                                 id<MTLRenderPipelineState> pipeline)
        : _vertex{vertex}, _pipeline{pipeline} {}

    render_pipeline_metal::~render_pipeline_metal() {
        [_pipeline release];
        [_vertex release];
    }

    buffer_metal::buffer_metal(id<MTLBuffer> b) : _buffer{b} {}

    buffer_metal::~buffer_metal() { [_buffer release]; }

}  // namespace spargel::gpu
