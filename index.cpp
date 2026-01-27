
//H.R
//last update : 2025-11-07 16:47:36 !!
//lets start !
//perspective : add : show orbit option (active or disactive via keyboard) added !!
//add : moon eclipse !!
//and others !
//..........................................................................................

#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <ctime>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define PI 3.1415926535
#define STARS_NUM 13
#include <algorithm>
#define DEG_TO_RAD (PI / 180.0f)
#define GL_CLAMP_TO_EDGE 0x812F
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif
#include <string>
#include <limits.h>
#include <unistd.h>
//----------------------------------------------
std::string getExecutableDir() {
    std::string path;
#ifdef _WIN32
    char buffer[MAX_PATH];
    if (GetModuleFileNameA(NULL, buffer, MAX_PATH) != 0) {
        path = std::string(buffer);
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }
    }
#else
    char buffer[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
        path = std::string(buffer);
        size_t pos = path.find_last_of("/");
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }
    }
#endif
    // Go up two levels: from /bin/Debug → /Project_Official
    for (int i = 0; i < 2; ++i) {
        size_t pos = path.find_last_of("\\/");
        if (pos != std::string::npos) {
            path = path.substr(0, pos);
        }
    }
    return path;
}
std::string tex(const std::string& filename) {
    return getExecutableDir() + "/textures/" + filename;
}
// Global loadTexture function (returns ID)
GLuint loadTexture(const char* file) {
    int w, h, c;
    stbi_set_flip_vertically_on_load(1);
    unsigned char* d = stbi_load(file, &w, &h, &c, 0);
    if (!d) return 0;
    GLuint t;
    glGenTextures(1, &t);
    glBindTexture(GL_TEXTURE_2D, t);
    GLenum f = (c == 4) ? GL_RGBA : GL_RGB;
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, f, w, h, 0, f, GL_UNSIGNED_BYTE, d);
    stbi_image_free(d);
    return t;
}
//--------------------------------------
float speed = 5.0f; //used for keyboard !
float previousMouseX = 0.0f; // Position précédente sur X
float previousMouseY = 0.0f; // Position précédente sur Y
float cameraStep = 0.05f;
enum { SUN, EARTH, MOON, MARS, SAT1, SAT2, SATURN, NEPTUNE, MERCURY , URANUS ,VENUS , JUPITER , PLOTU};
// ------------------- CAMERA -------------------
float viewX = 300.0f, viewY = 250.0f, viewZ = 450.0f;
float centerX = 0.0f, centerY = 0.0f, centerZ = 0.0f;
float upX = 0.0f, upY = 1.0f, upZ = 0.0f;
// ------------------- CAMERA ANIMATION -------------------
bool cameraAnimation = false; // Toggle for automatic camera orbit
bool showOrbits = true; // Orbits visible by default
#define OFFSET 10.0f
bool mouseMov = false;
float cameraAngle = 0.0f; // Angle for orbiting
float cameraRadius = 600.0f; // Distance from the Sun
float cameraHeight = 300.0f; // Height above the orbital plane
float cameraSpeed = 10.0f; // Speed of rotation (degrees per second)
// ---------------------------------------------------------------
// Print bitmap text (pixel coordinates)
// ---------------------------------------------------------------
void printBitmap(const char* txt, int x, int y, void* font)
{
    glRasterPos2i(x, y);
    for (const char* c = txt; *c; ++c)
        glutBitmapCharacter(font, *c);
}
// overload with default font
void printBitmap(const char* txt, int x, int y)
{
    printBitmap(txt, x, y, GLUT_BITMAP_TIMES_ROMAN_24);
}
// ------------------- SKYBOX -------------------
//fond spatial
class Skybox {
public:
    GLuint textureID = 0;
    float size = 5000.0f;
    Skybox(const char* texPath) { textureID = loadTexture(texPath); }
    void draw() {
        glDisable(GL_LIGHTING);
        glDisable(GL_DEPTH_TEST);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glColor3f(1,1,1);
        glBegin(GL_QUADS);
        // [same 6 faces – unchanged]
        glTexCoord2f(0,0); glVertex3f(-size,-size, size);
        glTexCoord2f(1,0); glVertex3f( size,-size, size);
        glTexCoord2f(1,1); glVertex3f( size, size, size);
        glTexCoord2f(0,1); glVertex3f(-size, size, size);
        glTexCoord2f(1,0); glVertex3f(-size,-size,-size);
        glTexCoord2f(1,1); glVertex3f(-size, size,-size);
        glTexCoord2f(0,1); glVertex3f( size, size,-size);
        glTexCoord2f(0,0); glVertex3f( size,-size,-size);
        glTexCoord2f(0,0); glVertex3f( size,-size,-size);
        glTexCoord2f(1,0); glVertex3f( size,-size, size);
        glTexCoord2f(1,1); glVertex3f( size, size, size);
        glTexCoord2f(0,1); glVertex3f( size, size,-size);
        glTexCoord2f(0,0); glVertex3f(-size,-size, size);
        glTexCoord2f(0,1); glVertex3f(-size, size, size);
        glTexCoord2f(1,1); glVertex3f(-size, size,-size);
        glTexCoord2f(1,0); glVertex3f(-size,-size,-size);
        glTexCoord2f(0,0); glVertex3f(-size, size, size);
        glTexCoord2f(1,0); glVertex3f( size, size, size);
        glTexCoord2f(1,1); glVertex3f( size, size,-size);
        glTexCoord2f(0,1); glVertex3f(-size, size,-size);
        glTexCoord2f(0,1); glVertex3f(-size,-size,-size);
        glTexCoord2f(1,1); glVertex3f( size,-size,-size);
        glTexCoord2f(1,0); glVertex3f( size,-size, size);
        glTexCoord2f(0,0); glVertex3f(-size,-size, size);
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_LIGHTING);
    }
};
// ------------------- HELPER FUNCTIONS (GLOBAL) -------------------
// ---------------------------------------------------------------
// Textured cube (with texture coords)
// ---------------------------------------------------------------
void drawTexturedCube(float size, GLuint tex) {
    if (tex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, tex);
    }
    float s = size * 0.5f;
    glBegin(GL_QUADS);
    // +Z
    glNormal3f(0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-s,-s, s);
    glTexCoord2f(1,0); glVertex3f( s,-s, s);
    glTexCoord2f(1,1); glVertex3f( s, s, s);
    glTexCoord2f(0,1); glVertex3f(-s, s, s);
    // -Z
    glNormal3f(0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
    glTexCoord2f(0,1); glVertex3f(-s, s,-s);
    glTexCoord2f(1,1); glVertex3f( s, s,-s);
    glTexCoord2f(1,0); glVertex3f( s,-s,-s);
    // +Y
    glNormal3f(0, 1, 0);
    glTexCoord2f(0,0); glVertex3f(-s, s,-s);
    glTexCoord2f(0,1); glVertex3f(-s, s, s);
    glTexCoord2f(1,1); glVertex3f( s, s, s);
    glTexCoord2f(1,0); glVertex3f( s, s,-s);
    // -Y
    glNormal3f(0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
    glTexCoord2f(1,0); glVertex3f( s,-s,-s);
    glTexCoord2f(1,1); glVertex3f( s,-s, s);
    glTexCoord2f(0,1); glVertex3f(-s,-s, s);
    // +X
    glNormal3f(1, 0, 0);
    glTexCoord2f(0,0); glVertex3f( s,-s,-s);
    glTexCoord2f(0,1); glVertex3f( s, s,-s);
    glTexCoord2f(1,1); glVertex3f( s, s, s);
    glTexCoord2f(1,0); glVertex3f( s,-s, s);
    // -X
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-s,-s,-s);
    glTexCoord2f(1,0); glVertex3f(-s,-s, s);
    glTexCoord2f(1,1); glVertex3f(-s, s, s);
    glTexCoord2f(0,1); glVertex3f(-s, s,-s);
    glEnd();
    if (tex) glDisable(GL_TEXTURE_2D);
}
// ---------------------------------------------------------------
// Detailed satellite model with textures
// ---------------------------------------------------------------
void drawSatellite(GLuint bodyTex, GLuint panelTex, GLuint antennaTex)
{
    // ---------- 1. Main body (bus) ----------
    glPushMatrix();
    glScalef(1.8f, 1.2f, 1.0f);
    //glColor3f(0.35f, 0.35f, 0.38f); // Commenté pour texture pleine
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(2.0f, bodyTex);
    glPopMatrix();
    // ---------- 2. Solar panels (two wings) ----------
    // Left panel
    glPushMatrix();
    glTranslatef(-2.8f, 0.0f, 0.0f);
    glScalef(3.6f, 0.08f, 1.4f);
    //glColor3f(0.05f, 0.05f, 0.15f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f, panelTex);
    glPopMatrix();
    // Right panel
    glPushMatrix();
    glTranslatef( 2.8f, 0.0f, 0.0f);
    glScalef(3.6f, 0.08f, 1.4f);
    //glColor3f(0.05f, 0.05f, 0.15f);
    glColor3f(1.0f, 1.0f, 1.0f);
    drawTexturedCube(1.0f, panelTex);
    glPopMatrix();
    // ---------- 3. Parabolic antenna ----------
    glPushMatrix();
    glTranslatef(0.0f, 1.3f, 0.0f);
    glRotatef(-90.0f, 1, 0, 0);
    //glColor3f(0.9f, 0.9f, 0.9f);
    glColor3f(1.0f, 1.0f, 1.0f);
    if (antennaTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, antennaTex);
    }
    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluQuadricNormals(q, GLU_SMOOTH);
    gluDisk(q, 0.0, 0.9, 26, 1);
    glTranslatef(0,0,-0.05f);
    gluCylinder(q, 0.9, 0.9, 0.05, 26, 1);
    glTranslatef(0,0,0.05f);
    gluCylinder(q, 0.12, 0.0, 0.35, 16, 1);
    gluDeleteQuadric(q);
    if (antennaTex) glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    // ---------- 4. Thrusters ----------
    const float thrSize = 0.25f;
    const float thrOff = 0.9f;
    //glColor3f(0.7f, 0.7f, 0.2f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix(); glTranslatef( thrOff, -0.6f, thrOff); glScalef(1,1,1.5f); drawTexturedCube(thrSize, bodyTex); glPopMatrix();
    glPushMatrix(); glTranslatef( thrOff, -0.6f, -thrOff); glScalef(1,1,1.5f); drawTexturedCube(thrSize, bodyTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-thrOff, -0.6f, thrOff); glScalef(1,1,1.5f); drawTexturedCube(thrSize, bodyTex); glPopMatrix();
    glPushMatrix(); glTranslatef(-thrOff, -0.6f, -thrOff); glScalef(1,1,1.5f); drawTexturedCube(thrSize, bodyTex); glPopMatrix();
}
void drawOrbit(float radius, int segments = 100) {
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(0.5f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < segments; ++i) {
        float angle = 2.0f * PI * i / segments;
        glVertex3f(radius * cosf(angle), 0.0f, radius * sinf(angle));
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glLineWidth(0.3f);
}
// ------------------- STAR BASE -------------------
class Star {
public:
    float radius, distance, speed, selfSpeed;
    float rgbaColor[3];
    Star* parentStar = nullptr;
    float alpha = 0, alphaSelf = 0;
    GLuint textureID = 0;
    float worldPos[3] = {0.0f, 0.0f, 0.0f};
    Star(float r, float d, float s, float ss, float rc, float gc, float bc, Star* p, const char* tex)
        : radius(r), distance(d), speed(s), selfSpeed(ss), parentStar(p) {
        rgbaColor[0]=rc; rgbaColor[1]=gc; rgbaColor[2]=bc;
        if (tex) textureID = loadTexture(tex);
    }
    virtual void draw() {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, textureID);
    static GLUquadric* q = nullptr;
    if (!q) {
        q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
    }
    // draw the sphere using the shared quadric
    gluSphere(q, radius, 40, 32);
    glDisable(GL_TEXTURE_2D);
}
    virtual ~Star() {} //destructor
    virtual void update(float dt) { alpha += dt*speed; alphaSelf += dt*selfSpeed; }
    void render(bool isSat = false) {
        //if planet has parent -> rotate & translate the parent in position space before drawing the planet
        //example : move earth first -> than draw the moon around earth
    glPushMatrix();
    if (parentStar && parentStar->distance > 0) {
        glRotatef(parentStar->alpha, 0, 1, 0);
        glTranslatef(parentStar->distance, 0, 0);
    }
    //rotate and move planet on its parent
    glRotatef(alpha, 0, 1, 0);
    glTranslatef(distance, 0, 0);
    // === MANUAL WORLD POSITION (NO GPU STALL!) ===
    float localX = distance;
    float localY = 0.0f;
    float localZ = 0.0f;
    float rad = alpha * DEG_TO_RAD;
    float cosA = cosf(rad);
    float sinA = sinf(rad);
    float orbX = localX * cosA - localZ * sinA;
    float orbY = localY;
    float orbZ = localX * sinA + localZ * cosA;
    if (parentStar && parentStar->distance > 0.0f) {
        float parentRad = parentStar->alpha * DEG_TO_RAD;
        float pCos = cosf(parentRad);
        float pSin = sinf(parentRad);
        float parentX = parentStar->distance;
        float parentOrbX = parentX * pCos;
        float parentOrbZ = parentX * pSin;
        worldPos[0] = parentOrbX + orbX;
        worldPos[1] = orbY;
        worldPos[2] = parentOrbZ + orbZ;
    } else {
        worldPos[0] = orbX;
        worldPos[1] = orbY;
        worldPos[2] = orbZ;
    }
    // =============================================
    if (!isSat) glRotatef(alphaSelf, 0, 1, 0); //turn around it self !!
    if (isSat) {

        if (isSat){
            GLuint bodyTex = 0;
    GLuint panelTex = 0;
    GLuint antennaTex = 0;
                bodyTex = loadTexture(tex("panneaux.jpg").c_str());
        antennaTex = loadTexture(tex("metal.png").c_str());
         panelTex = loadTexture(tex("anttenne.png").c_str());
                drawSatellite(bodyTex, panelTex, antennaTex);}
    } else{ draw();}
    glPopMatrix();
}
    void renderOrbit()  {
        // parentStar == nullptr  → Sun (distance == 0)
        // parentStar->distance > 0 → another planet (e.g. Earth)
        if (parentStar && parentStar->distance > 0.0f) {
            return;                     // <-- Moon / satellites → no line
        }

        // otherwise draw the normal orbit (around the Sun)
        if (distance > 0.0f) {
            glPushMatrix();
            drawOrbit(distance);
            glPopMatrix();
        }
    }
};
// ---------------------------------------------------------------
// ------------------- PLANET -------------------
class Planet : public Star {
    //r → planet radius
//d → distance from parent (e.g. from Sun)
//s → orbit speed
//ss → self rotation speed
//rc,gc,bc → RGB color values
//p → pointer to parent object (e.g. Sun or earth)
//tex → texture file path
public:
    Planet(float r,float d,float s,float ss,float rc,float gc,float bc,Star* p,const char* tex)
        : Star(r,d,s,ss,rc,gc,bc,p,tex) {}
    void draw() override {
        GLfloat amb[]={0.0f,0.0f,0.0f,1}, //0 → no ambient light (dark side becomes black) shadow
        dif[]={rgbaColor[0],rgbaColor[1],rgbaColor[2],1}, //diffuse: Color reflected from the light source (main visible color)
                spe[]={1,1,1,1};
        //This combination gives you realistic shading:
            //The day side bright,
            //The night side dark,
        //glmaterialfv = how the surface responds to that light !
        glMaterialfv(GL_FRONT,GL_AMBIENT,amb);
        glMaterialfv(GL_FRONT,GL_DIFFUSE,dif);
        glMaterialfv(GL_FRONT,GL_SPECULAR,spe); //color of shiny highlight
        glMaterialf(GL_FRONT,GL_SHININESS,50); //The planet’s surface reflects light fairly sharply — not mirror-like, but glossy.(value of 50)
                        //looks more mattte
        Star::draw(); //call draw funtion from parent class Star !!
    }
};
//
// ------------------- SATELLITE -------------------
class Satellite : public Planet {
public:

