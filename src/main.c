#include "raylib.h"
#include "raymath.h"
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>

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
#define MaxPlayersOnld 20


typedef struct{
    Vector2 postion;
    Vector2 velocity;
    float health;
    int maxHealth;
    float radius;
    Vector2 shootDir;
    int totalKills;
} Player;

typedef struct{
    Vector2 position;
    Vector2 velocity;
    float speed;
    bool alive;
    Color color;
}Enemy;

typedef struct{
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
typedef struct {
    char name[20];
    int LevelReached;
    int kills;
} ScoreBoard;

typedef enum{
    MENU,
    INGAME,
    USER,
    LADERBOARD,
    NAMEINPUT,
    GAMEOVER,
    CONFIRMUSER,
}GameOptions;

GameOptions options;
ScoreBoard Players[MaxPlayersOnld];
int players=0;

//strcuts inits
    Player player ={0};
    Enemy enemy[MaxEnemies]={0};
    Bullets bullet[MaxBullets]={0};
    Tree tree[MaxTree];
    Camera2D camera={0};
    ScoreBoard currentPlayer;

//Games Main variables 
    int level=1;          
    int EnemiesKilled=0;
    int EnemiesToKill=LevelEnimies;
    float EnemySpwanTime=0;    
    bool paused=false;
    bool gameOver=false;
    bool CanShoot=true;
    
    char playerName[20]= "";
    char nameEntering[20]= "";
    int nameLength= 0;
    bool isName =false;


void showLeaderboard();
void saveLeaderboard();
void updateLeaderboard();
int main(){

    InitWindow(ScreenWidth,ScreenHeight,"Shooting Game");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);
    srand(time(NULL));
    showLeaderboard();
    //Game setup
        //player
        player.postion= (Vector2){MapWidth/2,MapHeight/2};
        player.velocity= (Vector2){0,0};
        player.health=100;
        player.maxHealth=100;
        player.radius=18;
        player.totalKills=0;

        //Camera Move with player movement area
        camera.target=player.postion;
        camera.offset= (Vector2){ScreenWidth/2.0f,ScreenHeight/2.0f};
        camera.zoom=1.0f;

        //TreeBG
        for(int i=0; i<MaxTree; i++){
            tree[i].position=(Vector2){(float)(rand()%MapWidth),(float)(rand()%MapHeight)};
            tree[i].radius=(25+(rand()&30));
            tree[i].color=(Color){20+(rand()%60),60+(rand()%60),20,255};
        }

    //Game initialization
    while(!WindowShouldClose()){
        float deltaTime= GetFrameTime();
        Vector2 mousePos=GetMousePosition();

         if(IsKeyPressed(KEY_ESCAPE)) {
            if(options==INGAME){
                paused= !paused;
            }
            else if(options!=MENU){
                options=MENU;
            }
         }
        //Menu content and working
        if(options==MENU){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)){
                Rectangle startBtn={(ScreenWidth/2)-100,260,400,55};
                Rectangle boardBtn={(ScreenWidth/2)-100,330,400,55};
                Rectangle quitBtn={(ScreenWidth/2)-100,400,400,55};
                if(CheckCollisionPointRec(mousePos,startBtn)){
                    if(isName) options=CONFIRMUSER;
                    else       options=NAMEINPUT;
                }else if(CheckCollisionPointRec(mousePos,boardBtn)) options=LADERBOARD;
                else if(CheckCollisionPointRec(mousePos,quitBtn)) break;

            }
        }else if(options==NAMEINPUT){
            int keys=GetCharPressed();
            while(keys>0){
                if(keys>=32 && keys<=125){
                    nameEntering[nameLength]=(char)keys;
                    nameLength++;
                    nameEntering[nameLength]='\0';
                }
                keys=GetCharPressed();
            }
            if(IsKeyPressed(KEY_BACKSPACE)){
                nameLength--;
                nameEntering[nameLength]= '\0';
            }
            if(IsKeyPressed(KEY_ENTER)&&nameLength>0){
                strcpy(playerName, nameEntering);
                isName=true;
                options=INGAME;

                player.postion=(Vector2){MapWidth/2, MapHeight/2};
                player.health=100;
                EnemiesKilled=0;
                EnemiesToKill=LevelEnimies;
                level= 1;
                EnemySpwanTime=0;
                gameOver=false;
                paused=false;
                for(int i=0; i<MaxEnemies; i++) enemy[i].alive=false;
                for(int i=0; i<MaxBullets; i++) bullet[i].active=false;
            }
            
        }else if(options==CONFIRMUSER){
            if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                Rectangle changeBtn={(ScreenWidth/2)- 180, 380, 160, 60};
                Rectangle continueBtn={(ScreenWidth/2)+ 20, 380, 160, 60};

                //On Mouse Click options
                if(CheckCollisionPointRec(mousePos, changeBtn)) options=NAMEINPUT;
                else if(CheckCollisionPointRec(mousePos, continueBtn)) options=INGAME;
            }
        }else if(options==INGAME &&(!paused && !gameOver)){        

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

                }else{
                    player.velocity=Vector2Lerp(player.velocity, (Vector2){0,0}, 12*deltaTime);
                }

                player.postion=Vector2Add(player.postion,Vector2Scale((Vector2)player.velocity,deltaTime));
                // player.postion.x+=(player.velocity.x)*deltaTime;
                // player.postion.y+=(player.velocity.y)*deltaTime;
                
            //Player bound
                player.postion.x=Clamp(player.postion.x, player.radius, MapWidth-player.radius);
                player.postion.y=Clamp(player.postion.y, player.radius, MapHeight-player.radius);

            //aim 
                Vector2 mouseWorld=GetScreenToWorld2D(GetMousePosition(), camera);
                player.shootDir=Vector2Normalize(Vector2Subtract(mouseWorld,player.postion));
            //Bullet Shooting
                if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)&& CanShoot){
                    CanShoot=false;
                    for(int i=0; i<MaxBullets; i++){
                        if(!bullet[i].active){
                            bullet[i].active=true;
                            bullet[i].position=Vector2Add(player.postion, Vector2Scale(player.shootDir,player.radius));
                            bullet[i].direction=player.shootDir;
                            bullet[i].time=0;
                            break; //On 1 click only one bullet
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
            //EnemyLogic
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

                    if(Vector2Distance(enemy[i].position,player.postion)< player.radius+EnemyRadius){
                            player.health-=20;
                            //enemy[i].alive=false;
                            if(player.health<=0){
                                gameOver=true;
                                updateLeaderboard();
                                options=GAMEOVER;
                            }
                            Vector2 backSteps=Vector2Scale(Vector2Normalize(Vector2Subtract(enemy[i].position,player.postion)),200);
                            enemy[i].position=Vector2Add(enemy[i].position, backSteps);
                        }
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
                                    player.totalKills++;
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
        
        }
        //window and 2D drawing starting
        BeginDrawing();
        ClearBackground((Color){15, 35, 25, 255}); 
        //Paused/Unpaused
        if(IsKeyPressed(KEY_P)){
            paused=!paused;
        }
        //MainMenu Drawing
        if(options==MENU){
            Rectangle startBtn={(ScreenWidth/2)-170,260,400,55};
            Rectangle boardBtn={(ScreenWidth/2)-170,330,400,55};
            Rectangle quitBtn={(ScreenWidth/2)-170,400,400,55};

            DrawRectangleRec(startBtn,GREEN);
            DrawText("START GAME",startBtn.x+90, startBtn.y+12, 36,WHITE);

            DrawRectangleRec(boardBtn,BLUE);
            DrawText("LEADERBOARD",boardBtn.x+90, boardBtn.y+12, 36,WHITE);

            DrawRectangleRec(quitBtn,RED);
            DrawText("QUIT GAME",quitBtn.x+90, quitBtn.y+12, 36,WHITE);                
            

        }else if(options==NAMEINPUT){
            DrawText("ENTER YOUR NAME", (ScreenWidth/2)-280,200,60, GOLD);
            DrawRectangle(300, 320, 600, 80, LIGHTGRAY);
            DrawRectangleLines(300, 320, 600, 80, DARKGRAY);
            DrawText(nameEntering, 320, 340, 50, DARKGRAY);
    
        }else if(options == CONFIRMUSER) {
            DrawRectangle(0, 0, ScreenWidth, ScreenHeight, Fade(BLACK, 0.7f));
            DrawText(TextFormat("Play as %s?", playerName), ScreenWidth/2 - 200, 280, 50, WHITE);
            DrawRectangle(ScreenWidth/2 - 180, 380, 160, 60, RED);
            DrawText("Change", ScreenWidth/2 - 140, 395, 32, WHITE);
            DrawRectangle(ScreenWidth/2 + 20, 380, 160, 60, GREEN);
            DrawText("Continue", ScreenWidth/2 + 55, 395, 32, WHITE);

        }else if(options==INGAME){
        
            BeginMode2D(camera);        

            //Trees 
                for (int i = 0; i < MaxTree; i++)
            {
                Vector2 pos = tree[i].position;

                // Tree shadow
                DrawCircleV((Vector2){pos.x + 8, pos.y + 8}, tree[i].radius + 5, Fade((Color){0, 0, 0, 80}, 0.4f));
            
                DrawCircleV((Vector2){pos.x - 10, pos.y - 8}, tree[i].radius * 0.7f, Fade((Color){10, 70, 20}, 1.0f));
                DrawCircleV((Vector2){pos.x + 12, pos.y - 5}, tree[i].radius * 0.6f, Fade((Color){20, 80, 25}, 0.9f));
            }

            // player body draw
                Vector2 palpos=player.postion;
                DrawCircleV(palpos,player.radius, (Color){80, 50, 255, 255});
                DrawCircleV(palpos, 14,(Color){120, 180, 255, 255 });
                DrawCircleV(Vector2Add(palpos,player.shootDir),5, WHITE);//Player Eye follows aim/shootdirection
                DrawCircleV(Vector2Add(palpos, player.shootDir), 3, (Color){50, 100, 200});
                DrawLineEx(palpos, Vector2Add(palpos,Vector2Scale(player.shootDir,30)),4,ORANGE);

            //Draw bullets
                for(int i=0; i <MaxBullets; i++){
                    if(bullet[i].active){
                        DrawCircleV(bullet[i].position, 5, YELLOW);
                        DrawCircleV(bullet[i].position, 10, Fade(YELLOW, 0.3f));
                    }
                }

            //Draw Enemy Body
                Vector2 dirToPlayer_eye;
                Vector2 eyePos;
                    for(int i=0; i<MaxEnemies; i++){
                        if(enemy[i].alive){
                        
                        Vector2 enPos=enemy[i].position;
                        DrawCircleV(enPos, EnemyRadius, RED);
                        DrawCircleV(enPos, EnemyRadius+3, Fade(RED, 0.3f));
                        
                        //Enemey eye will see player 
                        dirToPlayer_eye=Vector2Normalize(Vector2Subtract(player.postion, enPos));
                        eyePos=Vector2Add(enPos,Vector2Scale(dirToPlayer_eye,8.0f));
                        
                        DrawCircleV(eyePos,5, WHITE);
                        DrawCircleV(eyePos,2, RED);

                        }
                }
           //Paused and QuiteGame condition ends here 
            //Camera moves with enemy
                camera.target = Vector2Lerp(camera.target, player.postion, 8*deltaTime);
    
            EndMode2D();    
            //Level show
                DrawText(TextFormat("Level %d", level), ScreenWidth / 2 - 90, 25, 52 ,WHITE);
                DrawText(TextFormat("Kills %d", player.totalKills), 0, 25, 52 ,WHITE);

                if(paused){
                    DrawRectangle(0, 0, ScreenWidth, ScreenHeight, DARKGREEN);
                    DrawText("PAUSED", (ScreenWidth/2)- 135, 20, 80, WHITE);

                    Rectangle resume = {(ScreenWidth/2)- 100, 200, 200, 60};
                    Rectangle Menu = {(ScreenWidth/2)- 100, 280, 200, 60};
                    DrawRectangleRec(resume, GREEN);
                    DrawRectangleRec(Menu, RED);

                    DrawText("Resume", resume.x+40, resume.y+15, 36, WHITE);
                    DrawText("Menu", Menu.x+50, Menu.y+15, 36, WHITE);

                    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                        if(CheckCollisionPointRec(mousePos, resume)) paused=false;
                        if(CheckCollisionPointRec(mousePos, Menu)){
                            options=MENU;
                            paused=false; 
                        }
                    }
                }
            //Showing GameOver but file handling is remaining
                // if(gameOver){
                //     DrawRectangle(0,0,ScreenWidth, ScreenHeight, GREEN);
                //     DrawText("GAME OVER", (ScreenWidth/2)-200, (ScreenHeight/2)-120, 72, WHITE);
                //     DrawText(TextFormat("Level Reached %d", level-1),(ScreenWidth/2)-120, (ScreenHeight/2), 36, LIGHTGRAY);
                // }
            //health Bar
                Rectangle healthbg={(ScreenWidth/2)-160, ScreenHeight-70, 320, 35};
                Rectangle healthfilling={healthbg.x+5, healthbg.y+5, (player.health/player.maxHealth)*310,25};
                DrawRectangleRec(healthbg, BLACK);

                Color colorForHealthBar;
                if(player.health>50) colorForHealthBar=GREEN;
                else if(player.health>15) colorForHealthBar=ORANGE;
                else colorForHealthBar=RED;
                DrawRectangleRec(healthfilling, colorForHealthBar);

        }else if (options==LADERBOARD){
            DrawText("LEADERBOARD", (ScreenWidth/2)-250, 10, 70, GOLD);

            for(int i = 0; i < players && i < 10; i++) {
                DrawText(TextFormat("Player Name Kills      Wave"),300, 90 , 40, WHITE);
                DrawText(TextFormat("%d. %-15s   %d         %2d", i+1, Players[i].name, Players[i].kills, Players[i].LevelReached),300, 160 + i*40, 40, WHITE);
            }
            DrawText("Press ESC to return", 30, ScreenHeight-30, 30, WHITE);
        }else if(options==GAMEOVER){
            DrawRectangle(0, 0, ScreenWidth, ScreenHeight, BLACK);
            DrawText("GAME OVER", (ScreenWidth/2)- 260, 140, 90, RED);
            DrawText(TextFormat("Name: %s", playerName), (ScreenWidth/2)- 150, 280, 50, WHITE);
            DrawText(TextFormat("Final Kills: %d", player.totalKills), (ScreenWidth/2)- 180, 340, 50, YELLOW);
            DrawText(TextFormat("Reached Wave: %d", level), (ScreenWidth/2)- 160, 400, 50, GOLD);
            DrawText("Press ESC for Menu", (ScreenWidth/2)- 200, 520, 40, WHITE);

        }
        
        EndDrawing(); 
        
    }
    
    CloseWindow();
    return 0;

}

