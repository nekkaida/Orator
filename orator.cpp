/**********************************************************
 *  Orator  
 * 
 *  This project renders a 3D spherical cap resembling a speaker,
 *  projects its shadow onto a floor, and includes interactive
 *  controls for camera movement and object rotation.
 **********************************************************/

// ------------------ Standard Includes -------------------
#include <GL/glut.h>   // GLUT for windowing/input, plus OpenGL
#include <math.h>      // Math functions like sin, cos
#include <stdlib.h>    // For exit(), etc.
#include <stdio.h>     // For printf

/* If M_PI isn't defined by math.h in some environments,
 * define it manually here. */
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --------------------------------------------------------
// SPHERICAL CAP + SHADOW PARAMETERS
// --------------------------------------------------------
// Toggles and IDs
bool  textureEnabled      = true;  // Switch for checkerboard texture
int   smoothShading       = 1;     // 1 = GL_SMOOTH, 0 = GL_FLAT
int   depthTestEnabled    = 1;     // 1 = enable depth testing, 0 = disable
GLuint textureID          = 0;     // OpenGL texture handle for checkerboard

// Camera variables
float cameraAngleX        = 0.0f;  // Horizontal camera angle
float cameraAngleY        = 30.0f; // Vertical camera angle
float distance            = 12.0f; // Distance from origin (zoom)

// Automatic shape rotation
float shapeRotationAngle  = 0.0f;  // Updated over time to spin the shape

// Mouse Interaction
int   isDragging          = 0;     // Are we currently dragging with mouse?
int   lastMouseX          = 0;     // Last mouse X position
int   lastMouseY          = 0;     // Last mouse Y position
// Extra user-driven rotation 
float rotationX           = 0.0f;  // Rotation around X set by mouse drag
float rotationY           = 0.0f;  // Rotation around Y set by mouse drag

// Spherical Cap + Ring geometry parameters
const float phi_max          = (3.0f * M_PI) / 4.0f; // How big the spherical cap is (angle)
const float innerRadiusFactor= 0.4f;  // Ratio: inner ring radius / outer ring radius
const float concaveDepth     = 0.1f;  // Depth for the concavity in center

// Shadow plane & light
// The "floor" is at z=-9.5 => plane eqn: z+9.5=0 => {0,0,1,9.5}
GLfloat planeFloor[4] = { 0.0f, 0.0f, 1.0f, 9.5f };
// A point light in 3D space at (5,5,5), w=1 means it's positional (not directional).
GLfloat lightPosition[4] = { 5.0f, 5.0f, 5.0f, 1.0f };

// --------------------------------------------------------
// TEXTURE GENERATION (Checkerboard)
// --------------------------------------------------------
#define TEX_SIZE 64  // We'll make a 64x64 texture

void generateTexture() 
{
    // A 2D array for storing RGB values (3 channels) for each pixel
    GLubyte textureData[TEX_SIZE][TEX_SIZE][3];

    // Fill it with a checkerboard pattern
    for(int i = 0; i < TEX_SIZE; i++) {
        for(int j = 0; j < TEX_SIZE; j++) {
            // The bitwise operations decide if (i,j) is black or white
            // c will be either 0 or 255
            int c = ((((i & 0x8) == 0) ^ ((j & 0x8) == 0))) * 255;
            // Assign the same value to R, G, B => black or white
            textureData[i][j][0] = (GLubyte)c; 
            textureData[i][j][1] = (GLubyte)c; 
            textureData[i][j][2] = (GLubyte)c; 
        }
    }

    // Generate one texture ID and store it in textureID
    glGenTextures(1, &textureID);
    // Bind it to the 2D texture target
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Set various texture parameters:
    // Wrap S/T means if texture coords go beyond [0,1], they will repeat.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT);
    // GL_LINEAR for minification/magnification filters => smooth
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // Upload the texture data to the GPU: size is 64x64, RGB format, 
    // each component is an unsigned byte.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE,
                 0, GL_RGB, GL_UNSIGNED_BYTE, textureData);
}

// --------------------------------------------------------
// DRAWING FUNCTIONS
// --------------------------------------------------------

/**********************************************************
 * drawFoundation() - Draws a big floor at z = -9.5
 **********************************************************/
