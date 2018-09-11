/**
 *
 * Hodaya Koslowsky
 * 313377673
 *
 * COMMENTS:
 * 1. The configuration file must end with '\n'
 * 2. TIMEOUT WAIT is currently set to 5 sec but can easily be lowered to lower the testing time
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <dirent.h>
#include <stdlib.h>
#include <stdint.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

#define ARG_SIZE 220
#define NUM_ARGS 3
#define MAX_NAME 30
#define MAX_GRADE_REASON 30
#define BAD_ARGS "Bad arguments to main"
#define ERROR "Error in system call"
#define COMPILED_FILE "compiled.out"
#define COMPILE_FILE_TO_RUN "./compiled.out"
#define OUTPUT_FILE "output.txt"
#define RESULT_FILE "results.csv"
#define COMPARE_PROG "./comp.out"
#define TIMEOUT_WAIT 5
#define FAIL (-1)

/**
 *
 *
 * DEFINE OF THE GRADES AND REASONS
 *
 *
 */
#define NO_C_FILE "0,NO_C_FILE"
#define COMPILATION_ERROR "0,COMPILATION_ERROR"
#define TIMEOUT "0,TIMEOUT"
#define BAD_OUTPUT "60,BAD_OUTPUT"
#define SIMILAR_OUTPUT "80,SIMILAR_OUTPUT"
#define GREAT_JOB "100,GREAT_JOB"

/**
 *
 *
 * STRUCTS AND ENUMS
 *
 *
 */
typedef struct {
  char directory[ARG_SIZE];
  char inputFile[ARG_SIZE];
  char correctOutput[ARG_SIZE];
  int resultFD;
}Config;

typedef struct {
  char name[MAX_NAME];
  char gradeAndReason[MAX_GRADE_REASON];
  char toString[MAX_NAME+MAX_GRADE_REASON];
}StudentResult;

typedef enum {TRUE, FALSE} Bool;

/**
 *
 *
 * DECLARATIONS OF FUNCTIONS AND COMMENTS
 *
 *
 */

/**
 * Find the file extension.
 * @param filename file containing the extension
 * @return char* of extension
 */
const char *GetFileExt(const char *filename);

/**
 * Check if an entry is a C file.
 * @param pDirent struct pointing to an entry in a directory
 * @return True if it is c, and false otherwise
 */
Bool IsFileC(struct dirent *pDirent);

/**
 * Print error message to stderr.
 */
void PrintError();

/**
 * Combine to strings in the following format:
 * <first_string>\<second string>
 * uses to create a subdirectory path -
 * by combining the directory with the entry name.
 * @param result the array of chars to put the result into
 * @param first string - directory
 * @param second string - subdirectory or file.
 */
void CombineTwoString(char result[ARG_SIZE], const char* first, const char* second);

/**
 * Take the name of student and his result
 * from the struct StudentResult,
 * and put intoo it the string formatting of the information.
 * @param result The struct to take the information from and put it back as a string.
 */
void TurnResultToString(StudentResult* result);

/**
 * Write stident result to the RESULT FILE.
 * @param studentName char*, the name of the student
 * @param gradeAndReason char*, the grade and the reason for it.
 * @param config the configuration information containing the result file descriptor
 */
void WriteResult(char* studentName, char* gradeAndReason, const Config* const config);

/**
 * Compare the output of the student with the correct output.
 * @param studentName the path of the student file.
 * @param config configuration conataining the correct output path.
 */
void CompareOutput(char* studentName, const Config* const config);

/**
 * Run the student executable file.
 * redirect the input to the input file from configuration, and the output to a txt file.
 * @param studentName tha of the file of the
 * @param config containing the path to the input file
 */
void RunFile(char *studentName, const Config *const config);

/**
 * Compile the student c file.
 * @param filePath the path of the file of the student.
 * @param studentName the name of the student.
 * @param config need to pass the configuration to the running function.
 */
void CompileFile(char *filePath, char *studentName, const Config *const config);

