#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio.h>
#include "serialport.h"
//#include "serialport.c"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>

#pragma comment (lib , "winmm.lib")
using namespace std;

//
////WINDOW SIZE
const int WINDOWS_WIDTH = 640*2;
const int WINDOWS_HEIGHT = 720;
//
#define ECG_DATA_BUFFER_SIZE 64

float ratio;
int readbuffer_int[64];


typedef struct
{
	GLfloat x, y, z;
	GLfloat r, g, b, a;
} Vertex;


typedef struct
{
	GLfloat x, y, z;
} Data;

//function prototype definitiosn
void drawLineSegment(Vertex v1, Vertex v2, GLfloat width=1.0f);
void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width);
void plotECGData(int offset, int size, float offset_y, float scale);
void ecg_demo(int counter);
char serial(int valor);