void drawFoundation() 
{
    glPushMatrix();          // Save current transform state
    glColor3f(1.0f, 1.0f, 1.0f);   // Color it white
    glNormal3f(0.0f, 0.0f, 1.0f);  // Upward-facing normal for light calculations

    float z = -10.0f;         // The floor plane's Z coordinate

    // We draw a large quad from (-20,-20) to (+20,+20) at z = -9.5
    glBegin(GL_QUADS);
        glVertex3f(-20.0f, -20.0f, z);
        glVertex3f( 20.0f, -20.0f, z);
        glVertex3f( 20.0f,  20.0f, z);
        glVertex3f(-20.0f,  20.0f, z);
    glEnd();

    glPopMatrix();           // Restore transform
}

/**********************************************************
 * computeShadowMatrix(...) - Creates a 4x4 shadow projection
 *    based on plane eqn and light position
 **********************************************************/
void computeShadowMatrix(GLfloat shadowMat[16],
                         const GLfloat lightPos[4],
                         const GLfloat plane[4]) 
{
    // plane[] = {A, B, C, D} => A*x + B*y + C*z + D = 0
    // lightPos[] = {Lx, Ly, Lz, Lw}
    // dot = A*Lx + B*Ly + C*Lz + D*Lw
    GLfloat dot = plane[0]*lightPos[0] +
                  plane[1]*lightPos[1] +
                  plane[2]*lightPos[2] +
                  plane[3]*lightPos[3];

    // Fill out the 16 entries of shadowMat
    // The formula is: shadowMat = dot*I - lightPos * plane (outer product)
    // We'll fill each column of the 4x4 matrix:
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

/**********************************************************
 * drawSphericalCap(...) - draws partial sphere from phi=0 to phi=phi_max
 **********************************************************/
void drawSphericalCap(int uSteps, int vSteps) 
{
    // If textures are enabled, bind and enable the texture
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        // Otherwise disable texturing
        glDisable(GL_TEXTURE_2D);
    }

    // We subdivide the sphere in the 'u' (theta) direction
    // and the 'v' (phi) direction.
    float dTheta = (2.0f * M_PI) / uSteps; // Full 360 deg / uSteps
    float dPhi   = phi_max / vSteps;      // From 0..phi_max
    float radius = 1.0f;                  // Radius of sphere

    // Loop over longitude slices
    for (int i = 0; i < uSteps; ++i) {
        float theta = i * dTheta;
        // We'll build a quad strip for each slice
        glBegin(GL_QUAD_STRIP);
        // Loop over latitude subdivisions
        for (int j = 0; j <= vSteps; ++j) {
            float phi = j * dPhi;
            // We do 2 vertices at a time in a strip
            for (int k = 0; k <= 1; ++k) {
                float theta_curr = theta + k * dTheta;

                // Cartesian coords from spherical:
                float x = radius * sin(phi) * cos(theta_curr);
                float y = radius * sin(phi) * sin(theta_curr);
                float z = radius * cos(phi);

                // Normal is just (x,y,z)/radius for a sphere
                float nx = x / radius;
                float ny = y / radius;
                float nz = z / radius;

                // Texture coords: 
                // - U mapped to theta_curr / 2π,
                // - V mapped to phi / phi_max
                float texU = theta_curr / (2.0f * M_PI);
                float texV = phi / phi_max;

                glNormal3f(nx, ny, nz);
                glTexCoord2f(texU, texV);
                glVertex3f(x, y, z);
            }
        }
        glEnd(); // End quad strip
    }
}

/**********************************************************
 * drawFlatOuterRing(...) - ring around the spherical cap
 **********************************************************/
void drawFlatOuterRing(int uSteps) 
{
    // Similarly handle texture enabling
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    // Outer ring radius (circle in x-y plane) is sin(phi_max)
    float outerRadius = sin(phi_max);
    // Inner radius is some fraction (innerRadiusFactor) of outer
    float innerRadius = outerRadius * innerRadiusFactor;
    // The ring is at height z = cos(phi_max)
    float z = cos(phi_max);

    // We'll build a triangle strip from outer radius to inner radius.
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= uSteps; ++i) {
        // Angle from 0..2π
        float theta = i * (2.0f * M_PI) / uSteps;
        float cosTheta = cos(theta);
        float sinTheta = sin(theta);

        // Outer edge coords
        float xOuter = outerRadius * cosTheta;
        float yOuter = outerRadius * sinTheta;

        // Inner edge coords
        float xInner = innerRadius * cosTheta;
        float yInner = innerRadius * sinTheta;

        // Normal pointing downward (z = -1)
        glNormal3f(0.0f, 0.0f, -1.0f);

        // Texture coords for the outer vertex
        glTexCoord2f(0.5f + 0.5f * cosTheta,
                     0.5f + 0.5f * sinTheta);
        glVertex3f(xOuter, yOuter, z);

        // Texture coords for the inner vertex
        glTexCoord2f(0.5f + 0.5f * (innerRadius / outerRadius) * cosTheta,
                     0.5f + 0.5f * (innerRadius / outerRadius) * sinTheta);
        glVertex3f(xInner, yInner, z);
    }
    glEnd();
}

