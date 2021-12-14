#include "raylib.h"
#include "raymath.h"
#include "glad.h"

#include "forgelight_lib.h"
#include "internal.h"



int
main(void)
    {
    //================================================================
    // Initialization
    const int screen_width = 800;
    const int screen_height = 450;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screen_width, screen_height, "Rhett breaking things");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 2.0f, 1.4f, 1.7f };  // Camera position
    camera.target = (Vector3){ -1.4f, -0.5f, -1.6f }; // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        // Camera up vector (rotation towards target)
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera mode type

    // NOTE(rhett): Load materials_3.xml
    u8* materials_file_buffer = calloc(FM_MATERIALS_FILE_SIZE, 1);
    fm_parse_materials_file(materials_file_buffer, FM_MATERIALS_FILE_SIZE);

    // NOTE(rhett): Setup sample model
    Model model_obj = LoadModel("../resources/Armor_Common_Male_Skins_Base_lod0.obj");
    Model model_dme = fm_dme_load_from_file("../resources/Armor_Common_Male_Skins_Base_LOD0.dme");

    model_obj.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("../resources/Armor_Common_Shared_All_C.png");
    model_dme.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = LoadTexture("../resources/Armor_Common_Shared_All_C.png");

    model_dme.materials[0].shader = LoadShader("../resources/base.vs", "../resources/base.fs");

    SetCameraMode(camera, CAMERA_FREE); // Set a free camera mode
    SetTargetFPS(60);                   // Set our game to run at 60 frames-per-second
    //================================================================

    //================================================================
    // Main game loop
    while (!WindowShouldClose())        // Detect window close button or ESC key
    {
        // Update
        UpdateCamera(&camera);          // Update camera

        // Draw
        BeginDrawing();

            ClearBackground(DARKGRAY);

            BeginMode3D(camera);

                // NOTE(rhett): DME stores as CW
                glFrontFace(GL_CW);
                DrawModel(model_dme, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
                glFrontFace(GL_CCW);
                DrawSphereWires((Vector3){ 0.0f, 0.0f, 0.0f }, 0.1f, 4, 4, RED);

                DrawModel(model_obj, (Vector3){ 1.0f, 0.0f, -1.0f }, 1.0f, GRAY);
                DrawSphereWires((Vector3){ 1.0f, 0.0f, -1.0f }, 0.1f, 4, 4, GREEN);

                DrawGrid(10, 1.0f);

            EndMode3D();

            DrawFPS(10, 10);

        EndDrawing();
    }

    // De-Initialization
    UnloadModel(model_obj);
    CloseWindow();        // Close window and OpenGL context
    //================================================================

    return 0;
    }