    Satellite(float r,float d,float s,float ss,float rc,float gc,float bc,Star* p,const char* tex)
        : Planet(r,d,s,ss,rc,gc,bc,p,tex) {

    }
};
// End satlite !!!!!!!!!!!!--------------
// ------------------- EARTH (Day/Night + Clouds) -------------------
// ------------------- EARTH (Day/Night + Clouds) -------------------
class Earth : public Planet {
    GLuint nightTex = 0;
    GLuint cloudTex = 0;
    float cloudOffset = 0.0f;
public:
    Earth(float r, float d, float s, float ss, float rc, float gc, float bc, Star* p,
          const char* dayTex, const char* nightTex, const char* cloudsTex = nullptr)
        : Planet(r, d, s, ss, rc, gc, bc, p, dayTex)  { //call to the parent constructor: to load the day texture !!
        if (nightTex) loadNightTexture(nightTex);
        if (cloudsTex) loadCloudTexture(cloudsTex);
    }
    void loadNightTexture(const char* f) {
    int w, h, n;
    unsigned char* d = stbi_load(f, &w, &h, &n, 0);
    if (d) {
        // DARKEN TEXTURE DATA
        for (int i = 0; i < w * h * n; i += n) {
            d[i]     = (unsigned char)(d[i]     * 0.01f); // R
            d[i+1]   = (unsigned char)(d[i+1]   * 0.01f); // G
            d[i+2]   = (unsigned char)(d[i+2]   * 0.03f); // B (keep blue glow)
            if (n == 4) d[i+3] = 255; // A
        }

        glGenTextures(1, &nightTex);
        glBindTexture(GL_TEXTURE_2D, nightTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        GLenum fmt = (n == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, fmt, w, h, 0, fmt, GL_UNSIGNED_BYTE, d);
        stbi_image_free(d);
    }
}


    void loadCloudTexture(const char* f) {


    int w, h, n;
    unsigned char* d = stbi_load(f, &w, &h, &n, 4); // FORCE 4 channels (RGBA)
    if (d) {
        // If original had no alpha (n == 3), we create one from brightness
        for (int i = 0; i < w * h; ++i) {
            int idx = i * 4;
            unsigned char r = d[idx], g = d[idx+1], b = d[idx+2];
            // Use luminance as alpha: clouds = white → opaque, black → transparent
            unsigned char alpha = (unsigned char)((r + g + b) / 3);
            d[idx + 3] = alpha > 30 ? alpha : 0; // threshold out dark areas
        }

        glGenTextures(1, &cloudTex);
        glBindTexture(GL_TEXTURE_2D, cloudTex);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, d);
        stbi_image_free(d);
    }
}


