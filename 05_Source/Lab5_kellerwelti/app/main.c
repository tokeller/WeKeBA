
#include <RTL.h>
#include <stdio.h>
#include "inc/hw_types.h"
#include "main.h"
#include "hal_led.h"
#include "hal_system.h"
//from here new for lab4
#include "uartstdio.h"
#include "ustdlib.h"
#include "cmdline.h"
//from here for Lab5
#include "sound.h"
#include "inc/hw_ints.h"
#include "driverlib/interrupt.h"
#include <string.h>
//******************************************************************************
//
// Basic wav file RIFF header information used to open and read a wav file.
//
//******************************************************************************
#define RIFF_CHUNK_ID_RIFF      0x46464952
#define RIFF_CHUNK_ID_FMT       0x20746d66
#define RIFF_CHUNK_ID_DATA      0x61746164

#define RIFF_TAG_WAVE           0x45564157

#define RIFF_FORMAT_UNKNOWN     0x0000
#define RIFF_FORMAT_PCM         0x0001
#define RIFF_FORMAT_MSADPCM     0x0002
#define RIFF_FORMAT_IMAADPCM    0x0011

//******************************************************************************
//
// The wav file header information.
//
//******************************************************************************
typedef struct
{
    //
    // Sample rate in bytes per second.
    //
    unsigned long ulSampleRate;

    //
    // The average byte rate for the wav file.
    //
    unsigned long ulAvgByteRate;

    //
    // The size of the wav data in the file.
    //
    unsigned long ulDataSize;

    //
    // The number of bits per sample.
    //
    unsigned short usBitsPerSample;

    //
    // The wav file format.
    //
    unsigned short usFormat;

    //
    // The number of audio channels.
    //
    unsigned short usNumChannels;
}
tWaveHeader;

//static tWaveHeader g_sWaveHeader;


typedef enum state {
    S_PLAY, S_STOP, S_PAUSE
} State;

typedef enum event {
    E_NO_EVENT, E_PRESS_PLAY, E_PRESS_STOP, E_PRESS_PAUSE
} Event;

Event event = E_NO_EVENT;
static char song[80]= "\0";
static char thesong[]="M:rockets.wav";
tWaveHeader g_songHeader;
static tWaveHeader *pSongHeader = &g_songHeader;
OS_MUT mutex;

//*****************************************************************************
//
// Defines the size of the buffers that hold the path, or temporary
// data from the SD card.  There are two buffers allocated of this size.
// The buffer size must be large enough to hold the longest expected
// full path name, including the file name, and a trailing null character.
//
//*****************************************************************************
#define PATH_BUF_SIZE   80

//*****************************************************************************
//
// Defines the size of the buffer that holds the command line.
//
//*****************************************************************************
#define CMD_BUF_SIZE    64

//*****************************************************************************
//
// This buffer holds the full path to the current working directory.
// Initially it is root ("/").
//
//*****************************************************************************
static char g_cCwdBuf[PATH_BUF_SIZE] = "/";

//*****************************************************************************
//
// The buffer that holds the command line.
//
//*****************************************************************************
static char g_cCmdBuf[CMD_BUF_SIZE];

#define INITIAL_VOLUME_PERCENT 60

//*******************************************************************************
//
//Variablen für Player Steuerung
//
//*******************************************************************************


static unsigned short g_usCount;

FILE *Fptr;

char psFileObject[]="M:ROCKETS.WAV";


