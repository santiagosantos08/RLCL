
#include "../raylib/src/raylib.h"
#include <stdio.h>


int limitSliderXmovement(int pos, bool upper, int cmp){
    int res = pos;
    if(upper && res <= cmp){
        res = cmp;
    }else if(!upper && res >= cmp){
        res = cmp;
    }
    if(res < 30){
        return 30;
    }else if(res > 460){
        return 460;
    }
    return res;
}

int getSliderPercent(int pos){
    return (pos - 30) * 100 / (460 - 30);
}

int main(void){
    char sliderStartLabel[5];
    char sliderStopLabel[5];

    InitWindow(500, 500, "RLCL");
    SetTargetFPS(30);

    Rectangle sliderStart = {30, 120, 10, 30};
    Rectangle sliderStop = {80, 240, 10, 30};
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
                sliderStart.x = limitSliderXmovement((int)cursorPos.x, false, sliderStop.x);
            }else if(CheckCollisionPointRec(cursorPos,sliderStopRangeDetection)){
                sliderStop.x = limitSliderXmovement((int)cursorPos.x, true, sliderStart.x);
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
        Rectangle separator = {10, 340, 480, 150};
        BeginDrawing();
        ClearBackground(BLACK);
        DrawText("RLCL [ ON ]", 10, 5, 40, RED);
        

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
        DrawRectangleRec(separator, DARKGRAY);
        DrawText("START THRESHOLD", 10, 70, 30, LIGHTGRAY);
        DrawText(sliderStartLabel, 350, 70, 30, LIGHTGRAY);

        DrawText("STOP THRESHOLD", 10, 180, 30, LIGHTGRAY);
        DrawText(sliderStopLabel, 350, 180, 30, LIGHTGRAY);

        DrawText("Save current as preset", 60, 300, 30, LIGHTGRAY);
        DrawText("50 - 70  ----  Load   Delete", 35, 360, 30, LIGHTGRAY);
        DrawText("45 - 80  ----  Load   Delete", 35, 400, 30, LIGHTGRAY);
        DrawText("80 - 90  ----  Load   Delete", 35, 440, 30, LIGHTGRAY);

        DrawRectangleRec(sliderStartRange, LIGHTGRAY);
        DrawRectangleRec(sliderStopRange, LIGHTGRAY);
        DrawRectangleRec(sliderStart, RED);
        DrawRectangleRec(sliderStop, RED);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
