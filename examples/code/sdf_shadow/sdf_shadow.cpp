#include "ces/ces_editor.h"
#include "ces/ces_resources.h"
#include "ces/ces_scene.h"
#include "ces/ces_utilities.h"
#include "volume_generator.h"

#include "camera.h"
#include "debug_render.h"
#include "dev_ui.h"
#include "loader.h"
#include "pmfx.h"

#include "file_system.h"
#include "hash.h"
#include "input.h"
#include "pen.h"
#include "pen_json.h"
#include "pen_string.h"
#include "renderer.h"
#include "str_utilities.h"
#include "timer.h"

#include "maths/vec.h"

using namespace put;
using namespace put::ces;

pen::window_creation_params pen_window{
    1280,                           // width
    720,                            // height
    4,                              // MSAA samples
    "signed_distance_field_shadows" // window title / process name
};

namespace physics
{
    extern PEN_TRV physics_thread_main(void* params);
}

vec3f random_vel(f32 min, f32 max)
{
    f32 x = min + (((f32)(rand() % RAND_MAX) / RAND_MAX) * (max - min));
    f32 y = min + (((f32)(rand() % RAND_MAX) / RAND_MAX) * (max - min));
    f32 z = min + (((f32)(rand() % RAND_MAX) / RAND_MAX) * (max - min));
    
    return vec3f(x, y, z);
}

void animate_lights(entity_scene* scene, f32 dt)
{
    if (scene->flags & PAUSE_UPDATE)
        return;

    extents e = scene->renderable_extents;
    e.min -= vec3f(10.0f, 2.0f, 10.0f);
    e.max += vec3f(10.0f, 10.0f, 10.0f);

    static f32 t = 0.0f;
    t += dt * 0.001f;

    static vec3f s_velocities[MAX_FORWARD_LIGHTS];
    static bool s_initialise = true;
    if (s_initialise)
    {
        s_initialise = false;
        srand(pen::get_time_us());

        for (u32 i = 0; i < MAX_FORWARD_LIGHTS; ++i)
            s_velocities[i] = random_vel(-1.0f, 1.0f);

        /*
        u32 vt = put::load_texture("C:/Users/alj/Desktop/vol512.dds");

        // create material for volume sdf sphere trace
        material_resource* sdf_material = new material_resource;
        sdf_material->material_name = "volume_sdf_material";
        sdf_material->shader_name = "pmfx_utility";
        sdf_material->id_shader = PEN_HASH("pmfx_utility");
        sdf_material->id_technique = PEN_HASH("volume_sdf");
        sdf_material->id_sampler_state[SN_VOLUME_TEXTURE] = PEN_HASH("clamp_linear_sampler_state");
        sdf_material->texture_handles[SN_VOLUME_TEXTURE]  = vt;
        add_material_resource(sdf_material);

        f32 single_scale = 20.4f;
        vec3f scale = vec3f(single_scale);

        u32 new_prim = get_new_node(scene);
        scene->names[new_prim] = "volume";
        scene->names[new_prim].appendf("%i", new_prim);
        scene->transforms[new_prim].rotation = quat();
        scene->transforms[new_prim].scale = scale;
        scene->transforms[new_prim].translation = vec3f::zero();
        scene->entities[new_prim] |= CMP_TRANSFORM | CMP_SDF_SHADOW;
        scene->parents[new_prim] = new_prim;
        instantiate_material(sdf_material, scene, new_prim);
        instantiate_model_cbuffer(scene, new_prim);
        */
    }

    u32 vel_index = 0;

    for (u32 n = 0; n < scene->num_nodes; ++n)
    {
        if (!(scene->entities[n] & CMP_LIGHT))
            continue;

        if (scene->lights[n].type == LIGHT_TYPE_DIR)
            continue;

        if (vel_index == 0)
        {
            f32 tx = sin(t);
            scene->transforms[n].translation = vec3f(tx * 20.0f, 2.0f, 15.0f);
            scene->entities[n] |= CMP_TRANSFORM;
        }

        if (vel_index == 1)
        {
            f32 tz = cos(t);
            scene->transforms[n].translation = vec3f(15.0f, 3.0f, tz * 20.0f);
            scene->entities[n] |= CMP_TRANSFORM;
        }

        if (vel_index == 2)
        {
            f32 tx = sin(t*0.5);
            f32 tz = cos(t*0.5);
            scene->transforms[n].translation = vec3f(tx * 40.0f, 1.0f, tz * 30.0f);
            scene->entities[n] |= CMP_TRANSFORM;
        }

        if (vel_index == 3)
        {
            f32 tx = cos(t*0.25);
            f32 tz = sin(t*0.25);
            scene->transforms[n].translation = vec3f(tx * 30.0f, 6.0f, tz * 30.0f);
            scene->entities[n] |= CMP_TRANSFORM;
        }

        /*
        vec3f t = scene->world_matrices[n].get_translation();
        if (!maths::point_inside_aabb(e.min, e.max, t))
        {
            vec3f cp = maths::closest_point_on_aabb(t, e.min, e.max);

            scene->transforms[n].translation = cp;

            vec3f v = normalised(cp - t);

            s_velocities[vel_index] = v + random_vel(-0.5f, 0.5f);
        }

        scene->transforms[n].translation += s_velocities[vel_index] * dt * 0.01f;
        scene->entities[n] |= CMP_TRANSFORM;
        */


        ++vel_index;
    }
}