//Other Functions
void showLeaderboard(){
    FILE *file = fopen("assets/scoreboard.txt","r");
    
    if(file==NULL) return;
    players=0;
    while(players<MaxPlayersOnld){
        if(fscanf(file, "%19[^,],%d,%d\n",Players[players].name,&Players[players].kills,&Players[players].LevelReached)==3){
            players++;
        }else break;
        
    }
    fclose(file);
}
void saveLeaderboard(){
    FILE *file= fopen("assets/scoreboard.txt","w");
    if (file==NULL) return;
    for(int i=0; i<players && i<MaxPlayersOnld; i++){
        fprintf(file, "%s,%d,%d\n",Players[i].name,Players[i].kills,Players[i].LevelReached);
    }
    fclose(file);

}
void updateLeaderboard(){
    strcpy(currentPlayer.name, playerName);
    currentPlayer.LevelReached=level;
    currentPlayer.kills=player.totalKills;

    int topper=0;
    for(int i=0; i<players; i++){
        if((currentPlayer.kills>Players[i].kills || currentPlayer.kills==Players[i].kills)){
         topper=i;
         break;
        }
    }
    if(topper<MaxPlayersOnld) {
        for(int j=players; j>topper; j--) {
            Players[j]=Players[j-1];
        }
        Players[topper]=currentPlayer;
        if(players<MaxPlayersOnld)players++;
        saveLeaderboard();
    }
}