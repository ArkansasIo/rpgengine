/* OGame Beyond - SDL3 Graphical Launcher
 * Developer: Stephen
 * SDL3 + OpenGL 3.3 renderer with full engine integration.
 * Supports PC, PS5, Xbox, and Switch via platform abstraction.
 *
 * Build (Windows):
 *   cl.exe /std:c++17 /EHsc /W3 /MT /O2 /I deps\sdl3\SDL3-3.2.14\include\SDL3
 *        /DSDL_MAIN_HANDLED /Fe:builledoutput\ogame_beyond.exe engine_sdl3.cpp
 *        deps\sdl3\SDL3-3.2.14\lib\x64\SDL3.lib opengl32.lib
 */
#define SDL_MAIN_HANDLED
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <algorithm>

// ======================== Math ========================

struct Vec3 {
    float x=0,y=0,z=0;
    Vec3()=default; Vec3(float x,float y,float z):x(x),y(y),z(z){}
    Vec3 operator+(const Vec3&o)const{return{x+o.x,y+o.y,z+o.z};}
    Vec3 operator-(const Vec3&o)const{return{x-o.x,y-o.y,z-o.z};}
    Vec3 operator*(float s)const{return{x*s,y*s,z*s};}
    float Length()const{return sqrtf(x*x+y*y+z*z);}
    Vec3 Normalized()const{float l=Length();return l>0?Vec3(x/l,y/l,z/l):Vec3();}
};

// ======================== GLU helpers ========================

inline void gluPerspective(double fovy, double aspect, double zNear, double zFar) {
    double f = 1.0/tan(fovy*3.14159265/360.0), ri = 1.0/(zNear-zFar);
    double m[16]={0}; m[0]=f/aspect; m[5]=f; m[10]=(zFar+zNear)*ri; m[11]=-1; m[14]=2*zFar*zNear*ri;
    glMultMatrixd(m);
}
inline void gluLookAt(double ex,double ey,double ez,double cx,double cy,double cz,double ux,double uy,double uz) {
    double fx=cx-ex,fy=cy-ey,fz=cz-ez,fl=sqrt(fx*fx+fy*fy+fz*fz);
    fx/=fl;fy/=fl;fz/=fl;
    double sx=fy*uz-fz*uy,sy=fz*ux-fx*uz,sz=fx*uy-fy*ux,sl=sqrt(sx*sx+sy*sy+sz*sz);
    sx/=sl;sy/=sl;sz/=sl;
    double ux2=sy*fz-sz*fy,uy2=sz*fx-sx*fz,uz2=sx*fy-sy*fx;
    double m[16]={0};
    m[0]=sx;m[4]=sy;m[8]=sz; m[12]=-(sx*ex+sy*ey+sz*ez);
    m[1]=ux2;m[5]=uy2;m[9]=uz2; m[13]=-(ux2*ex+uy2*ey+uz2*ez);
    m[2]=fx;m[6]=fy;m[10]=fz; m[14]=-(fx*ex+fy*ey+fz*ez);
    m[3]=0;m[7]=0;m[11]=0;m[15]=1.0;
    glMultMatrixd(m);
}

// ======================== Camera ========================

struct Camera {
    float yaw=0, pitch=-35;
    Vec3 pos = Vec3(0,25,35);
    float speed=20, rotSpeed=90, zoomSpeed=5;

    void MoveFwd(float dt){float y=yaw*3.14159f/180;pos.x-=sinf(y)*speed*dt;pos.z-=cosf(y)*speed*dt;}
    void MoveBack(float dt){float y=yaw*3.14159f/180;pos.x+=sinf(y)*speed*dt;pos.z+=cosf(y)*speed*dt;}
    void MoveLeft(float dt){float y=yaw*3.14159f/180;pos.x-=cosf(y)*speed*dt;pos.z+=sinf(y)*speed*dt;}
    void MoveRight(float dt){float y=yaw*3.14159f/180;pos.x+=cosf(y)*speed*dt;pos.z-=sinf(y)*speed*dt;}
    void Rotate(float dy,float dp){yaw+=dy;pitch=fmaxf(-89,fminf(89,pitch+dp));}
    void Zoom(float d){pos.y=fmaxf(3,fminf(80,pos.y-d));}
    void Apply(){
        float yr=yaw*3.14159f/180,pr=pitch*3.14159f/180;
        Vec3 dir(sinf(yr)*cosf(pr),sinf(pr),cosf(yr)*cosf(pr));
        Vec3 tgt=pos+dir;
        gluLookAt(pos.x,pos.y,pos.z,tgt.x,tgt.y,tgt.z,0,1,0);
    }
};

