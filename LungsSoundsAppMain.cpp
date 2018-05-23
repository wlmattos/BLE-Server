/***************************************************************
 * Name:      LungsSoundsAppMain.cpp
 * Purpose:   Code for Application Frame
 * Author:    Willian (wills07@gmail.com)
 * Created:   2018-04-01
 * Copyright: Willian (none)
 * License:
 **************************************************************/
#include "LungsSoundsAppMain.h"
#include <wx/msgdlg.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include "data_ecg.h"
#include <stdio.h>
#include "serialport.h"
#include <iostream>
#include <fstream>
#include <Windows.h>
#include <windowsx.h>
#include <mmsystem.h>
#include "serialport.c"
#include "Wav_Read.cpp"
#include <string>
#include <sstream>
#include <math.h>
#include <fstream>
#include <malloc.h>
#include <sndfile.h>
#include <SFML/Audio.hpp>

using std::string;
using std::fstream;
using namespace std;

//WINDOW SIZE
const int WINDOWS_WIDTH = 1280;
const int WINDOWS_HEIGHT = 720;

//#define ECG_DATA_BUFFER_SIZE 100000
#define ECG_DATA_BUFFER_SIZE 10000
#define USART_RECEIVER_BUFFER_SIZE 500UL

//int max_frames=100000-1;
int tempo_captura;
int taxa_amostragem_2ch=10000;
int total_amostras;
int error_reading;
float ratio;
int16_t readbuffer_int1[150000];
int16_t readbuffer_int2[150000];
char buffer_bits_1[8];
char buffer_bits_2[8];
std::string filename_local1;
std::string filename_local2;



unsigned int usartReceiverBuffer[USART_RECEIVER_BUFFER_SIZE];
unsigned int usartReceiverBufferNextRead;	// Pointer to the next read location
unsigned int usartReceiverBufferNextWrite;	// Pointer to the next write location
unsigned int usartReceiverBufferLength;	// Buffer length
HANDLE h;

typedef struct  WAV_HEADER{
    char                RIFF[4];        // RIFF Header      Magic header
    unsigned long       ChunkSize;      // RIFF Chunk Size
    char                WAVE[4];        // WAVE Header
    char                fmt[4];         // FMT header
    unsigned long       Subchunk1Size;  // Size of the fmt chunk
    unsigned short      AudioFormat;    // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM Mu-Law, 258=IBM A-Law, 259=ADPCM
    unsigned short      NumOfChan;      // Number of channels 1=Mono 2=Sterio
    unsigned long       SamplesPerSec;  // Sampling Frequency in Hz
    unsigned long       bytesPerSec;    // bytes per second
    unsigned short      blockAlign;     // 2=16-bit mono, 4=16-bit stereo
    unsigned short      bitsPerSample;  // Number of bits per sample
    char                Subchunk2ID[4]; // "data"  string
    unsigned long       Subchunk2Size;  // Sampled data length

}wav_hdr;

// Function prototypes
int getFileSize(FILE *inFile);

