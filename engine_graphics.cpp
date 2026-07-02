/* ArcLight Engine - Graphical Launcher
 * Developer: Stephen
 * Opens an SDL2 window with OpenGL, renders the engine UI and a 3D scene.
 * Compiles: cl.exe /std:c++17 /EHsc /W3 /MT /O2 /I deps\sdl2\SDL2-2.30.3\include /Fe:builledoutput\arclight.exe engine_graphics.cpp deps\sdl2\SDL2-2.30.3\lib\x64\SDL2.lib deps\sdl2\SDL2-2.30.3\lib\x64\SDL2main.lib opengl32.lib
 */

#include <SDL.h>
#include <SDL_opengl.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <functional>

// ======================== Minimal Engine Types ========================

struct float3 {
    float x=0,y=0,z=0;
    float3()=default;
    float3(float x,float y,float z):x(x),y(y),z(z){}
    float3 operator+(const float3&o)const{return{x+o.x,y+o.y,z+o.z};}
    float3 operator-(const float3&o)const{return{x-o.x,y-o.y,z-o.z};}
    float3 operator*(float s)const{return{x*s,y*s,z*s};}
    float Length()const{return sqrtf(x*x+y*y+z*z);}
    float3 Normalize()const{float l=Length();return l>0?float3(x/l,y/l,z/l):float3();}
};

// ======================== GLU helpers (inline) ========================

inline void gluPerspective(double fovy, double aspect, double zNear, double zFar) {
    double f = 1.0 / tan(fovy * 3.14159265 / 360.0);
    double rangeInv = 1.0 / (zNear - zFar);
    double m[16] = {0};
    m[0] = f / aspect;
    m[5] = f;
    m[10] = (zFar + zNear) * rangeInv;
    m[11] = -1.0;
    m[14] = 2.0 * zFar * zNear * rangeInv;
    glMultMatrixd(m);
}

inline void gluLookAt(double eyeX, double eyeY, double eyeZ,
                       double cenX, double cenY, double cenZ,
                       double upX, double upY, double upZ) {
    double fx = cenX-eyeX, fy = cenY-eyeY, fz = cenZ-eyeZ;
    double fl = sqrt(fx*fx+fy*fy+fz*fz);
    fx/=fl; fy/=fl; fz/=fl;
    double sx = fy*upZ - fz*upY, sy = fz*upX - fx*upZ, sz = fx*upY - fy*upX;
    double sl = sqrt(sx*sx+sy*sy+sz*sz);
    sx/=sl; sy/=sl; sz/=sl;
    double ux = sy*fz - sz*fy, uy = sz*fx - sx*fz, uz = sx*fy - sy*fx;
    double m[16] = {0};
    m[0]=sx; m[4]=sy; m[8]=sz;  m[12]=-(sx*eyeX+sy*eyeY+sz*eyeZ);
    m[1]=ux; m[5]=uy; m[9]=uz;  m[13]=-(ux*eyeX+uy*eyeY+uz*eyeZ);
    m[2]=fx; m[6]=fy; m[10]=fz; m[14]=-(fx*eyeX+fy*eyeY+fz*eyeZ);
    m[3]=0;  m[7]=0;  m[11]=0;  m[15]=1.0;
    glMultMatrixd(m);
}

// ======================== Simple 3D Camera ========================

struct Camera {
    float3 pos = float3(0, 15, 25);
    float yaw = 0, pitch = -30;
    float speed = 15.0f;
    float rotSpeed = 90.0f;

    void MoveForward(float dt) {
        float yr = yaw * 3.14159f / 180;
        pos.x -= sinf(yr) * speed * dt;
        pos.z -= cosf(yr) * speed * dt;
    }
    void MoveBackward(float dt) {
        float yr = yaw * 3.14159f / 180;
        pos.x += sinf(yr) * speed * dt;
        pos.z += cosf(yr) * speed * dt;
    }
    void MoveLeft(float dt) {
        float yr = yaw * 3.14159f / 180;
        pos.x -= cosf(yr) * speed * dt;
        pos.z += sinf(yr) * speed * dt;
    }
    void MoveRight(float dt) {
        float yr = yaw * 3.14159f / 180;
        pos.x += cosf(yr) * speed * dt;
        pos.z -= sinf(yr) * speed * dt;
    }
    void Rotate(float dy, float dp) {
        yaw += dy;
        pitch = fmaxf(-89, fminf(89, pitch + dp));
    }
    void Zoom(float d) { pos.y = fmaxf(2, fminf(100, pos.y - d)); }