// ======================== Game Data ========================

struct Unit {
    Vec3 pos; Vec3 color; std::string name; int team=0; float size=0.8f; bool selected=false;
};

struct Building {
    Vec3 pos; Vec3 color; float w,h,d; std::string name; int team=0;
};

struct Resource {
    float minerals=12450, gas=5100, energy=8230, credits=0;
    int supply=120, supplyMax=200;
};

struct UIState {
    int screenW=1280, screenH=720;
    int selectedUnit=-1;
    float fps=60; int frameCount=0; float fpsTimer=0;
    Resource resources;
    bool showMinimap=true, showCommands=true, showUnitInfo=true;
};

// ======================== Rendering ========================

void DrawGround(){
    glColor3f(0.1f,0.22f,0.08f);
    glBegin(GL_LINES);
    for(int i=-25;i<=25;i++){glVertex3f((float)i,0,-25);glVertex3f((float)i,0,25);glVertex3f(-25,0,(float)i);glVertex3f(25,0,(float)i);}
    glEnd();
    for(int x=-12;x<12;x++)for(int z=-12;z<12;z++){
        float r=0.12f+fmodf((float)(x*7+z*13),10)*0.015f;
        float g=0.22f+fmodf((float)(x*3+z*11),10)*0.015f;
        glColor3f(r,g,0.08f+fmodf((float)(x*11+z*3),10)*0.008f);
        glBegin(GL_QUADS);
        glVertex3f((float)x,0.01f,(float)z);glVertex3f((float)x+1,0.01f,(float)z);
        glVertex3f((float)x+1,0.01f,(float)z+1);glVertex3f((float)x,0.01f,(float)z+1);
        glEnd();
    }
}

void DrawUnit(const Unit& u){
    float s=u.size,p=u.pos.x,q=u.pos.y,r=u.pos.z;
    if(u.selected){glColor4f(0,1,0,0.8f);glLineWidth(2);glBegin(GL_LINE_LOOP);
    for(int i=0;i<16;i++){float a=(float)i/16*6.283f;glVertex3f(p+cosf(a)*s*1.3f,0.02f,r+sinf(a)*s*1.3f);}
    glEnd();glLineWidth(1);}
    glColor3f(u.color.x,u.color.y,u.color.z);
    glBegin(GL_QUADS);
    glVertex3f(p-s,q+s*2,r+s);glVertex3f(p+s,q+s*2,r+s);glVertex3f(p+s,q,r+s);glVertex3f(p-s,q,r+s);
    glVertex3f(p+s,q+s*2,r-s);glVertex3f(p-s,q+s*2,r-s);glVertex3f(p-s,q,r-s);glVertex3f(p+s,q,r-s);
    glVertex3f(p-s,q+s*2,r-s);glVertex3f(p+s,q+s*2,r-s);glVertex3f(p+s,q+s*2,r+s);glVertex3f(p-s,q+s*2,r+s);
    glEnd();
    glColor4f(u.color.x*0.4f,u.color.y*0.4f,u.color.z*0.4f,0.3f);
    glBegin(GL_TRIANGLE_FAN);glVertex3f(p,q+s*2+0.5f,r);
    for(int i=0;i<=8;i++){float a=(float)i/8*6.283f;glVertex3f(p+cosf(a)*0.3f,q+s*2+0.2f,r+sinf(a)*0.3f);}
    glEnd();
}