//*****************************************************************************
//
// This function implements the "help" command.  It prints a simple list
// of the available commands with a brief description.
//
//*****************************************************************************
int
Cmd_help(int argc, char *argv[])
{
    tCmdLineEntry *pEntry;

    //
    // Print some header text.
    //
    UARTprintf("\nAvailable commands\n");
    UARTprintf("------------------\n");

    //
    // Point at the beginning of the command table.
    //
    pEntry = &g_sCmdTable[0];

    //
    // Enter a loop to read each entry from the command table.  The
    // end of the table has been reached when the command name is NULL.
    //
    while(pEntry->pcCmd)
    {
        //
        // Print the command name and the brief description.
        //
        UARTprintf("%s%s\n", pEntry->pcCmd, pEntry->pcHelp);

        //
        // Advance to the next entry in the table.
        //
        pEntry++;

        //
        // Wiat for the UART to catch up.
        //
        UARTFlushTx(false);
    }

    //
    // Return success.
    //
    return(0);
}


//*****************************************************************************
//
// This is the table that holds the command names, implementing functions,
// and brief description.
//
//*****************************************************************************
tCmdLineEntry g_sCmdTable[] =
{
    { "help",   Cmd_help,      " : Display list of commands" },
    { "h",      Cmd_help,   "    : alias for help" },
    { "?",      Cmd_help,   "    : alias for help" },
    { "ls",     Cmd_ls,      "   : Display list of files" },
    { "cat",    Cmd_cat,      "  : Show contents of a text file" },
    { "play",   Cmd_play,     "  : Play a sound file" },
    { "pause",  Cmd_pause,    "  : Pause audio playback" },
    { "stop",   Cmd_stop,     "  : Stop audio playback" },
    { "dummy",  Cmd_dummy,      "  : dummy cmd" },
    { 0, 0, 0 }
};




//*****************************************************************************
//
// This function implements the "dummy" command.  It simply prints the
// current working directory.
//
//*****************************************************************************
int
Cmd_dummy(int argc, char *argv[])
{

	
 UARTprintf("\nEnter your code here\n");
	
    return(0);
}
//************* END Dummy ********************



//*****************************************************************************
//
// This function implements the "ls" command.  It opens the current
// directory and enumerates through the contents, and prints a line for
// each item it finds.  It shows details such as file attributes, time and
// date, and the file size, along with the name.  It shows a summary of
// file sizes at the end along with free space.
//
//*****************************************************************************
int
Cmd_ls(int argc, char *argv[])
{
FINFO info;
	info.fileID = 0;
	
	while (ffind ("M:*.*", &info) == 0) 
	{
		UARTprintf("\nName: %s %5d bytes ID: %04d", info.name, info.size, info.fileID);
	}
	if (info.fileID == 0) {
	UARTprintf("Empty Directory");
	}
    return(0);
}
//*****************************************************************************
// This function implements the "cat" command.
//*****************************************************************************

int
Cmd_cat(int argc, char *argv[])
{
	//FILE *fptr;
	char c;
	if(argc == 2) {
		Fptr = fopen(argv[1],"r");
		if (Fptr == NULL)  {
			UARTprintf("\nFile doesn't exist!\n");
		}
		else  {
			UARTprintf("\n");
			while(!feof(Fptr)) {
				c = fgetc(Fptr);
				if(c != '\0') {
					UARTprintf("%c",c);
				}
			}
			UARTprintf("\n");
			fclose(Fptr);
		}
	}
	else {
		UARTprintf("\nArgument missing!\n");
	}
	return(0);
}


//******************************************************************************
//
// Implementation of the play command
//
//******************************************************************************

int 
Cmd_play(int argc, char *argv[])
{
    if(argc == 2){
        UARTprintf("song to play: %s\n", argv[1]);
        strcpy(song, argv[1]);
        //strcpy(song, thesong);
        event = E_PRESS_PLAY;
    } else {
        event = E_NO_EVENT;
        UARTprintf("Gibsch Songtitel ah, mann!\n");
    }
    return 0;
}

//******************************************************************************
//
// Implementation of the stop command
//
//******************************************************************************

int 
Cmd_stop(int argc, char *argv[])
{
    event = E_PRESS_STOP;
    
    return 0;
}

//******************************************************************************
//
// Implementation of the play command
//
//******************************************************************************

int 
Cmd_pause(int argc, char *argv[])
{
    event = E_PRESS_PAUSE;
    
    return 0;
}