    void drawClouds() {
    if (!cloudTex) return;

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Enables alpha blending — lets parts of the cloud texture be semi-transparent.
                                                        //This makes the clouds look soft and realistic instead of solid white.
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, cloudTex);
    glDisable(GL_LIGHTING); //disable light , to control it manually because we have night and day !!! look at next steps
    glDepthFunc(GL_LEQUAL); // Allow same-depth drawing //Uses GL_LEQUAL so the clouds can draw on top of the planet without depth fighting (z-flicker).

    // ---- DAY SIDE: bright clouds ----
    glEnable(GL_CULL_FACE); //Enables back-face culling (so we only draw the outside of the sphere).
    glCullFace(GL_BACK);
    glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
    glPushMatrix();
    glRotatef(cloudOffset, 0, 1, 0);
    GLUquadric* q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluSphere(q, radius * 1.02f, 40, 32);
    gluDeleteQuadric(q);
    glPopMatrix();

    // ---- NIGHT SIDE: dark clouds ----
    //cull the front faces — so we draw the back side of the cloud sphere (the far side).
                //We tint them dark blue-gray and 70% transparent (alpha 0.7).
    glCullFace(GL_FRONT);
    glColor4f(0.15f, 0.15f, 0.2f, 0.7f);
    glPushMatrix();
    glRotatef(cloudOffset, 0, 1, 0);
    q = gluNewQuadric();
    gluQuadricTexture(q, GL_TRUE);
    gluSphere(q, radius * 1.02f, 40, 32);
    gluDeleteQuadric(q);
    glPopMatrix();

