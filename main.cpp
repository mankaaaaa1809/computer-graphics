#include <windows.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>

using namespace std;

vector<vector<GLfloat>> vertices = {
    {2,2,2},{4,2,2},{8,2,2},{10,2,2},{5,5,2},{7,5,2},
    {4,7,2},{6,7,2},{8,7,2},{2,10,2},{5,10,2},{7,10,2},{10,10,2},
    {2,2,4},{4,2,4},{8,2,4},{10,2,4},{5,5,4},{7,5,4},
    {4,7,4},{6,7,4},{8,7,4},{2,10,4},{5,10,4},{7,10,4},{10,10,4}
};

vector<pair<int, int>> edges = {
    {0,1},{1,6},{4,6},{4,5},{5,8},{8,2},{2,3},{3,12},{12,11},{11,7},
    {7,10},{10,9},{9,0},
    {0,13},{1,14},{2,15},{3,16},{4,17},{5,18},{6,19},{7,20},{8,21},
    {9,22},{10,23},{11,24},{12,25},
    {13,14},{14,19},{17,19},{17,18},{18,21},{21,15},{15,16},
    {16,25},{25,24},{24,20},{20,23},{23,22},{22,13}
};

GLfloat scaleObj = 1.5f;
GLfloat tx = 0, ty = 0, tz = 0;
GLfloat ax = 0, ay = 0, az = 0;
GLfloat angleAxis = 0, axisX = 1, axisY = 1, axisZ = 1;

GLfloat modelMatrix[16];

float pan2D[4][2] = {};
float pan3D[3] = { 15,15,15 };

bool mouseDown = false;
int lastX = 0, lastY = 0;

int w = 1700, h = 1100;

enum Mode { MODE_3D, MODE_OXY, MODE_OXZ, MODE_OYZ, MODE_ALL };
Mode mode = MODE_3D;

vector<float> center() {
    float x = 0, y = 0, z = 0;
    for (auto& v : vertices) { x += v[0]; y += v[1]; z += v[2]; }
    return { x / vertices.size(),y / vertices.size(),z / vertices.size() };
}

void applyTransform() {
    glTranslatef(tx, ty, tz);
    if (angleAxis != 0) glRotatef(angleAxis, axisX, axisY, axisZ);
    glRotatef(ax, 1, 0, 0);
    glRotatef(ay, 0, 1, 0);
    glRotatef(az, 0, 0, 1);
    glScalef(scaleObj, scaleObj, scaleObj);
}

void updateModelMatrix() {
    glPushMatrix();
    glLoadIdentity();
    applyTransform();
    glGetFloatv(GL_MODELVIEW_MATRIX, modelMatrix);
    glPopMatrix();
}

void drawAxes(float L = 10) {
    glBegin(GL_LINES);
    glColor3f(1, 0, 0); glVertex3f(0, 0, 0); glVertex3f(L, 0, 0);
    glColor3f(0, 1, 0); glVertex3f(0, 0, 0); glVertex3f(0, L, 0);
    glColor3f(0, 0, 1); glVertex3f(0, 0, 0); glVertex3f(0, 0, L);
    glEnd();
}

void drawLetter() {
    glColor3f(0, 1, 1);
    glBegin(GL_LINES);
    for (auto& e : edges) {
        auto& a = vertices[e.first];
        auto& b = vertices[e.second];
        glVertex3f(a[0], a[1], a[2]);
        glVertex3f(b[0], b[1], b[2]);
    }
    glEnd();
}

void drawMatrixOverlay() {
    if (mode == MODE_ALL) return;

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0, 0, 0);
    int y = h - 20;
    for (int i = 0; i < 4; i++) {
        stringstream ss;
        for (int j = 0; j < 4; j++)
            ss << setw(7) << fixed << setprecision(2) << modelMatrix[i * 4 + j] << " ";
        glRasterPos2i(10, y);
        for (char c : ss.str()) glutBitmapCharacter(GLUT_BITMAP_8_BY_13, c);
        y -= 15;
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
}