//******************************************************************************
//
// State information for keep track of time.
//
//******************************************************************************
static unsigned long g_ulBytesPlayed;

//******************************************************************************
//
// Buffer management and flags.
//
//******************************************************************************
#define AUDIO_BUFFER_SIZE       4096
static unsigned char g_pucBuffer[AUDIO_BUFFER_SIZE];
unsigned long g_ulMaxBufferSize;

//
// Flags used in the g_ulFlags global variable.
//
#define BUFFER_BOTTOM_EMPTY     0x00000001
#define BUFFER_TOP_EMPTY        0x00000002

static volatile unsigned long g_ulFlags;

//
// Globals used to track playback position.
//

static unsigned long g_ulBytesRemaining;
static unsigned short g_usMinutes;
static unsigned short g_usSeconds;


//******************************************************************************
//
// Handler for buffers being released.
//
//******************************************************************************
void
BufferCallback(void *pvBuffer, unsigned long ulEvent)
{
    if(ulEvent & BUFFER_EVENT_FREE)
    {
        if(pvBuffer == g_pucBuffer)
        {
            //
            // Flag if the first half is free.
            //
            g_ulFlags |= BUFFER_BOTTOM_EMPTY;
        }
        else
        {
            //
            // Flag if the second half is free.
            //
            g_ulFlags |= BUFFER_TOP_EMPTY;
        }

        //
        // Update the byte count.
        //
        g_ulBytesPlayed += AUDIO_BUFFER_SIZE >> 1;
    }
}


//******************************************************************************
//
// This function can be used to test if a file is a wav file or not and will
// also return the wav file header information in the pWaveHeader structure.
// If the file is a wav file then the psFileObject pointer will contain an
// open file pointer to the wave file ready to be passed into the WavePlay()
// function.
//
//******************************************************************************
FRESULT
WaveOpen(const char *pcFileName, tWaveHeader *pWaveHeader)
{
	  unsigned long *pulBuffer;
    unsigned short *pusBuffer;
    unsigned long ulChunkSize;
    unsigned long ulBytesPerSample;


    pulBuffer = (unsigned long *)g_pucBuffer;
    pusBuffer = (unsigned short *)g_pucBuffer;


		Fptr = fopen(pcFileName, "r");
		if(Fptr == NULL)
    {
        return(FR_INVALID_OBJECT);
    }
		
    //
    // Read the first 12 bytes.
    //
		
		fread (&g_pucBuffer[0], 1, 12, Fptr);    
		
    //
    // Look for RIFF tag.
    //
    if((pulBuffer[0] != RIFF_CHUNK_ID_RIFF) || (pulBuffer[2] != RIFF_TAG_WAVE))
    {
        fclose(Fptr);
        return(FR_INVALID_OBJECT);
    }

    //
    // Read the next chunk header.
    //
	
    fread(&g_pucBuffer[0], 1, 8, Fptr);    
    

    if(pulBuffer[0] != RIFF_CHUNK_ID_FMT)
    {
        fclose(Fptr);
        return(FR_INVALID_OBJECT);
    }

    //
    // Read the format chunk size.
    //
    ulChunkSize = pulBuffer[1];

    if(ulChunkSize > 16)
    {
        fclose(Fptr);
        return(FR_INVALID_OBJECT);
    }
    //
    // Read the next chunk header.
    //
		
    fread(&g_pucBuffer[0], 1, ulChunkSize, Fptr);    

    pWaveHeader->usFormat = pusBuffer[0];
    pWaveHeader->usNumChannels =  pusBuffer[1];
    pWaveHeader->ulSampleRate = pulBuffer[1];
    pWaveHeader->ulAvgByteRate = pulBuffer[2];
    pWaveHeader->usBitsPerSample = pusBuffer[7];

    //
    // Reset the byte count.
    //
    g_ulBytesPlayed = 0;


    //
    // Calculate the Maximum buffer size based on format.  There can only be
    // 1024 samples per ping pong buffer due to uDMA.
    //
    ulBytesPerSample = (pWaveHeader->usBitsPerSample *
                        pWaveHeader->usNumChannels) >> 3;

    if(((AUDIO_BUFFER_SIZE >> 1) / ulBytesPerSample) > 1024)
    {
        //
        // The maximum number of DMA transfers was more than 1024 so limit
        // it to 1024 transfers.
        //
        g_ulMaxBufferSize = 1024 * ulBytesPerSample;
    }
    else
    {
        //
        // The maximum number of DMA transfers was not more than 1024.
        //
        g_ulMaxBufferSize = AUDIO_BUFFER_SIZE >> 1;
    }

    //
    // Only mono and stereo supported.
    //
    if(pWaveHeader->usNumChannels > 2)
    {
        fclose(Fptr);
        return(FR_INVALID_OBJECT);
    }

    //
    // Read the next chunk header.
    //
	
    fread(&g_pucBuffer[0], 1, 8, Fptr);    

    if(pulBuffer[0] != RIFF_CHUNK_ID_DATA)
    {
        fclose(Fptr);
        return(FR_INVALID_OBJECT);
    }

    //
    // Save the size of the data.
    //
    pWaveHeader->ulDataSize = pulBuffer[1];

    g_usSeconds = pWaveHeader->ulDataSize/pWaveHeader->ulAvgByteRate;
    g_usMinutes = g_usSeconds/60;
    g_usSeconds -= g_usMinutes*60;

    //
    // Set the number of data bytes in the file.
    //
    g_ulBytesRemaining = pWaveHeader->ulDataSize;

    //
    // Adjust the average bit rate for 8 bit mono files.
    //
    if((pWaveHeader->usNumChannels == 1) && (pWaveHeader->usBitsPerSample == 8))
    {
        pWaveHeader->ulAvgByteRate <<=1;
    }

    //
    // Set the format of the playback in the sound driver.
    //
    SoundSetFormat(pWaveHeader->ulSampleRate, pWaveHeader->usBitsPerSample,
                   pWaveHeader->usNumChannels);
	
		

    return(FR_OK);
}