    glDisable(GL_CULL_FACE);
    glColor4f(1,1,1,1);
    glDepthFunc(GL_LESS); // restore
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_BLEND);
}


    void drawAtmosphereGlow() {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glDepthMask(GL_FALSE);
        GLUquadric* q = gluNewQuadric();
        gluQuadricDrawStyle(q, GLU_SILHOUETTE);
        for (int i = 0; i < 5; ++i) {
            float r = radius * (1.03f + 0.01f * i);
            float alpha = 0.15f * (1.0f - i / 5.0f);
            glColor4f(0.3f, 0.5f, 1.0f, alpha);
            gluSphere(q, r, 20, 16);
        }
        gluDeleteQuadric(q);
        glDepthMask(GL_TRUE);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }



    void update(float dt) override {

        Planet::update(dt);
        cloudOffset += dt * 2.0f;
        if (cloudOffset > 360) cloudOffset -= 360;
    }
    // ------------------------------------------------------------------

// --------------------------------------------------------------
// Dynamic clip plane based on Earth's rotation around its axis
// --------------------------------------------------------------
void setClipPlaneDynamic(bool forDay, float rotationAngle) {
    GLdouble plane[4];

    // Convert angle to radians (rotation around Y axis)
    double rad = (rotationAngle * M_PI / 180.0);
    double nx = cos(rad);
    double nz = sin(rad);

    // The Sun is assumed to shine from -X (left side of the screen)
    if (forDay) {
        plane[0] = -nx;  // toward the Sun
        plane[1] = 0.0;
        plane[2] = -nz;
    } else {
        plane[0] = nx;   // away from the Sun
        plane[1] = 0.0;
        plane[2] = nz;
    }

    plane[3] = 0.0;  // Plane passes through origin (Earth center)

    glClipPlane(GL_CLIP_PLANE0, plane);
    glEnable(GL_CLIP_PLANE0); //Enables GL_CLIP_PLANE0, so the next drawing call will only render one half of the sphere.
    //clipping plane is an invisible plane that cuts away part of your 3D geometry.
    //Anything on one side of that plane is rendered, and anything on the other side is discarded.
}


   // --------------------------------------------------------------
