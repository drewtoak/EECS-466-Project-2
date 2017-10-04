//Assignment 2
//@authors: Andrew Hwang and Timothy Kuo
//

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>

#define ON 1
#define OFF 0
#define LEFT 0							/*left-mouse button*/
#define MIDDLE 1						/*middle-mouse button*/
#define RIGHT 2							/*right-mouse button*/
#define PI 3.1415926535f
#define SENSITIVITY 150.0f				/*sensitivity of the rate that the camera's point of view changes*/
#define TRANSFORMATION 0.5f

//Global variables
int window_width, window_height;
int PERSPECTIVE = ON;
int displayCoordAxes = ON;
int displayObj = ON;
int WORLD = ON;
float clickX, clickY, clickedButton;	/*gloabl variables saving the data that is collected from clicking a button on the mouse*/
// Persepective angles
float theta = -PI / 2;					/*angle between x and z axes*/
float phi = 0;							/*angle between the y and combination(x+z) axes*/
float prevPhi = phi;
// Orthogonal angles
float alpha = -PI / 2;					/*angle between x and z axes*/
float omega = 0;						/*angle between the y and combination(x+z) axes*/
float prevOmega = omega;
// Camera coordinates and vectors
float camera[3] = { 0, 0, 5 };			/*position vector*/
float lookat[3] = { 0, 0, -1 };			/*target vector*/
float direction[3] = { 0, 0, 1 };		/*normalize between camera - lookat*/
float up[3] = { 0, 1, 0 };				/*up vector*/
float right[3] = { 1, 0, 0 };			/*normalize cross(up, direction)*/
float cameraUp[3] = { 0, 1, 0 };		/*cross(direction, right)*/
// Rotation variables
float angle = 0;
float prevAngle = angle;
float angleWx = 0, angleWy = 0, angleWz = 0;	/*world rotation angles*/
float angleLx = 0, angleLy = 0, angleLz = 0;	/*local rotation angles*/
float rx = 0, ry = 0, rz = 1;
// Model matrix coordinates
GLfloat objx = 0, objy = 0, objz = -5;
GLfloat objM[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, objx, objy, objz, 1 };	/*column order*/
GLfloat transx = 0, transy = 0, transz = 0;	/*translation variables*/
GLfloat uScale = 1;							/*global variable for scaling factor for the model*/

typedef struct _point {
	float x, y, z;
} point;

typedef struct _faceStruct {
	int v1, v2, v3;
	int n1, n2, n3;
} faceStruct;

int verts, faces, norms;
point *vertList, *normList;
faceStruct *faceList;