//******************************************************************************
//
// This closes out the wav file.
//
//******************************************************************************
// void
// WaveClose(void)
// {
//     //
//     // Close out the file.
//     //
//     fclose(Fptr);
// }
//******************************************************************************
//
// Convert an 8 bit unsigned buffer to 8 bit signed buffer in place so that it
// can be passed into the i2s playback.
//
//******************************************************************************
void
Convert8Bit(unsigned char *pucBuffer, unsigned long ulSize)
{
    unsigned long ulIdx;

    for(ulIdx = 0; ulIdx < ulSize; ulIdx++)
    {
        //
        // In place conversion of 8 bit unsigned to 8 bit signed.
        //
        *pucBuffer = ((short)(*pucBuffer)) - 128;
        pucBuffer++;
    }
}




//******************************************************************************
//
// This function will handle reading the correct amount from the wav file and
// will also handle converting 8 bit unsigned to 8 bit signed if necessary.
//
//******************************************************************************
unsigned short
WaveRead(const char *pcFileName, tWaveHeader *pWaveHeader, unsigned char *pucBuffer)
{
    unsigned long ulBytesToRead;
	 
    unsigned short usCount;
    //
    // Either read a half buffer or just the bytes remaining if we are at the
    // end of the file.
    //
    if(g_ulBytesRemaining < g_ulMaxBufferSize)
    {
        ulBytesToRead = g_ulBytesRemaining;
    }
    else
    {
        ulBytesToRead = g_ulMaxBufferSize;
    }

    // Calculate Number of Bytes which were read out and
		// Read out Sound Data from the File

		
		usCount = fread (&pucBuffer[0], 1, ulBytesToRead, Fptr);     
		if ( usCount == 0 ) {
			return 0;
	  }
	
    //
    // Decrement the number of data bytes remaining to be read.
    //
    g_ulBytesRemaining -= usCount;

    //
    // Need to convert the audio from unsigned to signed if 8 bit
    // audio is used.
    //
    if(pWaveHeader->usBitsPerSample == 8)
    {
        Convert8Bit(pucBuffer, usCount);
    }

    return(usCount);
}
//******************************************************************************
//
// This will play the file passed in via the psFileObject parameter based on
// the format passed in the pWaveHeader structure.  The WaveOpen() function
// can be used to properly fill the pWaveHeader and psFileObject structures.
//
//******************************************************************************
unsigned long
WavePlay(const char *pcFileName, tWaveHeader *pWaveHeader)
{
        //
        // Must disable I2S interrupts during this time to prevent state
        // problems.
        //
        IntDisable(INT_I2S0);

        //
        // If the refill flag gets cleared then fill the requested side of the
        // buffer.
        //
        if(g_ulFlags & BUFFER_BOTTOM_EMPTY)
        {
            //
            // Read out the next buffer worth of data.
            //
            g_usCount = WaveRead(psFileObject, pWaveHeader, g_pucBuffer);

            //
            // Start the playback for a new buffer.
            //
            SoundBufferPlay(g_pucBuffer, g_usCount, BufferCallback);

            //
            // Bottom half of the buffer is now not empty.
            //
            g_ulFlags &= ~BUFFER_BOTTOM_EMPTY;
        }

        if(g_ulFlags & BUFFER_TOP_EMPTY)
        {
            //
            // Read out the next buffer worth of data.
            //
            g_usCount = WaveRead(psFileObject, pWaveHeader,
                               &g_pucBuffer[AUDIO_BUFFER_SIZE >> 1]);

            //
            // Start the playback for a new buffer.
            //
            SoundBufferPlay(&g_pucBuffer[AUDIO_BUFFER_SIZE >> 1],
                            g_usCount, BufferCallback);

            //
            // Top half of the buffer is now not empty.
            //
            g_ulFlags &= ~BUFFER_TOP_EMPTY;
			
        }
        //
        // Audio playback is done once the count is below a full buffer.
        //
        if((g_usCount < g_ulMaxBufferSize) || (g_ulBytesRemaining == 0))
        {
            //
            // No longer playing audio.
            //
            event = E_PRESS_STOP;

            //
            // Wait for the buffer to empty.
            //
            while(g_ulFlags != (BUFFER_TOP_EMPTY | BUFFER_BOTTOM_EMPTY))
            {
            }
						fclose(Fptr);
        }

        //
        // Must disable I2S interrupts during this time to prevent state
        // problems.
        //
        IntEnable(INT_I2S0);

    return(0);
}