void setup3D() {
    auto c = center();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(70, (float)w / h, 0.1, 500);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(
        c[0] + pan3D[0], c[1] + pan3D[1], c[2] + pan3D[2],
        c[0], c[1], c[2],
        0, 1, 0
    );
}

void setupOrtho(int idx) {
    auto c = center();
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-12 + pan2D[idx][0], 12 + pan2D[idx][0],
        -12 + pan2D[idx][1], 12 + pan2D[idx][1],
        -100, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    if (idx == 1)
        gluLookAt(c[0], c[1], c[2] + 30, c[0], c[1], c[2], 0, 1, 0);
    if (idx == 2)
        gluLookAt(c[0], c[1] + 30, c[2], c[0], c[1], c[2], 0, 0, -1);
    if (idx == 3)
        gluLookAt(c[0] + 30, c[1], c[2], c[0], c[1], c[2], 0, 1, 0);
}

void drawScene(int idx) {
    if (idx == 0) setup3D();
    else setupOrtho(idx);

    drawAxes();
    glPushMatrix();
    applyTransform();
    drawLetter();
    glPopMatrix();
}

int viewportIndex(int x, int y) {
    if (mode != MODE_ALL) return mode == MODE_3D ? 0 : mode;
    if (x<w / 2 && y>h / 2) return 0;
    if (x >= w / 2 && y > h / 2) return 1;
    if (x < w / 2 && y <= h / 2) return 2;
    return 3;
}

void display() {
    updateModelMatrix();
    glClearColor(0.95, 0.95, 0.98, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    if (mode == MODE_ALL) {
        glViewport(0, h / 2, w / 2, h / 2); drawScene(0);
        glViewport(w / 2, h / 2, w / 2, h / 2); drawScene(1);
        glViewport(0, 0, w / 2, h / 2); drawScene(2);
        glViewport(w / 2, 0, w / 2, h / 2); drawScene(3);
    }
    else {
        glViewport(0, 0, w, h);
        drawScene(mode == MODE_3D ? 0 : mode);
    }

    glDisable(GL_DEPTH_TEST);
    drawMatrixOverlay();
    glutSwapBuffers();
}

void mouse(int b, int s, int x, int y) {
    if (b == GLUT_LEFT_BUTTON) {
        mouseDown = (s == GLUT_DOWN);
        lastX = x; lastY = y;
    }
}

void motion(int x, int y) {
    if (!mouseDown) return;
    int dx = x - lastX, dy = y - lastY;
    int idx = viewportIndex(x, h - y);

    if (idx == 0) {
        pan3D[0] -= dx * 0.05f;
        pan3D[1] += dy * 0.05f;
    }
    else {
        pan2D[idx][0] -= dx * 0.05f;
        pan2D[idx][1] += dy * 0.05f;
    }

    lastX = x; lastY = y;
    glutPostRedisplay();
}

void keyboard(unsigned char k, int, int) {
    if (k == '1')mode = MODE_3D;
    if (k == '2')mode = MODE_OXY;
    if (k == '3')mode = MODE_OXZ;
    if (k == '4')mode = MODE_OYZ;
    if (k == '5')mode = MODE_ALL;

    if (k == '+')scaleObj *= 1.1;
    if (k == '-')scaleObj /= 1.1;
    if (k == 'w')ty += 0.5;
    if (k == 's')ty -= 0.5;
    if (k == 'a')tx -= 0.5;
    if (k == 'd')tx += 0.5;
    if (k == 'q')tz += 0.5;
    if (k == 'e')tz -= 0.5;
    if (k == 'x')ax += 5;
    if (k == 'X')ax -= 5;
    if (k == 'y')ay += 5;
    if (k == 'Y')ay -= 5;
    if (k == 'z')az += 5;
    if (k == 'Z')az -= 5;
    if (k == 'r')angleAxis += 10;
    if (k == 't')angleAxis -= 10;
    if (k == 27)exit(0);

    glutPostRedisplay();
}

void reshape(int W, int H) { w = W; h = H; }

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(w, h);
    glutCreateWindow("Лабораторная работа 6");

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
