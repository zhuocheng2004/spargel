#pragma once

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
    public:
        void set_name(char const* name) {}
        void read(texture_handle handle) {}
        // write to color attachment
        void write(texture_handle handle, load_action load, store_action store) {}

        virtual void configure() {}
        virtual void execute(render_encoder& encoder) {}
    };

    class task_graph {
    public:
        explicit task_graph(device* d) {}

        texture_handle current_surface() { return {}; }
        template <typename T, typename... Args>
        void add_render_task(Args&&... args) {}
        void add_present_task(texture_handle handle) {}

        void execute() {}
    };

    enum class device_kind {
        directx,
        metal,
        vulkan,
    };

    class device {
    public:
        virtual ~device() = default;

        device_kind kind() const { return _kind; }

        virtual void make_pipeline() = 0;

    protected:
        explicit device(device_kind k) : _kind{k} {}

    private:
        device_kind _kind;
    };

    base::unique_ptr<device> make_device(device_kind kind);

}  // namespace spargel::gpu
