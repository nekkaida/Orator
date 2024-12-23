#include <GL/glut.h>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// If M_PI not defined, define it
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// -----------------------------------------------------
// Global toggles/IDs
// -----------------------------------------------------
bool textureEnabled = true;         // Toggle texture
int smoothShading   = 1;            // Toggle smooth shading
int depthTestEnabled= 1;            // Toggle depth testing
GLuint textureID;                   // Checkerboard texture

// -----------------------------------------------------
// Camera variables
// -----------------------------------------------------
float cameraAngleX = 0.0f;
float cameraAngleY = 30.0f;  // Start the camera slightly elevated
float distance     = 12.0f;  // Distance from origin

// We can animate the shape or shadow
float shapeRotationAngle = 0.0f;

// -----------------------------------------------------
// Spherical Cap + Ring Parameters
// -----------------------------------------------------
const float phi_max          = (3.0f * M_PI) / 4.0f;
const float innerRadiusFactor= 0.4f;
const float concaveDepth     = 0.1f;

// -----------------------------------------------------
// Mouse Interaction (optional if you want to drag-rotate)
// -----------------------------------------------------
int isDragging = 0;
int lastMouseX, lastMouseY;
float rotationX = 0.0f;  // Extra user rotation around X
float rotationY = 0.0f;  // Extra user rotation around Y

// -----------------------------------------------------
// For the shadow plane: z = -9.5 => plane eqn is z + 9.5 = 0
// i.e. plane[] = {0,0,1,9.5}
// -----------------------------------------------------
GLfloat planeFloor[4] = {0.0f, 0.0f, 1.0f, 9.49f};

// Light position (positional, w=1.0)
GLfloat lightPosition[4] = {5.0f, 5.0f, 5.0f, 1.0f};

// -----------------------------------------------------
// Generate a 64x64 Checkerboard Texture
// -----------------------------------------------------
#define TEX_SIZE 64
void generateTexture() {
    GLubyte textureData[TEX_SIZE][TEX_SIZE][3];
    for(int i = 0; i < TEX_SIZE; i++) {
        for(int j = 0; j < TEX_SIZE; j++) {
            int c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
            textureData[i][j][0] = (GLubyte)c; // R
            textureData[i][j][1] = (GLubyte)c; // G
            textureData[i][j][2] = (GLubyte)c; // B
        }
    }
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE,
                 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
}

// -----------------------------------------------------
// Floor (Foundation) at z = -9.5
// -----------------------------------------------------
void drawFoundation() {
    glPushMatrix();
    glColor3f(1.0f, 1.0f, 1.0f);   // White-ish floor
    glNormal3f(0.0f, 0.0f, 1.0f);

    float z = -9.5f;
    glBegin(GL_QUADS);
        glVertex3f(-20.0f, -20.0f, z);
        glVertex3f( 20.0f, -20.0f, z);
        glVertex3f( 20.0f,  20.0f, z);
        glVertex3f(-20.0f,  20.0f, z);
    glEnd();

    glPopMatrix();
}

// -----------------------------------------------------
// Compute a Shadow Matrix 
// -----------------------------------------------------
void computeShadowMatrix(GLfloat shadowMat[16],
                         const GLfloat lightPos[4],
                         const GLfloat plane[4]) {
    GLfloat dot = plane[0]*lightPos[0] +
                  plane[1]*lightPos[1] +
                  plane[2]*lightPos[2] +
                  plane[3]*lightPos[3];

    shadowMat[0]  = dot - lightPos[0]*plane[0];
    shadowMat[4]  =     - lightPos[0]*plane[1];
    shadowMat[8]  =     - lightPos[0]*plane[2];
    shadowMat[12] =     - lightPos[0]*plane[3];

    shadowMat[1]  =     - lightPos[1]*plane[0];
    shadowMat[5]  = dot - lightPos[1]*plane[1];
    shadowMat[9]  =     - lightPos[1]*plane[2];
    shadowMat[13] =     - lightPos[1]*plane[3];

    shadowMat[2]  =     - lightPos[2]*plane[0];
    shadowMat[6]  =     - lightPos[2]*plane[1];
    shadowMat[10] = dot - lightPos[2]*plane[2];
    shadowMat[14] =     - lightPos[2]*plane[3];

    shadowMat[3]  =     - lightPos[3]*plane[0];
    shadowMat[7]  =     - lightPos[3]*plane[1];
    shadowMat[11] =     - lightPos[3]*plane[2];
    shadowMat[15] = dot - lightPos[3]*plane[3];
}

