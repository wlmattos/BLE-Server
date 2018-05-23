/*
#include <iostream>
#include <string>
#include <fstream>
#include <windows.h>
#include <mmsystem.h>
#include <stdlib.h>
#include <malloc.h>
#include <sndfile.h>
#include <math.h>
#include <SFML/Audio.hpp>


using namespace std;
using std::string;
using std::fstream;

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

int main_WavWrite(){
//    SNDFILE *sf;
    SF_INFO info;
//    wav_hdr wavHeader;
//    FILE *wavFile;
//    int headerSize = sizeof(wav_hdr),filelength = 0;

    INT16 sample_val = 0;
    double freq1 = 200;        // Hz
//    double freq2 = 200;        // Hz
    double duration = 10;     // Seconds
    int sampleRate = 44100;   // Frames / second

    string output;
    string answer;
    string path;
    const char* filePath_out;
    const unsigned AMPLITUDE = 30000;


    // Calculate number of frames
   long numFrames = duration * sampleRate;

   // Allocate storage for frames
   double *buffer_out = (double *) malloc(2 * numFrames * sizeof(double));
   std::vector<sf::Int16> samples;

   if (buffer_out == NULL) {
      fprintf(stderr, "Could not allocate buffer for output\n");
   }

   // Create sample, a single tone
   long f;
   long i=0;
   for (f=0 ; f<numFrames ; f++) {
      double time = f * duration / numFrames;
      buffer_out[i] = sin(2.0 * M_PI * time * freq1);   // Channel 1
      sample_val = static_cast<sf::Int16>(AMPLITUDE) * (sin(2.0 * M_PI * time * freq1));
      samples.push_back(sample_val);
      i+=1;
   }

   // Set file settings, 16bit Mono PCM
   info.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
   info.channels = 1;
   info.samplerate = sampleRate;

   path = "CH1.WAV";
   filePath_out = path.c_str();

   // Open sound file for writing
   SNDFILE *sndFile_out = sf_open(filePath_out, SFM_WRITE, &info);
   if (sndFile_out == NULL) {
      fprintf(stderr, "Erro ao gravar o arquivo '%s': %s\n", filePath_out, sf_strerror(sndFile_out));
      free(buffer_out);
      return -1;
   }
    // Write frames
   long writtenFrames = sf_writef_double(sndFile_out, buffer_out, numFrames);

    // Check correct number of frames saved
   if (writtenFrames != numFrames) {
      fprintf(stderr, "Não foi gravada a quantidade de frames.\n");
      sf_close(sndFile_out);
      free(buffer_out);
      return -1;
   }

   sf_write_sync(sndFile_out);
   sf_close(sndFile_out);
   free(buffer_out);

   sf::SoundBuffer buffer;
   buffer.loadFromSamples(samples.data(), samples.size(),1,44100);
   sf::Sound sound;
   sound.setBuffer(buffer);
   sound.play();
   while (sound.getStatus() == sf::Sound::Playing)
    {
        sf::sleep(sf::milliseconds(100));
    }
}

int main_WavRead(){
//    SNDFILE *sf;
    SF_INFO sndInfo;
//    SF_INFO info;
    wav_hdr wavHeader;
//    FILE *wavFile;
    int headerSize = sizeof(wav_hdr),filelength = 0;

//    double freq1 = 200;        // Hz
//    double freq2 = 200;        // Hz
//    double duration = 10;     // Seconds
//    int sampleRate = 44100;   // Frames / second

    string input;
    string answer;
    string path;
    const char* filePath_in;


//    cout << "Digite o nome do arquivo a ser lido: ";
//    cin >> input;
//    cin.get();
//    cout << endl;

    //const wchar_t* path = "C:\\Wav\\Wheezing.wav.WAV";
    path = "Tone.WAV";
    filePath_in = path.c_str();

    SNDFILE *sndFile = sf_open(filePath_in, SFM_READ, &sndInfo);
    FILE* wavFile_in = fopen(filePath_in, "r");
    if (wavFile_in == NULL)
    {
        fprintf(stderr, "Não foi possível abrir o arquivo: %s\n", filePath_in);
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


int main_SFML(){
sf::SoundBuffer buffer;
if (!buffer.loadFromFile("tone.wav"))
wxMessageBox(wxT("Error"), _("Error."));
sf::Sound sound(buffer);
sound.play();

while (sound.getStatus() == sf::Sound::Playing)
{
    sf::sleep(sf::milliseconds(100));
}
}
*/
