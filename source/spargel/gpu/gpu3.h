#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/meta.h>
#include <spargel/base/object.h>
#include <spargel/base/span.h>
#include <spargel/base/string_view.h>
#include <spargel/base/unique_ptr.h>
#include <spargel/base/vector.h>

namespace spargel::ui {
    class window;
}

namespace spargel::gpu {

    class task_graph;

    enum class texture_format {
        bgra8_unorm,
        bgra8_srgb,
    };

    enum class load_action {
        dont_care,
        load,
        clear,
    };

    enum class store_action {
        dont_care,
        store,
    };

    enum class device_kind {
        directx,
        metal,
        vulkan,
    };

    // notes:
    //
    // msl = metal shading language specification
    //
    // [msl, v3.2, p84]
    // an address space attribute specifies the region of memory from where buffer memory objects
    // are allocated. these attributes describe disjoint address spaces:
    // - device
    // - constant
    // - thread
    // - threadgroup
    // - threadgroup_imageblock
    // - ray_data
    // - object_data
    // all arguments to a graphics or kernel function that are a pointer or reference to a type
    // needs to be declared with an address space attribute.
    //
    // the address space for a variable at program scope needs to be `constant`.
    //
    // [msl, v3.2, p98]
    // arguments to graphics and kernel functions can be any of the following:
    // - device buffer: a pointer or reference to any data type in the device address space
    // - constant buffer: a pointer or reference to any data type in the constant address space
    // - ...
    //
    // [msl, v3.2, p99]
    // for each argument, an attribute can be optionally specified to identify the location of a
    // buffer, texture, or sampler to use for this argument type.
    // - device and constant buffers: `[[buffer(index)]]`
    // - textures (including texture buffers): `[[texture(index)]]`
    // - samplers: `[[sampler(index)]]`
    //

    /// @brief triangles with particular facing will not be drawn
    ///
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_cull_mode
    /// metal: https://developer.apple.com/documentation/metal/mtlcullmode
    /// vulkan: https://registry.khronos.org/vulkan/specs/latest/man/html/VkCullModeFlagBits.html
    ///
    enum class cull_mode {
        /// @brief no triangles are discarded
        none,
        /// @brief front-facing triangles are discarded
        front,
        /// @brief back-facing triangles are discarded
        back,
    };

    /// @brief the front-facing orientation
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_rasterizer_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlwinding
    /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkFrontFace.html
    ///
    enum class orientation {
        /// @brief clockwise triangles are front-facing
        clockwise,
        /// @brief counter-clockwise triangles are front-facing
        counter_clockwise,
    };

    /// @brief geometric primitive type for rendering
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_primitive_topology_type
    /// metal: https://developer.apple.com/documentation/metal/mtlprimitivetopologyclass
    /// vulkan: https://registry.khronos.org/vulkan/specs/latest/man/html/VkPrimitiveTopology.html
    ///
    enum class primitive_kind {
        point,
        line,
        triangle,
    };

    /// @brief the rate of fetching vertex attributes
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_input_classification
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexstepfunction
    /// vulkan:
    /// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputRate.html
    ///
    enum class vertex_step_mode {
        /// @brief attributes are fetched per vertex
        vertex,
        /// @brief attributes are fetched per instance
        instance,
    };

    enum class vertex_attribute_format {
        uint8,
        uint8x2,
        uint8x4,
        sint8,
        sint8x2,
        sint8x4,
        unorm8,
        unorm8x2,
        unorm8x4,
        snorm8,
        snorm8x2,
        snorm8x4,
        uint16,
        uint16x2,
        uint16x4,
        sint16,
        sint16x2,
        sint16x4,
        unorm16,
        unorm16x2,
        unorm16x4,
        snorm16,
        snorm16x2,
        snorm16x4,
        float16,
        float16x2,
        float16x4,
        float32,
        float32x2,
        float32x4,
    };