// It can read *very* simple obj files
void meshReader(char *filename, int sign) {
	float x, y, z, len;
	int i;
	char letter;
	point v1, v2, crossP;
	int ix, iy, iz;
	int *normCount;
	FILE *fp;

	fp = fopen(filename, "r");
	if (fp == NULL) {
		printf("Cannot open %s\n!", filename);
		exit(0);
	}

	// Count the number of vertices and faces
	while (!feof(fp)) {
		fscanf(fp, "%c %f %f %f\n", &letter, &x, &y, &z);
		if (letter == 'v') {
			verts++;
		}
		else {
			faces++;
		}
	}

	fclose(fp);

	printf("verts : %d\n", verts);
	printf("faces : %d\n", faces);

	// Dynamic allocation of vetex and face lists
	faceList = (faceStruct *)malloc(sizeof(faceStruct)*faces);
	vertList = (point *)malloc(sizeof(point)*verts);
	normList = (point *)malloc(sizeof(point)*verts);

	fp = fopen(filename, "r");

	// Read the vertices
	for (i = 0; i < verts; i++) {
		fscanf(fp, "%c %f %f %f\n", &letter, &x, &y, &z);
		vertList[i].x = x;
		vertList[i].y = y;
		vertList[i].z = z;
	}

	// Read the faces
	for (i = 0; i < faces; i++) {
		fscanf(fp, "%c %d %d %d\n", &letter, &ix, &iy, &iz);
		faceList[i].v1 = ix - 1;
		faceList[i].v2 = iy - 1;
		faceList[i].v3 = iz - 1;
	}
	fclose(fp);

	normCount = (int *)malloc(sizeof(int)*verts);
	for (i = 0; i < verts; i++) {
		normList[i].x = normList[i].y = normList[i].z = 0.0;
		normCount[i] = 0;
	}

	for (i = 0; i < faces; i++) {
		v1.x = vertList[faceList[i].v2].x - vertList[faceList[i].v1].x;
		v1.y = vertList[faceList[i].v2].y - vertList[faceList[i].v1].y;
		v1.z = vertList[faceList[i].v2].z - vertList[faceList[i].v1].z;
		v2.x = vertList[faceList[i].v3].x - vertList[faceList[i].v2].x;
		v2.y = vertList[faceList[i].v3].y - vertList[faceList[i].v2].y;
		v2.z = vertList[faceList[i].v3].z - vertList[faceList[i].v2].z;

		crossP.x = v1.y*v2.z - v1.z*v2.y;
		crossP.y = v1.z*v2.x - v1.x*v2.z;
		crossP.z = v1.x*v2.y - v1.y*v2.x;

		len = sqrt(crossP.x*crossP.x + crossP.y*crossP.y + crossP.z*crossP.z);

		crossP.x = -crossP.x / len;
		crossP.y = -crossP.y / len;
		crossP.z = -crossP.z / len;

		normList[faceList[i].v1].x = normList[faceList[i].v1].x + crossP.x;
		normList[faceList[i].v1].y = normList[faceList[i].v1].y + crossP.y;
		normList[faceList[i].v1].z = normList[faceList[i].v1].z + crossP.z;
		normList[faceList[i].v2].x = normList[faceList[i].v2].x + crossP.x;
		normList[faceList[i].v2].y = normList[faceList[i].v2].y + crossP.y;
		normList[faceList[i].v2].z = normList[faceList[i].v2].z + crossP.z;
		normList[faceList[i].v3].x = normList[faceList[i].v3].x + crossP.x;
		normList[faceList[i].v3].y = normList[faceList[i].v3].y + crossP.y;
		normList[faceList[i].v3].z = normList[faceList[i].v3].z + crossP.z;
		normCount[faceList[i].v1]++;
		normCount[faceList[i].v2]++;
		normCount[faceList[i].v3]++;
	}
	for (i = 0; i < verts; i++) {
		normList[i].x = (float)sign*normList[i].x / (float)normCount[i];
		normList[i].y = (float)sign*normList[i].y / (float)normCount[i];
		normList[i].z = (float)sign*normList[i].z / (float)normCount[i];
	}
}

// Scaling function for the model
void scale(GLfloat s) {
	GLfloat scal[16] = { s, 0, 0, 0, 0, s, 0, 0, 0, 0, s, 0, 0, 0, 0, 1 };
	glLoadIdentity();
	glMultMatrixf(objM);
	glMultMatrixf(scal);
}

// Translation function for the model
void translate(GLfloat x, GLfloat y, GLfloat z) {
	objx = objM[12] = objx + x;
	objy = objM[13] = objy + y;
	objz = objM[14] = objz + z;
	glMultMatrixf(objM);
	transx = 0;
	transy = 0;
	transz = 0;
}