#define ARRAY_LEN(x)    ((int) (sizeof (x) / sizeof (x [0])))
#define MAX(x,y)        ((x) > (y) ? (x) : (y))
#define MIN(x,y)        ((x) < (y) ? (x) : (y))


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
char serial(int valor);
long binary_decimal(string num);
unsigned char usartGetDataFromReceiverBuffer(void);
void usartAddDataToReceiverBuffer(unsigned char data);
unsigned char usartIsReceiverBufferEmpty(void);
int main_ReadData(wxString filename1,wxString filename2);
int main_plotdata();
int main_stopdata();
int main_SFML(wxString filename);
/**
 * Draw a grid for visualization
 */

 int main_WavWrite(){
    SF_INFO info1;
    SF_INFO info2;
    int16_t sample_val1 = 0;
    int16_t sample_val2 = 0;

    double duration = tempo_captura;     // Seconds
    int sampleRate = 5000;   // Frames / second

    string output1;
    string output2;
    string answer;
    string path1;
    string path2;
    const char* filePath_out1;
    const char* filePath_out2;

    // Calculate number of frames
   long numFrames = duration * sampleRate;

   // Allocate storage for frames
   int16_t *buffer_out1 = (int16_t *) malloc(numFrames * sizeof(int16_t));
   int16_t *buffer_out2 = (int16_t *) malloc(numFrames * sizeof(int16_t));

   std::vector<sf::Int16> samples1;
   std::vector<sf::Int16> samples2;


   // Create sample
   long f1;
   for (f1=0 ; f1<numFrames ; f1++) {
      sample_val1 = static_cast<sf::Int16>((readbuffer_int1[f1]+32767));
      buffer_out1[f1] = sample_val1;   // Channel 1
      samples1.push_back(sample_val1);
   }

   long f2;
   for (f2=0 ; f2<numFrames ; f2++) {
      sample_val2 = static_cast<sf::Int16>((readbuffer_int2[f2]+32767));
      buffer_out2[f2] = sample_val2;   // Channel 2
      samples2.push_back(sample_val2);
   }

   // Set file settings, 16bit Mono PCM
   info1.frames=numFrames;
   info1.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
   info1.channels = 1;
   info1.samplerate = sampleRate;

   // Set file settings, 16bit Mono PCM
   info2.frames=numFrames;
   info2.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
   info2.channels = 1;
   info2.samplerate = sampleRate;

   path1 = filename_local1;
   filePath_out1 = path1.c_str();

   path2 = filename_local2;
   filePath_out2 = path2.c_str();

   // Open sound file for writing
   SNDFILE *sndFile_out1 = sf_open(filePath_out1, SFM_WRITE, &info1);
   SNDFILE *sndFile_out2 = sf_open(filePath_out2, SFM_WRITE, &info2);


   long writtenFrames1 = sf_writef_short(sndFile_out1, buffer_out1, numFrames);
   long writtenFrames2 = sf_writef_short(sndFile_out2, buffer_out2, numFrames);


   sf_write_sync(sndFile_out1);
   sf_write_sync(sndFile_out2);
   sf_close(sndFile_out1);
   sf_close(sndFile_out2);
   free(buffer_out1);
   free(buffer_out2);
}

int main_WavRead(){
    SF_INFO sndInfo;
    wav_hdr wavHeader;
    int headerSize = sizeof(wav_hdr),filelength = 0;

    string input;
    string answer;
    string path;
    const char* filePath_in;

    path = filename_local1;
    filePath_in = path.c_str();

    SNDFILE *sndFile = sf_open(filePath_in, SFM_READ, &sndInfo);
    FILE* wavFile_in = fopen(filePath_in, "r");
    if (wavFile_in == NULL)
    {
        //fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", filePath_in);
        return 1;
    }
   PlaySoundA(filePath_in,NULL,SND_FILENAME|SND_SYNC);


    // Allocate memory
    float *buffer_in =  (float*) malloc(sndInfo.frames * sizeof(float));
    if (buffer_in == NULL) {
       fprintf(stderr, "Não foi possível alocar memória para o arquivo\n");
       sf_close(sndFile);
       return 1;
    }


    size_t bytesRead = fread(&wavHeader, 1, headerSize, wavFile_in);
//    cout << "Header Read " << bytesRead << " bytes." << endl;
    if (bytesRead > 0)
    {
        //Read the data
        unsigned short bytesPerSample = wavHeader.bitsPerSample / 8;      //Number     of bytes per sample
        static const short BUFFER_SIZE = 4096;
        short* buffer = new short[BUFFER_SIZE];
        delete [] buffer;
        buffer = NULL;
        filelength = getFileSize(wavFile_in);
    }
//
    sf_close(sndFile);
}



// find the file size
int getFileSize(FILE *inFile){
    int fileSize = 0;
    fseek(inFile,0,SEEK_END);

    fileSize=ftell(inFile);

    fseek(inFile,0,SEEK_SET);
    return fileSize;
}


int main_SFML(wxString filename){
filename_local1 = string(filename.mb_str());
sf::SoundBuffer buffer;
if (!buffer.loadFromFile(filename_local1))
wxMessageBox(wxT("Error"), _("Error."));
sf::Sound sound(buffer);
sound.play();

while (sound.getStatus() == sf::Sound::Playing)
{
    sf::sleep(sf::milliseconds(100));
}
}



