#include <GL/glut.h>
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Global variables for toggles
int textureEnabled = 1;
int smoothShading = 1;
int depthTestEnabled = 1;

// Texture ID
GLuint textureID;

// Global variables for shape parameters
const float phi_max = (3.0f * M_PI) / 4.0f; // Adjust this value for desired cap size
const float innerRadiusFactor = 0.4f;        // Factor to determine inner ring size
const float concaveDepth = 0.1f;             // Depth of the concave indentation

// Variables for rotation
float rotationX = 0.0f;
float rotationY = 0.0f;
int isDragging = 0;
int lastMouseX, lastMouseY;

// Function to generate a simple checkerboard texture
#define TEX_SIZE 64
void generateTexture() {
    GLubyte textureData[TEX_SIZE][TEX_SIZE][3];
    int i, j, c;

    for (i = 0; i < TEX_SIZE; i++) {
        for (j = 0; j < TEX_SIZE; j++) {
            c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
            textureData[i][j][0] = (GLubyte)c;
            textureData[i][j][1] = (GLubyte)c;
            textureData[i][j][2] = (GLubyte)c;
        }
    }

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Upload texture to GPU
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
}

// Initialization function
void init() {
    glClearColor(0.0, 0.0, 0.0, 1.0);

    generateTexture();

    glEnable(GL_DEPTH_TEST);  // Enable depth testing by default

    glEnable(GL_LIGHTING);    // Enable lighting
    glEnable(GL_LIGHT0);      // Enable light #0

    glShadeModel(GL_SMOOTH);  // Enable smooth shading by default

    // Set up light parameters
    GLfloat light_pos[] = { 5.0, 5.0, 5.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

// Function to draw a spherical cap
void drawSphericalCap(int uSteps, int vSteps) {
    // Enable or disable texture mapping
    if (textureEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    // Bind texture
    if (textureEnabled)
        glBindTexture(GL_TEXTURE_2D, textureID);

    float theta, phi;
    float dTheta = 2.0f * M_PI / uSteps; // Theta from 0 to 2π
    float dPhi = phi_max / vSteps;       // Phi from 0 to phi_max

    float radius = 1.0f; // Radius of the sphere

    for (int i = 0; i < uSteps; ++i) {
        theta = i * dTheta;
        glBegin(GL_QUAD_STRIP);
        for (int j = 0; j <= vSteps; ++j) {
            phi = j * dPhi;
            for (int k = 0; k <= 1; ++k) {
                float theta_curr = theta + k * dTheta;

                // Convert spherical coordinates to Cartesian coordinates
                float x = radius * sin(phi) * cos(theta_curr);
                float y = radius * sin(phi) * sin(theta_curr);
                float z = radius * cos(phi);

                // Normal vector
                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;

                // Texture coordinates
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

// Function to draw the flat outer ring
void drawFlatOuterRing(int uSteps) {
    // Enable or disable texture mapping
    if (textureEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    // Bind texture
    if (textureEnabled)
        glBindTexture(GL_TEXTURE_2D, textureID);

    float outerRadius = sin(phi_max); // Outer radius of the base circle
    float innerRadius = outerRadius * innerRadiusFactor; // Inner radius for the concave area
    float z = cos(phi_max);      // Z-coordinate of the flat base

    // Draw the flat outer ring
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= uSteps; ++i) {
        float theta = i * 2.0f * M_PI / uSteps;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        // Outer vertex
        float xOuter = outerRadius * cosTheta;
        float yOuter = outerRadius * sinTheta;

        // Inner vertex
        float xInner = innerRadius * cosTheta;
        float yInner = innerRadius * sinTheta;

        // Normal pointing downwards
        glNormal3f(0.0f, 0.0f, -1.0f);

        // Texture coordinates and vertices for outer edge
        glTexCoord2f(0.5f + 0.5f * cosTheta, 0.5f + 0.5f * sinTheta);
        glVertex3f(xOuter, yOuter, z);

        // Texture coordinates and vertices for inner edge
        glTexCoord2f(0.5f + 0.5f * (innerRadius / outerRadius) * cosTheta,
                     0.5f + 0.5f * (innerRadius / outerRadius) * sinTheta);
        glVertex3f(xInner, yInner, z);
    }
    glEnd();
}

// Function to draw the concave inner circle
void drawConcaveInnerCircle(int uSteps, int vSteps) {
    // Enable or disable texture mapping
    if (textureEnabled)
        glEnable(GL_TEXTURE_2D);
    else
        glDisable(GL_TEXTURE_2D);

    // Bind texture
    if (textureEnabled)
        glBindTexture(GL_TEXTURE_2D, textureID);

    float innerRadius = sin(phi_max) * innerRadiusFactor;
    float zBase = cos(phi_max); // Z-coordinate of the flat base
    float maxDepth = concaveDepth; // Maximum depth of the concavity

    for (int j = 0; j < vSteps; ++j) {
        float r1 = innerRadius * (1 - (float)j / vSteps);
        float r2 = innerRadius * (1 - (float)(j + 1) / vSteps);
        float z1 = zBase + maxDepth * ((float)j / vSteps);
        float z2 = zBase + maxDepth * ((float)(j + 1) / vSteps);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= uSteps; ++i) {
            float theta = i * 2.0f * M_PI / uSteps;
            float cosTheta = cos(theta);
            float sinTheta = sin(theta);

            // First vertex
            float x1 = r1 * cosTheta;
            float y1 = r1 * sinTheta;
            float nx1 = -cosTheta;
            float ny1 = -sinTheta;
            float nz1 = (maxDepth / innerRadius);

            // Second vertex
            float x2 = r2 * cosTheta;
            float y2 = r2 * sinTheta;
            float nx2 = -cosTheta;
            float ny2 = -sinTheta;
            float nz2 = (maxDepth / innerRadius);

            // Normalize normals
            float len1 = sqrt(nx1 * nx1 + ny1 * ny1 + nz1 * nz1);
            nx1 /= len1; ny1 /= len1; nz1 /= len1;

            float len2 = sqrt(nx2 * nx2 + ny2 * ny2 + nz2 * nz2);
            nx2 /= len2; ny2 /= len2; nz2 /= len2;

            // Texture coordinates
            float texU = 0.5f + 0.5f * (x1 / innerRadius);
            float texV = 0.5f + 0.5f * (y1 / innerRadius);

            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f(texU, texV);
            glVertex3f(x1, y1, z1);

            texU = 0.5f + 0.5f * (x2 / innerRadius);
            texV = 0.5f + 0.5f * (y2 / innerRadius);

            glNormal3f(nx2, ny2, nz2);
            glTexCoord2f(texU, texV);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

// Display callback function
void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Set up modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Position the camera
    gluLookAt(6, 6, 6, 0, 0, 0, 0, 1, 0);

    // Toggle features based on global variables
    if (depthTestEnabled)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (smoothShading)
        glShadeModel(GL_SMOOTH);
    else
        glShadeModel(GL_FLAT);

    // Draw scene
    glPushMatrix();

    // Apply rotation
    glRotatef(rotationX, 1, 0, 0);
    glRotatef(rotationY, 0, 1, 0);

    // Set material properties
    GLfloat mat_ambient[]   = { 0.7, 0.7, 0.7, 1.0 };
    GLfloat mat_diffuse[]   = { 0.8, 0.8, 0.8, 1.0 };
    GLfloat mat_specular[]  = { 1.0, 1.0, 1.0, 1.0 };
    GLfloat mat_shininess[] = { 50.0 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT,   mat_ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE,   mat_diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, mat_shininess);

    // Draw spherical cap
    drawSphericalCap(100, 50);

    // Draw flat outer ring
    drawFlatOuterRing(100);

    // Draw concave inner circle
    drawConcaveInnerCircle(100, 20);

    glPopMatrix();

    glutSwapBuffers();
}

// Reshape callback function
void reshape(int w, int h) {
    if (h == 0) h = 1;  // Prevent division by zero
    float aspect = (float)w / (float)h;

    glViewport(0, 0, w, h);

    // Set up projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    gluPerspective(45.0, aspect, 1.0, 100.0);
}

// Keyboard callback function
void keyboard(unsigned char key, int x, int y) {
    switch (key) {
    case 't':
        textureEnabled = !textureEnabled;
        break;
    case 's':
        smoothShading = !smoothShading;
        break;
    case 'd':
        depthTestEnabled = !depthTestEnabled;
        break;
    case 27:  // Escape key
        exit(0);
        break;
    default:
        break;
    }
    glutPostRedisplay();
}

// Mouse callback functions
void mouseButton(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN) {
            isDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            isDragging = 0;
        }
    }
}

void mouseMotion(int x, int y) {
    if (isDragging) {
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;
        rotationX += dy * 0.5f;
        rotationY += dx * 0.5f;
        lastMouseX = x;
        lastMouseY = y;
        glutPostRedisplay();
    }
}

// Idle callback function to keep the shape rotating
void idle() {
    if (!isDragging) {
        rotationY += 0.3f;
        if (rotationY > 360.0f)
            rotationY -= 360.0f;
        glutPostRedisplay();
    }
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);

    // Set up double buffering and RGB color mode
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);

    // Create window
    glutInitWindowSize(640, 480);
    glutCreateWindow("OpenGL Spherical Cap with Flat Base and Concave Center");

    // Initialize OpenGL states
    init();

    // Register callback functions
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouseButton);
    glutMotionFunc(mouseMotion);
    glutIdleFunc(idle);

    // Start main loop
    glutMainLoop();

    return 0;
}
