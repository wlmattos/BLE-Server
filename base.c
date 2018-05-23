#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "data_ecg.h"
#include <stdio.h>
#include "serialport.h"
#include "serialport.c"
#include <iostream>
#include <fstream>
#include <windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include <string>
#include <sstream>
#include <math.h>
#pragma comment (lib , "winmm.lib")



using namespace std;

//WINDOW SIZE
const int WINDOWS_WIDTH = 640*2;
const int WINDOWS_HEIGHT = 720;

#define ECG_DATA_BUFFER_SIZE 5000
#define USART_RECEIVER_BUFFER_SIZE 5000UL

float ratio;
int readbuffer_int1[5000];
int readbuffer_int2[5000];
char buffer_bits_1[8];
char buffer_bits_2[8];

unsigned int usartReceiverBuffer[USART_RECEIVER_BUFFER_SIZE];
unsigned int usartReceiverBufferNextRead;	// Pointer to the next read location
unsigned int usartReceiverBufferNextWrite;	// Pointer to the next write location
unsigned int usartReceiverBufferLength;	// Buffer length


ofstream myfile ("output.txt",ios::out|ios::trunc);

//double data_serial[500];
//basic structure for a Vertex to simplify data passing
typedef struct
{
	GLfloat x, y, z;
	GLfloat r, g, b, a;
} Vertex;

//structure for data stream
typedef struct
{
	GLfloat x, y, z;
} Data;

//function prototype definitiosn
void drawLineSegment(Vertex v1, Vertex v2, GLfloat width=1.0f);
void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width);
void plotECGData(int offset, int size, float offset_y, float scale, int channel);
void ecg_demo(int counter);
void pad(char *s, int n, int c);
char serial(int valor);
long binary_decimal(string num);
unsigned char usartGetDataFromReceiverBuffer(void);
void usartAddDataToReceiverBuffer(unsigned char data);
unsigned char usartIsReceiverBufferEmpty(void);
/**
 * Draw a grid for visualization
 */