//	Rotation function for the model
void rotate(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
	GLfloat rad = (angle * PI) / 180;
	GLfloat rot[16] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };

	GLfloat dist = sqrtf(x*x + y*y + z*z);
	if (dist != 0) {
		GLfloat axis[3] = { x / dist, y / dist, z / dist };

		GLfloat a = rad;
		GLfloat c = cosf(a);
		GLfloat ac = 1.00f - c;
		GLfloat s = sinf(a);

		rot[0] = axis[0] * axis[0] * ac + c;
		rot[1] = axis[0] * axis[1] * ac + axis[2] * s;
		rot[2] = axis[0] * axis[2] * ac - axis[1] * s;

		rot[4] = axis[1] * axis[0] * ac - axis[2] * s;
		rot[5] = axis[1] * axis[1] * ac + c;
		rot[6] = axis[1] * axis[2] * ac + axis[0] * s;

		rot[8] = axis[2] * axis[0] * ac + axis[1] * s;
		rot[9] = axis[2] * axis[1] * ac - axis[0] * s;
		rot[10] = axis[2] * axis[2] * ac + c;

		if (WORLD) {
			glLoadIdentity();
			glMultMatrixf(rot);
			glMultMatrixf(objM);
			GLfloat m[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			for (int i = 0; i < 16; ++i) {
				objM[i] = m[i];
			}
			objx = objM[12];
			objy = objM[13];
			objz = objM[14];
		}
		else {
			glLoadIdentity();
			glMultMatrixf(objM);
			glMultMatrixf(rot);
			GLfloat m[16];
			glGetFloatv(GL_MODELVIEW_MATRIX, m);
			for (int i = 0; i < 16; ++i) {
				objM[i] = m[i];
			}
		}
		prevAngle = angle;
	}
	else {
		angle = prevAngle;
	}
	rx = 0;
	ry = 0;
	rz = 0;
}

// LookAt function for the viewing matrix
void lookAt(GLfloat posx, GLfloat posy, GLfloat posz, GLfloat tarx, GLfloat tary, GLfloat tarz, GLfloat upx, GLfloat upy, GLfloat upz) {
	GLfloat dirDist = sqrtf(powf(posx - tarx, 2) + powf(posy - tary, 2) + powf(posz - tarz, 2));
	direction[0] = (posx - tarx) / dirDist;
	direction[1] = (posy - tary) / dirDist;
	direction[2] = (posz - tarz) / dirDist;
	GLfloat crossR[3] = { upy*direction[2] - upz*direction[1], -(upx*direction[2] - upz*direction[0]), upx*direction[1] - upy*direction[0] };
	GLfloat rightDist = sqrtf(powf(crossR[0], 2) + powf(crossR[1], 2) + powf(crossR[2], 2));
	right[0] = crossR[0] / rightDist;
	right[1] = crossR[1] / rightDist;
	right[2] = crossR[2] / rightDist;
	cameraUp[0] = direction[1] * right[2] - direction[2] * right[1];
	cameraUp[1] = -(direction[0] * right[2] - direction[2] * right[0]);
	cameraUp[2] = direction[0] * right[1] - direction[1] * right[0];
	GLfloat m[16] = { right[0], cameraUp[0], direction[0], 0, right[1], cameraUp[1], direction[1], 0, right[2], cameraUp[2], direction[2], 0, 0, 0, 0, 1 };
	GLfloat t[16] = { 1,0,0,0,0,1,0,0,0,0,1,0,-posx,-posy,-posz,1 };
	glMultMatrixf(m);
	glMultMatrixf(t);
}

// The display function. It is called whenever the window needs
// redrawing (ie: overlapping window moves, resize, maximize)
// You should redraw your polygons here
void display(void) {
	// Clear the background
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (PERSPECTIVE) {
		// Perscpective Projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);
		glutSetWindowTitle("Assignment 4 (perspective)");
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	else {
		// Orthogonal Projection
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
		glutSetWindowTitle("Assignment 4 (orthogonal)");
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	// Set the camera position, orientation and target
	lookAt(camera[0], camera[1], camera[2], camera[0] + lookat[0], camera[1] + lookat[1], camera[2] + lookat[2], up[0], up[1], up[2]);

	if (displayCoordAxes) {
		// Draw Coordinate Axes
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 2, 0);
		glEnd();

		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(2, 0, 0);
		glEnd();

		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 2);
		glEnd();
	}
	glMultMatrixf(objM);
	
	glPushMatrix();
	glLoadIdentity();
	translate(transx, transy, transz);
	rotate(angle, rx, ry, rz);
	scale(uScale);

	if (displayObj) {
		// Draw mesh object
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glColor3f(1, 1, 1);
		for (int f = 0; f < faces; f++) {
			glBegin(GL_TRIANGLES);
			glVertex3f(vertList[faceList[f].v1].x, vertList[faceList[f].v1].y, vertList[faceList[f].v1].z);
			glVertex3f(vertList[faceList[f].v2].x, vertList[faceList[f].v2].y, vertList[faceList[f].v2].z);
			glVertex3f(vertList[faceList[f].v3].x, vertList[faceList[f].v3].y, vertList[faceList[f].v3].z);
		}
		glEnd();
	}

	if (displayCoordAxes) {
		// Draw Coordinate Axes
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 2, 0);
		glEnd();

		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(2, 0, 0);
		glEnd();

		glColor3f(0, 0, 1);
		glBegin(GL_LINES);
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 2);
		glEnd();
	}

	glPopMatrix();

	// (Note that the origin is lower left corner)
	// (Note also that the window spans (0,1) )
	// Finish drawing, update the frame buffer, and swap buffers
	glutSwapBuffers();
}

