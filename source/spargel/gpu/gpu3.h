#pragma once

#include <spargel/base/unique_ptr.h>

namespace spargel::gpu {

    enum class texture_format {
        bgra8_unorm,
        bgra8_srgb,
    };

    class device;
    struct texture;

    template <typename T>
    struct resource_handle {};

    using texture_handle = resource_handle<texture>;

    class render_encoder {
    public:
        void set_clear_color();
        void set_pipeline();
        void draw();
    };

    class render_task {
    public:
        render_task& set_name(char const* name) { return *this; }
        render_task& read(texture_handle handle) { return *this; }
        // write to color attachment
        render_task& write(texture_handle handle) { return *this; }
        template <typename F>
        render_task& callback(F&& f) {
            return *this;
        }
    };

    class present_task {
    public:
        void texture(texture_handle handle);
    };

    class task_graph {
    public:
        explicit task_graph(device* d) {}

        texture_handle current_surface();
        render_task& add_render_task();
        present_task& add_present_task();

        void execute() {}
    };

    class device {
    public:
        virtual ~device() = default;
    };

    base::unique_ptr<device> make_device();

}  // namespace spargel::gpu