void drawGrid(GLfloat width, GLfloat height, GLfloat grid_width)
{
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
void drawLineSegment(Vertex v1, Vertex v2, GLfloat width)
{
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
    const float space = 3.0f/size*ratio;

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
    //plotECGData(counter, data_size, 0.5f, 0.0000020f,1); // ecg
    //plotECGData(counter, data_size, -0.5f, 0.0000020f,2); // ecg
    plotECGData(counter, data_size, 0.5f, 0.0000080f,1); // ecg
    plotECGData(counter, data_size, -0.5f, 0.0000080f,2); // ecg
}

int main_stopdata()
{
    int bytesWrite=0;
    h = openSerialPort("COM5",B921600,one,off);
    if (h==INVALID_HANDLE_VALUE){
        wxMessageBox(wxT("Error Opening COM5..."), _(""));
        return(0);
    }
    else
    {
        bytesWrite = writeToSerialPort(h,"P",1);
        closeSerialPort(h);
    }
}


int main_plotdata()
{
    //serial(0);
    GLFWwindow* window;

    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Reading Data...", NULL, NULL);
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


    unsigned int exit_while=0;
    int counter = 0;
    while (!glfwWindowShouldClose(window) and (exit_while == 0))
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

		drawGrid(2.0f, 1.0f, 0.1f);
		counter+=1;
		if (counter==(total_amostras/2)) counter=0;
        ecg_demo(counter);
        glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwDestroyWindow(window);
	glfwTerminate();
}
int main_ReadData(wxString filename1,wxString filename2)
{
    filename_local1 = string(filename1.mb_str());
    filename_local2 = string(filename2.mb_str());
    h = openSerialPort("COM5",B921600,one,off);
    if (h==INVALID_HANDLE_VALUE){
        wxMessageBox(wxT("Error Opening COM5..."), _(""));
        return(0);
    }
    serial(0);

    if (error_reading==1)
    {
        //wxMessageBox(wxT("No data to read 2."), _(""));
        closeSerialPort(h);
        return (0);
    }


    main_WavWrite();
    GLFWwindow* window;

    if (!glfwInit())
        exit(EXIT_FAILURE);
    window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "Reading Data...", NULL, NULL);
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


    unsigned int exit_while=0;
    int counter = 0;
    while (!glfwWindowShouldClose(window) and (exit_while == 0))
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

		drawGrid(2.0f, 1.0f, 0.1f);
		counter+=1;
        ecg_demo(counter);
        glfwSwapBuffers(window);
		glfwPollEvents();
    }

    glfwDestroyWindow(window);
	glfwTerminate();
    closeSerialPort(h);
    //wxMessageBox(wxT("The file has been created."), _(""));
}

