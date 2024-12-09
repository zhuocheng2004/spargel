#include <spargel/gpu/gpu2.h>
#include <spargel/ui/ui.h>

#define USE_VULKAN 1
#if SPARGEL_IS_MACOS
#define USE_METAL 1
#else
#define USE_METAL 0
#endif

#if USE_METAL
#include <spargel/gpu/demo/shader.metallib.inc>
#endif

#if USE_VULKAN
#include <spargel/gpu/demo/fragment_shader.spirv.inc>
#include <spargel/gpu/demo/vertex_shader.spirv.inc>
#endif

struct renderer {
    sgpu2_device_id device;
    sgpu2_surface_id surface;

    /* shaders */
    sgpu2_shader_id shadow_vertex;

    sgpu2_buffer_id vertices;
    sgpu2_texture_id base_color;
    sgpu2_texture_id normal;
    sgpu2_texture_id specular;

    int frame;
};

static void render(struct renderer* r) {
    sgpu2_device_id device = r->device;
    sgpu2_surface_id surface = r->surface;

    sgpu2_prepare(device);

    sgpu2_texture_id shadow_map =
        sgpu2_declare_texture(device, &(struct sgpu2_texture_descriptor){});
    sgpu2_texture_id albedo = sgpu2_declare_texture(device, &(struct sgpu2_texture_descriptor){});
    sgpu2_texture_id normal = sgpu2_declare_texture(device, &(struct sgpu2_texture_descriptor){});
    sgpu2_texture_id depth = sgpu2_declare_texture(device, &(struct sgpu2_texture_descriptor){});

    /* shadow map pass */
    sgpu2_render_task_id shadow_map_pass = sgpu2_declare_render_task(device);
    sgpu2_declare_texture_write(device, shadow_map_pass, shadow_map);
    sgpu2_set_depth_enabled(device, shadow_map_pass, true);
    sgpu2_set_depth_compare(device, shadow_map_pass, SGPU2_DEPTH_LESS_EQUAL);
    sgpu2_set_cull_mode(device, shadow_map_pass, SGPU2_CULL_BACK);
    sgpu2_set_vertex_shader(device, shadow_map_pass, r->shadow_vertex);
    sgpu2_set_vertex_buffer(device, shadow_map_pass, 0, r->vertices);
    sgpu2_draw_indexed(device, shadow_map_pass, 5, 10);
    /* ... */

    sgpu2_render_task_id gbuffer_pass = sgpu2_declare_render_task(device);
    sgpu2_declare_texture_write(device, gbuffer_pass, albedo);
    sgpu2_declare_texture_write(device, gbuffer_pass, normal);
    sgpu2_declare_texture_write(device, gbuffer_pass, depth);
    sgpu2_declare_texture_read(device, gbuffer_pass, r->base_color);
    sgpu2_declare_texture_read(device, gbuffer_pass, r->normal);
    sgpu2_declare_texture_read(device, gbuffer_pass, r->specular);
    /* sgpu2_set_*** */
    /* sgpu2_draw_indexed(...) */

    sgpu2_render_task_id directional_light_pass = sgpu2_declare_render_task(device);
    sgpu2_declare_texture_read(device, directional_light_pass, albedo);
    sgpu2_declare_texture_read(device, directional_light_pass, normal);
    sgpu2_declare_texture_read(device, directional_light_pass, depth);
    /* ... */

    sgpu2_render_task_id point_light_pass = sgpu2_declare_render_task(device);
    sgpu2_render_task_id sky_pass = sgpu2_declare_render_task(device);
    sgpu2_render_task_id particle_pass = sgpu2_declare_render_task(device);

    sgpu2_declare_present_task(device, surface);

    sgpu2_execute(device);

    r->frame++;
}

static int create(int backend, char const* title, struct renderer* r) {
    sui_window_id window = sui_create_window(500, 500);
    sui_window_set_title(window, title);

    sgpu2_device_id device = sgpu2_create_device(backend);
    sgpu2_surface_id surface = sgpu2_create_surface(device, window);

    r->device = device;
    r->surface = surface;
    r->frame = 0;

    sui_window_set_render_callback(window, (void (*)(void*))render, r);

    return 0;
}

int main() {
    sui_init_platform();

#if USE_VULKAN
    struct renderer r_vk;
    if (create(SGPU2_BACKEND_VULKAN, "Spargel Demo - Vulkan", &r_vk) != 0) return 1;
#endif
#if USE_METAL
    struct renderer r_mtl;
    if (create(SGPU2_BACKEND_VULKAN, "Spargel Demo - Metal", &r_mtl) != 0) return 1;
#endif

    sui_platform_run();
    return 0;
}
