#include "mbed.h"
#include "MCIFileSystem.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

typedef bool (*syncFunc)(const char* name, bool isDir);



/******************************************************************************
 * function prototypes
 *****************************************************************************/

void ledShowProgress();

void handleError(const char* msg);

bool recursiveProcessFS(char* buff, const char* name, syncFunc func, bool adding);

uint32_t fileLen(FILE* f);

bool copyFH(FILE* fSrc, FILE* fDst);

bool copy(const char* fnameSrc, const char* fnameDest);

bool list(const char* name, bool isDir);

void recursiveList(const char* dirname);

void testAppend(const char* filename);
