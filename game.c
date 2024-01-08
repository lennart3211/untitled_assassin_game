#include "raylib.h"

#include <stdio.h>

#define NUM_WALLS 5
#define MAP_WIDTH  3508
#define MAP_HEIGHT 2480
#define ZOOM_IN 1.0f
#define ZOOM_OUT 4.0f
#define ZOOM_COOLDOWN 500

const float VIS_RADIUS = 500.0f;

Texture2D mapTexture; 
Texture2D fogTexture;
RenderTexture2D fogOfWarTexture;

typedef struct {
    int x;
    int y;
    int width;
    int height;
    int alertLevel;
} AlertIndicator;

typedef struct {
    Vector2 position;
    float speed;
    int radius;
    int isZoomedIn;
} Player;

Rectangle map[NUM_WALLS] = {
    { 200, 400, 200, 20 },
    { 500, 200, 20, 200 },
    { 600, 100, 150, 20 },
    { 300, 300, 20, 150 },
    { 100, 50, 200, 20 }
};

void DrawIndicator(AlertIndicator *indicator) {
    if (indicator->alertLevel == 0) return;

    DrawRectangle(indicator->x, indicator->y, indicator->width, indicator->height, RED);
}

int isColliding(Player *player) {
    for (int i = 0; i < NUM_WALLS; ++i) {
        if (CheckCollisionCircleRec(player->position, player->radius, map[i])) {
            return 1;
        }
    }
    return 0;
}

int main(void) {

    const int screenWidth = 1920;
    const int screenHeight = 1080;

    unsigned int lastZoomTime = 0;

    InitWindow(screenWidth, screenHeight, "Untitled Assassin Game");

    Player player;
    player.position = (Vector2) { screenWidth / 2.0f, screenHeight / 2.0f };
    player.speed = 2.0f;
    player.radius = 20;
    player.isZoomedIn = 1;

    AlertIndicator indicator;
    indicator.x = 20;
    indicator.y = 20;
    indicator.height = 40;
    indicator.width = 200;
    indicator.alertLevel = 1;

    mapTexture = LoadTexture("./image/game_actual.png");
    fogTexture = LoadTexture("./image/game_map.png");
    Shader shader = LoadShader(0, "./shader/fog.glsl");

    int playerPosLoc = GetShaderLocation(shader, "playerPos");
    int screenSizeLoc = GetShaderLocation(shader, "screenSize");
    int clearRadiusLoc = GetShaderLocation(shader, "clearRadius");

    Camera2D camera = { 0 };
    camera.target = player.position;
    camera.offset = (Vector2){ screenWidth / 2.0f, screenHeight / 2.0f };
    camera.rotation = 0.0f;
    camera.zoom = ZOOM_IN;

    SetTargetFPS(60);
    SetExitKey(KEY_L);

    while (!WindowShouldClose()) {
        Vector2 oldPosition = player.position;

        if (IsKeyDown(KEY_A)) player.position.x -= player.speed;
        if (IsKeyDown(KEY_D)) player.position.x += player.speed;
        if (IsKeyDown(KEY_W)) player.position.y -= player.speed;
        if (IsKeyDown(KEY_S)) player.position.y += player.speed;
        
        if (IsKeyPressed(KEY_M)) {
            unsigned int currentTime = GetTime() * 1000;
            if (currentTime - lastZoomTime >= ZOOM_COOLDOWN) {
                if (player.isZoomedIn) {
                    camera.zoom = ZOOM_OUT;
                    player.isZoomedIn = 0;
                } else {
                    camera.zoom = ZOOM_IN;
                    player.isZoomedIn = 1;
                }
                lastZoomTime = currentTime;
            }
        }

        camera.target = player.position;

        SetShaderValue(shader, playerPosLoc, &player.position, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, screenSizeLoc, &(Vector2){MAP_WIDTH, MAP_HEIGHT}, SHADER_UNIFORM_VEC2);
        SetShaderValue(shader, clearRadiusLoc, &VIS_RADIUS, SHADER_UNIFORM_FLOAT);
        
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode2D(camera);
                DrawTexture(mapTexture, 0, 0, WHITE);

                BeginShaderMode(shader);
                    DrawTexture(fogTexture, 0, 0, WHITE);
                EndShaderMode();

                DrawCircleV(player.position, player.radius, MAROON);
            EndMode2D();

            DrawIndicator(&indicator);
        EndDrawing();
    }
    UnloadShader(shader);
    UnloadTexture(mapTexture);
    UnloadTexture(fogTexture);
    CloseWindow();

    return 0;
}

