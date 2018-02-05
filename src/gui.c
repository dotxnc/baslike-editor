#include "gui.h"

void gui_label(const char* text, int x, int y, int w, int h)
{
    Vector2 mpos = GetMousePosition();
    GUISTATE state = NORMAL;
    
    int tw = MeasureText(text, 10);
    int th = 10;
    
    if (w < tw) w = tw;
    if (h < th) h = th;
    
    if (CheckCollisionPointRec(mpos, (Rectangle){x,y,w,h})) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else state = FOCUSED;
    }
    
    switch (state)
    {
        case NORMAL: DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){144,172,180,255}); break;
        case FOCUSED: DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){106,169,184,255}); break;
        case PRESSED: DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){151,175,129,255}); break;
        default: break;
    }
}

bool gui_button(const char* text, int x, int y, int w, int h)
{
    Vector2 mpos = GetMousePosition();
    bool clicked = false;
    GUISTATE state = NORMAL;
    
    int tw = MeasureText(text, 10);
    int th = 10;
    
    if (w < tw) w = tw;
    if (h < th) h = th;
    
    if (CheckCollisionPointRec(mpos, (Rectangle){x,y,w,h})) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
        else state = FOCUSED;
    }
    
    switch (state)
    {
        case NORMAL:
            DrawRectangle(x, y, w, h, (Color){52,64,65,255});
            DrawRectangleLines(x, y, w, h, (Color){96,130,125,255});
            DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){144,172,180,255});
            break;
        case FOCUSED:
            DrawRectangle(x, y, w, h, (Color){51,79,89,255});
            DrawRectangleLines(x, y, w, h, (Color){95,154,168,255});
            DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){106,169,184,255});
            break;
        case PRESSED:
            DrawRectangle(x, y, w, h, (Color){59,99,87,255});
            DrawRectangleLines(x, y, w, h, (Color){169,203,141,255});
            DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){151,175,129,255});
            break;
        default:
            break;
    }
    
    return clicked;
}

bool gui_toggle(const char* text, int x, int y, int w, int h, bool active)
{
    Vector2 mpos = GetMousePosition();
    bool clicked = false;
    GUISTATE state = NORMAL;
    
    int tw = MeasureText(text, 10);
    int th = 10;
    
    if (w < tw) w = tw;
    if (h < th) h = th;
    
    if (CheckCollisionPointRec(mpos, (Rectangle){x,y,w,h})) {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            state = NORMAL;
            active = !active;
        }
        else state = FOCUSED;
    }
    
    switch (state)
    {
        case NORMAL:
            if (active) {
                DrawRectangle(x, y, w, h, (Color){59,99,87,255});
                DrawRectangleLines(x, y, w, h, (Color){169,203,141,255});
                DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){151,175,129,255});
            } else {
                DrawRectangle(x, y, w, h, (Color){52,64,65,255});
                DrawRectangleLines(x, y, w, h, (Color){96,130,125,255});
                DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){144,172,180,255});
            }
            break;
        case FOCUSED:
            DrawRectangle(x, y, w, h, (Color){51,79,89,255});
            DrawRectangleLines(x, y, w, h, (Color){95,154,168,255});
            DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){106,169,184,255});
            break;
        case PRESSED:
            DrawRectangle(x, y, w, h, (Color){59,99,87,255});
            DrawRectangleLines(x, y, w, h, (Color){169,203,141,255});
            DrawText(text, x+w/2-tw/2, y+h/2-th/2, 10, (Color){151,175,129,255});
            break;
        default:
            break;
    }
    return active;
}

int gui_tgroup(char** text, int x, int y, int w, int h, int count, int active)
{
    for (int i = 0; i < count; i++)
    {
        if (i == active) gui_toggle(text[i], x+i*(w+5), y, w, h, true);
        else if(gui_toggle(text[i], x+i*(w+5), y, w, h, false) == true) active = i;
    }
    return active;
}

void gui_textbox(char* text, int x, int y, int w, int h, int max)
{
    static int frames = 0;
    static int backspace = 259;
    
    Vector2 mpos = GetMousePosition();
    GUISTATE state = NORMAL;
    
    int tw = MeasureText(text, 10);
    
    if (tw+4 > w) {
        w = tw+10;
    }
    
    if (CheckCollisionPointRec(mpos, (Rectangle){x,y,w,h}))
    {
        state = FOCUSED;
        frames++;
        
        int letter = -1;
        letter = GetKeyPressed();
        
        if (letter != -1) {
            if (letter == backspace) {
                for (int i = 0; i < max; i++) {
                    if (text[i]=='\0' && i>0) {
                        text[i-1] = '\0';
                        break;
                    }
                }
                text[max-1] = '\0';
            }
            else {
                if (letter > 31 && letter < 127) {
                    for (int i = 0; i < max; i++)
                    {
                        if (text[i] == '\0')
                        {
                            text[i] = (char)(letter);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    switch (state)
    {
        case NORMAL:
            DrawRectangle(x, y, w, h, (Color){52,64,65,255});
            DrawRectangleLines(x, y, w, h, (Color){96,130,125,255});
            DrawText(text, x+4, y+h/2-10/2, 10, (Color){144,172,180,255});
            break;
        case FOCUSED:
            DrawRectangle(x, y, w, h, (Color){51,79,89,255});
            DrawRectangleLines(x, y, w, h, (Color){95,154,168,255});
            DrawText(text, x+4, y+h/2-10/2, 10, (Color){106,169,184,255});
            
            if ((frames/20)%2 == 0) DrawRectangle(x+4+tw, y+2, 1, h-4, (Color){95,154,168,255});
            break;
        case PRESSED:
            break;
        default:
            break;
    }
    
}
