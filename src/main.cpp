#include "raylib.h"
#include <string>
#include <vector>
#include <memory>

class Drawable {
    public:
        virtual void draw() {
            drawSelf();
            for(auto& child: children){
                child->draw();
            }
        }
        virtual void tick(Vector2 v){
            tickSelf(v);
            for(auto& child: children){
                child->tick(v);
            }
        }
        virtual ~Drawable() = default;
        void addChild(std::unique_ptr<Drawable> child) {
            children.push_back(std::move(child));
        }

    protected:
        virtual void drawSelf() = 0;
        virtual void tickSelf(Vector2) = 0;
        std::vector<std::unique_ptr<Drawable>> children;
};

class PresetBox: public Drawable {
    public:
        PresetBox(int start, int stop, int x, int y, int w, int h, int fontSize, Color color, int padding){
            this->x = x;
            this->y = y;
            this->label = std::to_string(start) + "-" + std::to_string(stop);
            Vector2 dimensions = MeasureTextEx(GetFontDefault(),this->label.c_str(),fontSize,3);
            this->h = dimensions.y + 2 * padding;
            this->w = dimensions.x + 2 * padding;
            this->fontSize = fontSize;
            this->color = color;
            this->padding = padding;
        }
    protected:
        void drawSelf() override {
            DrawRectangle(this->x,this->y,this->w,this->h,this->color);
            DrawText(this->label.c_str(),this->x + this->padding, this->y + this->padding, this->fontSize,BLACK);
        }
        void tickSelf(Vector2 v) override {
            return;
        }
    private:
        int x;
        int y;
        int w;
        int h;
        int padding;
        int fontSize;
        std::string label;
        Color color;

};

class Button: public Drawable {
    public:
        Button(std::string label, int x, int y, int fontSize, Color color, Color textColor, int padding, int animDuration){
            this->label = label;
            this->x = x;
            this->y = y;
            this->baseColor = color;
            this->disabledColor = GRAY;
            this->activeColor = GREEN;
            this->enabled = true;
            this->active = false;
            this->activeTime = 5;
            Vector2 dimensions = MeasureTextEx(GetFontDefault(),label.c_str(),fontSize,3);
            this->h = dimensions.y;
            this->w = dimensions.x;
            this->fontSize = fontSize;
            this-> textColor = textColor;
            this->padding = padding;
            Rectangle rec = {(int) x,(int) y,(float) dimensions.x + 2 * padding, (float) dimensions.y + 2 * padding};
            this->backdrop = rec;
            this->activeTime = animDuration;
            this->currAnimFrame = animDuration;
        }
        bool checkClick(Vector2 mousePos){
            if(CheckCollisionPointRec(mousePos, this->backdrop) && IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                if(this->enabled && !this->active){
                    return true;
                }
            }
            return false;
        }
        
    protected:
        void tickSelf(Vector2 mousePos) override{
            if(this->checkClick(mousePos)){
                this->active = true;
                this->currAnimFrame = 0;
            }
            if(currAnimFrame < activeTime && this->active){
                this->currAnimFrame ++;
            }else if(this->active){
                this->active = false;
            }
        }
        void drawSelf() override {
            Color btnColor;
            if(this->active){
                btnColor = this->activeColor;
            }else{
                btnColor = this->baseColor;
            }
            DrawRectangle(this->x,this->y,this->w + 2 * padding,this->h + 2 * padding,btnColor);
            DrawText(this->label.c_str(), this->x + this->padding, this->y + this->padding, this->fontSize, this->textColor);
        }

    private:
        int padding;
        int x,y,h,w;
        int fontSize;
        Rectangle backdrop;
        std::string label;
        Color activeColor;
        Color baseColor;
        Color disabledColor;
        Color textColor;
        Color disabledTextColor;
        bool enabled, active;
        int activeTime; //frame duration for the active color when pressed
        int currAnimFrame;
};

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
    InitWindow(500, 500, "RLCL");
    SetTargetFPS(30);
    //Button testbtn1 = Button("Prueba1CAPO",10,10,30,GRAY,BLACK,5,5);
    PresetBox presBox1 = PresetBox(35,40,30,30,0,0,30,RED,5);
    presBox1.addChild(std::make_unique<Button>("Prueba1CAPO",10,10,30,GRAY,BLACK,5,5));
    while (!WindowShouldClose()) {
        presBox1.tick(GetMousePosition());
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        presBox1.draw();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}

/*
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
*/