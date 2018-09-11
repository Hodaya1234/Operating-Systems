/**
 * Hodaya Koslowsky 313377673
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <sys/stat.h>

#define ERROR_BAD_ARGUMENTS "Bad arguments, try again.\n"
#define ERROR_BAD_NUM_ARGS "Bad number of arguments, try again.\n"
#define ERROR_SYSTEM_CALL "Error in system call.\n"


typedef enum { CHAR_IDENTICAL, CASE_DIFF, FIRST_SPACE, SECOND_SPACE, BOTH_SPACE, CHAR_DIFF, CHAR_EMPTY } CompareCharResult;
typedef enum {TRUE, FALSE} Bool;
typedef enum { FILES_IDENTICAL=3, FILES_SIMILAR=2, FILES_DIFF=1, ERROR=4 } FILESResult;
const char spaces[] = {'\n', ' '};

/**
 * Print error message to stderr
 */
void PrintError(){
  write(2, ERROR_SYSTEM_CALL, strlen(ERROR_SYSTEM_CALL));
}

/**
 * Get a file size in bytes.
 * @param filename the file
 * @return int of how many bytes are in the file
 */
int GetFileSize(char *filename) {
  struct stat fileStat;
  if (stat(filename, &fileStat) < 0){
    return -1;
  }
  return (int)fileStat.st_size;
}

/**
 * Copy a file from filename to char*
 * @param filename the name of the file
 * @return a pointer to the beginning of the buffered
 */
char* CopyFileDynamically(char *filename, int fileSize){
  if(filename == NULL || fileSize < 0){
    return NULL;
  }
  if(fileSize == 0){
    return calloc(1, sizeof(char));
  }
  char *copiedFile = malloc(sizeof(char)*fileSize + 1);
  if(copiedFile == NULL){ return NULL; }
  int fileDescriptor = open(filename, O_RDONLY);
  if (fileDescriptor < 0){
    PrintError();
    free(copiedFile);
    return NULL;
  }
  ssize_t numRead = read(fileDescriptor, copiedFile, (size_t)fileSize);
  if (numRead < 0) {
    PrintError();
    free(copiedFile);
    close(fileDescriptor);
    return NULL;
  }
  close(fileDescriptor);
  return copiedFile;
}


/**
 * Check if a character represents space.
 * @param c the given character.
 * @return true if it represents a space character, and false otherwise
 */
Bool IsCharSpace(char c) {
  int i;
  for (i = 0; i < sizeof(spaces); i++){
    if(c == spaces[i]) {
      return TRUE;
    }
  }
  return FALSE;
}

/**
 * Compare two characters.
 * @param c1 first char
 * @param c2 second char
 * @return 0 if they are identical.
 *
 */
CompareCharResult CompareCharacters(char c1, char c2){
  if (c1 == '\0' || c2 == '\0') {
    return CHAR_EMPTY;
  }
  if (c1 == c2){ return CHAR_IDENTICAL; }
  if (IsCharSpace(c1) == TRUE) {
    if (IsCharSpace(c2) == TRUE) {
      return BOTH_SPACE;
    }
    return FIRST_SPACE;
  }
  if (IsCharSpace(c2) == TRUE){
    return SECOND_SPACE;
  }
  if (tolower(c1) == tolower(c2)) { return CASE_DIFF; }
  return CHAR_DIFF;
}

/**
 * Compare two files and check if they are identical.
 * @param first first file
 * @param second second file
 * @return 1 if they are different, 2 if they are similar and 3 if they are identical.
 */
FILESResult CompareFiles(char *first, char *second, int size1, int size2){
  if(first == NULL || second == NULL || size1 < 1 || size2 < 1){ return ERROR; }
  FILESResult result = FILES_IDENTICAL;
  CompareCharResult charComp;
  char *c1 = &first[0], *c2 = &second[0];
  if (size1 != size2) {
    result = FILES_SIMILAR;
  }

  /**
   * FIRST LOOP
   */
  while(c1 < &first[size1] && c2 < &second[size2] && result == FILES_IDENTICAL) {
    charComp = CompareCharacters(*c1, *c2);
    if (charComp == CHAR_DIFF){
      result = FILES_DIFF;
    } else if (charComp == CHAR_IDENTICAL) {
     c1++;
     c2++;
    } else {
      result = FILES_SIMILAR;
    }
  }


  /**
   * SECOND LOOP
   */
  while(c1 < &first[size1] && c2 < &second[size2] && result == FILES_SIMILAR) {
    charComp = CompareCharacters(*c1, *c2);
    if (charComp == CHAR_DIFF){
      result = FILES_DIFF;
      continue;
    }
    if (charComp == FIRST_SPACE) {
      c1++;
    } else if (charComp == SECOND_SPACE) {
      c2++;
    }
    else {
      c1++;
      c2++;
    }
  }

  /**
   * CHECK THE REMAINING LETTERS OF THE LONGER FILE
   */
  if (result == FILES_SIMILAR) {
    if (CompareCharacters(*c1, *c2) == CHAR_DIFF) {
      result = FILES_DIFF;
    } else {
      while(c1 < &first[size1]) {
        if (IsCharSpace(*c1) == FALSE) {
          result = FILES_DIFF;
          break;
        }
        c1++;
      }
      while(c2 < &second[size2]) {
        if (IsCharSpace(*c2) == FALSE) {
          result = FILES_DIFF;
          break;
        }
        c2++;
      }
    }

  }
  free(first);
  free(second);
  return result;
}

/**
 * Check if an entire file is made from spaces
 * @param file a pointer to the beginning of the text
 * @param size the size of the text
 * @return True if all the characters are spaces and false otherwise
 */
Bool IsFileSpaces(char *file, int size){
  if(file == NULL || size < 0) { return FALSE; }
  if(size == 0){ return TRUE; }
  char *c  = &file[0];
  while (c < &file[size]) {
    if (IsCharSpace(*c) == FALSE) { return FALSE; }
    c++;
  }
  return TRUE;
}


int main(int argc, char *argv[]) {

  if(argc != 3){
    write(2, ERROR_BAD_NUM_ARGS, strlen(ERROR_BAD_NUM_ARGS));
    return ERROR;
  }
  int size1 = GetFileSize(argv[1]);
  int size2 = GetFileSize(argv[2]);
  if(size1 < 0 || size2 < 0) {
    write(2, ERROR_BAD_ARGUMENTS, strlen(ERROR_BAD_ARGUMENTS));
    return ERROR;
  }
  if(size1 == 0 && size2 == 0){
    return FILES_IDENTICAL;
  }

  char *firstFile = CopyFileDynamically(argv[1], size1);
  char *secondFile = CopyFileDynamically(argv[2], size2);
  if(firstFile == NULL || secondFile == NULL) {
    PrintError();
    return ERROR;
  }

  if(size1 == 0 || size2 == 0) {
    if(IsFileSpaces(firstFile, size1) == TRUE && IsFileSpaces(secondFile, size2) == TRUE) {
      return FILES_SIMILAR;
    }
    return FILES_DIFF;
  }

  int result = CompareFiles(firstFile, secondFile, size1, size2);
  return result;
}