// This function is called whenever the window is resized. 
// Parameters are the new dimentions of the window
void resize(int x, int y) {
	glViewport(0, 0, x, y);
	window_width = x;
	window_height = y;
	if (PERSPECTIVE) {
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective(60, (GLdouble)window_width / window_height, 0.01, 10000);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}
	printf("Resized to %d %d\n", x, y);
}

// This function is called whenever the mouse is pressed or released
// button is a number 0 to 2 designating the button
// state is 1 for release 0 for press event
// x and y are the location of the mouse (in window-relative coordinates)
void mouseButton(int button, int state, int x, int y) {
	printf("Mouse click at %d %d, button: %d, state: %d,\n", x, y, button, state);

	// Saves the x and y coordinates to a global variable when a mouse button is clicked
	clickX = x;
	clickY = y;

	// Checks if the left or right mouse button is clicked
	if (button == 0) clickedButton = LEFT;
	else if (button == 1) clickedButton = MIDDLE;
	else if (button == 2) clickedButton = RIGHT;
}

// This function is called whenever the mouse is moved with a mouse button held down.
// x and y are the location of the mouse (in window-relative coordinates)
void mouseMotion(int x, int y) {
	printf("Mouse is at %d, %d\n", x, y);

	// swings the viewing axis based on the mouse motion
	if (clickedButton == LEFT && PERSPECTIVE) {
		float deltax = (x - clickX) / SENSITIVITY;
		float deltay = (y - clickY) / SENSITIVITY;
		theta += deltax;
		phi += deltay;
		if (phi > 1.55) {
			phi = 1.55;
		}
		else if (phi < -1.55){
			phi = -1.55;
		}
		lookat[0] = cosf(theta) * cosf(phi);
		lookat[1] = -sinf(phi);
		lookat[2] = sinf(theta) * cosf(phi);
		clickX = x;
		clickY = y;
	}
	else if (clickedButton == LEFT && !PERSPECTIVE) {
		float deltax = (x - clickX) / SENSITIVITY;
		float deltay = (y - clickY) / SENSITIVITY;
		alpha += deltax;
		omega += deltay;
		if (omega > 1.55) {
			omega = 1.55;
		}
		else if (omega < -1.55) {
			omega = -1.55;
		}
		lookat[0] = cosf(alpha) * cosf(omega);
		lookat[1] = -sinf(omega);
		lookat[2] = sinf(alpha) * cosf(omega);
		clickX = x;
		clickY = y;
	}
	// moves the camera up, down, right, and left in the plane perpendicular to its viewing axis.
	else if (clickedButton == MIDDLE) {
		float deltax = (x - clickX) / SENSITIVITY;
		float deltay = (y - clickY) / SENSITIVITY;
		camera[0] += deltax * right[0];
		camera[1] -= deltay * cameraUp[1];
		clickX = x;
		clickY = y;
	}
	// moves the camera forward and back along the viewing vector.
	else if (clickedButton == RIGHT && PERSPECTIVE) {
		float delta = ((y - clickY) / SENSITIVITY);
		camera[2] += delta * lookat[2];
		clickY = y;
	}

	glutPostRedisplay();
}


