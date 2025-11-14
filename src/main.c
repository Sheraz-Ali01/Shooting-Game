#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ScreenWidth 1200
#define ScreenHeight 800
#define MapHeight 3000
#define MapWidth 3000
#define MaxEnemies 50
#define MaxBullets 200 
#define MaxTree 150
#define LevelEnimies 12
#define PlayerSpeed 300.0f




typedef struct 
{
    Vector2 postion;
    Vector2 speed;
    float health;
    int maxHealth;
    float radius;
    Vector2 shootDir;

} Player;

typedef struct 
{
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool alive;
    Color color;
}Enemy;

typedef struct
{
    Vector2 position;
    Vector2 direction;
    bool active;
    float time;
}Bullets ;

typedef struct {
    Vector2 position;
    float radius;
    Color color; 
}Tree;





int main(){

    InitWindow(ScreenWidth, ScreenHeight, "Servival Game");
    SetTargetFPS(60);

    srand(time(NULL));

    //Game setup
        //player
        Player player = {0};
        player.postion= (Vector2){MapWidth,MapHeight};
        player.speed = (Vector2){0,0};
        player.health=100;
        player.maxHealth=100;
        player.radius=18;
        //enemies
        Enemy enemy[MaxEnemies]={0};
        //bullets
        Bullets bullet[MaxBullets]={0};
        //screen camera according to player movement
        Camera2D camera={0};
        camera.target=player.postion;
        camera.offset = (Vector2){ScreenWidth/2.0f, ScreenHeight/2.0f};
        camera.zoom=1.0f;

        //Trees (Backgrund) Using this for tracking motion of player and will appear rendomly
        Tree tree[MaxTree];
        for(int i=0; i<MaxTree; i++){
            tree[i].position=(Vector2){(float)(rand()%MapWidth), (float)(rand()%MapHeight)};
            tree[i].radius=(25+ (rand()&30));
            tree[i].color=(Color){20+(rand()%60), 60+(rand()%60),20,255};
        }

        //Games Main variables 
        int level=1;          
        int EnemiesKilled=0;
        int EnemiesToKill=LevelEnimies;
        float EnemySpwanTimer=0;    //Enemy spwan delay;
        bool paused=false;
        bool gameOver=false;
        bool CanShoot=true;

    //Game initialization
    while(!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        if(!paused && !gameOver){

                // /// STEP 1: Input (THIS CODE)
                // Vector2 moveDir = {0, 0};
                // if (IsKeyDown(KEY_W)) moveDir.y -= 1;  // moveDir = (0, -1)

                // // STEP 2: Normalize (if length > 0)
                // moveDir = Vector2Normalize(moveDir);   // Still (0, -1) [length was 1]

                // // STEP 3: Set Velocity  
                // player.velocity = Vector2Scale(moveDir, PLAYER_SPEED);  
                // // velocity = (0, -1) * 300 = (0, -300)  ← 300 pixels/SECOND UP!

                // // STEP 4: UPDATE POSITION (NEXT in code!)
                // player.position.x += player.velocity.x * deltaTime;  
                // player.position.y += player.velocity.y * deltaTime;  
                // // position.y += -300 * 0.0167 = -5 pixels!  ← MOVES UP!
            Vector2 moveDir={0,0};
            if(IsKeyDown(KEY_W)){
                moveDir.y-=1;
            }
            if(IsKeyDown(KEY_S)){
                moveDir.y+=1;
            }
            if(IsKeyDown(KEY_A)){
                moveDir.x-=1;
            }
            if(IsKeyDown(KEY_D)){
                moveDir.x+=1;
            }
            if(Vector2lenght(moveDir)>0){
                moveDir=Vector2Normalize(moveDir);
            }
            
            BeginDrawing();
            ClearBackground(BLACK);
            EndDrawing();
            
        }






    }










    
    CloseWindow();


    
    
}