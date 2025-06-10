
#include "raylib.h"
#include <stdio.h>


int limitSliderXmovement(int pos){
    if(pos < 30){
        return 30;
    }else if(pos > 460){
        return 460;
    }
    return pos;
}

int getSliderPercent(int pos){
    return (pos - 30) * 100 / (460 - 30);
}

int main(void){
    char sliderStartLabel[5];
    char sliderStopLabel[5];

    InitWindow(500, 300, "RLCL");
    SetTargetFPS(30);

    Rectangle sliderStart = {30, 130, 10, 20};
    Rectangle sliderStop = {30, 250, 10, 20};
    Rectangle sliderStartRange = {30, 130, 440, 10};
    Rectangle sliderStopRange = {30, 250, 440, 10};
    Rectangle sliderStartRangeDetection = {10, 110, 480, 50};
    Rectangle sliderStopRangeDetection = {10, 230, 480, 50};

    int lastStartValue = 0;
    int lastStopValue = 0;
    int percentStart = 0;
    int percentStop = 0;
    int hlDecay = 0;
    while (!WindowShouldClose()) {
        bool appliedThisFrame = false;
        if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
            Vector2 cursorPos = GetMousePosition();
            if(CheckCollisionPointRec(cursorPos,sliderStartRangeDetection)){
                sliderStart.x = limitSliderXmovement((int)cursorPos.x);
            }else if(CheckCollisionPointRec(cursorPos,sliderStopRangeDetection)){
                sliderStop.x = limitSliderXmovement((int)cursorPos.x);
            }
        }
        percentStart = getSliderPercent(sliderStart.x);
        percentStop = getSliderPercent(sliderStop.x);
        sprintf(sliderStartLabel,"%d", percentStart);
        sprintf(sliderStopLabel,"%d", percentStop);

        bool enableApply = (lastStartValue != percentStart || lastStopValue != percentStop);
        Rectangle applyDetection = {345, 5 , 142, 38};
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            
            Vector2 cursorPos = GetMousePosition();
            if(CheckCollisionPointRec(cursorPos, applyDetection)){
                lastStartValue = percentStart;
                lastStopValue = percentStop;
                appliedThisFrame = true;
                if(enableApply){
                    hlDecay = 10;
                }
                
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("RLCL", 10, 5, 40, RED);
        

        if(hlDecay > 0){
            DrawRectangleRec(applyDetection, RED);
            hlDecay --;
        }
        if(enableApply){
            DrawText("APPLY", 350, 5, 40, RED);
        }else if(hlDecay > 0){
            DrawText("APPLY", 350, 5, 40, GREEN);
        }else{
            DrawText("APPLY", 350, 5, 40, GRAY);
        }

        DrawText("START THRESHOLD", 10, 70, 30, LIGHTGRAY);
        DrawText(sliderStartLabel, 350, 70, 30, LIGHTGRAY);

        DrawText("STOP THRESHOLD", 10, 180, 30, LIGHTGRAY);
        DrawText(sliderStopLabel, 350, 180, 30, LIGHTGRAY);

        DrawRectangleRec(sliderStartRange, LIGHTGRAY);
        DrawRectangleRec(sliderStopRange, LIGHTGRAY);
        DrawRectangleRec(sliderStart, RED);
        DrawRectangleRec(sliderStop, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
