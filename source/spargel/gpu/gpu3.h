#pragma once

#include <spargel/base/meta.h>
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

    enum class cull_mode {
        none,
        front,
        back,
    };

    enum class front_face {
        clockwise,
        counterclockwise,
    };

    enum class primitive_topology {
        point,
        line,
        triangle,
    };

    enum class vertex_step_mode {
        vertex,
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

    // example:
    //
    // struct vertex {
    //   float16x2 position;
    //   float16x2 normal;
    // }
    // pipeline = {
    //     primitive: {
    //         cull_mode: back, // enum cull_mode
    //         front_face: clockwise, // enum front_face
    //         topology: triangle, // enum primitive
    //     },
    //     vertex: {
    //         function: vertex_shader,
    //         buffers: [
    //             {
    //                 stride: sizeof(struct vertex),
    //                 step_mode: vertex, // enum vertex_step_mode
    //                 attributes: [
    //                     {
    //                         format: float16x2, // enum vertex_attribute_format
    //                         offset: offsetof(struct vertex, position),
    //                     },
    //                     {
    //                         format: float16x2, //enum vertex_attribute_format
    //                         offset: offsetof(struct vertex, normal),
    //                     },
    //                 ]
    //             }
    //         ]
    //     },
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
    struct pipeline_descriptor {
    };

    class device {
    public:
        virtual ~device() = default;

        device_kind kind() const { return _kind; }

        virtual void make_pipeline(pipeline_descriptor const& descriptor) = 0;

    protected:
        explicit device(device_kind k) : _kind{k} {}

    private:
        device_kind _kind;
    };

    base::unique_ptr<device> make_device(device_kind kind);

}  // namespace spargel::gpu