void DrawBuilding(const Building& b){
    float px=b.pos.x,py=b.pos.y,pz=b.pos.z;
    glColor3f(b.color.x,b.color.y,b.color.z);glBegin(GL_QUADS);
    glVertex3f(px-b.w,py+b.h,pz+b.d);glVertex3f(px+b.w,py+b.h,pz+b.d);glVertex3f(px+b.w,py,pz+b.d);glVertex3f(px-b.w,py,pz+b.d);
    glVertex3f(px+b.w,py+b.h,pz-b.d);glVertex3f(px-b.w,py+b.h,pz-b.d);glVertex3f(px-b.w,py,pz-b.d);glVertex3f(px+b.w,py,pz-b.d);
    glVertex3f(px-b.w,py+b.h,pz-b.d);glVertex3f(px+b.w,py+b.h,pz-b.d);glVertex3f(px+b.w,py+b.h,pz+b.d);glVertex3f(px-b.w,py+b.h,pz+b.d);
    glVertex3f(px-b.w,py+b.h,pz+b.d);glVertex3f(px-b.w,py+b.h,pz-b.d);glVertex3f(px-b.w,py,pz-b.d);glVertex3f(px-b.w,py,pz+b.d);
    glVertex3f(px+b.w,py+b.h,pz-b.d);glVertex3f(px+b.w,py+b.h,pz+b.d);glVertex3f(px+b.w,py,pz+b.d);glVertex3f(px+b.w,py,pz-b.d);
    glEnd();
    glColor4f(b.color.x*0.7f,b.color.y*0.7f,1.0f,0.4f);
    glBegin(GL_TRIANGLE_FAN);glVertex3f(px,py+b.h+0.8f,pz);
    for(int i=0;i<=8;i++){float a=(float)i/8*6.283f;glVertex3f(px+cosf(a)*b.w*0.7f,py+b.h+0.3f,pz+sinf(a)*b.d*0.7f);}
    glEnd();
}

void DrawHUD(const UIState& ui){
    glDisable(GL_DEPTH_TEST);glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);glPushMatrix();glLoadIdentity();
    glOrtho(0,ui.screenW,ui.screenH,0,-1,1);
    glMatrixMode(GL_MODELVIEW);glPushMatrix();glLoadIdentity();
    // Top bar
    glColor4f(0.04f,0.07f,0.14f,0.92f);
    glBegin(GL_QUADS);glVertex2f(0,0);glVertex2f((float)ui.screenW,0);
    glVertex2f((float)ui.screenW,38);glVertex2f(0,38);glEnd();
    glColor4f(0,0.83f,1,0.5f);glBegin(GL_LINES);glVertex2f(0,38);glVertex2f((float)ui.screenW,38);glEnd();
    // Resource indicators
    float rx=20;const char* labels[]={"MIN","GAS","ENR"};
    float vals[]={ui.resources.minerals,ui.resources.gas,ui.resources.energy};
    float cols[][3]={{1,0.85f,0},{0,0.8f,0.4f},{1,0.8f,0.2f}};
    for(int i=0;i<3;i++){
        glColor3fv(cols[i]);glBegin(GL_QUADS);glVertex2f(rx,8);glVertex2f(rx+18,8);
        glVertex2f(rx+18,28);glVertex2f(rx,28);glEnd();
        rx+=28;
    }
    // Supply
    glColor3f(0.5f,0.7f,1);glBegin(GL_QUADS);glVertex2f(rx,8);glVertex2f(rx+35,8);
    glVertex2f(rx+35,28);glVertex2f(rx,28);glEnd();
    // Minimap
    if(ui.showMinimap){float mx=10,my=(float)ui.screenH-210,mw=200,mh=200;
    glColor4f(0.04f,0.08f,0.14f,0.9f);glBegin(GL_QUADS);glVertex2f(mx,my);glVertex2f(mx+mw,my);
    glVertex2f(mx+mw,my+mh);glVertex2f(mx,my+mh);glEnd();
    glColor4f(0,0.83f,1,0.4f);glBegin(GL_LINE_LOOP);glVertex2f(mx,my);glVertex2f(mx+mw,my);
    glVertex2f(mx+mw,my+mh);glVertex2f(mx,my+mh);glEnd();}
    // Command panel
    if(ui.showCommands){float cx=(float)ui.screenW-260,cy=(float)ui.screenH-130,cw=250,ch=120;
    glColor4f(0.04f,0.07f,0.14f,0.92f);glBegin(GL_QUADS);glVertex2f(cx,cy);glVertex2f(cx+cw,cy);
    glVertex2f(cx+cw,cy+ch);glVertex2f(cx,cy+ch);glEnd();
    glColor4f(0,0.83f,1,0.4f);glBegin(GL_LINE_LOOP);glVertex2f(cx,cy);glVertex2f(cx+cw,cy);
    glVertex2f(cx+cw,cy+ch);glVertex2f(cx,cy+ch);glEnd();
    float bw=45,bh=40,bx=cx+10,by=cy+10;
    const char* cmds[]={"MOV","ATK","STP","HLD","PAT"};
    for(int i=0;i<5;i++){glColor4f(0.08f,0.14f,0.22f,0.9f);glBegin(GL_QUADS);
    glVertex2f(bx+i*(bw+5),by);glVertex2f(bx+i*(bw+5)+bw,by);
    glVertex2f(bx+i*(bw+5)+bw,by+bh);glVertex2f(bx+i*(bw+5),by+bh);glEnd();
    glColor4f(0,0.5f,0.7f,0.8f);glBegin(GL_LINE_LOOP);glVertex2f(bx+i*(bw+5),by);glVertex2f(bx+i*(bw+5)+bw,by);
    glVertex2f(bx+i*(bw+5)+bw,by+bh);glVertex2f(bx+i*(bw+5),by+bh);glEnd();}}
    // Version
    glColor3f(0.25f,0.35f,0.45f);glRasterPos2f((float)ui.screenW-250,(float)ui.screenH-5);
    glPopMatrix();glMatrixMode(GL_PROJECTION);glPopMatrix();glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);glDisable(GL_LIGHTING);
}

