#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define ScreenWidth 1200
#define ScreenHeight 700
#define MapHeight 3000
#define MapWidth 3000
#define MaxEnemies 50
#define MaxBullets 200 
#define MaxTree 150
#define LevelEnimies 12
#define PlayerSpeed 300.0f
#define BulletSpeed 700.0
#define EnemeySpawnDelay 0.7f
#define EnemeySpawnRadius 900
#define EnemeySpeed 70.0f
#define EnemyRadius 16.0f





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
        float EnemySpwanTime=0;    
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

        //window and 2D drawing starting

        BeginDrawing();
        ClearBackground((Color){15, 35, 25, 255}); 
        BeginMode2D(camera);

            //Trees 
                for (int i = 0; i < MaxTree; i++)
            {
                Vector2 pos = tree[i].position;

                // Tree shadow
                DrawCircleV((Vector2){pos.x + 8, pos.y + 8}, tree[i].radius + 5, Fade((Color){0, 0, 0, 80}, 0.4f));

                // Tree trunk
                DrawRectangle(pos.x - 8, pos.y - tree[i].radius / 2, 16, tree[i].radius, (Color){90, 60, 30, 255});

                // Tree canopy (multiple layers for depth)
                DrawCircleV(pos, tree[i].radius, Fade(tree[i].color, 0.9f));
                DrawCircleV((Vector2){pos.x - 10, pos.y - 8}, tree[i].radius * 0.7f, Fade((Color){10, 70, 20}, 1.0f));
                DrawCircleV((Vector2){pos.x + 12, pos.y - 5}, tree[i].radius * 0.6f, Fade((Color){20, 80, 25}, 0.9f));
            }


            // player body draw
                Vector2 palpos=player.postion;
                DrawCircleV(palpos,player.radius, (Color){80, 50, 255, 255 });
                DrawCircleV(palpos, 14,(Color){120, 180, 255, 255 });
                DrawCircleV(Vector2Add(palpos,player.shootDir),5, WHITE);//Player Eye follows aim/shootdirection
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

            //Enemy Sypawning
                EnemySpwanTime+=deltaTime;
                if(EnemySpwanTime>=EnemeySpawnDelay && EnemiesKilled<EnemiesToKill){
                    EnemySpwanTime=0;
                    for(int i=0; i<MaxEnemies; i++){
                        if(!enemy[i].alive){
                            float angle=(float)rand()/RAND_MAX*2*PI;
                            float distance=200+(rand()%300);

                            enemy[i].position=Vector2Add(player.postion, (Vector2){cosf(angle)*distance,sinf(angle)*distance});
                            Vector2 dirToPlayer=Vector2Normalize(Vector2Subtract(player.postion,enemy[i].position));
                            enemy[i].velocity=Vector2Scale(dirToPlayer,EnemeySpeed);
                            enemy[i].speed=EnemeySpeed;
                            enemy[i].alive=true;
                            enemy[i].color=(Color){180 + rand() % 70, 60 + rand() % 40, 60, 255};
                            break;
                            

                        }

                    }   
                }
            //Enemy AI(move towards player)
                for(int i=0; i<MaxEnemies; i++){
                    if(enemy[i].alive){
                    Vector2 dirToPlayer=Vector2Normalize(Vector2Subtract(player.postion, enemy[i].position));
                    enemy[i].velocity=Vector2Lerp(enemy[i].velocity,Vector2Scale(dirToPlayer,enemy[i].speed),3*deltaTime);
                    enemy[i].position=Vector2Add(enemy[i].position,Vector2Scale(enemy[i].velocity,deltaTime));

                    if(Vector2Distance(enemy[i].position,player.postion) < player.radius+EnemyRadius){
                            player.health-=20;
                            //enemy[i].alive=false;
                            if(player.health<=0){
                                gameOver=true;
                            }
                            Vector2 backSteps=Vector2Scale(Vector2Normalize(Vector2Subtract(enemy[i].position,player.postion)),200);
                            enemy[i].position=Vector2Add(enemy[i].position, backSteps);
                        }
                    }
                }
                


            //Draw Enemy Body
                Vector2 dirToPlayer_eye;
                Vector2 eyePos;
                    for(int i=0; i<MaxEnemies; i++){
                        if(enemy[i].alive){
                        
                        Vector2 enPos=enemy[i].position;
                        //Enemey eye will see player 
                        dirToPlayer_eye=Vector2Normalize(Vector2Subtract(player.postion, enPos));
                        eyePos=Vector2Add(enPos,Vector2Scale(dirToPlayer_eye,8.0f));
                        DrawCircleV(enPos, EnemyRadius, enemy[i].color);
                        DrawCircleV(eyePos,5, WHITE);
                        DrawCircleV(eyePos,2, RED);


                    }
                }

            //Bullets kills Enemy
                for(int i=0; i<MaxBullets; i++){
                    if(bullet[i].active){
                        for(int j=0; j<MaxEnemies; j++){
                            if(enemy[j].alive){
                                if(Vector2Distance(bullet[i].position,enemy[j].position) < EnemyRadius){
                                    enemy[j].alive=false;
                                    bullet[i].active=false;
                                    EnemiesKilled++;
                                        //Levels upragation
                                        if(EnemiesKilled>EnemiesToKill){
                                            
                                            level++;
                                            EnemiesToKill=LevelEnimies*level;
                                            EnemiesKilled=0;
                                            EnemySpwanTime=0;
                                            
                                        }
                                    break;
                                }
                            }
                        }
                    }
                }

        }//Paused and QuiteGame condition ends here 
            //Camera moves with enemy
                camera.target = Vector2Lerp(camera.target, player.postion, 8 * deltaTime);
        //Paused or Unpaused
        if(IsKeyPressed(KEY_P)){
            paused=!paused;
        }
            
    
        EndMode2D();
        //All Non 2D things drawing    
            //Level show
                DrawText(TextFormat("Level %d", level), ScreenWidth / 2 - 90, 25, 52 ,WHITE);
                if(paused){
                    DrawRectangle(0,0,ScreenWidth, ScreenHeight, (Color){0,0,0,150});
                    DrawText("PAUSED", ScreenWidth/2-180, ScreenHeight/2, 72, WHITE);
                    DrawText("Press P to Reasume", ScreenWidth/2 -120, ScreenHeight/2 -60, 36, LIGHTGRAY);
                }
            //Showing GameOver but file handling is remaining
                if(gameOver){
                    DrawRectangle(0,0,ScreenWidth, ScreenHeight, (Color){0,0,0,150});
                    DrawText("GAME OVER", ScreenWidth/2-200, ScreenHeight/2 -120, 72, WHITE);
                    DrawText(TextFormat("Level Reached %d", level-1), ScreenWidth/2 -120, ScreenHeight/2 , 36, LIGHTGRAY);
                }
            //health Bar
                Rectangle healthbg={ScreenWidth/2 -160, ScreenHeight-70, 320, 35};
                Rectangle healthfilling={healthbg.x+5, healthbg.y+5, (player.health/player.maxHealth)*310,25};
                DrawRectangleRec(healthbg, (Color){60,60,60,255});
                DrawRectangleRec(healthfilling, player.health>40? GREEN: player.health>15? ORANGE: RED );


        EndDrawing();
            
        
    }




    










    
    CloseWindow();
    return 0;


    
    
}