void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width){
	//horizontal lines
	for(float i=-height; i<height; i+=grid_width){
		Vertex v1 = {-width, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		Vertex v2 = {width, i, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		drawLineSegment(v1, v2);
	}
	//vertical lines
	for(float i=-width; i<width; i+=grid_width){
		Vertex v1 = {i, -height, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		Vertex v2 = {i, height, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f};
		drawLineSegment(v1, v2);
	}
}

/**
 * Draw a line segment on screen
 */
void drawLineSegment(Vertex v1, Vertex v2, GLfloat width){
	glLineWidth(width);
	glBegin(GL_LINES);
	glColor4f(v1.r, v1.g, v1.b, v1.a);
	glVertex3f(v1.x, v1.y, v1.z);
	glColor4f(v2.r, v2.g, v2.b, v2.a);
	glVertex3f(v2.x, v2.y, v2.z);
	glEnd();
}

/*
 * Plot the data from the data_ecg dataset.
 */
void plotECGData(int offset, int size, float offset_y, float scale, int channel){
	//space between samples
	const float space = 2.0f/size*ratio;

	//initial position of the first vertex to render
	float pos = -size*space/2.0f;

	//set the width of the line
	glLineWidth(3.0f);

	glBegin(GL_LINE_STRIP);

	//set the color of the line to green
	glColor4f(0.1f, 1.0f, 0.1f, 0.8f);

	for (int i=offset; i<size+offset; i++){
        if (channel==1) // onda senoidal
        {
            const float data1 = scale*readbuffer_int1[i]+offset_y;
            glVertex3f(pos, data1, 0.0f);
        }
        else // ECG
        {
              const float data2 = scale*readbuffer_int2[i]+offset_y;
              glVertex3f(pos, data2, 0.0f);
        }
		pos += space;
	}

	glEnd();

}

/*
 * Helper function for running ECG demo
 */
void ecg_demo(int counter){
    const int data_size=ECG_DATA_BUFFER_SIZE;

    //Emulate the presence of multiple ECG leads (just for demo/display purposes)
    //plotECGData(counter, data_size, -0.5f, 0.01f);
    plotECGData(counter, data_size, 0.5f, 0.000010f,1); // ecg
    plotECGData(counter, data_size, -0.5f, 0.000010f,2); // ecg
    //plotECGData(counter, data_size, -0.5f, 0.001f);
    //plotECGData(counter, data_size, 0.0f, 0.045f);
    //plotECGData(counter+data_size, data_size, 0.0f, 0.5f);
    //plotECGData(counter+data_size*2, data_size, 0.5f, -0.25f);
    //plotECGData(counter, data_size, 0.0f, 0.01f);
}

int main(void)
{
    //myfile.open("output.txt");
//    PlaySound(TEXT("C:\\Test.wav"), NULL, SND_FILENAME);
    //PlaySound(TEXT("C:\\olga.bin"), NULL, SND_FILENAME);
    serial(0);
 	GLFWwindow* window;

	if (!glfwInit())
		exit(EXIT_FAILURE);
	window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Chapter 2: Primitive drawings", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);

	//enable anti-aliasing
	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_POINT_SMOOTH);
	glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int counter = 0;
	while (!glfwWindowShouldClose(window))
	{
		int width, height;

		glfwGetFramebufferSize(window, &width, &height);

		//aspect ratio of the screen (changes upon resizing the window)
		ratio = (float) width / (float)height;

		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT);

		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();

		//Orthographic Projection
		glOrtho(-ratio, ratio, -1.f, 1.f, 1.f, -1.f);
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//simple grid
		//drawGrid(2.0f, 1.0f, 0.1f);
		drawGrid(2.0f, 1.0f, 0.1f);
		counter+=1;

		//if(counter>16)
         //{
         //    counter=0;
         //}
        serial(0);
        ecg_demo(counter);
        glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
    myfile.close();
}

char serial(int valor)
{

    int bytesRead;
    int k;
    int k_inc1;
    int k_inc2;
    unsigned char readbuffer[20];
    unsigned char workbuffer[20];
    BYTE byte_buffer;


    int currrent_value_int;
    unsigned int currrent_value_int16;

	//HANDLE h = openSerialPort("COM5",B115200,one,off);
	HANDLE h = openSerialPort("COM5",B921600,one,off);
	//char sendbuffer[] = "test";

	//write test
	//int bytesWritten = writeToSerialPort(h,sendbuffer,strlen(sendbuffer));
	//printf("%d Bytes were written\n",bytesWritten);
	//read something

	//k = 0;
	k_inc1=0;
	k_inc2=0;
    for (int k=0; k<10000;){
        bytesRead = readFromSerialPort(h,readbuffer,20);
//		readbuffer[0] = '~';
//		readbuffer[1] = '}';
//		readbuffer[2] = '|';
//		readbuffer[3] = '{';
//		readbuffer[4] = 'z';
//		readbuffer[5] = 'y';
//		readbuffer[6] = 'x';
//		readbuffer[7] = 'w';
		//readbuffer[8] = '\0';

        //printf("%s",readbuffer);


        //Sleep(1);
		for (int j=0; j<bytesRead; j++)
        {
            usartAddDataToReceiverBuffer(readbuffer[j]);
        }
       //printf("%d %d ",bytesRead, usartReceiverBufferLength);
       //printf("%d ",bytesRead);
        while (usartReceiverBufferLength>=20)
        {
            for (int j=0;j<20;j++)
            {
                workbuffer[j]=usartGetDataFromReceiverBuffer();
            }
            //printf("%d ",usartReceiverBufferLength);
            for (int j=0;j<20;j=j+2)
            {
               currrent_value_int16 = 0;
               currrent_value_int16 = readbuffer[j];
               currrent_value_int16 <<= 8;
               currrent_value_int16 |=readbuffer[j+1];
               if ( k % 2 == 0)
               {
                   readbuffer_int1[k_inc1] = currrent_value_int16;
                   k_inc1++;
                   k++;
               }
               else
               {
                   readbuffer_int2[k_inc2] = currrent_value_int16;
                   k_inc2++;
                   k++;
               }
               //printf("%d\n",k_inc1);


              if (myfile.is_open())
                {
                    myfile << currrent_value_int16;
                    myfile << '\n';
                }
              printf("%d\n",currrent_value_int16);
            }

        }
        //printf("\n");

            //currrent_value_int = static_cast<int>(readbuffer[j]);
            //itoa(currrent_value_int,buffer_bits_1,2);
            //pad(buffer_bits_1, 1, '0');

            //currrent_value_int = readbuffer[j+1];
            //itoa(currrent_value_int,buffer_bits_2,2);
            //pad(buffer_bits_2, 2, '0');
            //char buffer_final[16];
            //char buffer_final_12[12];
            //char buffer_completo[16];
            //strcpy(buffer_completo,buffer_bits_2);
            //strcat(buffer_completo,buffer_bits_1);
            //strncpy(buffer_final_12,buffer_completo,12);
            //std::string buffer_string_12_menos=buffer_completo;
            //buffer_string_12_menos = buffer_string_12_menos.substr(0,16);
            //buffer_string_12_menos = buffer_string_12_menos.substr(4,15);
            //std::string buffer_string_12=buffer_final_12;
            //buffer_string_12 = buffer_string_12.substr(0,12);
            //long current_value_long = binary_decimal(buffer_string_12_menos);
            //current_value_long = current_value_long;
            //if (current_value_long<=0) current_value_long=0;

              //printf("%d\n",bytesRead);

          //    j=j+1;
        //}
	}
 	closeSerialPort(h);
    return readbuffer[0];
}

void pad(char *s, int n, int c) {
    char p_buffer[8];
    int tamanho=strlen(s);
    switch (tamanho)
    {
    case 0:
        strcpy(p_buffer,"00000000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 1:
        strcpy(p_buffer,"0000000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 2:
        strcpy(p_buffer,"000000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 3:
        strcpy(p_buffer,"00000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 4:
        strcpy(p_buffer,"0000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 5:
        strcpy(p_buffer,"000");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 6:
        strcpy(p_buffer,"00");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 7:
        strcpy(p_buffer,"0");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    case 8:
        strcpy(p_buffer,"");
        if (n==1) strcat(p_buffer,buffer_bits_1);
        else strcat(p_buffer,buffer_bits_2);
        break;
    }
    if (n==1) strcpy(buffer_bits_1,p_buffer);
    else strcpy(buffer_bits_2,p_buffer);
}
long binary_decimal(string num) /* Function to convert binary to dec */
{
    long dec = 0, n = 1, exp = 0;
    string bin = num;
       if(bin.length() > 1020){
          cout << "Binary Digit too large" << endl;
       }
       else {

            for(int i = bin.length() - 1; i > -1; i--)
            {
                 n = pow(2,exp++);
                 if(bin.at(i) == '1')
                 dec += n;
            }

       }
  return dec;
}


/* -----------------------------------------------------------------------------
 * Adds data to the reception buffer. The function has NO CONTROL of lost data.
 * -------------------------------------------------------------------------- */



void usartAddDataToReceiverBuffer(unsigned char data)
{
	if(((usartReceiverBufferNextWrite + 1) % USART_RECEIVER_BUFFER_SIZE) != usartReceiverBufferNextRead){
		usartReceiverBuffer[usartReceiverBufferNextWrite] = data;
		usartReceiverBufferNextWrite = (usartReceiverBufferNextWrite + 1) % USART_RECEIVER_BUFFER_SIZE;
		usartReceiverBufferLength++;
	}
	return;
}

/* -----------------------------------------------------------------------------
 * Gets data from the reception buffer. If the buffer is empty, the last
 * retrieved data will be returned and the pointer will not be changed. The
 * usartIsReceiverBufferEmpty() function must be called to check if there is new
 * data in the buffer.
 * -------------------------------------------------------------------------- */

unsigned char usartGetDataFromReceiverBuffer(void)
{
	unsigned char data = usartReceiverBuffer[usartReceiverBufferNextRead];
	if(usartReceiverBufferLength > 0){
		usartReceiverBufferNextRead = (usartReceiverBufferNextRead + 1) % USART_RECEIVER_BUFFER_SIZE;
		usartReceiverBufferLength--;
	}
	return data;
}

/* -----------------------------------------------------------------------------
 * Verifies if there is new data in the receiver buffer. Must be called before
 * reading the buffer.
 * -------------------------------------------------------------------------- */

unsigned char usartIsReceiverBufferEmpty(void)
{
	if(usartReceiverBufferLength == 0)
		return TRUE;
	return FALSE;
}