// MAIN DRAW – Corrected implementation
// --------------------------------------------------------------
void draw() override {
    glPushMatrix();

    // 1. DAY SIDE: Lit with day texture
    glEnable(GL_CULL_FACE); //face culling system — a way to tell OpenGL not to draw certain faces of your 3D objects !!
    glCullFace(GL_BACK);//ensures only the front side of the clipped sphere (the day half) is visible —
                            //so you get a smooth transition between day and night.
    setClipPlaneDynamic(true, alphaSelf);  // Keep sun-facing side
    Planet::draw();  // Day texture + lighting
    glDisable(GL_CLIP_PLANE0);

    // 2. NIGHT SIDE: Dark with night lights
    glCullFace(GL_FRONT);
    setClipPlaneDynamic(false, alphaSelf);  // Keep dark side
    glDisable(GL_LIGHTING);
    if (nightTex) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, nightTex);
        glColor3f(1.0f, 1.0f, 1.0f);
        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
        gluSphere(q, radius, 40, 32);
        gluDeleteQuadric(q);
        glDisable(GL_TEXTURE_2D);
    }
    glEnable(GL_LIGHTING);
    glDisable(GL_CLIP_PLANE0); // disactivate to not touch others !
    glDisable(GL_CULL_FACE); //ensures no faces are accidentally hidden afterward

    // 3. CLOUDS
    drawClouds();

    // 4. ATMOSPHERE
    drawAtmosphereGlow();

    glPopMatrix();
}
};
// ------------------- SUN -------------------
class LightPlanet : public Planet {
public:
    LightPlanet(float r,float d,float s,float ss,float rc,float gc,float bc,Star* p,const char* tex)
        : Planet(r,d,s,ss,rc,gc,bc,p,tex) {}
    void draw() override {
        //la source de la lumiere
        //Light 0
        GLfloat pos[]={0,0,0,1}; //The last value 1 means it’s a positional light (not directional)
        glLightfv(GL_LIGHT0,GL_POSITION,pos); // Sun's position at 0,0,0
        GLfloat amb[]={0.2f,0.2f,0.2f,1}; //Small ambient light
        GLfloat dif[]={1,1,1,1};
        GLfloat spe[]={1,1,1,1};
        glLightfv(GL_LIGHT0,GL_AMBIENT,amb);
        glLightfv(GL_LIGHT0,GL_DIFFUSE,dif); //// White light
        glLightfv(GL_LIGHT0,GL_SPECULAR,spe); //how strong the highlights are (e.g., shiny reflections on surfaces).
        ////This makes the Sun appear bright even if it’s not illuminated by anything else.
        GLfloat emi[]={1,1,0.8f,1};
        glMaterialfv(GL_FRONT,GL_EMISSION,emi); //this makes the Sun “glow” by itself (so it’s not darkened by shading).
        Planet::draw(); //call draw funtion form parent class Planet !!
    }
};
// ------------------- SATURN (rings) -------------------
class Saturn : public Planet {
    GLuint ringTex = 0;
public:
    Saturn(float r,float d,float s,float ss,float rc,float gc,float bc,Star* p,
           const char* bodyTex, const char* ringTexPath)
        : Planet(r,d,s,ss,rc,gc,bc,p,bodyTex) {
        if (ringTexPath) ringTex = loadTexture(ringTexPath);
    }
    void drawRings() {
        if (!ringTex) return;
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D,ringTex);
        glColor4f(1,1,1,0.8f);
        const float inner = radius*1.2f, outer = radius*2.4f;
        const int slices = 70;
        glBegin(GL_QUAD_STRIP);
        for (int i=0;i<=slices;++i) {
            float a = i*2*PI/slices;
            float c = cosf(a), s = sinf(a);
            glTexCoord2f(0,0.5f); glVertex3f(c*inner,0,s*inner);
            glTexCoord2f(1,0.5f); glVertex3f(c*outer,0,s*outer);
        }
        glEnd();
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
    void draw() override { Planet::draw(); drawRings(); }
};
//End Saturn !
// ------------------- VENUS (with atmosphere overlay) -------------------
class Venus : public Planet {
    GLuint atmTex = 0; // Atmosphere texture ID
public:
    Venus(float r, float d, float s, float ss, float rc, float gc, float bc, Star* p,
          const char* surfaceTex, const char* atmTexPath)
        : Planet(r, d, s, ss, rc, gc, bc, p, surfaceTex) {
        if (atmTexPath) atmTex = loadTexture(atmTexPath);
    }
    void drawAtmosphere() {
        if (!atmTex) return;
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_LIGHTING); // Atmosphere is unlit for glow effect
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, atmTex);
        glColor4f(1.0f, 0.95f, 0.8f, 0.6f); // Slight yellow tint, 60% opacity
        // Larger semi-transparent sphere for atmosphere
        float atmRadius = radius * 1.05f; // Slightly bigger than planet
        GLUquadric* q = gluNewQuadric();
        gluQuadricTexture(q, GL_TRUE);
        gluSphere(q, atmRadius, 40, 32);
        gluDeleteQuadric(q);
        glDisable(GL_TEXTURE_2D);
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
    void draw() override {
        Planet::draw(); // Draw surface first
        drawAtmosphere(); // Then overlay atmosphere
    }
};
//End Venus !
// ------------------- SOLAR SYSTEM -------------------
class SolarSystem {
public:
    Star* stars[STARS_NUM];
    // ---------------------------------------------------------------
    // count how many objects are planets / satellites
    // ---------------------------------------------------------------
    int countPlanets() const {
        int n = 0;
        for (int i = 0; i < STARS_NUM; ++i) {
            // SUN is a LightPlanet, not a regular planet
            if (i != SUN && dynamic_cast<Planet*>(stars[i]) && !dynamic_cast<Satellite*>(stars[i]))
                ++n;
        }
        return n;
    }
    int countSatellites() const {
        int n = 0;
        for (int i = 0; i < STARS_NUM; ++i)
            if (dynamic_cast<Satellite*>(stars[i]))
                ++n;
        return n;
    }
    // ---------------------------------------------------------------
    SolarSystem() {
        srand(time(NULL)); // seed once
        stars[SUN] = new LightPlanet(50,0,0,0.5,1,1,0,nullptr,tex("8k_sun.jpg").c_str());
        stars[MERCURY] = new Planet(8,95,0.15,3.0,0.7,0.7,0.6,stars[SUN],tex("8k_mercury.jpg").c_str());
        stars[VENUS] = new Venus(9.5,135,0.12,1.6,1.0,0.85,0.6,stars[SUN],
                                   tex("8k_venus_surface.jpg").c_str(),
                                   tex("4k_venus_atmosphere.jpg").c_str());
        stars[EARTH] = new Earth(10, 180, 0.1, 2, 0, 0, 1, stars[SUN],
                         tex("8k_earth.jpg").c_str(),
                         tex("8k_earth_night.jpg").c_str(),
                         tex("8k_earth_clouds.jpg").c_str());
        stars[MOON] = new Planet(8,45,0.8,3,0.7,0.7,0.7,stars[EARTH],tex("8k_moon.jpg").c_str());
        stars[MARS] = new Planet(9,255,0.05,2.5,0.8,0.3,0.1,stars[SUN],tex("2k_mars.jpg").c_str());
        //jupiter !
        stars[JUPITER] = new Planet(14,295,0.05,2.5,0.8,0.3,0.1,stars[SUN],tex("8k_jupiter.jpg").c_str());
        stars[SATURN] = new Saturn(15,335,0.04,1.5,0.9,0.7,0.4,stars[SUN],
                                    tex("2k_saturn.jpg").c_str(),tex("2k_saturn_ring_alpha.png").c_str());
        stars[URANUS] = new Planet(10,370,0.15,3.0,0.7,0.7,0.6,stars[SUN],tex("2k_uranus.jpg").c_str());
        stars[NEPTUNE] = new Planet(11,400,0.03,1.8,0.1,0.3,0.8,stars[SUN],tex("2k_neptune.jpg").c_str());
        stars[PLOTU] = new Planet(10,430,0.05,1.7,0.1,0.3,0.8,stars[SUN],tex("8k_plotu.png").c_str());
        stars[SAT1] = new Satellite(3.0f,22,1.5,5,0.5,0.5,0.5,stars[EARTH],nullptr);
        stars[SAT2] = new Satellite(2.0f,30,0.7,3,0.6,0.6,0.6,stars[EARTH],nullptr);
        //this part is for random placing the planets !
        for (int i = 1; i < STARS_NUM; ++i) {
            stars[i]->alpha = (rand() % 360); // 0–359 degrees
        }
    //end of the part !
    }
    ~SolarSystem() { for(int i=0;i<STARS_NUM;++i) delete stars[i]; }
    void update(float dt) { for(int i=0;i<STARS_NUM;++i) stars[i]->update(dt); }
    void render() {
        if (showOrbits) {
        for (int i = 0; i < STARS_NUM; ++i)
            if (i != SUN ) stars[i]->renderOrbit();
    }
        for (int i = 0; i < STARS_NUM; ++i) {
            bool isSat = (i == SAT1 || i == SAT2);
            stars[i]->render(isSat);
        }
    }
    void onKeyboard(unsigned char k, int, int) {
        if(!cameraAnimation){
            switch (k) {
        case 'w': case 'W': viewY += OFFSET; break;
        case 'x': case 'X': viewY -= OFFSET; break;
        case 'a': case 'A': viewX -= OFFSET; break;
        case 'd': case 'D': viewX += OFFSET; break;
        case 's': case 'S': viewZ += OFFSET; break;
        case 'z': case 'Z': viewZ -= OFFSET; break;
        case 'r': case 'R': viewX = 0.0f; viewY = 100.0f; viewZ = 450.0f; break;
        case 'o': case 'O': cameraAnimation = !cameraAnimation; break;
        case 'm': case 'M': mouseMov = !mouseMov; break;
        case 'b': case 'B': showOrbits = !showOrbits; break; // TOGGLE ORBITS
    }
        }else{
            switch (k) {
        case 'o': case 'O': cameraAnimation = !cameraAnimation; break;
        case 'b': case 'B': showOrbits = !showOrbits; break; // TOGGLE ORBITS
    }
        }
    glutPostRedisplay();
}
// -------- Déplacement de l’objet au clavier --------
void OnspecialKeys(int key, int x, int y) {
    switch (key) {
        case GLUT_KEY_UP: centerZ -= speed; break; // Avant
        case GLUT_KEY_DOWN: centerZ += speed; break; // Arrière
        case GLUT_KEY_LEFT: centerX -= speed; break; // Gauche
        case GLUT_KEY_RIGHT: centerX += speed; break; // Droite
        case GLUT_KEY_F1: centerY += speed; break; // Haut
        case GLUT_KEY_F2: centerY -= speed; break; // Bas
    }
    glutPostRedisplay();
}
// Gestion du mouvement de la souris
void OnmousMouv(int x, int y) {
    if(mouseMov!=FALSE){
    float deltaX = x - previousMouseX;
    float deltaY = y - previousMouseY;
    viewX -= deltaX * (cameraStep * 5);
    viewY -= deltaY * (cameraStep * 5);
    previousMouseX = x;
    previousMouseY = y;
    glutPostRedisplay();
    }
}
};
//End solar system !
Skybox* skybox;
SolarSystem* solarSystem;