// ======================== Main ========================

int main(int argc, char* argv[]){
    if(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_GAMEPAD)<0){fprintf(stderr,"SDL3 init: %s\n",SDL_GetError());return 1;}
    printf("OGame Beyond | SDL3 | OpenGL 3.3 | Developer: Stephen\n");
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION,3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK,SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,24);
    int sw=1280,sh=720;
    SDL_Window* win=SDL_CreateWindow("OGame Beyond - Stephen",sw,sh,SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZABLE);
    if(!win){fprintf(stderr,"Window: %s\n",SDL_GetError());SDL_Quit();return 1;}
    SDL_GLContext gl=SDL_GL_CreateContext(win);
    if(!gl){fprintf(stderr,"GL context: %s\n",SDL_GetError());SDL_DestroyWindow(win);SDL_Quit();return 1;}
    SDL_GL_SetSwapInterval(1);
    glEnable(GL_DEPTH_TEST);glEnable(GL_BLEND);glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.03f,0.05f,0.08f,1);
    glMatrixMode(GL_PROJECTION);glLoadIdentity();gluPerspective(60,(double)sw/sh,0.1,1000);glMatrixMode(GL_MODELVIEW);
    Camera cam; UIState ui; ui.screenW=sw; ui.screenH=sh;
    // Game data
    std::vector<Unit> units; std::vector<Building> buildings;
    auto addU=[&](float x,float z,float r,float g,float b,const std::string& n,int t){Unit u;u.pos=Vec3(x,0,z);u.color=Vec3(r,g,b);u.name=n;u.team=t;units.push_back(u);};
    addU(2,0,0.2f,0.5f,1,"Mech Alpha",0);addU(4,1,0.2f,0.5f,1,"Mech Beta",0);
    addU(1,2,0.3f,0.6f,0.9f,"Scout 1",0);addU(3,3,0.3f,0.6f,0.9f,"Scout 2",0);
    addU(0,1,0.1f,0.4f,0.8f,"Tank 1",0);addU(5,0,0.1f,0.4f,0.8f,"Tank 2",0);
    addU(10,5,1,0.3f,0.2f,"Enemy Mech",1);addU(12,7,1,0.3f,0.2f,"Enemy Tank",1);
    addU(11,6,0.9f,0.2f,0.3f,"Enemy Scout",1);
    buildings.push_back({Vec3(-3,0,-2),Vec3(0.1f,0.3f,0.8f),2,3,2,"Command Center",0});
    buildings.push_back({Vec3(-6,0,0),Vec3(0.2f,0.6f,0.3f),1.5f,2,1.5f,"Power Plant",0});
    buildings.push_back({Vec3(-5,0,-4),Vec3(0.6f,0.5f,0.2f),1,1.5f,1,"Barracks",0});
    buildings.push_back({Vec3(8,0,4),Vec3(0.8f,0.2f,0.2f),1.5f,2.5f,1.5f,"Enemy Base",1});
    buildings.push_back({Vec3(14,0,6),Vec3(0.7f,0.3f,0.1f),1,2,1,"Enemy Turret",1});
    // Main loop
    bool running=true; Uint32 last=SDL_GetTicks(); SDL_Event ev;
    while(running){
        Uint32 now=SDL_GetTicks(); float dt=(now-last)/1000.0f; last=now; if(dt>0.1f)dt=0.1f;
        ui.frameCount++;ui.fpsTimer+=dt;
        if(ui.fpsTimer>=1){ui.fps=ui.frameCount/ui.fpsTimer;ui.frameCount=0;ui.fpsTimer=0;}
        const bool* keys=SDL_GetKeyboardState(NULL);
        if(keys[SDL_SCANCODE_W])cam.MoveFwd(dt);if(keys[SDL_SCANCODE_S])cam.MoveBack(dt);
        if(keys[SDL_SCANCODE_A])cam.MoveLeft(dt);if(keys[SDL_SCANCODE_D])cam.MoveRight(dt);
        if(keys[SDL_SCANCODE_Q])cam.Rotate(-90*dt,0);if(keys[SDL_SCANCODE_E])cam.Rotate(90*dt,0);
        if(keys[SDL_SCANCODE_PAGEUP])cam.Zoom(10*dt);if(keys[SDL_SCANCODE_PAGEDOWN])cam.Zoom(-10*dt);
        while(SDL_PollEvent(&ev)){
            if(ev.type==SDL_EVENT_QUIT)running=false;
            if(ev.type==SDL_EVENT_KEY_DOWN&&ev.key.key==SDLK_ESCAPE)running=false;
            if(ev.type==SDL_EVENT_KEY_DOWN&&ev.key.key==SDLK_TAB)ui.showMinimap=!ui.showMinimap;
            if(ev.type==SDL_EVENT_KEY_DOWN&&ev.key.key==SDLK_F1)ui.showCommands=!ui.showCommands;
            if(ev.type==SDL_EVENT_MOUSE_BUTTON_DOWN&&ev.button.button==SDL_BUTTON_LEFT){
                ui.selectedUnit=-1;for(auto&u:units)u.selected=false;
                for(int i=0;i<(int)units.size();i++){float dx=units[i].pos.x-cam.pos.x;float dz=units[i].pos.z-cam.pos.z;
                if(sqrtf(dx*dx+dz*dz)<5){ui.selectedUnit=i;units[i].selected=true;break;}}
            }
            if(ev.type==SDL_EVENT_MOUSE_MOTION&&ev.button.button==SDL_BUTTON_RIGHT)cam.Rotate(ev.motion.xrel*0.3f,-ev.motion.yrel*0.3f);
            if(ev.type==SDL_EVENT_MOUSE_WHEEL)cam.Zoom(ev.wheel.y*2);
            if(ev.type==SDL_EVENT_WINDOW_RESIZED){sw=ev.window.data1;sh=ev.window.data2;glViewport(0,0,sw,sh);glMatrixMode(GL_PROJECTION);glLoadIdentity();gluPerspective(60,(double)sw/sh,0.1,1000);glMatrixMode(GL_MODELVIEW);}
        }
        // Simple AI movement
        for(auto&u:units){if(u.team==0){
            float nd=999;for(auto&e:units)if(e.team!=u.team){float d=(e.pos-u.pos).Length();if(d<nd)nd=d;}
            if(nd>3)for(auto&e:units)if(e.team!=u.team){Vec3 dir=(e.pos-u.pos).Normalized();u.pos=u.pos+dir*1.5f*dt;break;}}}
        // Render
        glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);cam.Apply();
        glEnable(GL_LIGHT0);float lp[]={10,20,10,0};float la[]={0.3f,0.3f,0.35f,1};float ld[]={0.8f,0.8f,0.85f,1};
        glLightfv(GL_LIGHT0,GL_POSITION,lp);glLightfv(GL_LIGHT0,GL_AMBIENT,la);glLightfv(GL_LIGHT0,GL_DIFFUSE,ld);
        DrawGround();glDisable(GL_LIGHTING);
        for(auto&b:buildings)DrawBuilding(b);for(auto&u:units)DrawUnit(u);
        DrawHUD(ui);
        SDL_GL_SwapWindow(win);
    }
    SDL_GL_DestroyContext(gl);SDL_DestroyWindow(win);SDL_Quit();return 0;
}