PEN_TRV pen::user_entry(void* params)
{
    // unpack the params passed to the thread and signal to the engine it ok to proceed
    pen::job_thread_params* job_params    = (pen::job_thread_params*)params;
    pen::job*               p_thread_info = job_params->job_info;
    pen::thread_semaphore_signal(p_thread_info->p_sem_continue, 1);

    pen::thread_create_job(physics::physics_thread_main, 1024 * 10, nullptr, pen::THREAD_START_DETACHED);

    put::dev_ui::init();
    put::dbg::init();

    // create main camera and controller
    put::camera main_camera;
    put::camera_create_perspective(&main_camera, 60.0f, (f32)pen_window.width / (f32)pen_window.height, 0.1f, 1000.0f);

    put::scene_controller cc;
    cc.camera          = &main_camera;
    cc.update_function = &ces::update_model_viewer_camera;
    cc.name            = "model_viewer_camera";
    cc.id_name         = PEN_HASH(cc.name.c_str());

    // create the main scene and controller
    put::ces::entity_scene* main_scene;
    main_scene = put::ces::create_scene("main_scene");
    put::ces::editor_init(main_scene);

    put::scene_controller sc;
    sc.scene           = main_scene;
    sc.update_function = &ces::update_model_viewer_scene;
    sc.name            = "main_scene";
    sc.camera          = &main_camera;
    sc.id_name         = PEN_HASH(sc.name.c_str());

    // create view renderers
    put::scene_view_renderer svr_main;
    svr_main.name            = "ces_render_scene";
    svr_main.id_name         = PEN_HASH(svr_main.name.c_str());
    svr_main.render_function = &ces::render_scene_view;

    put::scene_view_renderer svr_editor;
    svr_editor.name            = "ces_render_editor";
    svr_editor.id_name         = PEN_HASH(svr_editor.name.c_str());
    svr_editor.render_function = &ces::render_scene_editor;

    pmfx::register_scene_view_renderer(svr_main);
    pmfx::register_scene_view_renderer(svr_editor);

    pmfx::register_scene_controller(sc);
    pmfx::register_scene_controller(cc);

    // volume rasteriser tool
    put::vgt::init(main_scene);

    pmfx::init("data/configs/editor_renderer.json");

    bool enable_dev_ui = true;

    f32 frame_time = 0.0f;

    while (1)
    {
        static u32 frame_timer = pen::timer_create("frame_timer");
        pen::timer_start(frame_timer);

        put::dev_ui::new_frame();

        pmfx::update();
        animate_lights(main_scene, frame_time);

        pmfx::render();

        pmfx::show_dev_ui();

        put::vgt::show_dev_ui();

        if (enable_dev_ui)
        {
            put::dev_ui::console();
            put::dev_ui::render();
        }

        if (pen::input_is_key_held(PK_MENU) && pen::input_is_key_pressed(PK_D))
            enable_dev_ui = !enable_dev_ui;

        frame_time = pen::timer_elapsed_ms(frame_timer);

        pen::renderer_present();
        pen::renderer_consume_cmd_buffer();

        put::vgt::post_update();
        pmfx::poll_for_changes();
        put::poll_hot_loader();

        // msg from the engine we want to terminate
        if (pen::thread_semaphore_try_wait(p_thread_info->p_sem_exit))
            break;
    }

    ces::destroy_scene(main_scene);
    ces::editor_shutdown();

    // clean up mem here
    put::pmfx::shutdown();
    put::dbg::shutdown();
    put::dev_ui::shutdown();

    pen::renderer_consume_cmd_buffer();

    // signal to the engine the thread has finished
    pen::thread_semaphore_signal(p_thread_info->p_sem_terminated, 1);

    return PEN_THREAD_OK;
}