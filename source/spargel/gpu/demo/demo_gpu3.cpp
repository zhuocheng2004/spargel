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
        .write(surface)
        .callback([=](render_encoder& encoder) {
            encoder.set_clear_color();
            encoder.set_pipeline();
            encoder.draw();
        });
    graph.add_present_task().texture(surface);
    graph.execute();
}

int main() { return 0; }