/**
 * Handle the student file: search for a C file and move forward to compile and run.
 * @param dirName the name of the current checked directory.
 * @param studentName the name of the student.
 * @param config Configuration, need to pass forward.
 * @param exist A boolean - true if a c file was found, false otherwise.
 */
void HandleStudentDir(char *dirName, char *studentName, const Config* const config, Bool* exist);

/**
 * Check the input directory and send each subdirectory to a function for the student.
 * @param dirName the path of the directory.
 * @param config the configuration struct, need to pass forward.
 */
void CheckInputDir(char *dirName, const Config* const config);

/**
 * Get the parameters from the configuration file into a struct.
 * @param buffer The content of the configuration file.
 * @param config The struct to put the information in to.
 */
void getParameters(char buffer[ARG_SIZE * NUM_ARGS], Config *config);

/**
 * remove the used files.
 */
void CleanUp();

/**
 *
 *
 * IMPLEMENTATION OF FUNCTIONS
 *
 *
 */

const char *GetFileExt(const char *filename) {
  const char *dot = strrchr(filename, '.');
  if(!dot || dot == filename) return "";
  return dot + 1;
}

Bool IsFileC(struct dirent *pDirent){
  if((pDirent->d_type == DT_REG || pDirent->d_type == DT_UNKNOWN)
          && strcmp(GetFileExt(pDirent->d_name), "c") == 0){
    return TRUE;
  }
}

void PrintError(){
  write(2, ERROR, strlen(ERROR));
}

void CombineTwoString(char result[ARG_SIZE], const char* first, const char* second){
  memset(result, 0, ARG_SIZE);
  snprintf(result, ARG_SIZE, "%s/%s", first, second);
}

void TurnResultToString(StudentResult* result){
  strcpy(result->toString, result->name);
  strcat(result->toString, ",");
  strcat(result->toString, result->gradeAndReason);
  strcat(result->toString, "\0");
}

void WriteResult(char* studentName, char* gradeAndReason, const Config* const config){
  StudentResult res;
  strcpy(res.name, studentName);
  strcpy(res.gradeAndReason, gradeAndReason);
  TurnResultToString(&res);
  write(config->resultFD, res.toString, strlen(res.toString));
  write(config->resultFD, "\n", strlen("\n"));
}

void CompareOutput(char* studentName, const Config* const config){
  char correctOutput[ARG_SIZE];
  strcpy(correctOutput, config->correctOutput);
  int pid = fork();
  if(pid < 0) exit(FAIL);
  if(pid == 0){
    char *args[] = {COMPARE_PROG, OUTPUT_FILE, correctOutput, NULL};
    if (execvp(args[0], args) < 0) {
      PrintError();
      exit(FAIL);
    }
  } else {
    int status;
    if (waitpid(pid, &status, 0) < 0) {
      PrintError();
      return;
    }
    if(WIFEXITED(status)){
      switch(WEXITSTATUS(status)){
        case 1:
          WriteResult(studentName, BAD_OUTPUT, config);
          break;
        case 2:
          WriteResult(studentName, SIMILAR_OUTPUT, config);
          break;
        case 3:
          WriteResult(studentName, GREAT_JOB, config);
          break;
        default:
          return;
      }
    }
  }
}

