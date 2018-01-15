#include "stdafx.h"

// standard
#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <vector>
#include <list>
#include <iostream>
#include <fstream>

// glut
#include <GL/glut.h>

// source
#include "vec3.h"
#include "model.h"


//================================
// global variables
//================================
//z buffer
ZBuffer g_zb;

// screen size
int g_screenWidth = 0;
int g_screenHeight = 0;

// frame index
int g_frameIndex = 0;

//illumination
Illumination illu;

// model
Model g_model;
Model g_mode2;
Model g_mode3;



//================================
// init
//================================
void init(void) {
	// init something before main loop...
	//set title
	glutSetWindowTitle("Lab 3");

	// show object in screen's size
	gluOrtho2D(0.0, window_width, 0.0, window_height);

	// load model
	g_model.LoadModel("Data/house.d");
	//Model * View * Perspective
	g_model.ModelTranslation(vec3(0, 0, 0));
	g_model.SetNormals(illu);
	g_model.ViewTranslation(vec3(0.0f, 0.0f, 0.0f), vec3(-20.0f, 0.0f, -30.0f), vec3(0.0f, 1.0f, 0.0f));
	g_model.PerspectiveTranslation(5.0f, 50.5f, 2.0f);

	
	// load mode2
	g_mode2.LoadModel("Data/house.d");
	//Model * View * Perspective
	g_mode2.ModelTranslation(vec3(20, 0, 0));
	g_mode2.SetNormals(illu);
	g_mode2.ViewTranslation(vec3(0.0f, 0.0f, 0.0f), vec3(-20.0f, 0.0f, -30.0f), vec3(0.0f, 1.0f, 0.0f));
	g_mode2.PerspectiveTranslation(5.0f, 50.5f, 2.0f);

	// load mode3
	g_mode3.LoadModel("Data/house.d");
	//Model * View * Perspective
	g_mode3.ModelTranslation(vec3(40, 0, 0));
	g_mode3.SetNormals(illu);
	g_mode3.ViewTranslation(vec3(0.0f, 0.0f, 0.0f), vec3(-20.0f, 0.0f, -30.0f), vec3(0.0f, 1.0f, 0.0f));
	g_mode3.PerspectiveTranslation(5.0f, 50.5f, 2.0f);
	

}

//================================
// update
//================================
void update(void) {
	// do something before rendering...
}

//================================
// render
//================================
void render(void) {

	// clear color and depth buffer
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearDepth(1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//fill color with different shading modes
	//0 is constant shading, 1 is Gouraud shading, 2 is Phong shading
	g_model.fillColor(g_zb.zb, illu, 2);
	g_mode2.fillColor(g_zb.zb, illu ,1);
	g_mode3.fillColor(g_zb.zb, illu, 0);

	// swap back and front buffers
	glutSwapBuffers();
}

//================================
// keyboard input
//================================
void keyboard(unsigned char key, int x, int y) {
}

//================================
// reshape : update viewport and projection matrix when the window is resized
//================================
void reshape(int w, int h) {
	// screen size
	g_screenWidth = w;
	g_screenHeight = h;

}


//================================
// timer : triggered every 16ms ( about 60 frames per second )
//================================
void timer(int value) {
	// increase frame index
	g_frameIndex++;

	update();

	// render
	glutPostRedisplay();

	// reset timer
	glutTimerFunc(360, timer, 0);
}

//================================
// main
//================================
int main(int argc, char** argv) {

	// create opengL window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(600, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(argv[0]);

	// init
	init();

	// set callback functions
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutTimerFunc(360, timer, 0);

	// main loop
	glutMainLoop();

	return 0;
}