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




typedef struct 
{
    Vector2 postion;
    Vector2 speed;
    float healt;
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





int main(){

    InitWindow(ScreenWidth, ScreenHeight, "Servival Game");
    SetTargetFPS(60);

    srand(time(NULL));

    //Game startup
        //player
        Player player = {0};
        player.postion= (Vector2){MapWidth,MapHeight};
        player.speed = (Vector2){0,0};
        player.healt=100;
        player.maxHealth=100;
        player.radius=18;

        //enemies
        Enemy enemy[MaxEnemies]={0};

        //bullets
        Bullets bullet[MaxBullets]={0};

        //screen camera according to player movement
        Camera2D camera={0};

    
    
}