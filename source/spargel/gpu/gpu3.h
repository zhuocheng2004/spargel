#pragma once

namespace spargel::gpu {

    using texture_handle = int;

    class render_task_builder {
    public:
        render_task_builder& set_name(char const* name);
        render_task_builder& read(texture_handle handle);
        // write to color attachment
        render_task_builder& write(texture_handle handle);
        render_task_builder& depth_stencil(texture_handle handle);
    };

    class task_graph {};

}  // namespace spargel::gpu