char serial(int valor)
{

    int contador=998;
    int contpkt=0;
    int contpkt_perdidos=0;
    int ult_pkt=0;
    int amostra=0;
    int descarta_am=0;
    int amostra_ch1=0;
    int amostra_ch2=0;
    int amostra_zero_ch1=0;
    int amostra_zero_ch2=0;
    int bytesRead=0;
    int bytesWrite=0;
    int k=0;
    int k_inc1=0;
    int k_inc2=0;
    int leituras=0;
    unsigned char readbuffer[202];
    unsigned char workbuffer[202];
    char  writebuffer[] = "S";
    BYTE byte_buffer;


    ofstream LOG_FILE ("LOG.txt",ios::out|ios::trunc);
    ofstream CH1_FILE ("CH1.txt",ios::out|ios::trunc);
    ofstream CH2_FILE ("CH2.txt",ios::out|ios::trunc);
    ofstream CH1_CH2_FILE ("CH1_CH2.txt",ios::out|ios::trunc);
    ofstream ZEROS_CH1_FILE ("ZEROS_CH1.txt",ios::out|ios::trunc);
    ofstream ZEROS_CH2_FILE ("ZEROS_CH2.txt",ios::out|ios::trunc);
    ofstream HEADER_FILE ("HEADER.txt",ios::out|ios::trunc);


    int16_t currrent_value_int=0;
    int16_t currrent_value_int16=0;
    int16_t header_int16=0;
    int16_t cont_while=0;
    int16_t cont_for=0;

    int16_t checksum_int16=0;
    int16_t currrent_value_int16_text=0;
    usartReceiverBufferLength=0;


    wxMessageBox(wxT("Press OK to start the reading."), _(""));
    bytesWrite = writeToSerialPort(h,"S",1);
    for (int k=0; k<total_amostras;){
        cont_for+=1;
        bytesRead = readFromSerialPort(h,readbuffer,202);
        if (bytesRead==202)
        {
            header_int16 = readbuffer[0]<< 8 | readbuffer[1];
            for (int j=0; j<bytesRead; j++)
            {
                usartAddDataToReceiverBuffer(readbuffer[j]);
            }
        }

        if (bytesRead==0) leituras+=1;
        if (leituras==3)
        {
            wxMessageBox(wxT("No data to read."), _(""));
            error_reading=1;
            return (0);
        }

        if (cont_for>=100000)
        {
            wxMessageBox(wxT("Error reading."), _(""));
            error_reading=1;
            return (0);
        }

        while (usartReceiverBufferLength>=202)
        {
            for (int j=0;j<202;j++)
            {
                workbuffer[j]=usartGetDataFromReceiverBuffer();
            }
            for (int j=0;j<202;j=j+2)
            {
               if (j >0 && ult_pkt>=999)
               {
                    currrent_value_int16 = workbuffer[j+1]<< 8 | workbuffer[j];
                    amostra+=1;
                    CH1_CH2_FILE << currrent_value_int16;
                    CH1_CH2_FILE << ';';
                    CH1_CH2_FILE << contador;
                    CH1_CH2_FILE << ';';
                    CH1_CH2_FILE << amostra;
                    CH1_CH2_FILE << '\n';

                   if ( k % 2 == 0)
                   {
                        currrent_value_int16 = workbuffer[j+1]<< 8 | workbuffer[j];
                        readbuffer_int1[k_inc1] = currrent_value_int16;
                        k_inc1++;
                        amostra_ch1+=1;
                        k++;
                        CH1_FILE << currrent_value_int16;
                        CH1_FILE << ';';
                        CH1_FILE << contador;
                        CH1_FILE << ';';
                        CH1_FILE << amostra_ch1;
                        CH1_FILE << '\n';
                   }
                   else
                   {
                        currrent_value_int16 = workbuffer[j+1]<< 8 | workbuffer[j];
                        readbuffer_int2[k_inc2] = currrent_value_int16;
                        k_inc2++;
                        amostra_ch2+=1;
                        k++;
                        CH2_FILE << currrent_value_int16;
                        CH2_FILE << ';';
                        CH2_FILE << contador;
                        CH2_FILE << ';';
                        CH2_FILE << amostra_ch1;
                        CH2_FILE << '\n';
                   }
               }
               if (j==0)
               {
                   currrent_value_int16 = workbuffer[j]<< 8 | workbuffer[j+1];
                   ult_pkt=currrent_value_int16;
                   if (ult_pkt >=999)
                   {
                       contador+=1;
                       HEADER_FILE << contador;
                       HEADER_FILE << ';';
                       HEADER_FILE << ult_pkt;
                       HEADER_FILE << '\n';
                   }
               }
            }
        }
	}
	contpkt_perdidos = ((ult_pkt-1)-contador);
    if (LOG_FILE.is_open())
    {
        LOG_FILE << 'R';
        LOG_FILE << ':';
        LOG_FILE << contador;
        LOG_FILE << '\n';

        LOG_FILE << 'T';
        LOG_FILE << ':';
        LOG_FILE << ult_pkt;
        LOG_FILE << '\n';

        LOG_FILE << 'A';
        LOG_FILE << ':';
        LOG_FILE << amostra;
        LOG_FILE << '\n';

        LOG_FILE << 'Z';
        LOG_FILE << '1';
        LOG_FILE << ':';
        LOG_FILE << amostra_zero_ch1;
        LOG_FILE << '\n';

        LOG_FILE << 'Z';
        LOG_FILE << '2';
        LOG_FILE << ':';
        LOG_FILE << amostra_zero_ch2;
    }

    LOG_FILE.close();
    CH1_FILE.close();
    CH2_FILE.close();
    ZEROS_CH1_FILE.close();
    ZEROS_CH2_FILE.close();
    CH1_CH2_FILE.close();
    HEADER_FILE.close();
    bytesWrite = writeToSerialPort(h,"P",1);
    error_reading=0;
    return readbuffer[0];
}


//(*InternalHeaders(LungsSoundsAppDialog)
#include <wx/bitmap.h>
#include <wx/settings.h>
#include <wx/font.h>
#include <wx/intl.h>
#include <wx/image.h>
#include <wx/string.h>
//*)
//helper functions
enum wxbuildinfoformat
{
    short_f, long_f
};

