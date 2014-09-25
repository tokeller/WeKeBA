/******************************************************************************
 * Includes
 *****************************************************************************/
 
#include "mbed.h"
#include "MCIFileSystem.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

typedef bool (*syncFunc)(const char* name, bool isDir);

/******************************************************************************
 * Local variables
 *****************************************************************************/

MCIFileSystem mcifs("mci");

DigitalOut myled1(LED1);
DigitalOut myled2(LED2);

/******************************************************************************
 * Local functions
 *****************************************************************************/

static void ledShowProgress()
{
  static int state = 0;
  state = (state + 1) % 2;
  switch (state)
  {
    case 0:
      myled1 = 1;
      myled2 = 0;
      break;

    case 1:
    default:
      myled1 = 0;
      myled2 = 1;
  }
}

static void handleError(const char* msg)
{
  printf(msg);
  while(true) {
    myled1 = 1;
    myled2 = 1;
    wait(0.3);
    myled1 = 0;
    myled2 = 0;
    wait(0.3);
  }
}

static bool recursiveProcessFS(char* buff, const char* name, syncFunc func, bool adding)
{
  uint32_t len = strlen(buff);
  if (len > 0) {
    if (buff[len - 1] != '/') {
      buff[len++] = '/';
      buff[len] = '\0';
    }
  }
  strcat(buff, name);
  len += strlen(name);
  
  if (len > 60) {
    // ugly fix to avoid crashes that occurs when file name is larger than buffer 
    // in FATFileSystem.
    printf("skipped:   %s\n", buff);
    return true;
  }

  DIR *d = opendir(buff);
  bool result = true; // success
  if (d != NULL) {
    if (adding) {
      // when processing in adding mode folders must be created before it's content
      result = func(buff, true);
    }
    struct dirent *p;
    while (result && ((p = readdir(d)) != NULL)) {
      result = recursiveProcessFS(buff, p->d_name, func, adding);
      buff[len] = '\0';
    }
    closedir(d);
    if (result && !adding) {
      // when processing in removing mode folders must be deleted after it's content
      result = func(buff, true);
    }
  } else {
    // a file
    result = func(buff, false);
  }
  return result;
}

static uint32_t fileLen(FILE* f)
{
  uint32_t pos = ftell(f);
  fseek(f, 0, SEEK_END);
  uint32_t size = ftell(f);
  fseek(f, pos, SEEK_SET);
  return size;
}

static bool copyFH(FILE* fSrc, FILE* fDst)
{
  char buff[512];
  uint32_t left = fileLen(fSrc);
  uint32_t num;
  uint32_t chunk;

  fseek(fSrc, 0, SEEK_SET);
  do {
    chunk = (left < 512) ? left : 512;
    num = fread(buff, 1, chunk, fSrc);
    if (num > 0) {
      uint32_t tmp = fwrite(buff, 1, num, fDst);
      if (tmp != num) {
        // failed to write
        return false;
      }
      left -= num;
      ledShowProgress();
    }
  } while(num > 0 && left > 0);

  // copied entire file
  return true;
}

static bool copy(const char* fnameSrc, const char* fnameDest)
{
  FILE* fhSrc = NULL;
  FILE* fhDest = NULL;
  bool success = false;
  
  do {
    fhSrc = fopen(fnameSrc, "r");
    if (fhSrc == NULL) {
      break;
    }
    
    fhDest = fopen(fnameDest, "w");
    if (fhDest == NULL) {
      break;
    }
    
    if (!copyFH(fhSrc, fhDest)) {
      break;
    }
    
    success = true;
  } while (false);
  
  if (fhSrc != NULL) {
    fclose(fhSrc);
  }
  if (fhDest != NULL) {
    fclose(fhDest);
  }
  
  return success;
}

static bool list(const char* name, bool isDir)
{
  if (isDir) {
    printf("d:         %s\n", name);
  } else {
    FILE* f = fopen(name, "r");
    if (f != NULL) {
      uint32_t len = fileLen(f);
      printf("f: %7u %s\n", len, name);
      fclose(f);
    } else {
      printf("f:     ??? %s\n", name);
    }
  }
  return true;
}

static void recursiveList(const char* dirname)
{
  printf("\nRecursive list of file and folders in %s\n", dirname);
  char* buff = (char*)malloc(512);
  if (buff != NULL)
  {
    buff[0] = '\0';
    recursiveProcessFS(buff, dirname, list, true);
    free(buff);
  }
}

static void testAppend(const char* filename)
{
    FILE *fp = fopen(filename, "a");
    if (fp != NULL) {
        fprintf(fp, "Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!Hello World!");
        fclose(fp);
    }
}

/******************************************************************************
 * Main function
 *****************************************************************************/

int main()
{
  printf("\n-----------------\n\nWelcome to the MCI file system example...\n");
  
  if (!mcifs.cardInserted()) {
    printf("Please insert a SD/MMC card...\n");
    while (!mcifs.cardInserted()) {
      wait(0.5);
    }
    printf("Card detected!\n");
  }

  recursiveList("/mci/");

  copy("/mci/expanding.txt", "/mci/expanding.old");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");
  testAppend("/mci/expanding.txt");

  recursiveList("/mci/");
  
  handleError("Program completed!\n");
}

