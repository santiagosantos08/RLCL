#include "raylib.h"
#include <string>
#include <vector>
#include <memory>
#include <functional>

//container alignmed modes
#define ROW 0
#define COLUMN 1
//alignment across the main and cross axis, similar to flexbox | CURRENTLY UNUSED
#define START 0
#define CENTER 1
#define END 2
//spacing across main axis | CURRENTLY UNUSED
#define FILL 0
#define CONCENTRATE 1

std::string testBanner = "inic";

void handleClick(std::string s){
    testBanner = s;
}

class UINode {
    public:
        virtual void draw(int offsetX, int offsetY){drawSelf(offsetX, offsetY);for(auto& child: children){child->draw(offsetX, offsetY);}} //it makes no sense to use floats since its pixels
        virtual void tick(Vector2 v){tickSelf(v);for(auto& child: children){child->tick(v);}}
        virtual int getWidth(){return this->w;}
        virtual int getHeight(){return this->h;}
        virtual int getX(){return this->x;}
        virtual int getY(){return this->y;}
        virtual void setX(int x){this->x = x;}
        virtual void setY(int y){this->y = y;}
        virtual ~UINode() = default;
        virtual void addChild(std::unique_ptr<UINode> child) {
            child->setX(this->x + child->getX());
            child->setY(this->y + child->getY());
            children.push_back(std::move(child));
        }
    protected:
        virtual void drawSelf(int offsetX, int offsetY) = 0;
        virtual void tickSelf(Vector2) = 0;
        std::vector<std::unique_ptr<UINode>> children;
        int x,y,w,h;
};

class Container: public UINode{
    public:
        Container(Container&& other) noexcept;
        Container& operator=(Container&& other) noexcept;
        // delete copy construction and assignment
        Container(const Container&) = delete;
        Container& operator=(const Container&) = delete;
        Container(int mode, int alignMain, int alignCross, int spacing, int padding, int gap, Color bgColor){
            this->mode = mode;
            this->alignMain = alignMain;
            this->alignCross = alignCross;
            this->spacing = spacing;
            this->padding = padding;
            this->x = 0;
            this->y = 0;
            this->w = 2 * padding; //check for padding > 0 or this will be a nightmare later on lol
            this->h = 2 * padding;
            this->bgColor = bgColor;
            this->gap = gap;
        }
        void draw(int offsetX, int offsetY) override {
            drawSelf(offsetX, offsetY);
            int offsetXSum = offsetX + this->padding;
            int offsetYSum = offsetY + this->padding;
            for(auto& child: children){
                child->draw(0,0);
            }
        }
        void addChild(std::unique_ptr<UINode> child) override {
            if(this->mode == ROW){
                this->w += child->getWidth() + (gap * (this->children.size() > 0));
                child->setX(this->w - child->getWidth() - this->padding);
                child->setY(this->y + padding); //change for alignment etc
            }else{
                if(this->alignCross == START){
                    this->h += child->getHeight() + (gap * (this->children.size() > 0));
                    child->setY(this->h - child->getHeight() - this->padding);
                    child->setX(this->x + padding);
                }else if(this->alignCross == CENTER){
                    this->h += child->getHeight() + (gap * (this->children.size() > 0));
                    child->setY(this->h - child->getHeight() - this->padding);
                    child->setX(this->x + padding + (child->getWidth() - this->w - this->padding) / 2);
                }
                
            }
            if(child->getWidth() > this->w){this->w = child->getWidth() + 2 * padding;}
            if(child->getHeight() > this->h){this->h = child->getHeight() + 2 * padding;}
        
            children.push_back(std::move(child));
        }
        void setX(int x) override {
            this->x = x;
            for(auto& child: children){
                child->setX(child->getX() + x);
            }
        }
        void setY(int y) override {
            this->y = y;
            for(auto& child: children){
                child->setY(child->getY() + y);
            }
        }
    private:
        int mode; //row or col
        int alignMain; //alignment across main axis (either row or col)
        int alignCross; //alignment across cross axis
        int spacing; //spacing across main axis
        int padding;
        int gap;
        Color bgColor;
    protected:
        void drawSelf(int offsetX, int offsetY) override {
            DrawRectangle(this->x,this->y,this->w,this->h,this->bgColor);
        }
        void tickSelf(Vector2 v){
            return;
        }
};

Container::Container(Container&& other) noexcept
    : mode(other.mode),
      alignMain(other.alignMain),
      alignCross(other.alignCross),
      spacing(other.spacing),
      padding(other.padding),
      gap(other.gap),
      bgColor(other.bgColor)
{
    x = other.x;
    y = other.y;
    w = other.w;
    h = other.h;
    children = std::move(other.children);
}

Container& Container::operator=(Container&& other) noexcept {
    if (this != &other) {
        mode = other.mode;
        alignMain = other.alignMain;
        alignCross = other.alignCross;
        spacing = other.spacing;
        padding = other.padding;
        gap = other.gap;
        bgColor = other.bgColor;
        x = other.x;
        y = other.y;
        w = other.w;
        h = other.h;
        children = std::move(other.children);
    }
    return *this;
}