    void Apply() {
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        float yr = yaw * 3.14159f / 180;
        float pr = pitch * 3.14159f / 180;
        float3 dir(sinf(yr)*cosf(pr), sinf(pr), cosf(yr)*cosf(pr));
        float3 target = pos + dir;
        gluLookAt(pos.x, pos.y, pos.z, target.x, target.y, target.z, 0, 1, 0);
    }
};

// ======================== Simple Unit ========================

struct GameUnit {
    float3 pos;
    float3 color;
    float health = 100;
    float size = 0.8f;
    bool selected = false;
    std::string name;
    int team = 0;
};

// ======================== UI State ========================

struct UIState {
    bool showMinimap = true;
    bool showUnitInfo = true;
    bool showCommands = true;
    bool showProduction = false;
    int selectedUnit = -1;
    float resourceMinerals = 12450;
    float resourceEnergy = 8230;
    float resourceGas = 5100;
    int supply = 120;
    int supplyMax = 200;
    float fps = 60;
    int frameCount = 0;
    float fpsTimer = 0;
};

// ======================== Rendering Helpers ========================

void DrawGround() {
    // Grid
    glColor3f(0.1f, 0.25f, 0.1f);
    glBegin(GL_LINES);
    for (int i = -20; i <= 20; i++) {
        glVertex3f((float)i, 0, -20.0f);
        glVertex3f((float)i, 0, 20.0f);
        glVertex3f(-20.0f, 0, (float)i);
        glVertex3f(20.0f, 0, (float)i);
    }
    glEnd();

    // Terrain patches
    for (int x = -10; x < 10; x++) {
        for (int z = -10; z < 10; z++) {
            float r = 0.15f + fmodf((float)(x*7+z*13), 10.0f) * 0.02f;
            float g = 0.25f + fmodf((float)(x*3+z*11), 10.0f) * 0.02f;
            float b = 0.1f + fmodf((float)(x*11+z*3), 10.0f) * 0.01f;
            glColor3f(r, g, b);
            glBegin(GL_QUADS);
            glVertex3f((float)x, 0.01f, (float)z);
            glVertex3f((float)x+1, 0.01f, (float)z);
            glVertex3f((float)x+1, 0.01f, (float)z+1);
            glVertex3f((float)x, 0.01f, (float)z+1);
            glEnd();
        }
    }
}

void DrawUnit(const GameUnit& unit) {
    float s = unit.size;
    float3 p = unit.pos;

    // Selection ring
    if (unit.selected) {
        glColor4f(0.0f, 1.0f, 0.0f, 0.8f);
        glLineWidth(2.0f);
        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 16; i++) {
            float a = (float)i / 16.0f * 6.28318f;
            glVertex3f(p.x + cosf(a)*s*1.3f, 0.02f, p.z + sinf(a)*s*1.3f);
        }
        glEnd();
        glLineWidth(1.0f);
    }

    // Unit body
    glColor3fv(&unit.color.x);
    glBegin(GL_QUADS);
    // Front face
    glVertex3f(p.x-s, p.y+s*2, p.z+s);
    glVertex3f(p.x+s, p.y+s*2, p.z+s);
    glVertex3f(p.x+s, p.y, p.z+s);
    glVertex3f(p.x-s, p.y, p.z+s);
    // Back face
    glVertex3f(p.x+s, p.y+s*2, p.z-s);
    glVertex3f(p.x-s, p.y+s*2, p.z-s);
    glVertex3f(p.x-s, p.y, p.z-s);
    glVertex3f(p.x+s, p.y, p.z-s);
    // Top face
    glVertex3f(p.x-s, p.y+s*2, p.z-s);
    glVertex3f(p.x+s, p.y+s*2, p.z-s);
    glVertex3f(p.x+s, p.y+s*2, p.z+s);
    glVertex3f(p.x-s, p.y+s*2, p.z+s);
    glEnd();

    // Glow effect
    glColor4f(unit.color.x*0.5f, unit.color.y*0.5f, unit.color.z*0.5f, 0.3f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(p.x, p.y+s*2+0.5f, p.z);
    for (int i = 0; i <= 8; i++) {
        float a = (float)i / 8.0f * 6.28318f;
        glVertex3f(p.x + cosf(a)*0.3f, p.y+s*2+0.2f, p.z + sinf(a)*0.3f);
    }
    glEnd();
}

