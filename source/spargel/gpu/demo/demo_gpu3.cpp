#include <spargel/gpu/gpu3.h>
#include <spargel/ui/ui.h>

using namespace spargel::gpu;

struct render_data {
    device* dev;
};

void render(void* data) {
    render_data* rdata = static_cast<render_data*>(data);
    device* dev = rdata->dev;
    task_graph graph(dev);
    auto surface = graph.current_surface();
    graph.add_render_task()
        .set_name("triangle_pass")
        .set_pipeline()
        .write(surface)
        .callback([=](render_encoder& encoder) {
            encoder.set_clear_color();
            encoder.draw();
        });
    graph.add_present_task().texture(surface);
    graph.execute();
}

int main() {
    spargel::ui::init_platform();
    spargel::ui::window_id window = spargel::ui::create_window(500, 500);
    spargel::ui::window_set_title(window, "Spargel Engine - GPU Task Graph");

    render_data d;
    window_set_render_callback(window, render, &d);

    return 0;
}
