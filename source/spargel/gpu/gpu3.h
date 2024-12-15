#pragma once

#include <spargel/base/allocator.h>
#include <spargel/base/meta.h>
#include <spargel/base/object.h>
#include <spargel/base/span.h>
#include <spargel/base/unique_ptr.h>

namespace spargel::gpu {

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

    class device;
    struct texture;

    template <typename T>
    struct resource_handle {};

    using texture_handle = resource_handle<texture>;

    class render_encoder {
    public:
        void set_pipeline() {}
        void draw() {}
    };

    class render_task {
    private:
        class callback_shape {
        public:
            virtual ~callback_shape() = default;
            virtual void execute(render_encoder& encoder) = 0;
        };

        template <typename F>
        class callback final : public callback_shape {
        public:
            callback(F&& f) : _f(base::move(f)) {}
            ~callback() override = default;

            void execute(render_encoder& encoder) override { _f(encoder); }

        private:
            F _f;
        };

    public:
        void set_name(char const* name) {}

        void add_read(texture_handle handle) {}

        // write to color attachment
        void add_write(texture_handle handle, load_action load, store_action store) {}

        template <typename F>
        void set_execute(F&& f) {
            _execute = base::make_unique<callback<F>>(base::move(f));
        }

    private:
        base::unique_ptr<callback_shape> _execute;
    };

    class task_graph {
    public:
        explicit task_graph(device* d) {}

        texture_handle current_surface() { return {}; }
        render_task* add_render_task() { return nullptr; }
        void add_present_task(texture_handle handle) {}

        void execute() {}
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
    // an address space attribute specifies the region of memory from where buffer memory objects are allocated.
    // these attributes describe disjoint address spaces:
    // - device
    // - constant
    // - thread
    // - threadgroup
    // - threadgroup_imageblock
    // - ray_data
    // - object_data
    // all arguments to a graphics or kernel function that are a pointer or reference to a type needs to be declared with an address space attribute.
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
    // for each argument, an attribute can be optionally specified to identify the location of a buffer, texture, or sampler to use for this argument type.
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
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_rasterizer_desc
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
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_primitive_topology_type
    /// metal: https://developer.apple.com/documentation/metal/mtlprimitivetype
    /// vulkan: https://registry.khronos.org/vulkan/specs/latest/man/html/VkPrimitiveTopology.html
    ///
    enum class primitive_kind {
        point,
        line,
        triangle,
    };

    /// @brief the rate of fetching vertex attributes
    ///
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_input_classification
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexstepfunction
    /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputRate.html
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

    //     fragment: {
    //         function: fragment_shader,
    //         targets: [
    //             {
    //                 format: bgra8_unorm, // enum texture_format
    //                 write_mask: r | g | b | a, // bitmask write_mask
    //                 blend: {
    //                     color: {
    //                         operation: add, // enum blend_action
    //                         src_factor: one, // enum blend_factor
    //                         dst_factor: zero, // enum blend_factor
    //                     },
    //                     alpha: {
    //                         operation: add, // enum blend_action
    //                         src_factor: one, // enum blend_factor
    //                         dst_factor: zero, // enum blend_factor
    //                     },
    //                 },
    //             },
    //         ]
    //     },
    //     depth_stencil: {
    //         depth_compare: less, // enum depth_compare,
    //         depth_write_enable: true,
    //         // todo
    //     }
    // }
    //

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
        object_ptr(object_ptr<U> ptr) : _ptr{ptr.get()} {}

        T* operator->() { return _ptr; }
        T const* operator->() const { return _ptr; }

        T* get() { return _ptr; }

        template <typename U>
        object_ptr<U> cast() {
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

    struct shader_library_descriptor {
        ssize size;
        u8* bytes;
    };

    struct shader_function {
        object_ptr<shader_library> library;
        char const* entry;
    };

    /// @brief
    ///
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_input_element_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexattributedescriptor
    /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputAttributeDescription.html
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
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_input_layout_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlvertexdescriptor
    /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkPipelineVertexInputStateCreateInfo.html
    ///
    struct vertex_buffer_descriptor {
        /// @brief the number of bytes between consecutive elements in the buffer
        /// 
        /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_vertex_buffer_view
        /// metal: https://developer.apple.com/documentation/metal/mtlvertexbufferlayoutdescriptor/1515441-stride
        /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkVertexInputBindingDescription.html
        ///
        ssize stride;

        /// @brief the rate of fetching vertex attributes
        vertex_step_mode step_mode;

        // TODO: step rate is not supported by vulkan.
    };

    /// @brief description of a render pipeline
    ///
    /// directx: https://learn.microsoft.com/en-us/windows/win32/api/d3d12/ns-d3d12-d3d12_graphics_pipeline_state_desc
    /// metal: https://developer.apple.com/documentation/metal/mtlrenderpipelinedescriptor
    /// vulkan: https://registry.khronos.org/VulkanSC/specs/1.0-extensions/man/html/VkGraphicsPipelineCreateInfo.html
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

    class device {
    public:
        virtual ~device() = default;

        device_kind kind() const { return _kind; }

        virtual object_ptr<shader_library> make_shader_library(shader_library_descriptor const& descriptor) = 0;
        virtual object_ptr<render_pipeline> make_render_pipeline(render_pipeline_descriptor const& descriptor) = 0;

        virtual void destroy_shader_library(object_ptr<shader_library> library) = 0;

    protected:
        explicit device(device_kind k) : _kind{k} {}

    private:
        device_kind _kind;
    };

    base::unique_ptr<device> make_device(device_kind kind);

}  // namespace spargel::gpu