// -----------------------------------------------------
// Spherical Cap
// -----------------------------------------------------
void drawSphericalCap(int uSteps, int vSteps) {
    // Toggle texture
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    float dTheta = (2.0f * M_PI) / uSteps;
    float dPhi   = phi_max / vSteps;
    float radius = 1.0f;

    for (int i = 0; i < uSteps; ++i) {
        float theta = i * dTheta;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= vSteps; ++j) {
            float phi = j * dPhi;
            for (int k = 0; k <= 1; ++k) {
                float theta_curr = theta + k * dTheta;

                float x = radius * sin(phi) * cos(theta_curr);
                float y = radius * sin(phi) * sin(theta_curr);
                float z = radius * cos(phi);

                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;

                float texU = theta_curr / (2.0f * M_PI);
                float texV = phi / phi_max;

                glNormal3f(nx, ny, nz);
                glTexCoord2f(texU, texV);
                glVertex3f(x, y, z);
            }
        }
        glEnd();
    }
}

// -----------------------------------------------------
// Flat Outer Ring
// -----------------------------------------------------
void drawFlatOuterRing(int uSteps) {
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    float outerRadius = sin(phi_max);
    float innerRadius = outerRadius * innerRadiusFactor;
    float z           = cos(phi_max);

    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= uSteps; ++i) {
        float theta = i * (2.0f * M_PI) / uSteps;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        float xOuter = outerRadius * cosTheta;
        float yOuter = outerRadius * sinTheta;
        float xInner = innerRadius * cosTheta;
        float yInner = innerRadius * sinTheta;

        glNormal3f(0.0f, 0.0f, -1.0f);

        // Outer edge
        glTexCoord2f(0.5f + 0.5f * cosTheta,
                     0.5f + 0.5f * sinTheta);
        glVertex3f(xOuter, yOuter, z);

        // Inner edge
        glTexCoord2f(0.5f + 0.5f * (innerRadius / outerRadius) * cosTheta,
                     0.5f + 0.5f * (innerRadius / outerRadius) * sinTheta);
        glVertex3f(xInner, yInner, z);
    }
    glEnd();
}