/**********************************************************
 * drawConcaveInnerCircle(...) - concavity in the center
 **********************************************************/
void drawConcaveInnerCircle(int uSteps, int vSteps) 
{
    if (textureEnabled) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, textureID);
    } else {
        glDisable(GL_TEXTURE_2D);
    }

    // This circle's outer radius is the ring's inner radius
    float innerR = sin(phi_max) * innerRadiusFactor;
    // The base Z is cos(phi_max)
    float zBase  = cos(phi_max);
    // The maximum depth for the concavity
    float maxD   = concaveDepth;

    // We'll draw it in vSteps radial layers
    for (int j = 0; j < vSteps; ++j) {
        // r1/r2 = radii of the current ring and the next ring
        float r1 = innerR * (1 - (float)j / vSteps);
        float r2 = innerR * (1 - (float)(j+1) / vSteps);
        // z1/z2 = corresponding heights
        float z1 = zBase + maxD * ((float)j / vSteps);
        float z2 = zBase + maxD * ((float)(j+1) / vSteps);

        glBegin(GL_QUAD_STRIP);
        for (int i = 0; i <= uSteps; ++i) {
            float theta = i * (2.0f * M_PI) / uSteps;
            float cosT  = cos(theta);
            float sinT  = sin(theta);

            // Coordinates for first ring (r1)
            float x1 = r1 * cosT;
            float y1 = r1 * sinT;
            // Normal attempt: pointing somewhat inward/up
            float nx1 = -cosT;
            float ny1 = -sinT;
            float nz1 = (maxD / innerR);

            // Coordinates for second ring (r2)
            float x2 = r2 * cosT;
            float y2 = r2 * sinT;
            float nx2 = -cosT;
            float ny2 = -sinT;
            float nz2 = (maxD / innerR);

            // Normalize these normals
            float len1 = sqrt(nx1*nx1 + ny1*ny1 + nz1*nz1);
            if (len1 != 0.f) {
                nx1 /= len1; ny1 /= len1; nz1 /= len1;
            }
            float len2 = sqrt(nx2*nx2 + ny2*ny2 + nz2*nz2);
            if (len2 != 0.f) {
                nx2 /= len2; ny2 /= len2; nz2 /= len2;
            }

            // Texture coords (center-based)
            float texU = 0.5f + 0.5f*(x1 / innerR);
            float texV = 0.5f + 0.5f*(y1 / innerR);

            // Push first vertex
            glNormal3f(nx1, ny1, nz1);
            glTexCoord2f(texU, texV);
            glVertex3f(x1, y1, z1);

            // Texture coords for second vertex
            texU = 0.5f + 0.5f*(x2 / innerR);
            texV = 0.5f + 0.5f*(y2 / innerR);

            // Push second vertex
            glNormal3f(nx2, ny2, nz2);
            glTexCoord2f(texU, texV);
            glVertex3f(x2, y2, z2);
        }
        glEnd();
    }
}

// --------------------------------------------------------
// LIGHTING SETUP
// --------------------------------------------------------

/**********************************************************
 * initLighting() - Basic lighting setup
 **********************************************************/
void initLighting() 
{
    // Enable depth testing so nearer objects block farther ones
    glEnable(GL_DEPTH_TEST);
    // Turn on lighting in general, and a single light (LIGHT0)
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);

    // Position the light at (5,5,5)
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Set the diffuse, specular, and ambient intensities for this light
    GLfloat light_diffuse[]   = {1.0, 1.0, 1.0, 1.0};
    GLfloat light_specular[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat ambient_light[]   = {0.2, 0.2, 0.2, 1.0};
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_light);

    // Enable color material so we can use glColor3f() for diffuse color
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

    // A bit of specular highlight
    GLfloat mat_specular[]  = {1.0, 1.0, 1.0, 1.0};
    GLfloat mat_shininess[] = {50.0};
    glMaterialfv(GL_FRONT, GL_SPECULAR,  mat_specular);
    glMaterialfv(GL_FRONT, GL_SHININESS, mat_shininess);
}