class TextLabel: public UINode {
    public:
        std::function<void(std::string)> onClick;
        TextLabel(std::string label, int x, int y, int fontSize, Color color, Color textColor, int padding){
            this->label = label;
            this->x = x;
            this->y = y;
            this->baseColor = color;
            Vector2 dimensions = MeasureTextEx(GetFontDefault(),label.c_str(),fontSize,3);
            this->h = dimensions.y;
            this->w = dimensions.x;
            this->fontSize = fontSize;
            this-> textColor = textColor;
            this->padding = padding;
            Rectangle rec = {(int) x,(int) y,(float) dimensions.x + 2 * padding, (float) dimensions.y + 2 * padding};
            this->backdrop = rec;
        }
        bool checkClick(Vector2 mousePos){
            return false;
        }
        void setX(int x) override {
            this->x = x; 
            Rectangle newBack = Rectangle{this->x,this->y,this->w + 2 * this->padding,this->h + 2 * this->padding};
            this->backdrop = newBack;
        }
        void setY(int y) override {
            this->y = y; 
            Rectangle newBack = Rectangle{this->x,this->y,this->w + 2 * this->padding,this->h + 2 * this->padding};
            this->backdrop = newBack;
        }
        int getWidth(){return this->backdrop.width;}
        int getHeight(){return this->backdrop.height;}
    protected:
        void tickSelf(Vector2 mousePos) override{
            return;
        }
        void drawSelf(int offsetX, int offsetY) override {
            DrawRectangleRec(this->backdrop,this->baseColor);
            DrawText(this->label.c_str(), this->x + this->padding, this->y + this->padding, this->fontSize, this->textColor);
        }

    private:
        int padding;
        int fontSize;
        Rectangle backdrop;
        std::string label;
        Color baseColor;
        Color textColor;
};

class Button: public UINode {
    public:
        std::function<void(std::string)> onClick;
        Button(std::string label, int x, int y, int fontSize, Color color, Color textColor, int padding, int animDuration, std::function<void(std::string)> callback){
            onClick = callback;
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
                    if (onClick) onClick(this->label);
                    return true;
                }
            }
            return false;
        }
        void setX(int x) override {
            this->x = x; 
            Rectangle newBack = Rectangle{this->x,this->y,this->w + 2 * this->padding,this->h + 2 * this->padding};
            this->backdrop = newBack;
        }
        void setY(int y) override {
            this->y = y; 
            Rectangle newBack = Rectangle{this->x,this->y,this->w + 2 * this->padding,this->h + 2 * this->padding};
            this->backdrop = newBack;
        }
        int getWidth(){return this->backdrop.width;}
        int getHeight(){return this->backdrop.height;}
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
        void drawSelf(int offsetX, int offsetY) override {
            Color btnColor;
            if(this->active){
                btnColor = this->activeColor;
            }else{
                btnColor = this->baseColor;
            }
            DrawRectangleRec(this->backdrop,btnColor);
            DrawText(this->label.c_str(), this->x + this->padding, this->y + this->padding, this->fontSize, this->textColor);
        }

    private:
        int padding;
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

class Slider: public UINode{

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
    InitWindow(1, 1, "RLCL");
    SetTargetFPS(30);
    Container UI = Container(COLUMN, START, START, CONCENTRATE, 10, 10, BLACK);
    Container topButtons = Container(ROW, START, START, CONCENTRATE, 10, 10, BLACK);
    topButtons.addChild(std::make_unique<TextLabel>("RLCL",0,0,30,RED,BLACK,5));
    topButtons.addChild(std::make_unique<TextLabel>("--",0,0,30,BLACK,LIGHTGRAY,5));
    topButtons.addChild(std::make_unique<Button>("ON",0,0,30,RED,BLACK,5,5,handleClick));
    topButtons.addChild(std::make_unique<TextLabel>(" / ",0,0,30,BLACK,LIGHTGRAY,5));

    Container startRow = Container(ROW, START, START, CONCENTRATE, 10, 10, BLACK);
    Container stopRow = Container(ROW, START, START, CONCENTRATE, 10, 10, BLACK);
    Button addCurrent = Button("Add current to preset",0,0,30,BLACK,WHITE,5,5,handleClick);
    startRow.addChild(std::make_unique<TextLabel>("Start.",0,0,30,BLACK,WHITE,5));
    stopRow.addChild(std::make_unique<TextLabel>("Stop. ",0,0,30,BLACK,WHITE,5));
    startRow.addChild(std::make_unique<TextLabel>("30%",0,0,30,BLACK,WHITE,5));
    stopRow.addChild(std::make_unique<TextLabel>("40%",0,0,30,BLACK,WHITE,5));
    stopRow.addChild(std::make_unique<Button>("-5%",0,0,30,RED,BLACK,5,5,handleClick));
    stopRow.addChild(std::make_unique<Button>("+5%",0,0,30,RED,BLACK,5,5,handleClick));
    startRow.addChild(std::make_unique<Button>("-5%",0,0,30,RED,BLACK,5,5,handleClick));
    startRow.addChild(std::make_unique<Button>("+5%",0,0,30,RED,BLACK,5,5,handleClick));
    topButtons.addChild(std::make_unique<Button>("OFF",0,0,30,DARKGRAY,BLACK,5,5,handleClick));
    UI.addChild(std::make_unique<Container>(std::move(topButtons)));
    UI.addChild(std::make_unique<Container>(std::move(startRow)));
    UI.addChild(std::make_unique<Container>(std::move(stopRow)));
    for(int i = 0; i < 10; i++){
        Container c0 = Container(ROW, START, START, CONCENTRATE, 10, 10, RED);
        c0.addChild(std::make_unique<TextLabel>(" 30 - 70 ",0,0,30,RED,BLACK,5));
        c0.addChild(std::make_unique<Button>("load",0,0,30,LIGHTGRAY,BLACK,5,5,handleClick));
        c0.addChild(std::make_unique<Button>("delete",0,0,30,LIGHTGRAY,BLACK,5,5,handleClick));
        UI.addChild(std::make_unique<Container>(std::move(c0)));
    }
    SetWindowSize(UI.getWidth(), UI.getHeight());

    while (!WindowShouldClose()) {
        UI.tick(GetMousePosition());
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        UI.draw(0,0);
        //DrawText(testBanner.c_str(),0,250,40,RED);
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