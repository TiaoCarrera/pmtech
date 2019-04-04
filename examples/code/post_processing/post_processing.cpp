#if 0
#include "../example_common.h"

using namespace put;
using namespace put::ecs;

pen::window_creation_params pen_window{
    1280,             // width
    720,              // height
    4,                // MSAA samples
    "post_processing" // window title / process name
};

camera pp_camera;

void example_setup(ecs::ecs_scene* scene, camera& cam)
{
    put::camera_create_perspective(&pp_camera, 60.0f, put::k_use_window_aspect, 0.1f, 1000.0f);
    pp_camera.name = "pp_camera";
    
    pmfx::register_camera(&pp_camera, "pp_camera");
    
    pmfx::init("data/configs/pp_demo.jsn");
}

void example_update(ecs::ecs_scene* scene, camera& cam, f32 dt)
{
    // animate camera
    static bool start = true;
    
    if (start)
    {
        pp_camera.pos = vec3f(0.0f, 0.0f, 0.0f);
        start = false;
    }
    
    pp_camera.pos += vec3f::unit_x();
    
    pp_camera.view.set_row(2, vec4f(0.0f, 0.0f, 1.0f, pp_camera.pos.x));
    pp_camera.view.set_row(1, vec4f(0.0f, 1.0f, 0.0f, pp_camera.pos.y));
    pp_camera.view.set_row(0, vec4f(1.0f, 0.0f, 0.0f, pp_camera.pos.z));
    pp_camera.view.set_row(3, vec4f(0.0f, 0.0f, 0.0f, 1.0f));
    
    pp_camera.flags |= CF_INVALIDATED;
    
    cam = pp_camera;
}

#else
#include "ecs/ecs_editor.h"
#include "ecs/ecs_resources.h"
#include "ecs/ecs_scene.h"
#include "ecs/ecs_utilities.h"
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
using namespace put::ecs;

pen::window_creation_params pen_window{
    1280,             // width
    720,              // height
    4,                // MSAA samples
    "post_processing" // window title / process name
};

namespace physics
{
    extern PEN_TRV physics_thread_main(void* params);
}

PEN_TRV pen::user_entry(void* params)
{
    // unpack the params passed to the thread and signal to the engine it ok to proceed
    pen::job_thread_params* job_params = (pen::job_thread_params*)params;
    pen::job*               p_thread_info = job_params->job_info;
    pen::semaphore_post(p_thread_info->p_sem_continue, 1);

    pen::jobs_create_job(physics::physics_thread_main, 1024 * 10, nullptr, pen::THREAD_START_DETACHED);

    put::dev_ui::init();
    put::dbg::init();

    // create main camera and controller
    put::camera main_camera;
    put::camera_create_perspective(&main_camera, 60.0f, put::k_use_window_aspect, 0.1f, 1000.0f);

    put::scene_controller cc;
    cc.camera = &main_camera;
    // cc.update_function = &ces::update_model_viewer_camera;
    cc.update_function = nullptr;
    cc.name = "model_viewer_camera";
    cc.id_name = PEN_HASH(cc.name.c_str());

    // create the main scene and controller
    put::ecs::ecs_scene* main_scene;
    main_scene = put::ecs::create_scene("main_scene");
    put::ecs::editor_init(main_scene);

    put::scene_controller sc;
    sc.scene = main_scene;
    sc.update_function = &ecs::update_model_viewer_scene;
    sc.name = "main_scene";
    sc.camera = &main_camera;
    sc.id_name = PEN_HASH(sc.name.c_str());

    // create view renderers
    put::scene_view_renderer svr_main;
    svr_main.name = "ces_render_scene";
    svr_main.id_name = PEN_HASH(svr_main.name.c_str());
    svr_main.render_function = &ecs::render_scene_view;

    put::scene_view_renderer svr_editor;
    svr_editor.name = "ces_render_editor";
    svr_editor.id_name = PEN_HASH(svr_editor.name.c_str());
    svr_editor.render_function = &ecs::render_scene_editor;

    pmfx::register_scene_view_renderer(svr_main);
    pmfx::register_scene_view_renderer(svr_editor);

    pmfx::register_scene_controller(sc);
    pmfx::register_scene_controller(cc);

    pmfx::init("data/configs/pp_demo.jsn");

    f32 frame_time = 0.0f;

    while (1)
    {
        static u32 frame_timer = pen::timer_create("frame_timer");
        pen::timer_start(frame_timer);

        put::dev_ui::new_frame();

        pmfx::update();

        // animate camera
        static bool start = true;

        if (start)
        {
            main_camera.pos = vec3f(0.0f, 0.0f, 0.0f);
            start = false;
        }

        main_camera.pos += vec3f::unit_x();

        main_camera.view.set_row(2, vec4f(0.0f, 0.0f, 1.0f, main_camera.pos.x));
        main_camera.view.set_row(1, vec4f(0.0f, 1.0f, 0.0f, main_camera.pos.y));
        main_camera.view.set_row(0, vec4f(1.0f, 0.0f, 0.0f, main_camera.pos.z));
        main_camera.view.set_row(3, vec4f(0.0f, 0.0f, 0.0f, 1.0f));

        main_camera.flags |= CF_INVALIDATED;

        pmfx::render();

        pmfx::show_dev_ui();

        put::vgt::show_dev_ui();

        put::dev_ui::render();

        frame_time = pen::timer_elapsed_ms(frame_timer);

        pen::renderer_present();

        // for unit test
        pen::renderer_test_run();

        pen::renderer_consume_cmd_buffer();

        put::vgt::post_update();
        pmfx::poll_for_changes();
        put::poll_hot_loader();

        // msg from the engine we want to terminate
        if (pen::semaphore_try_wait(p_thread_info->p_sem_exit))
            break;
    }

    ecs::destroy_scene(main_scene);
    ecs::editor_shutdown();

    // clean up mem here
    put::pmfx::shutdown();
    put::dbg::shutdown();
    put::dev_ui::shutdown();

    pen::renderer_consume_cmd_buffer();

    // signal to the engine the thread has finished
    pen::semaphore_post(p_thread_info->p_sem_terminated, 1);

    return PEN_THREAD_OK;
}
#endif