void DrawBuilding(const float3& pos, float3 color, float w, float h, float d) {
    glColor3fv(&color.x);
    glBegin(GL_QUADS);
    // Front
    glVertex3f(pos.x-w, pos.y+h, pos.z+d);
    glVertex3f(pos.x+w, pos.y+h, pos.z+d);
    glVertex3f(pos.x+w, pos.y, pos.z+d);
    glVertex3f(pos.x-w, pos.y, pos.z+d);
    // Back
    glVertex3f(pos.x+w, pos.y+h, pos.z-d);
    glVertex3f(pos.x-w, pos.y+h, pos.z-d);
    glVertex3f(pos.x-w, pos.y, pos.z-d);
    glVertex3f(pos.x+w, pos.y, pos.z-d);
    // Top
    glVertex3f(pos.x-w, pos.y+h, pos.z-d);
    glVertex3f(pos.x+w, pos.y+h, pos.z-d);
    glVertex3f(pos.x+w, pos.y+h, pos.z+d);
    glVertex3f(pos.x-w, pos.y+h, pos.z+d);
    // Left
    glVertex3f(pos.x-w, pos.y+h, pos.z+d);
    glVertex3f(pos.x-w, pos.y+h, pos.z-d);
    glVertex3f(pos.x-w, pos.y, pos.z-d);
    glVertex3f(pos.x-w, pos.y, pos.z+d);
    // Right
    glVertex3f(pos.x+w, pos.y+h, pos.z-d);
    glVertex3f(pos.x+w, pos.y+h, pos.z+d);
    glVertex3f(pos.x+w, pos.y, pos.z+d);
        glVertex3f(pos.x+w, pos.y, pos.z-d);
    glEnd();

    // Energy glow on top
    glColor4f(color.x*0.8f, color.y*0.8f, 1.0f, 0.5f);
    glBegin(GL_TRIANGLE_FAN);
    glVertex3f(pos.x, pos.y+h+1.0f, pos.z);
    for (int i = 0; i <= 8; i++) {
        float a = (float)i / 8.0f * 6.28318f;
        glVertex3f(pos.x + cosf(a)*w*0.8f, pos.y+h+0.5f, pos.z + sinf(a)*d*0.8f);
    }
    glEnd();
}

// ======================== HUD Drawing ========================