// --- FPS CALCULATION (ACCURATE & SMOOTH) ---
int frameCount = 0;
float fps = 0.0f;
clock_t fpsStartTime = 0;
const int FPS_UPDATE_INTERVAL = 500; // Update every 500ms
//--------- init()-----------
void init() {
    glClearColor(0,0,0,1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0); //light 0
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    solarSystem = new SolarSystem();

    skybox = new Skybox(tex("8k_stars_milky_way.jpg").c_str());
    fpsStartTime = clock();
}
//fonction display
void display() {
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // ----- 3-D scene ---------------------------------
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(75,1,1,10000);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(viewX,viewY,viewZ, centerX,centerY,centerZ, upX,upY,upZ);
    skybox->draw();
    solarSystem->render();
    // ----- 2-D HUD ----------------------------------------------------
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, glutGet(GLUT_WINDOW_WIDTH),
               0, glutGet(GLUT_WINDOW_HEIGHT));
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST); //“Ignore depth — just draw it anyway!”
    //disbale depth test : So every new pixel you draw will appear on top of everything, even if it’s behind other geometry in 3D space.
    glColor3f(1.0f, 1.0f, 0.0f);
    printBitmap("Solar System – Projet 01 -- Informatique Graphique",
                10, glutGet(GLUT_WINDOW_HEIGHT) - 20);
    glColor3f(0.7f, 1.0f, 0.7f);
    char buf[128];
    snprintf(buf, sizeof(buf),
             "Camera: (viewX=%.0f, viewY=%.0f, viewZ=%.0f)",
             viewX, viewY, viewZ);
    printBitmap(buf, 10, glutGet(GLUT_WINDOW_HEIGHT) - 50);
    glColor3f(fps > 55 ? 0.7f : 1.0f, 1.0f, fps > 55 ? 0.7f : 0.5f); // Green if >55, red if low
    snprintf(buf, sizeof(buf),
             "FPS: %.1f",fps);
    printBitmap(buf, glutGet(GLUT_WINDOW_WIDTH)-150, glutGet(GLUT_WINDOW_HEIGHT) - 50);
    snprintf(buf, sizeof(buf),
             "(CenterX=%.0f, CenterY=%.0f, CenterZ=%.0f)",
             centerX, centerY, centerZ);
    printBitmap(buf, 10, glutGet(GLUT_WINDOW_HEIGHT) - 75);
    glColor3f(1.0f, 0.8f, 0.8f);
    printBitmap("W/S/Z: up/down/zoom A/D: left/right",
                10, 20);
    glColor3f(1.0f, 0.8f, 0.8f);
        printBitmap(" R: reset ", 10, 50);
    glColor3f(1.0f, 1.0f, 1.0f);
    float nb = (glutGet(GLUT_WINDOW_WIDTH)/2) - 60;
    printBitmap("Rouibah Hanine 2025",
                nb, 20);
    // ----- Stats ----------------------------------------------------
    char line[256];
    glColor3f(0.9f, 0.9f, 1.0f); // light blue
    snprintf(line, sizeof(line), "Stars : 1 (SUN)");
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 110);
    snprintf(line, sizeof(line), "Planet : %d", solarSystem->countPlanets());
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 140);
    snprintf(line, sizeof(line), "Satellites : %d", solarSystem->countSatellites());
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 170);
    glColor3f(showOrbits ? 0.7f : 0.4f, showOrbits ? 1.0f : 0.6f, 0.7f);
    snprintf(line, sizeof(line), "Orbits : %s (Press B)", showOrbits ? "ON" : "OFF");
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 200);
    glColor3f(showOrbits ? 0.7f : 0.4f, mouseMov ? 1.0f : 0.6f, 0.7f);
    snprintf(line, sizeof(line), "Mouse Control : %s (Press M)", mouseMov ? "ON" : "OFF");
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 230);
    glColor3f(showOrbits ? 0.7f : 0.4f, cameraAnimation ? 1.0f : 0.6f, 0.7f);
    snprintf(line, sizeof(line), "Orbit Anim : %s (Press O)", cameraAnimation ? "ON" : "OFF");
    printBitmap(line, 10, glutGet(GLUT_WINDOW_HEIGHT) - 260);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glutSwapBuffers();
}
//End Display
//-----for animations -----------
void idle() {
    clock_t now = clock();
    static clock_t lastTime = 0;
    if (lastTime == 0) lastTime = now;
    float dt = (float)(now - lastTime) / CLOCKS_PER_SEC;
    lastTime = now;
    // --- FPS UPDATE (every 500ms) ---
    ++frameCount;
    if (now - fpsStartTime >= (CLOCKS_PER_SEC * FPS_UPDATE_INTERVAL / 1000)) {
        fps = frameCount / ((float)(now - fpsStartTime) / CLOCKS_PER_SEC);
        frameCount = 0;
        fpsStartTime = now;
    }
    solarSystem->update(dt*5);
    // ------------------- CAMERA ANIMATION UPDATE -------------------
    if (cameraAnimation) {
        mouseMov=FALSE;
        cameraAngle += dt * cameraSpeed;
        if (cameraAngle > 360.0f) cameraAngle -= 360.0f;
        viewX = cameraRadius * cosf(cameraAngle * PI / 180.0f);
        viewZ = cameraRadius * sinf(cameraAngle * PI / 180.0f);
        viewY = cameraHeight;
        centerX = centerY = centerZ = 0.0f; // Look at the Sun
    }
    // ---------------------------------------------------------------
    glutPostRedisplay();
}
void keyboard(unsigned char k,int x,int y) { solarSystem->onKeyboard(k,x,y); }
void specialKeys(int key , int x ,int y){ solarSystem->OnspecialKeys(key,x,y);}
void mouseMouv (int x , int y){ solarSystem->OnmousMouv(x,y);}
// Gestion du clic de souris
void SourisCB(int button, int state, int x, int y) {
    //btn : Which button triggered the event (GLUT_LEFT_BUTTON, GLUT_RIGHT_BUTTON, etc.)
    //state : Whether it was pressed (GLUT_DOWN) or released (GLUT_UP)
    // (X,Y) : Mouse position (in window coordinates) when event occurred
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && mouseMov!=FALSE) {
        previousMouseX = x;
        previousMouseY = y;
    }
}
int main(int argc,char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
    glutInitWindowSize(1100,700);
    glutCreateWindow("Solar System - Projet 01");
    init();
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutSpecialFunc(specialKeys);
    //glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(SourisCB); //on click Mouse !!!
    glutMotionFunc(mouseMouv);
     glutMainLoop();
    delete solarSystem; delete skybox; return 0;
}