/**********************************************************
 * initGL() - Sets up texture, lighting, background color
 **********************************************************/
void initGL() 
{
    // Generate the checkerboard texture
    generateTexture();
    // Setup lighting
    initLighting();
    // Clear background to black
    glClearColor(0.f, 0.f, 0.f, 1.f);

    // Start with a shading model (smooth or flat) depending on toggle
    glShadeModel(smoothShading ? GL_SMOOTH : GL_FLAT);
}

// --------------------------------------------------------
// ANIMATION
// --------------------------------------------------------

/**********************************************************
 * timer(...) - Called periodically by GLUT to animate
 **********************************************************/
void timer(int value) 
{
    // Increase shape rotation angle
    shapeRotationAngle += 0.5f;
    // Wrap around if angle exceeds 360
    if (shapeRotationAngle >= 360.f)
        shapeRotationAngle -= 360.f;

    // Request a redisplay
    glutPostRedisplay();
    // Schedule the next timer call ~16ms later (~60FPS)
    glutTimerFunc(16, timer, 0);
}

// --------------------------------------------------------
// RENDERING
// --------------------------------------------------------

/**********************************************************
 * display() - Main rendering function
 **********************************************************/
void display() 
{
    // Clear the color and depth buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Work with the modelview matrix
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Compute camera position from our polar angles and distance
    float cameraX = distance * cosf(cameraAngleX * M_PI / 180.f) * cosf(cameraAngleY * M_PI / 180.f);
    float cameraY = distance * sinf(cameraAngleX * M_PI / 180.f) * cosf(cameraAngleY * M_PI / 180.f);
    float cameraZ = distance * sinf(cameraAngleY * M_PI / 180.f);

    // Place camera at (cameraX,cameraY,cameraZ), looking at origin (0,0,0), with up = (0,0,1)
    gluLookAt(cameraX, cameraY, cameraZ,
              0.0, 0.0, 0.0,
              0.0, 0.0, 1.0);

    // 1) Draw the floor
    drawFoundation();

    // 2) Draw main 3D geometry
    glPushMatrix(); 
      // Apply user-driven rotation from mouse
      glRotatef(rotationX, 1, 0, 0);
      glRotatef(rotationY, 0, 1, 0);
      // Apply automatic spinning
      glRotatef(shapeRotationAngle, 0, 0, 1);

      // Draw spherical cap, ring, and concave center
      drawSphericalCap(100, 50);      // subdiv = 100x50
      drawFlatOuterRing(100);
      drawConcaveInnerCircle(100, 20);
    glPopMatrix();

    // 3) Draw the shadow
    GLfloat shadowMatrix[16];
    // Compute matrix that projects onto planeFloor from lightPosition
    computeShadowMatrix(shadowMatrix, lightPosition, planeFloor);

    // Disable lighting & texture for a solid black silhouette
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);
    glColor3f(0.0f, 0.0f, 0.0f);

    glPushMatrix();
      // Multiply current matrix by the shadow projection matrix
      glMultMatrixf(shadowMatrix);

      // Optional: offset the shadow slightly
      glTranslatef(-0.5f, 2.0f, 0.0f);

      // Apply same user & automatic rotations
      glRotatef(rotationX, 1, 0, 0);
      glRotatef(rotationY, 0, 1, 0);
      glRotatef(shapeRotationAngle, 0, 0, 1);

      // Redraw the same geometry -> it now appears flattened on the plane
      drawSphericalCap(100, 50);
      drawFlatOuterRing(100);
      drawConcaveInnerCircle(100, 20);

    glPopMatrix();

    // Re-enable lighting, and if texture was on, re-enable it
    glEnable(GL_LIGHTING);
    if(textureEnabled) glEnable(GL_TEXTURE_2D);

    // Swap front/back buffers (double buffering)
    glutSwapBuffers();
}

/**********************************************************
 * reshape(...) - Called when window is resized
 **********************************************************/