void DrawHUD(const UIState& ui, int screenW, int screenH) {
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    // 2D overlay
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0, screenW, screenH, 0, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Top bar background
    glColor4f(0.05f, 0.08f, 0.15f, 0.9f);
    glBegin(GL_QUADS);
    glVertex2f(0, 0); glVertex2f((float)screenW, 0);
    glVertex2f((float)screenW, 36); glVertex2f(0, 36);
    glEnd();

    // Top bar accent line
    glColor4f(0.0f, 0.83f, 1.0f, 0.5f);
    glBegin(GL_LINES);
    glVertex2f(0, 36); glVertex2f((float)screenW, 36);
    glEnd();

    // Minimap background
    if (ui.showMinimap) {
        float mx = 10, my = (float)screenH - 210, mw = 200, mh = 200;
        glColor4f(0.05f, 0.1f, 0.15f, 0.9f);
        glBegin(GL_QUADS);
        glVertex2f(mx, my); glVertex2f(mx+mw, my);
        glVertex2f(mx+mw, my+mh); glVertex2f(mx, my+mh);
        glEnd();
        glColor4f(0.0f, 0.83f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(mx, my); glVertex2f(mx+mw, my);
        glVertex2f(mx+mw, my+mh); glVertex2f(mx, my+mh);
        glEnd();
    }

    // Unit info panel
    if (ui.showUnitInfo && ui.selectedUnit >= 0) {
        float px = 280, py = (float)screenH - 130, pw = 400, ph = 120;
        glColor4f(0.05f, 0.08f, 0.15f, 0.95f);
        glBegin(GL_QUADS);
        glVertex2f(px, py); glVertex2f(px+pw, py);
        glVertex2f(px+pw, py+ph); glVertex2f(px, py+ph);
        glEnd();
        glColor4f(0.0f, 0.83f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(px, py); glVertex2f(px+pw, py);
        glVertex2f(px+pw, py+ph); glVertex2f(px, py+ph);
        glEnd();
    }

    // Command panel
    if (ui.showCommands) {
        float cx = (float)screenW - 260, cy = (float)screenH - 130, cw = 250, ch = 120;
        glColor4f(0.05f, 0.08f, 0.15f, 0.95f);
        glBegin(GL_QUADS);
        glVertex2f(cx, cy); glVertex2f(cx+cw, cy);
        glVertex2f(cx+cw, cy+ch); glVertex2f(cx, cy+ch);
        glEnd();
        glColor4f(0.0f, 0.83f, 1.0f, 0.5f);
        glBegin(GL_LINE_LOOP);
        glVertex2f(cx, cy); glVertex2f(cx+cw, cy);
        glVertex2f(cx+cw, cy+ch); glVertex2f(cx, cy+ch);
        glEnd();

        // Command buttons
        float bw = 55, bh = 45, bx = cx+10, by = cy+10;
        const char* cmds[] = {"Move","Atk","Stop","Hld","Ptrl"};
        for (int i = 0; i < 5; i++) {
            glColor4f(0.1f, 0.16f, 0.25f, 0.9f);
            glBegin(GL_QUADS);
            glVertex2f(bx+i*(bw+5), by); glVertex2f(bx+i*(bw+5)+bw, by);
            glVertex2f(bx+i*(bw+5)+bw, by+bh); glVertex2f(bx+i*(bw+5), by+bh);
            glEnd();
            glColor4f(0.0f, 0.5f, 0.7f, 0.8f);
            glBegin(GL_LINE_LOOP);
            glVertex2f(bx+i*(bw+5), by); glVertex2f(bx+i*(bw+5)+bw, by);
            glVertex2f(bx+i*(bw+5)+bw, by+bh); glVertex2f(bx+i*(bw+5), by+bh);
            glEnd();
        }
    }

    // FPS
    glColor3f(0.5f, 0.6f, 0.7f);
    char fpsBuf[32];
    snprintf(fpsBuf, sizeof(fpsBuf), "FPS: %.0f", ui.fps);

    // Version
    glColor3f(0.3f, 0.4f, 0.5f);
    char verBuf[64];
    snprintf(verBuf, sizeof(verBuf), "ArcLight Engine v1.0.0 | Developer: Stephen");

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
}

// ======================== Main ========================

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) < 0) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    int screenW = 1280, screenH = 720;
    SDL_Window* window = SDL_CreateWindow(
        "ArcLight Engine v1.0.0 - Developer: Stephen",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        screenW, screenH,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE
    );
    if (!window) {
        fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext glCtx = SDL_GL_CreateContext(window);
    if (!glCtx) {
        fprintf(stderr, "SDL_GL_CreateContext failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_GL_SetSwapInterval(1); // VSync

    // OpenGL setup
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.04f, 0.06f, 0.1f, 1.0f);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (double)screenW / screenH, 0.1, 1000.0);
    glMatrixMode(GL_MODELVIEW);

    Camera camera;
    UIState ui;

    // Create game units
    std::vector<GameUnit> units;
    auto addUnit = [&](float x, float z, float r, float g, float b, const std::string& n, int t) {
        GameUnit u; u.pos = float3(x, 0, z); u.color = float3(r, g, b); u.name = n; u.team = t;
        units.push_back(u);
    };

    // Friendly units (blue team)
    addUnit(2, 0, 0.2f, 0.5f, 1.0f, "Mech Alpha", 0);
    addUnit(4, 1, 0.2f, 0.5f, 1.0f, "Mech Beta", 0);
    addUnit(1, 2, 0.3f, 0.6f, 0.9f, "Scout 1", 0);
    addUnit(3, 3, 0.3f, 0.6f, 0.9f, "Scout 2", 0);
    addUnit(0, 1, 0.1f, 0.4f, 0.8f, "Tank 1", 0);
    addUnit(5, 0, 0.1f, 0.4f, 0.8f, "Tank 2", 0);

    // Enemy units (red team)
    addUnit(10, 5, 1.0f, 0.3f, 0.2f, "Enemy Mech", 1);
    addUnit(12, 7, 1.0f, 0.3f, 0.2f, "Enemy Tank", 1);
    addUnit(11, 6, 0.9f, 0.2f, 0.3f, "Enemy Scout", 1);

    // Buildings
    struct Building { float3 pos; float3 color; float w,h,d; std::string name; };
    std::vector<Building> buildings;
    buildings.push_back({float3(-3,0,-2), float3(0.1f,0.3f,0.8f), 2.0f, 3.0f, 2.0f, "Command Center"});
    buildings.push_back({float3(-6,0,0), float3(0.2f,0.6f,0.3f), 1.5f, 2.0f, 1.5f, "Power Plant"});
    buildings.push_back({float3(-5,0,-4), float3(0.6f,0.5f,0.2f), 1.0f, 1.5f, 1.0f, "Barracks"});
    buildings.push_back({float3(8,0,4), float3(0.8f,0.2f,0.2f), 1.5f, 2.5f, 1.5f, "Enemy Base"});
    buildings.push_back({float3(14,0,6), float3(0.7f,0.3f,0.1f), 1.0f, 2.0f, 1.0f, "Enemy Turret"});

    // Main loop
    bool running = true;
    Uint32 lastTick = SDL_GetTicks();
    SDL_Event event;

    while (running) {
        Uint32 now = SDL_GetTicks();
        float dt = (now - lastTick) / 1000.0f;
        lastTick = now;
        if (dt > 0.1f) dt = 0.1f;

        // FPS counter
        ui.frameCount++;
        ui.fpsTimer += dt;
        if (ui.fpsTimer >= 1.0f) {
            ui.fps = ui.frameCount / ui.fpsTimer;
            ui.frameCount = 0;
            ui.fpsTimer = 0;
        }

        // Input
        const Uint8* keys = SDL_GetKeyboardState(NULL);
        if (keys[SDL_SCANCODE_W]) camera.MoveForward(dt);
        if (keys[SDL_SCANCODE_S]) camera.MoveBackward(dt);
        if (keys[SDL_SCANCODE_A]) camera.MoveLeft(dt);
        if (keys[SDL_SCANCODE_D]) camera.MoveRight(dt);
        if (keys[SDL_SCANCODE_Q]) camera.Rotate(-90*dt, 0);
        if (keys[SDL_SCANCODE_E]) camera.Rotate(90*dt, 0);
        if (keys[SDL_SCANCODE_PAGEUP]) camera.Zoom(10*dt);
        if (keys[SDL_SCANCODE_PAGEDOWN]) camera.Zoom(-10*dt);

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_TAB) ui.showMinimap = !ui.showMinimap;
            if (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_F1) ui.showCommands = !ui.showCommands;
            if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT) {
                // Click to select unit
                ui.selectedUnit = -1;
                for (int i = 0; i < (int)units.size(); i++) {
                    float dx = units[i].pos.x - (units[i].pos.x);
                    float dz = units[i].pos.z - (units[i].pos.z);
                    // Simple proximity check
                    float3 clickWorld = camera.pos + float3(0, -camera.pos.y, 0);
                    float dist = (units[i].pos - clickWorld).Length();
                    if (dist < 5.0f) {
                        ui.selectedUnit = i;
                        for (auto& u : units) u.selected = false;
                        units[i].selected = true;
                        break;
                    }
                }
            }
            if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON_RMASK)) {
                camera.Rotate(event.motion.xrel * 0.3f, -event.motion.yrel * 0.3f);
            }
            if (event.type == SDL_MOUSEWHEEL) {
                camera.Zoom(event.wheel.y * 2.0f);
            }
            if (event.type == SDL_WINDOWEVENT && event.window.event == SDL_WINDOWEVENT_RESIZED) {
                screenW = event.window.data1;
                screenH = event.window.data2;
                glViewport(0, 0, screenW, screenH);
                glMatrixMode(GL_PROJECTION);
                glLoadIdentity();
                gluPerspective(60.0, (double)screenW / screenH, 0.1, 1000.0);
                glMatrixMode(GL_MODELVIEW);
            }
        }

        // Simple unit AI - units move toward each other
        for (auto& u : units) {
            if (u.team == 0) {
                // Move toward nearest enemy
                float nearestDist = 999;
                for (auto& e : units) {
                    if (e.team != u.team) {
                        float d = (e.pos - u.pos).Length();
                        if (d < nearestDist) nearestDist = d;
                    }
                }
                if (nearestDist > 3.0f) {
                    for (auto& e : units) {
                        if (e.team != u.team) {
                            float3 dir = (e.pos - u.pos).Normalize();
                            u.pos = u.pos + dir * 1.5f * dt;
                            break;
                        }
                    }
                }
            }
        }

        // Render
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        camera.Apply();

        // Lighting
        glEnable(GL_LIGHTING);
        glEnable(GL_LIGHT0);
        GLfloat lightPos[] = {10.0f, 20.0f, 10.0f, 0.0f};
        GLfloat lightAmb[] = {0.3f, 0.3f, 0.35f, 1.0f};
        GLfloat lightDiff[] = {0.8f, 0.8f, 0.85f, 1.0f};
        glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
        glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmb);
        glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiff);

        DrawGround();

        glDisable(GL_LIGHTING);
        for (auto& b : buildings) DrawBuilding(b.pos, b.color, b.w, b.h, b.d);
        for (auto& u : units) DrawUnit(u);
        glEnable(GL_LIGHTING);

        glDisable(GL_LIGHTING);
        DrawHUD(ui, screenW, screenH);

        SDL_GL_SwapWindow(window);
    }

    SDL_GL_DeleteContext(glCtx);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
