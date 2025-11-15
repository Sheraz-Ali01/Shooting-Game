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
#define BulletSpeed 700.0




typedef struct 
{
    Vector2 postion;
    Vector2 velocity;
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

    InitWindow(ScreenWidth, ScreenHeight, "Shooting Game");
    SetTargetFPS(60);

    srand(time(NULL));

    //Game setup
        //player
        Player player = {0};
        player.postion= (Vector2){MapWidth/2,MapHeight/2};
        player.velocity = (Vector2){0,0};
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
            if(Vector2Length(moveDir)>0){
                moveDir=Vector2Normalize(moveDir);
                player.velocity=Vector2Scale(moveDir,PlayerSpeed);

            }
            else{
                player.velocity=Vector2Lerp(player.velocity, (Vector2){0,0}, 12*deltaTime);
            }

            player.postion.x+=(player.velocity.x)*deltaTime;
            player.postion.y+=(player.velocity.y)*deltaTime;
            
            //Player bound
            player.postion.x=Clamp(player.postion.x, player.radius, MapWidth-player.radius);
            player.postion.y=Clamp(player.postion.y, player.radius, MapHeight-player.radius);

            //aim 
            Vector2 mouseWorld=GetScreenToWorld2D(GetMousePosition(), camera);
            player.shootDir=Vector2Normalize(Vector2Subtract(mouseWorld, player.postion));

            //window starting

            BeginDrawing();
            ClearBackground((Color){15, 35, 25, 255}); 
            BeginMode2D(camera);


            // player body
            Vector2 palpos=player.postion;
            DrawCircleV(palpos, player.radius, (Color){80, 50, 255, 255 });
            DrawCircleV(palpos, 14, (Color){120, 180, 255, 255 });

            DrawCircleV(Vector2Add(palpos,player.shootDir), 5, WHITE);
            DrawCircleV(Vector2Add(palpos, player.shootDir), 3, (Color){50, 100, 200});
            DrawLineEx(palpos, Vector2Add(palpos,Vector2Scale(player.shootDir,30)),4,ORANGE);

            //Bullet Shooting
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&& CanShoot){
                CanShoot=false;
                for(int i=0; i<MaxBullets; i++){
                    if(!bullet[i].active){
                        bullet[i].position=Vector2Add(player.postion, Vector2Scale(player.shootDir,player.radius));
                        bullet[i].direction=player.shootDir;
                        bullet[i].active=true;
                        bullet[i].time=0;
                        break; //On 1 click only one bullet than exit loop;
                    }
                }
            }
            if(!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                CanShoot=true;
            }
            //bullets update time to time
            for(int i=0; i<MaxBullets; i++){
                if(bullet[i].active){
                    bullet[i].position=Vector2Add(bullet[i].position, Vector2Scale(bullet[i].direction, BulletSpeed*deltaTime));
                    bullet[i].time+=deltaTime;

                    //bullet restrictions/limitations
                    if(bullet[i].time>2.0f || bullet[i].position.x<0 || bullet[i].position.x>MapWidth || bullet[i].position.y <0 || bullet[i].position.y>MapHeight){
                        bullet[i].active=false;
                    }

                }
            }
            //Draw bullets

            for(int i=0; i <MaxBullets; i++){
                if(bullet[i].active){
                    DrawCircleV(bullet[i].position, 5, YELLOW);
                    DrawCircleV(bullet[i].position, 10, Fade(YELLOW, 0.3f));
                }
            }
            



            



            EndMode2D();
            EndDrawing();
            
        }





    }










    
    CloseWindow();


    
    
}