// -----------------------------------------------------
// Concave Inner Circle
// -----------------------------------------------------
void drawConcaveInnerCircle(int uSteps, int vSteps) {
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    float innerR = sin(phi_max) * innerRadiusFactor;
    float zBase  = cos(phi_max);
    float maxD   = concaveDepth;

    for (int j = 0; j < vSteps; ++j) {
        float r1 = innerR * (1 - (float)j / vSteps);
        float r2 = innerR * (1 - (float)(j+1) / vSteps);
        float z1 = zBase + maxD * ((float)j / vSteps);
        float z2 = zBase + maxD * ((float)(j+1) / vSteps);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= uSteps; ++i) {
            float theta = i * (2.0f * M_PI) / uSteps;
            float cosT  = cos(theta);
            float sinT  = sin(theta);

            float x1 = r1 * cosT;
            float y1 = r1 * sinT;
            float nx1 = -cosT;
            float ny1 = -sinT;
            float nz1 = (maxD / innerR);

            float x2 = r2 * cosT;
            float y2 = r2 * sinT;
            float nx2 = -cosT;
            float ny2 = -sinT;
            float nz2 = (maxD / innerR);

            // Normalize
            float len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
            if (len1!=0.0f) {
                nx1 /= len1; ny1 /= len1; nz1 /= len1;
            }
            float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
            if (len2!=0.0f) {
                nx2 /= len2; ny2 /= len2; nz2 /= len2;
            }

            float texU = 0.5f + 0.5f * (x1 / innerR);
            float texV = 0.5f + 0.5f * (y1 / innerR);

            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f(texU, texV);
            glVertex3f(x1, y1, z1);

            texU = 0.5f + 0.5f * (x2 / innerR);
            texV = 0.5f + 0.5f * (y2 / innerR);

            glNormal3f(nx2, ny2, nz2);
            glTexCoord2f(texU, texV);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

// -----------------------------------------------------
// Initialization & Lighting Setup
// -----------------------------------------------------
void initLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Position the light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Light properties
    GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat ambient_light[]   = {0.2, 0.2, 0.2, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

    // Allow coloring via glColor
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // Material properties
    GLfloat mat_specular[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

// -----------------------------------------------------
// Main Setup
// -----------------------------------------------------
void initGL() {
    generateTexture();            // Our checkerboard
    initLighting();               // Turn on lighting, etc.
    glClearColor(0.f,0.f,0.f,1.f); // BG color
}

// -----------------------------------------------------
// Timer 
// We'll rotate shape automatically
// -----------------------------------------------------
void timer(int value) {
    // Keep shape rotating
    shapeRotationAngle += 0.5f;
    if (shapeRotationAngle >= 360.0f)
        shapeRotationAngle -= 360.0f;

    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

// -----------------------------------------------------
// Display
// -----------------------------------------------------
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position the camera using spherical angles
    float cameraX = distance * cosf(cameraAngleX * M_PI / 180.0f) 
                             * cosf(cameraAngleY * M_PI / 180.0f);
    float cameraY = distance * sinf(cameraAngleX * M_PI / 180.0f) 
                             * cosf(cameraAngleY * M_PI / 180.0f);
    float cameraZ = distance * sinf(cameraAngleY * M_PI / 180.0f);

    gluLookAt(cameraX, cameraY, cameraZ,  
              0.0, 0.0, 0.0,    
              0.0, 0.0, 1.0);

    // Draw foundation/floor at z = -9.5
    drawFoundation();

    // 1) Draw the actual geometry (with lighting on)
    glPushMatrix();
      // Let user drag-rotate
      glRotatef(rotationX, 1,0,0);
      glRotatef(rotationY, 0,1,0);

      // Also apply an automatic rotation
      glRotatef(shapeRotationAngle, 0, 0, 1);

      // Draw the shapes
      drawSphericalCap(100, 50);
      drawFlatOuterRing(100);
      drawConcaveInnerCircle(100, 20);

    glPopMatrix();

    // 2) Draw the shadow 
    GLfloat shadowMatrix[16];
    computeShadowMatrix(shadowMatrix, lightPosition, planeFloor);

    // Turn off lighting to render a dark silhouette
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 0.0f); // dark color for shadow

    glPushMatrix();
      glMultMatrixf(shadowMatrix);

      // We can do small translations if we want the shadow offset
      // from the original shape. This is optional.
      glTranslatef(-0.5f, 2.0f, 0.0f);

      // Apply same transformations as geometry
      glRotatef(rotationX, 1,0,0);
      glRotatef(rotationY, 0,1,0);
      glRotatef(shapeRotationAngle, 0, 0, 1);

      // Draw the same shapes to create the shadow
      drawSphericalCap(100, 50);
      drawFlatOuterRing(100);
      drawConcaveInnerCircle(100, 20);

    glPopMatrix();

    // Re-enable lighting/texture
    glEnable(GL_LIGHTING);
    if(textureEnabled)
        glEnable(GL_TEXTURE_2D);

    glutSwapBuffers();
}

// -----------------------------------------------------
// Reshape
// -----------------------------------------------------
void reshape(int w, int h) {
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (float)w/(float)h, 1.0, 100.0);

    glMatrixMode(GL_MODELVIEW);
}

// -----------------------------------------------------
// Keyboard
// -----------------------------------------------------
void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 27: // ESC
            exit(0);
            break;
        case 't':
            textureEnabled = !textureEnabled;
            break;
        case 's':
            smoothShading = !smoothShading;
            glShadeModel(smoothShading ? GL_SMOOTH : GL_FLAT);
            break;
        case 'd':
            depthTestEnabled = !depthTestEnabled;
            if(depthTestEnabled) glEnable(GL_DEPTH_TEST);
            else                 glDisable(GL_DEPTH_TEST);
            break;
        default:
            break;
    }
    glutPostRedisplay();
}

// -----------------------------------------------------
// Special Keys -> adjust camera angles
// (like your Weierstrass handleSpecialKeys())
// -----------------------------------------------------
void specialKeys(int key, int x, int y) {
    const float angleStep = 5.0f;
    if(key == GLUT_KEY_LEFT) {
        cameraAngleX -= angleStep;
    } else if(key == GLUT_KEY_RIGHT) {
        cameraAngleX += angleStep;
    } else if(key == GLUT_KEY_UP) {
        cameraAngleY += angleStep;
        if(cameraAngleY > 89.0f) cameraAngleY = 89.0f;
    } else if(key == GLUT_KEY_DOWN) {
        cameraAngleY -= angleStep;
        if(cameraAngleY < -89.0f) cameraAngleY = -89.0f;
    }
    glutPostRedisplay();
}

// -----------------------------------------------------
// Mouse
// -----------------------------------------------------
void mouseButton(int button, int state, int x, int y) {
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            isDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = 0;
        }
    }
}

void mouseMotion(int x, int y) {
    if(isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        rotationX += dy * 0.5f;
        rotationY += dx * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

// -----------------------------------------------------
// Main
// -----------------------------------------------------
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Spherical Cap with Planar Shadow");

    initGL();

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);

    // Timer to animate shape rotation
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