wxString wxbuildinfo(wxbuildinfoformat format)
{
    wxString wxbuild(wxVERSION_STRING);

    if (format == long_f )
    {
#if defined(__WXMSW__)
        wxbuild << _T("-Windows");
#elif defined(__UNIX__)
        wxbuild << _T("-Linux");
#endif

#if wxUSE_UNICODE
        wxbuild << _T("-Unicode build");
#else
        wxbuild << _T("-ANSI build");
#endif // wxUSE_UNICODE
    }

    return wxbuild;
}

//(*IdInit(LungsSoundsAppDialog)
const long LungsSoundsAppDialog::ID_STATICBITMAP1 = wxNewId();
const long LungsSoundsAppDialog::ID_STATICTEXT2 = wxNewId();
const long LungsSoundsAppDialog::ID_STATICTEXT1 = wxNewId();
const long LungsSoundsAppDialog::ID_SLIDER1 = wxNewId();
const long LungsSoundsAppDialog::ID_TEXTCTRL2 = wxNewId();
const long LungsSoundsAppDialog::ID_STATICTEXT4 = wxNewId();
const long LungsSoundsAppDialog::ID_TEXTCTRL3 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON4 = wxNewId();
const long LungsSoundsAppDialog::ID_STATICTEXT3 = wxNewId();
const long LungsSoundsAppDialog::ID_TEXTCTRL1 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON2 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON1 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON3 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON6 = wxNewId();
const long LungsSoundsAppDialog::ID_BUTTON5 = wxNewId();
//*)

BEGIN_EVENT_TABLE(LungsSoundsAppDialog,wxDialog)
    //(*EventTable(LungsSoundsAppDialog)
    //*)
END_EVENT_TABLE()

