#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "raylib.h"
#include "rlgl.h"

#ifdef PLATFORM_WEB
#include <emscripten/emscripten.h>
#endif

#pragma comment(lib, "libraylib.a")

#ifdef PLATFORM_WIN32
#pragma comment(lib, "opengl32")
#pragma comment(lib, "Gdi32")
#pragma comment(lib, "WinMM")
#pragma comment(lib, "User32")
#pragma comment(lib, "shell32")
#endif

Camera3D cam = {0};

Vector3 player = {0.f, 20.f, 0.f};
Vector2 player_vel = {0.f, 0.f};
const float GRAVITY = 0.04f;
const float TERMINAL_VEL = 2.f;
bool gameover = false;

Vector3 pipe = {0.f, 0.f, -20.f};

unsigned int score = 0;
unsigned int high_score = 0;

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

static bool check_cube_collision(Vector3 c1, Vector3 c1s, Vector3 c2, Vector3 c2s)
{
    return (fabs(c1.x - c2.x) <= c1s.x/2.f + c2s.x/2.f &&
    fabs(c1.y - c2.y) <= c1s.y/2.f + c2s.y/2.f &&
    fabs(c1.z - c2.z) <= c1s.z/2.f + c2s.z/2.f);
}

static inline float frand(float a, float b)
{
    return (((float)rand() / (float)RAND_MAX) * (b-a)) + a;
}

static inline void randomize_pipe(float* pipe_x, float* pipe_y)
{
    *pipe_x = frand(-50.f, 50.f);
    *pipe_y = frand(5.f, 40.f);
}

void update()
{
    // handle input
    {
        if(IsKeyPressed(KEY_Z)) 
            player_vel.x = 0.8f;
        if(IsKeyPressed(KEY_SLASH)) 
            player_vel.y = 0.8f;
        if(IsKeyPressed(KEY_R))
        {
            gameover = false;
            player = {0.f, 20.f, 0.f};
            player_vel = {0.8f, 0.8f};
            pipe = {0.f, 0.f, -20.f};
            randomize_pipe(&pipe.x, &pipe.y);
            score = 0;
        }
    }

    // game logic
    {
        if(player_vel.x < -TERMINAL_VEL)
            player_vel.x = -TERMINAL_VEL;
        else
            player_vel.x -= GRAVITY;
        if(player_vel.y < -TERMINAL_VEL)
            player_vel.y = -TERMINAL_VEL;
        else
            player_vel.y -= GRAVITY;

        if(!gameover)
        {
            player.x += player_vel.x;
            player.y += player_vel.y;
            if(player.y <= 0.f+1.f)
            {
                player.y = 0.f+1.f;
                gameover = true;
            }
            if(check_cube_collision(player, {2.f, 2.f, 2.f}, {0.f, pipe.y-50.f, pipe.z}, {500.f, 100.f, 3.f}) ||
            (check_cube_collision(player, {2.f, 2.f, 2.f}, {0.f, pipe.y+50.f+11.f, pipe.z}, {500.f, 100.f, 3.f})) ||
            (check_cube_collision(player, {2.f, 2.f, 2.f}, {pipe.x+50.f+5.5f, 0.f, pipe.z}, {100.f, 500.f, 3.f})) ||
            (check_cube_collision(player, {2.f, 2.f, 2.f}, {pipe.x-50.f-5.5f, 0.f, pipe.z}, {100.f, 500.f, 3.f})))
            {
                gameover = true;
            }

            pipe.z += 0.1f;
            if(pipe.z >= 0.f)
            {
                score++;
                randomize_pipe(&pipe.x, &pipe.y);
                pipe.z = -20.f;
            }

            cam.position.x = player.x;
            cam.position.y = player.y;
            cam.target = player;
        }
        else
        {
            player_vel = {0.f, 0.f};
        }
    }

    // render
    BeginDrawing();
    {
        static int timer = 0; timer++;

        ClearBackground(SKYBLUE);

        BeginMode3D(cam);
        {
            DrawGrid(10, 10.f);

            DrawCube({0.f, pipe.y-50.f, pipe.z}, 500.f, 100.f, 3.f, GREEN);
            DrawCube({0.f, pipe.y+50.f+11.f, pipe.z}, 500.f, 100.f, 3.f, GREEN);

            DrawCube({pipe.x-50.f-5.5f, 0.f, pipe.z}, 100.f, 500.f, 3.f, GREEN);
            DrawCube({pipe.x+50.f+5.5f, 0.f, pipe.z}, 100.f, 500.f, 3.f, GREEN);

            DrawCube(player, 2.f, 2.f, 2.f, YELLOW);
            DrawCubeWires(player, 2.f, 2.f, 2.f, WHITE);
        }
        EndMode3D();

        char buf[32];
        snprintf(buf, 32, "SCORE: %u", score);
        DrawText(buf, 10, 10, 20, BLACK);

        snprintf(buf, 32, "HIGH SCORE: %u", high_score);
        DrawText(buf, 10, 30, 20, BLACK);

        DrawText("Z and SLASH to flap", 10, SCREEN_HEIGHT-20, 20, BLACK);

        if(gameover)
        {
            const char* gameover_text = "GAME OVER!";
            int gameover_text_length = MeasureText(gameover_text, 40);
            DrawText(gameover_text, SCREEN_WIDTH/2-gameover_text_length/2, SCREEN_HEIGHT/2-40.f/2, 40, BLACK);
            gameover_text = "R to restart";
            gameover_text_length = MeasureText(gameover_text, 20);
            DrawText(gameover_text, SCREEN_WIDTH/2-gameover_text_length/2, SCREEN_HEIGHT/2+30.f/2, 20, BLACK);
            if(score > high_score)
                high_score = score;
        }
    }
    EndDrawing();
}

int main(int argc, char* argv[])
{
    srand(time(0));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "3d flapy bird");

    cam.position = {0.f, 20.f, 30.f};
    cam.target = {0.f, 20.f, 0.f};
    cam.up = {1.f, 1.f, 0.f};
    cam.fovy = 80.f;
    cam.projection = CAMERA_PERSPECTIVE;

    randomize_pipe(&pipe.x, &pipe.y);

#ifdef PLATFORM_WEB
    emscripten_set_main_loop(update,0,1);
#else
    SetTargetFPS(60);
    while(!WindowShouldClose())
    {
        update();
    }
#endif
    
    CloseWindow();

    return 0;
}