void reshape(int w, int h) 
{
    // Avoid divide-by-zero if height is 0
    if (h == 0) h = 1;
    // Reset the viewport to match new window size
    glViewport(0, 0, w, h);

    // Switch to projection matrix to set up perspective
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Set perspective with 45° FOV, aspect ratio = w/h, near=1, far=100
    gluPerspective(45.0, (float)w/(float)h, 1.0, 100.0);

    // Return to modelview for subsequent transforms
    glMatrixMode(GL_MODELVIEW);
}

// --------------------------------------------------------
// INPUT HANDLING
// --------------------------------------------------------

/**********************************************************
 * keyboard(...) - Handle normal key presses
 **********************************************************/
void keyboard(unsigned char key, int x, int y) 
{
    switch(key) {
        // ESC: exit
        case 27:
            exit(0);
            break;
        // 't': toggle texture
        case 't':
            textureEnabled = !textureEnabled;
            break;
        // 's': toggle shading mode
        case 's':
            smoothShading = !smoothShading;
            glShadeModel(smoothShading ? GL_SMOOTH : GL_FLAT);
            break;
        // 'd': toggle depth test
        case 'd':
            depthTestEnabled = !depthTestEnabled;
            if(depthTestEnabled) glEnable(GL_DEPTH_TEST);
            else                 glDisable(GL_DEPTH_TEST);
            break;
        default:
            break;
    }
    // Request a redraw after changing settings
    glutPostRedisplay();
}

/**********************************************************
 * specialKeys(...) - Handle arrow keys for camera angles
 **********************************************************/
void specialKeys(int key, int x, int y) 
{
    const float angleStep = 5.0f; // Degrees per key press
    if(key == GLUT_KEY_LEFT) {
        cameraAngleX -= angleStep;
    } else if(key == GLUT_KEY_RIGHT) {
        cameraAngleX += angleStep;
    } else if(key == GLUT_KEY_UP) {
        cameraAngleY += angleStep;
        if(cameraAngleY > 89.f) cameraAngleY = 89.f; // Avoid flipping
    } else if(key == GLUT_KEY_DOWN) {
        cameraAngleY -= angleStep;
        if(cameraAngleY < -89.f) cameraAngleY = -89.f;
    }
    glutPostRedisplay();
}

/**********************************************************
 * mouseButton(...) - Called when mouse buttons are pressed
 **********************************************************/
void mouseButton(int button, int state, int x, int y)
{
    // We only care about left-button interactions
    if(button == GLUT_LEFT_BUTTON) {
        if(state == GLUT_DOWN) {
            // Start dragging
            isDragging = 1;
            lastMouseX = x;
            lastMouseY = y;
        } else {
            // Release
            isDragging = 0;
        }
    }
}

/**********************************************************
 * mouseMotion(...) - Called when mouse moves while dragging
 **********************************************************/
void mouseMotion(int x, int y)
{
    if(isDragging) {
        // Compute how far the mouse moved since last event
        int dx = x - lastMouseX;
        int dy = y - lastMouseY;

        // Update rotation angles based on mouse movement
        rotationX += dy * 0.5f;
        rotationY += dx * 0.5f;

        // Remember new mouse position
        lastMouseX = x;
        lastMouseY = y;

        // Request redraw
        glutPostRedisplay();
    }
}

// --------------------------------------------------------
// MAIN FUNCTION
// --------------------------------------------------------

/**********************************************************
 * main(...) - Program entry point
 **********************************************************/
int main(int argc, char** argv)
{
    // 1) Initialize GLUT
    glutInit(&argc, argv);
    // Double-buffered, RGB color, with a depth buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // Initial window size
    glutInitWindowSize(800, 600);
    // Create a window with a title
    glutCreateWindow("Orator");

    // 2) Initialize OpenGL states (texture, lighting, etc.)
    initGL();

    // 3) Register GLUT callbacks
    glutDisplayFunc(display);     // Called to draw each frame
    glutReshapeFunc(reshape);     // Called when window is resized
    glutKeyboardFunc(keyboard);   // Normal key events
    glutSpecialFunc(specialKeys); // Arrow keys
    glutMouseFunc(mouseButton);   // Mouse clicks
    glutMotionFunc(mouseMotion);  // Mouse dragging

    // 4) Start a timer to rotate shape at about 60fps (16ms)
    glutTimerFunc(16, timer, 0);

    // 5) Enter the infinite main loop
    //    (GLUT will now handle window events and call callbacks)
    glutMainLoop();

    return 0;
}
