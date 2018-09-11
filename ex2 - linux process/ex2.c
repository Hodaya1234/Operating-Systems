/**
 *
 * Hodaya Koslowsky
 * 313377673
 * Comments are in the header files
 *
 */


#include <wait.h>
#include <stdlib.h>
#include "ex2.h"
#include "jobList.h"

char **getUserInput(int *numWords) {
  // read user input into the buffer
  printf("%s", PROMPT);
  char buffer[MAX_ARG_LINE_LENGTH];
  fgets(buffer, MAX_ARG_LINE_LENGTH, stdin);
  char **result = NULL;
  char *bufferWord = strtok(buffer, DELIMITER);
  char *newWord = NULL;
  int numberOfSpaces = 0;
  while (bufferWord) {
    result = realloc(result, sizeof(char *) * ++numberOfSpaces);
    newWord = (char *) malloc(strlen(bufferWord) + 1);
    strcpy(newWord, bufferWord);
    result[numberOfSpaces - 1] = newWord;
    bufferWord = strtok(NULL, DELIMITER);
  }
  // delete the \n of the last input word
  if (newWord != NULL && newWord[strlen(newWord) - 1] == '\n') {
    newWord[strlen(newWord) - 1] = '\0';
  }

  result = realloc(result, sizeof(char *) * (numberOfSpaces + 1));
  result[numberOfSpaces] = NULL;
  *numWords = numberOfSpaces;
  return result;
}

void Execute(Head *head, char **args, int numArgs) {
  // create a new process to execute the args
  pid_t pid = fork();
  if (pid < 0) {
    fprintf(stderr, "%s", NEW_PROCESS_ERROR);
  } else if (pid == 0) {
    int i = 0;
    while (args[i] != NULL) {
      if (strcmp(args[i], "&") == 0) {
        args[i] = NULL;
        break;
      }
      i++;
    }
    if (execvp(args[0], args) == -1) {
      fprintf(stderr, SYSTEM_CALL_ERROR);
    }
  } else {
    Job *j = CreateJob(pid, args, numArgs);
    printf("%d\n", pid);
    j->pid = pid;
    AddJob(head, j);
    if (j->state == fg) {
      int status;
      waitpid(pid, &status, 0);
      RemoveJob(head, j);
    }
  }
}

void PrintJobs(Head *head) {
  if (head == NULL || head->first == NULL) {
    return;
  }
  Node *currentNode = head->first;
  do {
    printf("%d ", currentNode->job->pid);
    for (int i = 0; i < currentNode->job->numArgs; i++) {
      if (currentNode->job->args[i] == NULL) {
        continue;
      }
      printf("%s ", currentNode->job->args[i]);
    }
    printf("\n");
    currentNode = currentNode->next;
  } while (currentNode != NULL);
}

void CleanJobs(Head *head) {
  if (head == NULL || head->first == NULL) { return; }
  int notRunningPid[MAX_JOBS];
  int i = 0, status, j;
  Node *currentNode;
  for (currentNode = head->first; currentNode != NULL; currentNode = currentNode->next) {
    if (waitpid(currentNode->job->pid, &status, WNOHANG != 0)) {
      notRunningPid[i++] = currentNode->job->pid;
    }
  }
  for (j = 0; j < i; j++) {
    RemoveJobId(head, notRunningPid[j]);
  }
}

void RunPrompt() {
  int running = 1;
  Head *head = CreateHead();
  char **args = NULL;
  int numArgs = 0;
  while (running) {
    args = getUserInput(&numArgs);
    if (strcmp(args[0], "exit") == 0) {
      running = 0;
      KillAllProcesses(head);
      FreeMemory(head);
    } else if (strcmp(args[0], "jobs") == 0) {
      CleanJobs(head);
      PrintJobs(head);
    } else if (strcmp(args[0], "cd") == 0) {
      if (args[1] == NULL) { return; }
      char *path = args[1];
      if(strcmp(path, "~") == 0){
        path = getenv("HOME");
      }
      if(chdir(path)!=0){
        fprintf(stderr, SYSTEM_CALL_ERROR);
      }

    } else if (strcmp(args[0], "help") == 0) {
      printf("%s", HELP_MESSAGE);
    } else {
      Execute(head, args, numArgs);
    }
    FreeArgsWithNum(args, numArgs);
  }
}

void KillAllProcesses(Head *head) {
  Node *currentNode;
  for (currentNode = head->first; currentNode != NULL; currentNode = currentNode->next) {
    kill(currentNode->job->pid, SIGKILL);
  }
}

/**
 * Main function, calls run prompt loop.
 * @return 0 if succeeded, or an error code.
 */
int main() {
  RunPrompt();
  return 0;
}