    enum class blend_action {
        min,
        max,
        add,
        subtract,
        reverse_subtract,
    };

    enum class blend_factor {
        zero,
        one,
        src_color,
        dst_color,
        one_minus_src_color,
        one_minus_dst_color,
        src_alpha,
        dst_alpha,
        one_minus_src_alpha,
        one_minus_dst_alpha,
    };

    enum class depth_compare {
        never,
        less,
        equal,
        less_equal,
        greater,
        not_equal,
        greater_equal,
        always,
    };

    template <typename T>
    class object_ptr {
    public:
        object_ptr() = default;
        object_ptr(nullptr_t) {}

        template <typename U>
            requires(base::is_convertible<U*, T*>)
        explicit object_ptr(U* ptr) : _ptr{ptr} {}

        template <typename U>
            requires(base::is_convertible<U*, T*>)
        object_ptr(object_ptr<U> const ptr) : _ptr{ptr.get()} {}

        T* operator->() { return _ptr; }
        T const* operator->() const { return _ptr; }

        T* get() const { return _ptr; }

        template <typename U>
        object_ptr<U> cast() {
            return object_ptr<U>(static_cast<U*>(_ptr));
        }
        template <typename U>
        object_ptr<U> const cast() const {
            return object_ptr<U>(static_cast<U*>(_ptr));
        }

    private:
        T* _ptr = nullptr;
    };

    template <typename T, typename... Args>
    object_ptr<T> make_object(Args&&... args) {
        T* ptr = static_cast<T*>(base::default_allocator()->alloc(sizeof(T)));
        base::construct_at(ptr, base::forward<Args>(args)...);
        return object_ptr<T>(ptr);
    }

    template <typename T>
    void destroy_object(object_ptr<T> ptr) {
        ptr->~T();
        base::default_allocator()->free(ptr.get(), sizeof(T));
    }

    class shader_library {};
    class render_pipeline {};
    class bind_group {};
    class bind_group_layout {};
    class buffer {};
    class texture {};
    class surface {
    public:
        virtual object_ptr<texture> next_texture() = 0;
        virtual float width() = 0;
        virtual float height() = 0;
    };

    struct shader_library_descriptor {
        base::span<u8> bytes;
    };

    struct shader_function {
        object_ptr<shader_library> library;
        char const* entry;
    };

    /// @brief
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_input_element_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexattributedescriptor
    /// vulkan:
    /// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputAttributeDescription.html
    ///
    struct vertex_attribute_descriptor {
        /// @brief index of the vertex buffer where the data of the attribute is fetched
        int buffer;

        /// @brief the location of the vertex attribute
        ///
        /// this is called input element in directx.
        ///
        /// synatx in shaders:
        /// - glsl: `layout (location = 0) in vec3 position;`
        /// - msl: `float3 position [[attribute(0)]];`
        /// - hlsl: this is specified via semantic name and semantic index.
        ///
        /// note:
        /// - both dawn and wgpu use dummy SemanticName, and use SemanticIndex for location.
        /// - dawn chooses "TEXCOORD", and wgpu/naga uses "LOC".
        /// - SPIRV-Cross uses "TEXCOORD" by default, and provides the options for remapping.
        /// - dxc relies on user declaration `[[vk::location(0)]]`.
        ///
        int location;

        /// @brief the format of the vertex attribute
        vertex_attribute_format format;

        /// @brief the offset of the vertex attribute to the start of the vertex data
        ssize offset;
    };

    /// @brief description of one vertex buffer
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_input_layout_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexdescriptor
    /// vulkan:
    /// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkPipelineVertexInputStateCreateInfo.html
    ///
    struct vertex_buffer_descriptor {
        /// @brief the number of bytes between consecutive elements in the buffer
        ///
        /// directx:
        /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_vertex_buffer_view
        /// metal:
        /// https://developer.apple.com/documentation/metal/mtlvertexbufferlayoutdescriptor/1515441-stride
        /// vulkan:
        /// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputBindingDescription.html
        ///
        ssize stride;

