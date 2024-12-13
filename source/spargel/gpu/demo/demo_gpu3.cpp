#include <spargel/gpu/gpu3.h>
#include <spargel/ui/ui.h>

using namespace spargel::gpu;

struct render_data {
    device* dev;
};

struct my_pass final : render_task {
    void configure() override {}
    void execute(render_encoder& encoder) override {
        encoder.set_pipeline();
        encoder.draw();
    }
};

void render(void* data) {
    render_data* rdata = static_cast<render_data*>(data);
    device* dev = rdata->dev;
    task_graph graph(dev);
    auto surface = graph.current_surface();
    graph.add_render_task<my_pass>();
    graph.add_present_task(surface);
    graph.execute();
}

int main() { return 0; }