//*****************************************************************************
//
// TASKS
//
//*****************************************************************************
OS_TID tskID2, tskID3;
__task void audioplay_task(void);
__task void UART_task(void)
{
    int nStatus;
    
    tskID3 = os_tsk_create(audioplay_task, 0x20);
    os_dly_wait(1);
    while(1){
        os_mut_wait(&mutex, 0xFFFF);
        //
        // Print a prompt to the console.  Show the CWD.
        //
        UARTprintf("\n%s> ", g_cCwdBuf);

        //
        // Get a line of text from the user.
        //
        UARTgets(g_cCmdBuf, sizeof(g_cCmdBuf));

        //
        // Pass the line from the user to the command processor.
        // It will be parsed and valid commands executed.
        //
        nStatus = CmdLineProcess(g_cCmdBuf);

        //
        // Handle the case of bad command.
        //
        if(nStatus == CMDLINE_BAD_CMD)
        {
            UARTprintf("Bad command!\n");
        }

        //
        // Handle the case of too many arguments.
        //
        else if(nStatus == CMDLINE_TOO_MANY_ARGS)
        {
            UARTprintf("Too many arguments for command processor!\n");
        }
        
        os_mut_release(&mutex);
    }
}


__task void audioplay_task(void){
    // State machine
    
    static State currentState = S_STOP;
    static State nextState = S_STOP;
    unsigned long result;
    
    while(1){
        
        os_mut_wait(&mutex, 0xFFFF);
        if(event != E_NO_EVENT){
            switch(currentState){
                case(S_STOP):
                    switch(event){
                        case(E_PRESS_PLAY):
                            UARTprintf("stop->play %s\n",song);
                            result = WaveOpen(song, pSongHeader);
                            if(result == FR_OK){
                                UARTprintf("start playing %s.\n", song);
                                g_ulFlags = BUFFER_BOTTOM_EMPTY | BUFFER_TOP_EMPTY;
                                WavePlay(song, pSongHeader);
                                nextState = S_PLAY;
                            } else {
                                UARTprintf("geht net: %d\n", result);
                                nextState = S_STOP;
                            }
                            event = E_NO_EVENT;
                            break;
                        
                        case(E_PRESS_PAUSE):
                            UARTprintf("stop->pause\n");
                            nextState = S_PAUSE;
                            event = E_NO_EVENT;
                            
                            break;
                        
                        case(E_PRESS_STOP):
                            UARTprintf("stop->stop\n");
                            event = E_NO_EVENT;
                            break;
                        default:
                            event = E_NO_EVENT;
                            break;
                    }
                    break;
                
                case(S_PLAY):
                    
                    switch(event){
                        case(E_PRESS_PLAY):
                            UARTprintf("play->play\n");
                            event = E_NO_EVENT;
                            break;
                        
                        case(E_PRESS_PAUSE):
                            UARTprintf("play->pause\n");
                            nextState = S_PAUSE;
                            event = E_NO_EVENT;
                            break;
                        
                        case(E_PRESS_STOP):
                            UARTprintf("play->stop\n");
                            nextState = S_STOP;
                            event = E_NO_EVENT;
                            break;
                        default:
                            event = E_NO_EVENT;
                            break;
                    }
                    break;
                
                case(S_PAUSE):
                    
                    switch(event){
                        case(E_PRESS_PLAY):
                            UARTprintf("pause->play\n");
                            nextState = S_PLAY;
                            event = E_NO_EVENT;
                            break;
                        
                        case(E_PRESS_PAUSE):
                            UARTprintf("pause->play(unpause)\n");
                            nextState = S_PLAY;
                            event = E_NO_EVENT;
                            break;
                        
                        case(E_PRESS_STOP):
                            UARTprintf("pause->stop\n");
                            nextState = S_STOP;
                            event = E_NO_EVENT;
                            break;
                        default:
                            event = E_NO_EVENT;
                            break;
                    }
                    break;
                
                default:
                    break;
            }
            currentState = nextState;
        } else if(currentState == S_PLAY){
            WavePlay(song, pSongHeader);
        }
        os_mut_release(&mutex);
    } 
    
        
}


__task void init (void) {
    tskID2 = os_tsk_create(UART_task, 0x20);
    os_tsk_delete_self();
}


//*****************************************************************************
//
// BEGIN MAIN
//
//*****************************************************************************

int main(void) 
{
  sysInit_hal();
  ledInit_hal();
	finit();
  PinoutSet();   // Start the RTX
    os_mut_init(mutex);
    os_sys_init (init);

    
	  //
    // Configure the I2S peripheral.
    //
    SoundInit(0);

    //
    // Set the initial volume to something sensible.  Beware - if you make the
    // mistake of using 24 ohm headphones and setting the volume to 100% you
    // may find it is rather too loud!
    //
    SoundVolumeSet(INITIAL_VOLUME_PERCENT);
	
	
 
	
    //
    // Enter an (almost) infinite loop for reading and processing commands from
    //the user.
    //
    while(1)
    { 
    }
}