        /// @brief the rate of fetching vertex attributes
        vertex_step_mode step_mode;

        // TODO: step rate is not supported by vulkan.
    };

    /// @brief description of a render pipeline
    ///
    /// directx:
    /// https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_graphics_pipeline_state_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlrenderpipelinedescriptor
    /// vulkan:
    /// https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkGraphicsPipelineCreateInfo.html
    ///
    struct render_pipeline_descriptor {
        // the input-assembler stage

        /// @brief the geometric primitive for this render pipeline
        primitive_kind primitive;
        /// @brief the orientation of front-facing triangles
        orientation front_face;
        /// @brief which triangles to discard
        cull_mode cull;

        // the vertex stage

        /// @brief the vertex function
        shader_function vertex_shader;
        /// @brief the vertex buffers used in this pipeline
        base::span<vertex_buffer_descriptor> vertex_buffers;
        /// @brief the vertex attributes used in this pipeline
        base::span<vertex_attribute_descriptor> vertex_attributes;

        /// @brief the fragment function
        shader_function fragment_shader;
    };

    struct viewport {
        float x;
        float y;
        float width;
        float height;
        float z_near;
        float z_far;
    };

    struct vertex_buffer_location {
        struct {
            int buffer_index;
        } apple;
        struct {
            int vertex_buffer_index;
        } vulkan;
    };

    class task_graph {
    public:
        void add_render_task() {}
        void add_host_task() {}
        void add_present_task() {}
    };

    struct prepared_graph {
        enum class node_kind {
            render,
            present,
            texture,
            buffer,
        };
        struct node_info {
            node_info(node_kind k, u32 i, base::string_view n) : kind{k}, index{i}, name{n} {}

            node_kind kind;
            u32 index;
            base::string_view name;
            u32 refcount = 0;
            bool culled = false;
            bool target = false;
            base::vector<u32> inputs;
            base::vector<u32> outputs;
        };
        struct render_node {};
        struct present_node {};
        struct texture_node {};
        struct buffer_node {};
        base::vector<node_info> nodes;
        base::vector<render_node> renders;
        base::vector<present_node> presents;
        base::vector<texture_node> textures;
        base::vector<buffer_node> buffers;
    };

    class device {
    public:
        virtual ~device() = default;

        device_kind kind() const { return _kind; }

        virtual object_ptr<shader_library> make_shader_library(
            shader_library_descriptor const& descriptor) = 0;
        virtual object_ptr<render_pipeline> make_render_pipeline(
            render_pipeline_descriptor const& descriptor) = 0;
        virtual object_ptr<surface> make_surface(ui::window* w) = 0;
        virtual void destroy_shader_library(object_ptr<shader_library> library) = 0;
        virtual void destroy_render_pipeline(object_ptr<render_pipeline> pipeline) = 0;

        virtual object_ptr<buffer> make_buffer_with_bytes(base::span<u8> bytes) = 0;
        virtual void destroy_buffer(object_ptr<buffer> b) = 0;

        virtual void begin_render_pass(object_ptr<texture> t) = 0;
        virtual void set_render_pipeline(object_ptr<render_pipeline> p) = 0;
        virtual void set_vertex_buffer(object_ptr<buffer> b, vertex_buffer_location const& loc) = 0;
        virtual void set_viewport(viewport v) = 0;
        virtual void draw(int vertex_start, int vertex_count) = 0;
        virtual void draw(int vertex_start, int vertex_count, int instance_start,
                          int instance_count) = 0;
        virtual void end_render_pass() = 0;
        virtual void present(object_ptr<surface> s) = 0;

        virtual void execute(task_graph& graph) = 0;

    protected:
        explicit device(device_kind k) : _kind{k} {}

    private:
        device_kind _kind;
    };

    base::unique_ptr<device> make_device(device_kind kind);

}  // namespace spargel::gpu
