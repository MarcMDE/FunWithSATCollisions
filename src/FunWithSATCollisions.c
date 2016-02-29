/*******************************************************************************************
*
*   FunWithSATCollisions.c
*
*   SAT collisions demo created using satcollision.h made by Marc Montagut - @MarcMDE  
*
*   Copyright (c) 2016 Marc Montagut
*
*   This demo has been created using raylib 1.0 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "satcollision.h"
#include "raylib.h"
#include "c2dmath.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//#include "rlgl.h"

#define ARROWS_OFFSET 35
#define SELECTION_POINT_RADIUS 5
#define POINT_RADIUS 2

typedef enum DemoStates {SETUP_POLY, COLLIDE}DemoStates;
typedef enum PolySetUpStates {SELECT, EDIT}PolySetUpStates;

//void DrawSATConvexPoly(Vector2 *points, Vector2 center, int numPoints, Color color);

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    const Vector2 midScreen = {screenWidth/2, screenHeight/2};
    
    // Declaration--------------------------------------
    SATRegPoly poly[2];
    
    Color pColor[2];
    float pAlpha[2];
    
    bool colliding;
    
    Vector2 *aPolyDrawPoints;
    Vector2 *bPolyDrawPoints;
    
    Vector2 newPolyPosition;
    Vector2 polyTranslation;
    int aPolyRotation;
    int bPolyRotation;
    
    bool isPointSelected;
    bool showDebugPoints;
    int selectedPointIndex;
    int currentPolyIndex;
    
    DemoStates demoState;
    PolySetUpStates polySetUpState;
    
    Texture2D arrowKey;
    //--------------------------------------------------
    
    // Initialization-----------------------------------
    for (int i=0; i<2; i++)
    {
        poly[i].sides = 3;
        poly[i].radius = 20;
    }
    
    pColor[0] = BLUE;
    pColor[1] = RED;
    
    colliding = false;
    
    pAlpha[0] = 1;
    pAlpha[1] = 0.4f;
    
    demoState = SETUP_POLY;
    polySetUpState = SELECT;
    
    showDebugPoints = false;
    isPointSelected = false;
    
    aPolyRotation = 0;
    bPolyRotation = 0;
    
    selectedPointIndex = 0;
    currentPolyIndex = 0;
    
    
    InitWindow(screenWidth, screenHeight, "Fun With SAT Collisions v1.0 - @MarcMDE");
    
    arrowKey = LoadTexture("assets/arrow_key.png");
    //Texture2D whitePixel = CreateWhitePixelTexture();
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        switch (demoState)
        {
            case SETUP_POLY:
            {
                switch (polySetUpState)
                {
                    case SELECT:
                    {   
                        if (IsKeyPressed(KEY_LEFT) && poly[currentPolyIndex].sides > 3) poly[currentPolyIndex].sides--;
                        else if (IsKeyPressed(KEY_RIGHT) && poly[currentPolyIndex].sides < 20) poly[currentPolyIndex].sides++;
                        
                        if (IsKeyDown(KEY_UP) && poly[currentPolyIndex].radius < 150) poly[currentPolyIndex].radius++;
                        else if (IsKeyDown(KEY_DOWN) && poly[currentPolyIndex].radius > 10) poly[currentPolyIndex].radius--;
                        
                        if (IsKeyPressed(KEY_ENTER)) 
                        {
                            InitSATRegPoly(&poly[currentPolyIndex], midScreen, poly[currentPolyIndex].radius, poly[currentPolyIndex].sides, 0);
                            
                            if (currentPolyIndex == 0) 
                            {
                                aPolyDrawPoints = malloc(sizeof(Vector2)*poly[currentPolyIndex].sides);
                                for (int i=0; i<poly[currentPolyIndex].sides; i++) aPolyDrawPoints[i] = poly[currentPolyIndex].points[poly[currentPolyIndex].sides-i-1];
                            }
                            else if (currentPolyIndex == 1) 
                            {
                                bPolyDrawPoints = malloc(sizeof(Vector2)*poly[currentPolyIndex].sides);
                                for (int i=0; i<poly[currentPolyIndex].sides; i++) bPolyDrawPoints[i] = poly[currentPolyIndex].points[poly[currentPolyIndex].sides-i-1];
                            }

                            polySetUpState = EDIT;
                        }
                    }
                    break;
                    
                    case EDIT:
                    {
                        if (!isPointSelected)
                        {
                            for (int i=0; i<poly[currentPolyIndex].sides; i++)
                            {
                                if (Vector2Lenght(Vector2Sub(poly[currentPolyIndex].points[i], GetMousePosition())) < SELECTION_POINT_RADIUS
                                && IsMouseButtonPressed(0)) 
                                {
                                    selectedPointIndex = i;
                                    isPointSelected = true;
                                    i = poly[currentPolyIndex].sides;
                                }
                            }
                        }
                        else
                        {
                            poly[currentPolyIndex].points[selectedPointIndex] = GetMousePosition();
                           
                            if (currentPolyIndex == 0) aPolyDrawPoints[poly[currentPolyIndex].sides-selectedPointIndex-1] = poly[currentPolyIndex].points[selectedPointIndex];
                            else if (currentPolyIndex == 1) bPolyDrawPoints[poly[currentPolyIndex].sides-selectedPointIndex-1] = poly[currentPolyIndex].points[selectedPointIndex];
                            
                            if (IsMouseButtonUp(0)) isPointSelected = false;
                        }
                        
                        if (IsKeyPressed(KEY_ENTER))
                        {
                            isPointSelected = false;
                            polySetUpState = SELECT;
                            
                            if (currentPolyIndex == 0)
                            {
                                currentPolyIndex++;
                            }
                            else if (currentPolyIndex == 1)
                            {
                                currentPolyIndex = 0;
                                demoState = COLLIDE;
                            }
                        }
                    }
                    break;
                }
            }
            break;
            
            case COLLIDE:
            {
                aPolyRotation = 0;
                bPolyRotation = 0;
                
                newPolyPosition = GetMousePosition();
                polyTranslation = Vector2Sub(newPolyPosition, poly[0].position);
                
                if (IsKeyDown(KEY_LEFT)) aPolyRotation--;
                else if (IsKeyDown(KEY_RIGHT)) aPolyRotation++;
                
                if (IsKeyDown(KEY_UP)) bPolyRotation++;
                else if (IsKeyDown(KEY_DOWN)) bPolyRotation--;
                
                for (int i=0; i<poly[0].sides; i++) poly[0].points[i] = Vector2Add(poly[0].points[i], polyTranslation);
                poly[0].position = newPolyPosition;
                if (aPolyRotation!=0) RotatePoints(poly[0].points, poly[0].sides, poly[0].position, aPolyRotation);
                for (int i=0; i<poly[0].sides; i++) aPolyDrawPoints[i] = poly[0].points[poly[0].sides-i-1];
                
                if (bPolyRotation!=0) RotatePoints(poly[1].points, poly[1].sides, poly[1].position, bPolyRotation);
                for (int i=0; i<poly[1].sides; i++) bPolyDrawPoints[i] = poly[1].points[poly[1].sides-i-1];
                
                if (IsKeyPressed(KEY_SPACE)) showDebugPoints = !showDebugPoints;
  
                colliding = SATPolysCollide(poly[0].points, poly[0].sides, poly[1].points, poly[1].sides);
                
                if (IsKeyPressed('R')) 
                {
                    // Reset Demo
                    demoState = SETUP_POLY;
                    
                    // Initialization-----------------------------------
                    for (int i=0; i<2; i++)
                    {
                        poly[i].sides = 3;
                        poly[i].radius = 20;
                    }
                    
                    pColor[0] = BLUE;
                    pColor[1] = RED;
                    
                    colliding = false;
                    
                    pAlpha[0] = 1;
                    pAlpha[1] = 0.4f;
                    
                    demoState = SETUP_POLY;
                    polySetUpState = SELECT;
                    
                    showDebugPoints = false;
                    isPointSelected = false;
                    
                    aPolyRotation = 0;
                    bPolyRotation = 0;
                    
                    selectedPointIndex = 0;
                    currentPolyIndex = 0;
                    
                    free(poly[0].points);
                    free(poly[1].points);
                    free(aPolyDrawPoints);
                    free(bPolyDrawPoints);
                    //-------------------------------------------------------
                }
            }
            break;
            
            default: break;
        }
        
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            switch (demoState)
            {
                case SETUP_POLY:
                {
                    switch (polySetUpState)
                    {
                        case SELECT:
                        {
                            DrawPoly(midScreen, poly[currentPolyIndex].sides, poly[currentPolyIndex].radius, 180, pColor[currentPolyIndex]);
                            
                            DrawTexturePro(arrowKey, (Rectangle){0, 0, arrowKey.width, arrowKey.height}, (Rectangle){midScreen.x, midScreen.y-poly[currentPolyIndex].radius-ARROWS_OFFSET, arrowKey.width, arrowKey.height}, 
                            (Vector2){arrowKey.width/2, arrowKey.height/2}, 0, WHITE);
                            DrawTexturePro(arrowKey, (Rectangle){0, 0, arrowKey.width, arrowKey.height}, (Rectangle){midScreen.x, midScreen.y+poly[currentPolyIndex].radius+ARROWS_OFFSET, arrowKey.width, arrowKey.height}, 
                            (Vector2){arrowKey.width/2, arrowKey.height/2}, 180, WHITE);
                            DrawTexturePro(arrowKey, (Rectangle){0, 0, arrowKey.width, arrowKey.height}, (Rectangle){midScreen.x-poly[currentPolyIndex].radius-ARROWS_OFFSET, midScreen.y, arrowKey.width, arrowKey.height}, 
                            (Vector2){arrowKey.width/2, arrowKey.height/2}, 90, WHITE);
                            DrawTexturePro(arrowKey, (Rectangle){0, 0, arrowKey.width, arrowKey.height}, (Rectangle){midScreen.x+poly[currentPolyIndex].radius+ARROWS_OFFSET, midScreen.y, arrowKey.width, arrowKey.height}, 
                            (Vector2){arrowKey.width/2, arrowKey.height/2}, -90, WHITE);
                            
                            DrawCircleLines(midScreen.x, midScreen.y, poly[currentPolyIndex].radius, GRAY);
                            
                            if (currentPolyIndex == 0) 
                            {
                                DrawText("SET UP 'A' POLYGON.", 10, 10, 30, DARKGRAY);
                                DrawText("Press ENTER to confirm 'A' polygon and go to EDIT MODE.", 10, screenHeight-25, 24, DARKGRAY);
                            }
                            else if (currentPolyIndex == 1) 
                            {
                                DrawText("SET UP 'B' POLYGON.", 10, 10, 30, DARKGRAY);
                                DrawText("Press ENTER to confirm 'B' polygon and go to EDIT MODE.", 10, screenHeight-25, 24, DARKGRAY);
                            }
                        }
                        break;
                        
                        case EDIT:
                        {
                            if (currentPolyIndex == 0) DrawPolyEx(aPolyDrawPoints, poly[currentPolyIndex].position, poly[currentPolyIndex].sides, pColor[currentPolyIndex]);
                            else if (currentPolyIndex == 1) DrawPolyEx(bPolyDrawPoints, poly[currentPolyIndex].position, poly[currentPolyIndex].sides, pColor[currentPolyIndex]);
                            for (int i=0; i<poly[currentPolyIndex].sides; i++) DrawCircleV(poly[currentPolyIndex].points[i], SELECTION_POINT_RADIUS, BLACK);
                            
                            DrawText("Drag BLACK POINTS with your mouse.", 10, 40, 20, DARKGRAY);
                            DrawText("Angles can't surpass 180º. The polygon must be convex!!!", 10, 60, 20, DARKGRAY);
                            
                            if (currentPolyIndex == 0) 
                            {
                                DrawText("CUSTOMIZE 'A' POLY!", 10, 5, 30, DARKGRAY);
                                DrawText("Press ENTER to confirm 'A' polygon and go to 'B' polygon creation.", 10, screenHeight-25, 22, DARKGRAY);
                            }
                            else if (currentPolyIndex == 1) 
                            {
                                DrawText("CUSTOMIZE 'B' POLY!", 10, 5, 30, DARKGRAY);
                                DrawText("Press ENTER to confirm 'B' polygon and go to COLLIDE MODE!.", 10, screenHeight-25, 22, DARKGRAY);
                            }

                        }
                        break;
                    }
                }
                break;
                
                case COLLIDE:
                {
                    DrawText("COLLIDE!!!", 10, 5, 30, DARKGRAY);
                    DrawText("<SPACE> Show Points", 10, 35, 20, DARKGRAY);
                    DrawText("<ARROW KEYS> Rotate A and B poly", 10, 55, 20, DARKGRAY);
                    DrawText("<MOUSE> Move 'A' poly", 10, 75, 20, DARKGRAY);
                    
                    
                    DrawText("<R> Reset Demo", 10, screenHeight-25, 23, DARKGRAY);
                    
                    DrawPolyEx(aPolyDrawPoints, poly[0].position, poly[0].sides, Fade(pColor[0], pAlpha[colliding]));
                    DrawPolyEx(bPolyDrawPoints, poly[1].position, poly[1].sides, Fade(pColor[1], pAlpha[colliding]));
                    
                    if (showDebugPoints) 
                    {
                        for (int j=0; j<2; j++)
                        {
                            for (int i=0; i<poly[j].sides; i++) DrawCircleV(poly[j].points[i], POINT_RADIUS, BLACK);
                        }
                    }
                }
                break;
                
                default: break;
            }
            
            DrawLine(200, screenHeight/2, screenWidth-200, screenHeight/2, LIGHTGRAY);
            DrawLine(screenWidth/2, 100, screenWidth/2, screenHeight-100, LIGHTGRAY);
            
            DrawText("@MarcMDE", screenWidth-120, 5, 20, GRAY);

            //DrawText("Congrats! You created your first window!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------   
    if(demoState == COLLIDE)
    {
        free(poly[0].points);
        free(poly[1].points);
        free(aPolyDrawPoints);
        free(bPolyDrawPoints);
    }
    
    UnloadTexture(arrowKey);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}