LungsSoundsAppDialog::LungsSoundsAppDialog(wxWindow* parent,wxWindowID id)
{
    //(*Initialize(LungsSoundsAppDialog)
    wxBoxSizer* BoxSizer4;
    wxBoxSizer* BoxSizer6;
    wxBoxSizer* BoxSizer5;
    wxBoxSizer* BoxSizer7;
    wxBoxSizer* BoxSizer8;
    wxBoxSizer* BoxSizer9;

    Create(parent, wxID_ANY, _("Sistema de Captacao de Sons Pulmonares"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE, _T("wxID_ANY"));
    BoxSizer1 = new wxBoxSizer(wxHORIZONTAL);
    BoxSizer5 = new wxBoxSizer(wxHORIZONTAL);
    StaticBitmap1 = new wxStaticBitmap(this, ID_STATICBITMAP1, wxBitmap(wxImage(_T("C:\\lungs.jpg")).Rescale(wxSize(406,278).GetWidth(),wxSize(406,278).GetHeight())), wxDefaultPosition, wxSize(406,278), wxNO_BORDER, _T("ID_STATICBITMAP1"));
    BoxSizer5->Add(StaticBitmap1, 1, wxALL, 5);
    BoxSizer1->Add(BoxSizer5, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer4 = new wxBoxSizer(wxVERTICAL);
    BoxSizer7 = new wxBoxSizer(wxVERTICAL);
    BoxSizer3 = new wxBoxSizer(wxHORIZONTAL);
    StaticText2 = new wxStaticText(this, ID_STATICTEXT2, _("Sistema de Captacao de Sons Pulmonares"), wxDefaultPosition, wxSize(393,29), 0, _T("ID_STATICTEXT2"));
    wxFont StaticText2Font(14,wxFONTFAMILY_SWISS,wxFONTSTYLE_NORMAL,wxFONTWEIGHT_BOLD,false,wxEmptyString,wxFONTENCODING_DEFAULT);
    StaticText2->SetFont(StaticText2Font);
    BoxSizer3->Add(StaticText2, 1, wxALL|wxEXPAND, 5);
    BoxSizer7->Add(BoxSizer3, 1, wxALL|wxEXPAND, 5);
    BoxSizer6 = new wxBoxSizer(wxHORIZONTAL);
    StaticText1 = new wxStaticText(this, ID_STATICTEXT1, _("Tempo de Captura"), wxDefaultPosition, wxSize(122,24), wxALIGN_RIGHT, _T("ID_STATICTEXT1"));
    BoxSizer6->Add(StaticText1, 1, wxALL, 5);
    Slider1 = new wxSlider(this, ID_SLIDER1, 20, 5, 30, wxDefaultPosition, wxSize(114,33), 0, wxDefaultValidator, _T("ID_SLIDER1"));
    Slider1->SetPageSize(10);
    Slider1->SetThumbLength(10);
    Slider1->SetSelection(10, 30);
    BoxSizer6->Add(Slider1, 1, wxALL|wxSHAPED, 5);
    TextCtrl2 = new wxTextCtrl(this, ID_TEXTCTRL2, _("20 Segundos"), wxDefaultPosition, wxSize(111,19), wxNO_BORDER, wxDefaultValidator, _T("ID_TEXTCTRL2"));
    TextCtrl2->SetMinSize(wxSize(-1,-1));
    TextCtrl2->SetMaxSize(wxSize(-1,-1));
    TextCtrl2->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));
    BoxSizer6->Add(TextCtrl2, 1, wxTOP|wxLEFT|wxALIGN_TOP|wxSHAPED, 5);
    BoxSizer7->Add(BoxSizer6, 1, wxALL|wxSHAPED, 5);
    BoxSizer8 = new wxBoxSizer(wxHORIZONTAL);
    StaticText4 = new wxStaticText(this, ID_STATICTEXT4, _("Nome do Arquivo (Canal 1):"), wxDefaultPosition, wxDefaultSize, 0, _T("ID_STATICTEXT4"));
    BoxSizer8->Add(StaticText4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    TextCtrl3 = new wxTextCtrl(this, ID_TEXTCTRL3, _("CH1.WAV"), wxDefaultPosition, wxDefaultSize, 0, wxDefaultValidator, _T("ID_TEXTCTRL3"));
    BoxSizer8->Add(TextCtrl3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button4 = new wxButton(this, ID_BUTTON4, _("Reproduzir Canal 1"), wxDefaultPosition, wxSize(122,38), 0, wxDefaultValidator, _T("ID_BUTTON4"));
    BoxSizer8->Add(Button4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer7->Add(BoxSizer8, 1, wxALL|wxEXPAND, 5);
    BoxSizer9 = new wxBoxSizer(wxHORIZONTAL);
    StaticText3 = new wxStaticText(this, ID_STATICTEXT3, _("Nome do Arquivo (Canal 2):"), wxDefaultPosition, wxSize(142,31), wxALIGN_LEFT, _T("ID_STATICTEXT3"));
    BoxSizer9->Add(StaticText3, 1, wxALL|wxALIGN_CENTER_VERTICAL|wxSHAPED, 5);
    TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("CH2.WAV"), wxDefaultPosition, wxSize(129,20), 0, wxDefaultValidator, _T("ID_TEXTCTRL1"));
    TextCtrl1->SetMaxSize(wxSize(-1,-1));
    BoxSizer9->Add(TextCtrl1, 1, wxALL|wxALIGN_CENTER_VERTICAL, 5);
    Button2 = new wxButton(this, ID_BUTTON2, _("Reproduzir Canal 2"), wxDefaultPosition, wxSize(142,32), 0, wxDefaultValidator, _T("ID_BUTTON2"));
    BoxSizer9->Add(Button2, 1, wxALL|wxEXPAND, 5);
    BoxSizer7->Add(BoxSizer9, 1, wxALL|wxEXPAND, 5);
    BoxSizer2 = new wxBoxSizer(wxHORIZONTAL);
    Button1 = new wxButton(this, ID_BUTTON1, _("Inicializar Leitura"), wxDefaultPosition, wxSize(91,38), 0, wxDefaultValidator, _T("ID_BUTTON1"));
    BoxSizer2->Add(Button1, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 4);
    Button3 = new wxButton(this, ID_BUTTON3, _("Reproduzir Sinais"), wxDefaultPosition, wxSize(98,38), 0, wxDefaultValidator, _T("ID_BUTTON3"));
    BoxSizer2->Add(Button3, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button6 = new wxButton(this, ID_BUTTON6, _("Parar Leitura"), wxDefaultPosition, wxSize(113,37), 0, wxDefaultValidator, _T("ID_BUTTON6"));
    BoxSizer2->Add(Button6, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    Button5 = new wxButton(this, ID_BUTTON5, _("Sair"), wxDefaultPosition, wxSize(88,38), 0, wxDefaultValidator, _T("ID_BUTTON5"));
    BoxSizer2->Add(Button5, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    BoxSizer7->Add(BoxSizer2, 0, wxEXPAND|wxSHAPED, 4);
    BoxSizer4->Add(BoxSizer7, 1, wxALL|wxEXPAND, 5);
    BoxSizer1->Add(BoxSizer4, 1, wxALL|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 5);
    SetSizer(BoxSizer1);
    BoxSizer1->Fit(this);
    BoxSizer1->SetSizeHints(this);

    Connect(ID_SLIDER1,wxEVT_SCROLL_TOP|wxEVT_SCROLL_BOTTOM|wxEVT_SCROLL_LINEUP|wxEVT_SCROLL_LINEDOWN|wxEVT_SCROLL_PAGEUP|wxEVT_SCROLL_PAGEDOWN|wxEVT_SCROLL_THUMBTRACK|wxEVT_SCROLL_THUMBRELEASE|wxEVT_SCROLL_CHANGED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnSlider1CmdScroll);
    Connect(ID_SLIDER1,wxEVT_COMMAND_SLIDER_UPDATED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnSlider1CmdSliderUpdated);
    Connect(ID_BUTTON4,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnButton4Click);
    Connect(ID_BUTTON2,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnButton2Click1);
    Connect(ID_BUTTON1,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnAbout);
    Connect(ID_BUTTON3,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnButton3Click1);
    Connect(ID_BUTTON6,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnButton6Click);
    Connect(ID_BUTTON5,wxEVT_COMMAND_BUTTON_CLICKED,(wxObjectEventFunction)&LungsSoundsAppDialog::OnButton5Click);
    Connect(wxID_ANY,wxEVT_INIT_DIALOG,(wxObjectEventFunction)&LungsSoundsAppDialog::OnInit);
    //*)
}

LungsSoundsAppDialog::~LungsSoundsAppDialog()
{
    //(*Destroy(LungsSoundsAppDialog)
    //*)
}

void LungsSoundsAppDialog::OnQuit(wxCommandEvent& event)
{
    Close();
}

void LungsSoundsAppDialog::OnAbout(wxCommandEvent& event)
{
    int intValue = Slider1->GetValue();
    tempo_captura = intValue;
    total_amostras = taxa_amostragem_2ch * tempo_captura;
    main_ReadData(TextCtrl3->GetValue(),TextCtrl1->GetValue());
    //wxString myString = wxString::Format(wxT("%i"),total_amostras);
    //TextCtrl2->SetValue(myString + wxT(" amostras."));
}



void LungsSoundsAppDialog::OnButton5Click(wxCommandEvent& event)
{
    Close();
}

void LungsSoundsAppDialog::OnButton3Click(wxCommandEvent& event)
{
    main_WavWrite();
    wxMessageBox(wxT("The file has been created."), _("The file has been generated."));
}

void LungsSoundsAppDialog::OnButton4Click(wxCommandEvent& event)
{
    main_SFML(TextCtrl3->GetValue());
    wxMessageBox(wxT("The file has been played."), _("The file has been played."));
}

void LungsSoundsAppDialog::OnButton2Click(wxCommandEvent& event)
{
    main_SFML(TextCtrl1->GetValue());
    wxMessageBox(wxT("Done."), _("Done."));
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




void LungsSoundsAppDialog::OnInit(wxInitDialogEvent& event)
{
}

void LungsSoundsAppDialog::OnSlider1CmdScroll(wxScrollEvent& event)
{
    //TextCtrl2->SetValue(wxT("TESTE"));
}

void LungsSoundsAppDialog::OnSlider1CmdSliderUpdated(wxScrollEvent& event)
{
    int intValue = Slider1->GetValue();
    tempo_captura = intValue;
    total_amostras = taxa_amostragem_2ch * tempo_captura;
    wxString myString = wxString::Format(wxT("%i"),intValue);
    TextCtrl2->SetValue(myString + wxT(" Seconds."));
}

void LungsSoundsAppDialog::OnButton2Click1(wxCommandEvent& event)
{
    main_SFML(TextCtrl1->GetValue());
    wxMessageBox(wxT("The file has been played."), _("The file has been played."));
}

void LungsSoundsAppDialog::OnButton3Click1(wxCommandEvent& event)
{
    main_plotdata();
}

void LungsSoundsAppDialog::OnButton6Click(wxCommandEvent& event)
{
 main_stopdata();
}