void RunFile(char *studentName, const Config *const config){
  int pid2 = fork();
  if(pid2 < 0){ exit(1); }
  if(pid2 == 0){
    int outFd;
    int inFd;
    if ((inFd = open(config->inputFile, O_RDONLY)) < 0) exit(FAIL);
    if ((outFd = open(OUTPUT_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) exit(FAIL);
    if (dup2(inFd, 0) < 0) {
      PrintError();
      exit(FAIL);
    }
    if (dup2(outFd, 1) < 0) {
      PrintError();
      exit(FAIL);
    }
    if (execlp(COMPILE_FILE_TO_RUN, COMPILE_FILE_TO_RUN, NULL) < 0){
      PrintError();
      exit(FAIL);
    }
  } else {
    int status;
    sleep(TIMEOUT_WAIT);
    pid_t return_pid = waitpid(pid2, &status, WNOHANG);
    if (return_pid < 0) {
      PrintError();
      return;
    } else if (return_pid == 0) {
      WriteResult(studentName, TIMEOUT, config);
      kill(pid2, SIGKILL);
      return;
    } else if (return_pid == pid2) {
      if(WIFEXITED(status)) {
        CompareOutput(studentName, config);
      }
    }
  }
}

void CompileFile(char *filePath, char *studentName, const Config *const config){
  int pid1 = fork();
  if(pid1 < 0){ exit(FAIL); }
  if(pid1 == 0) {
    char *args[] = {"gcc", "-o", COMPILED_FILE, filePath, NULL};
    execvp(args[0], args);
    PrintError();
    exit(FAIL);
  } else {
    int returnStatus;
    if (waitpid(pid1, &returnStatus, WCONTINUED) < 0) {
      return;
    }
    if(returnStatus == 0){
      RunFile(studentName, config);
    } else {
      WriteResult(studentName, COMPILATION_ERROR, config);
      return;
    }
  }
}

void HandleStudentDir(char *dirName, char *studentName, const Config* const config, Bool* exist){
  DIR *dir;
  struct dirent *entry;
  if (!(dir = opendir(dirName))) return;
  while ((entry = readdir(dir)) != NULL) {
    if (IsFileC(entry) == TRUE) {
      *exist = TRUE;
      char filePath[ARG_SIZE];
      CombineTwoString(filePath, dirName, entry->d_name);
      CompileFile(filePath, studentName, config);
      closedir(dir);
      return;
    } else if(entry->d_type == DT_DIR){
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      char path[ARG_SIZE];
      CombineTwoString(path, dirName, entry->d_name);
      HandleStudentDir(path, studentName, config, exist);
    }
  }
  closedir(dir);
}

void CheckInputDir(char *dirName, const Config* const config){
  DIR *dir;
  struct dirent *entry;
  if (!(dir = opendir(dirName)))
    return;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type == DT_DIR) {
      if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
        continue;
      char dirPath[ARG_SIZE];
      memset(dirPath, 0, sizeof(dirPath));
      CombineTwoString(dirPath, dirName, entry->d_name);
      Bool exist = FALSE;
      HandleStudentDir(dirPath, entry->d_name, config, &exist);
      if (exist  == FALSE){
        WriteResult(entry->d_name, NO_C_FILE, config);
      }
    }
  }
  closedir(dir);
}

void getParameters(char buffer[ARG_SIZE * NUM_ARGS], Config *config){
  char *line;
  char lines[NUM_ARGS][ARG_SIZE];
  int i = 0;
  line = strtok(buffer, "\n");
  while(line != NULL){
    strcpy(lines[i++], line);
    line = strtok(NULL, "\n");
  }
  strcpy(config->directory, lines[0]);
  strcpy(config->inputFile, lines[1]);
  strcpy(config->correctOutput, lines[2]);

  if ((config->resultFD = open(RESULT_FILE, O_CREAT | O_WRONLY | O_TRUNC, 0644)) < 0) exit(1);
}

void CleanUp(){
  if (unlink(OUTPUT_FILE) < 0) {
    PrintError();
  }
  if (unlink(COMPILED_FILE) < 0) {
    PrintError();
  }
}

int main(int argc, char** argv) {
  if(argc != 2){
    write(2, BAD_ARGS, strlen(BAD_ARGS));
    return 1;
  }
  char *configName = argv[1];
  int configFile = open(configName, O_RDONLY);
  char buffer[ARG_SIZE * NUM_ARGS];
  ssize_t bytesRead = read(configFile, buffer, sizeof(buffer));
  if(bytesRead < 0){
    PrintError();
    return 1;
  }

  Config* config = calloc(sizeof(Config), sizeof(char));
  getParameters(buffer, config);

  CheckInputDir(config->directory, config);
  CleanUp();
  close(config->resultFD);
  free(config);
  return 0;
}