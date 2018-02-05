#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include "script.h"
#include "gui.h"

#define MAXLINES 999
#define MAXLENGTH 64
#define DRAWMAX 35
static int startline = 0;
char lines[MAXLINES][MAXLENGTH];
int numlines=1;
Vector2 cursorpos = {0,0};

static SpriteFont font;
static int WIDTH;
static int HEIGHT;
baslike_t script;

static Color ogreen1 = (Color){100, 125, 100, 255};
static Color ogreen2 = (Color){75, 255, 75, 255};
static Color ored1 = (Color){125, 100, 100, 255};
static Color ored2 = (Color){255, 75, 75, 255};

void DrawTextB(const char* text, int x, int y, int size, Color color)
{
    DrawTextEx(font, text, (Vector2){x,y}, size, 0, color);
}

void append(char subject[], const char insert[], int pos) {
    char buf[MAXLENGTH] = {};
    strncpy(buf, subject, pos);
    int len = strlen(buf);
    strcpy(buf+len, insert);
    len += strlen(insert);
    strcpy(buf+len, subject+pos);

    strcpy(subject, buf);
}

int main(int argc, char** argv)
{
    InitWindow(640, 480, "BASLIKE 0.4");
    SetTargetFPS(60);
    SetExitKey(KEY_F12);
    
    font = LoadSpriteFont("ProggyClean.ttf");
    WIDTH = MeasureTextEx(font, " ", 13, 0).x;
    HEIGHT = MeasureTextEx(font, " ", 13, 0).y;
    
    reset(&script);
    
    while (!WindowShouldClose()) {
        int c = GetKeyPressed();
        if (c!=-1 && strlen(lines[(int)cursorpos.y+startline]) < MAXLENGTH) {
            memmove(
                lines[(int)cursorpos.y+startline]+(int)cursorpos.x+1,
                lines[(int)cursorpos.y+startline]+(int)cursorpos.x,
                MAXLENGTH - ((int)cursorpos.x+1)
            );
            lines[(int)cursorpos.y+startline][(int)cursorpos.x] = c;
            cursorpos.x++;
        }
        if (IsKeyPressed(KEY_BACKSPACE)) {
            if (cursorpos.x > 0) {
                for(int i = (int)cursorpos.x-1; i < MAXLENGTH - 1; i++) lines[(int)cursorpos.y+startline][i] = lines[(int)cursorpos.y+startline][i + 1];
                cursorpos.x--;
            }
            else if (strlen(lines[(int)cursorpos.y+startline]) == 0 && cursorpos.y > 0) {
                for (int i = cursorpos.y+startline; i < MAXLINES-1; i++) {
                    strcpy(lines[i], lines[i+1]);
                }
                
                if (cursorpos.y < 2 && startline > 0){ startline--; } else { cursorpos.y--; }
                cursorpos.x = strlen(lines[(int)cursorpos.y+startline]);
                numlines--;
            }
        }
        if (IsKeyPressed(KEY_ENTER) && cursorpos.x<MAXLINES) {
            for (int i = MAXLINES-1; i > cursorpos.y+startline+1; i--) {
                strcpy(lines[i], lines[i-1]);
            }
            if (cursorpos.y > 30){ startline++; } else { cursorpos.y++; }
            memset(lines[(int)cursorpos.y+startline], '\0', MAXLENGTH);
            if (cursorpos.x > strlen(lines[(int)cursorpos.y+startline])) cursorpos.x = strlen(lines[(int)cursorpos.y+startline]);
            numlines++;
        }
        if (IsKeyPressed(KEY_LEFT) && cursorpos.x > 0) {
            cursorpos.x--;
        }
        if (IsKeyPressed(KEY_RIGHT) && cursorpos.x < strlen(lines[(int)cursorpos.y+startline])) {
            cursorpos.x++;
        }
        if (IsKeyPressed(KEY_UP) && cursorpos.y+startline > 0) {
            if (cursorpos.y < 1){ startline--; } else { cursorpos.y--; }
            if (cursorpos.x > strlen(lines[(int)cursorpos.y+startline])) cursorpos.x = strlen(lines[(int)cursorpos.y+startline]);
        }
        if (IsKeyPressed(KEY_DOWN) && cursorpos.y+startline < numlines-1) {
            if (cursorpos.y > 30){ startline++; } else { cursorpos.y++; }
            if (cursorpos.x > strlen(lines[(int)cursorpos.y+startline])) cursorpos.x = strlen(lines[(int)cursorpos.y+startline]);
        }
        if (IsKeyPressed(KEY_TAB) && strlen(lines[(int)cursorpos.y+startline]) < MAXLENGTH-4) {
            for (int i = 0; i < 4; i++) {
                memmove(
                    lines[(int)cursorpos.y+startline]+(int)cursorpos.x+1,
                    lines[(int)cursorpos.y+startline]+(int)cursorpos.x,
                    MAXLENGTH - ((int)cursorpos.x+1)
                );
                lines[(int)cursorpos.y+startline][(int)cursorpos.x] = c;
                cursorpos.x++;
            }
        }
        if (IsKeyPressed(KEY_HOME)) {
            cursorpos.x = 0;
        }
        if (IsKeyPressed(KEY_END)) {
            cursorpos.x = strlen(lines[(int)cursorpos.y+startline]);
        }
        
        if (IsFileDropped()) {
            int count;
            char** files = GetDroppedFiles(&count);
            printf("FILE DROPPED %d: %s\n", 0, files[0]);
            FILE* fp;
            char buffer[MAXLENGTH];
            fp = fopen(files[0], "r");
            int i = 0;
            while (fgets(buffer, sizeof(buffer), fp) != NULL) {
                if (buffer[strlen(buffer)-1] == '\n')
                    buffer[strlen(buffer)-1] = '\0';
                strcpy(lines[i], buffer);
                i++;
                numlines++;
            }
            fclose(fp);
            ClearDroppedFiles();
        }
        
        ClearBackground(BLACK);
        BeginDrawing();
        for (int i = 0; i < DRAWMAX; i++) {
            char* l = strlen(lines[startline+i])>0 ? lines[startline+i] : i+startline<numlines?"":"~";
            DrawTextB(FormatText("%03d: %s", startline+i, l), 10, 10+i*13, 13, RAYWHITE);
        }
        DrawRectangle(10+WIDTH*5+cursorpos.x*WIDTH, 10+cursorpos.y*HEIGHT, WIDTH+1, HEIGHT, (Color){155, 155, 155, 155});
        EndDrawing();
        DrawRectangle(640-160, 50, 155, 400, script.failed?ored1:ogreen1);
        DrawRectangleLines(640-160, 50, 155, 400, script.failed?ored2:ogreen2);
        DrawText(script.output, 640-150, 60, 10, WHITE);
        
        DrawRectangle(640-260, 50, 75, 400, script.failed?ored1:ogreen1);
        DrawRectangleLines(640-260, 50, 75, 400, script.failed?ored2:ogreen2);
        for (int i = 0; i < script.labelsize; i++) {
            DrawText(FormatText("%d:%s", script.labels[i], script.stack[script.labels[i]]), 640-250, 60+i*13, 10, WHITE);
        }
        
        gui_label("Output", 640-80-80, 30, 75, 25);
        gui_label("Labels", 640-270, 30, 75, 25);
        if (gui_button("Execute", 640-80-80, 5, 75, 25)) {
            char code[MAXLINES*MAXLENGTH] = "";
            for (int i = 0; i < numlines; i++) {
                strcat(code, lines[i]);
                strcat(code, " ");
            }
            execute(&script, code);
        }
        gui_button("Preprocess", 640-80, 5, 75, 25);
    }
    
    return 0;
}