// This function is called whenever there is a keyboard input
// key is the ASCII value of the key pressed
// x and y are the location of the mouse
void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case 'q':	// Quit
		exit(1);
		break;
	case 'p':	// Toggle between perspective and orthogonal
		if (PERSPECTIVE) PERSPECTIVE = OFF;
		else PERSPECTIVE = ON;
		break;
	case 'a':	// Toggle display of coordinate axes
		if (displayCoordAxes) displayCoordAxes = OFF;
		else displayCoordAxes = ON;
		break;
	case 's':	// Toggle display of object
		if (displayObj) displayObj = OFF;
		else displayObj = ON;
		break;
	case 'c':	// snap camera to pointing at the world origin and pointing at the object at alternating order
		if (WORLD) {
			lookat[0] = 0;
			lookat[1] = 0;
			lookat[2] = 0;
			WORLD = OFF;
		}
		else {
			lookat[0] = objx;
			lookat[1] = objy;
			lookat[2] = objz;
			WORLD = ON;
		}
		break;

	// the following are with respect to the WORLD coordinate system
	case '4':	// negative translation along x axis
		transx = -TRANSFORMATION;
		break;
	case '6':	// positive translation along x axis
		transx = TRANSFORMATION;
		break;
	case '8':	// positive translation along y axis
		transy = TRANSFORMATION;
		break;
	case '2':	// negative translation along y axis
		transy = -TRANSFORMATION;
		break;
	case '9':	// positive translation along z axis
		transz = TRANSFORMATION;
		break;
	case '1':	// negative translation along z axis
		transz = -TRANSFORMATION;
		break;
	case '[':	// negative rotation around x axis
		WORLD = ON;
		rx = 1.0;
		angleWx = -1.0;
		angle = angleWx;
		break;
	case ']':	// positive rotation around x axis
		WORLD = ON;
		rx = 1.0;
		angleWx = 1.0;
		angle = angleWx;
		break;
	case ';':	// negative rotation around y axis
		WORLD = ON;
		ry = 1.0;
		angleWy = -1.0;
		angle = angleWy;
		break;
	case '\'':	// positive rotation around y axis
		WORLD = ON;
		ry = 1.0;
		angleWy = 1.0;
		angle = angleWy;
		break;
	case '.':	// negative rotation around z axis
		WORLD = ON;
		rz = 1.0;
		angleWz = -1.0;
		angle = angleWz;
		break;
	case '/':	// positive rotation around z axis
		WORLD = ON;
		rz = 1.0;
		angleWz = 1.0;
		angle = angleWz;
		break;
	case '=':	// increase uniform scaling
		uScale *= 1.5;
		break;
	case '-':	// decrease uniform scaling
		uScale *= 0.5;
		if (uScale) {

		}
		break;

	// the following are with respect to the LOCAL object coordinate system
	case 'i':	// negative rotation around local x axis
		WORLD = OFF;
		rx = 1.0;
		angleLx = -1.0;
		angle = angleLx;
		break;
	case 'o':	// positive rotation around local x axis
		WORLD = OFF;
		rx = 1.0;
		angleLx = 1.0;
		angle = angleLx;
		break;
	case 'k':	// negative rotation around local y axis
		WORLD = OFF;
		ry = 1.0;
		angleLy = -1.0;
		angle = angleLy;
		break;
	case 'l':	// positive rotation around local y axis
		WORLD = OFF;
		ry = 1.0;
		angleLy = 1.0;
		angle = angleLy;
		break;
	case 'm':	// negative rotation around local z axis
		WORLD = OFF;
		rz = 1.0;
		angleLz = -1.0;
		angle = angleLz;
		break;
	case ',':	// positive rotation around local z axis
		WORLD = OFF;
		rz = 1.0;
		angleLz = 1.0;
		angle = angleLz;
		break;
	default:
		break;
	}

	// Schedule a new display event
	glutPostRedisplay();
}

int main(int argc, char* argv[]) {
	// Create mesh from object file
	char *filename = "teapot.obj";
	//char *filename = "sphere.obj";
	meshReader(filename, 2);

	// Initialize GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow("Assignment 4 (orthogonal)");
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);
	glutMotionFunc(mouseMotion);
	glutKeyboardFunc(keyboard);

	// Initialize GL
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-2.5, 2.5, -2.5, 2.5, -10000, 10000);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);

	// Switch to main loop
	glutMainLoop();
	return 0;
}