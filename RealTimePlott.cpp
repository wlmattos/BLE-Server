//#include <GLFW/glfw3.h>
//#include <stdlib.h>
//#include <stdio.h>
//#include "data_ecg.h"
//#include <stdio.h>
//#include "serialport.h"
//#include "serialport.c"
//#include <iostream>
//#include <fstream>
//#include <windows.h>
//#include <windowsx.h>
//#include <mmsystem.h>
//
//#pragma comment (lib , "winmm.lib")
//using namespace std;
//
////WINDOW SIZE
//const int WINDOWS_WIDTH = 640*2;
//const int WINDOWS_HEIGHT = 720;
//
//#define ECG_DATA_BUFFER_SIZE 64
//float ratio;
//int readbuffer_int[64];
//ofstream myfile ("output.txt",ios::out|ios::trunc);
//
////double data_serial[500];
////basic structure for a Vertex to simplify data passing
//typedef struct
//{
//	GLfloat x, y, z;
//	GLfloat r, g, b, a;
//} Vertex;
//
////structure for data stream
//typedef struct
//{
//	GLfloat x, y, z;
//} Data;
//
////function prototype definitiosn
//void drawLineSegment(Vertex v1, Vertex v2, GLfloat width=1.0f);
//void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width);
//void plotECGData(int offset, int size, float offset_y, float scale);
//void ecg_demo(int counter);
//char serial(int valor);
//
///**
// * Draw a grid for visualization
// */
//void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width){
//	//horizontal lines
//	for(float i=-height; i<height; i+=grid_width){
//		Vertex v1 = {-width, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
//		Vertex v2 = {width, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
//		drawLineSegment(v1, v2);
//	}
//	//vertical lines
//	for(float i=-width; i<width; i+=grid_width){
//		Vertex v1 = {i, -height, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
//		Vertex v2 = {i, height, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
//		drawLineSegment(v1, v2);
//	}
//}
//
///**
// * Draw a line segment on screen
// */
//void drawLineSegment(Vertex v1, Vertex v2, GLfloat width){
//	glLineWidth(width);
//	glBegin(GL_LINES);
//	glColor4f(v1.r, v1.g, v1.b, v1.a);
//	glVertex3f(v1.x, v1.y, v1.z);
//	glColor4f(v2.r, v2.g, v2.b, v2.a);
//	glVertex3f(v2.x, v2.y, v2.z);
//	glEnd();
//}
//
///*
// * Plot the data from the data_ecg dataset.
// */
//void plotECGData(int offset, int size, float offset_y, float scale){
//	//space between samples
//	const float space = 10.0f/size*ratio;
//
//	//initial position of the first vertex to render
//	float pos = -size*space/2.0f;
//
//	//set the width of the line
//	glLineWidth(4.0f);
//
//	glBegin(GL_LINE_STRIP);
//
//	//set the color of the line to green
//	glColor4f(0.1f, 1.0f, 0.1f, 0.8f);
//
//	for (int i=offset; i<size+offset; i++){
////		const float data = scale*readbuffer_int[i]+offset_y;
////      const float data = readbuffer_int[i];
//		const float data = scale*readbuffer_int[i]+offset_y;
//		glVertex3f(pos, data, 0.0f);
//		pos += space;
//		//Sleep(0.1);
//	}
//
//	glEnd();
//
//}
//
///*
// * Helper function for running ECG demo
// */
//void ecg_demo(int counter){
//    const int data_size=ECG_DATA_BUFFER_SIZE;
//
//    //Emulate the presence of multiple ECG leads (just for demo/display purposes)
//    plotECGData(counter, data_size, 0.0f, 0.1f);
//    //plotECGData(counter+data_size, data_size, 0.0f, 0.5f);
//    //plotECGData(counter+data_size*2, data_size, 0.5f, -0.25f);
//    //plotECGData(counter, data_size, 0.0f, 0.01f);
//}
//
//int main_2(void)
//{
////    PlaySound(TEXT("C:\\Test.wav"), NULL, SND_FILENAME);
//    //PlaySound(TEXT("C:\\olga.bin"), NULL, SND_FILENAME);
//    serial(0);
// 	GLFWwindow* window;
//
//	if (!glfwInit())
//		exit(EXIT_FAILURE);
//	window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Chapter 2: Primitive drawings", NULL, NULL);
//	if (!window)
//	{
//		glfwTerminate();
//		exit(EXIT_FAILURE);
//	}
//	glfwMakeContextCurrent(window);
//
//	//enable anti-aliasing
//	glEnable(GL_LINE_SMOOTH);
//	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
//	glEnable(GL_POINT_SMOOTH);
//	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
//	glEnable(GL_BLEND);
//	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//
//	int counter = 0;
//	while (!glfwWindowShouldClose(window))
//	{
//		int width, height;
//
//		glfwGetFramebufferSize(window, &width, &height);
//
//		//aspect ratio of the screen (changes upon resizing the window)
//		ratio = (float) width / (float)height;
//
//		glViewport(0, 0, width, height);
//		glClear(GL_COLOR_BUFFER_BIT);
//
//		glMatrixMode(GL_PROJECTION);
//		glLoadIdentity();
//
//		//Orthographic Projection
//		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
//		glMatrixMode(GL_MODELVIEW);
//
//		glLoadIdentity();
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//
//		//simple grid
//		drawGrid(5.0f, 1.0f, 0.1f);
//		counter+=1;
//
//		if(counter>16)
//         {
//             counter=0;
//         }
//        serial(0);
//        ecg_demo(counter);
//        glfwSwapBuffers(window);
//		glfwPollEvents();
//	}
//
//	glfwDestroyWindow(window);
//	glfwTerminate();
//	exit(EXIT_SUCCESS);
//    myfile.close();
//}
//
//char serial(int valor)
//{
//
//    int bytesRead;
//    int k;
//    char readbuffer[8];
//
//    int currrent_value_int;
//
//	//HANDLE h = openSerialPort("COM6",B115200,one,off);
//	//char sendbuffer[] = "test";
//
//	//write test
//	//int bytesWritten = writeToSerialPort(h,sendbuffer,strlen(sendbuffer));
//	//printf("%d Bytes were written\n",bytesWritten);
//	//read something
//
//	k = 0;
//    for (int i=0; i<8; i++){
////		bytesRead = readFromSerialPort(h,readbuffer,8);
//		readbuffer[0] = '0';
//		readbuffer[1] = '1';
//		readbuffer[2] = '2';
//		readbuffer[3] = '3';
//		readbuffer[4] = '4';
//		readbuffer[5] = '5';
//		readbuffer[6] = '6';
//		readbuffer[7] = '7';
//		readbuffer[8] = '\0';
//
//        printf("%s",readbuffer);
//        if (myfile.is_open())
//        {
//            myfile << readbuffer;
//            //myfile << '\n';
//        }
//
//        //Sleep(1);
//		for (int j=0; j<8; j++)
//            {
//           char buf[2] = {readbuffer[j], '\0' };
//           currrent_value_int = atoi(buf);
//           readbuffer_int[k] = currrent_value_int;
//           k=k+1;
//           }
//	}
// 	//closeSerialPort(h);
//    //PlaySound(TEXT("C:\\Test.wav"), NULL, SND_FILENAME);
//    return readbuffer[0];
//
//}
