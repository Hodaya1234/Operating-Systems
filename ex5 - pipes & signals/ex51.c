//
// Hodaya Koslowsky
// 313377673
//

#include <unistd.h>
#include <termios.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>

#define END 'q'
#define LEFT 'a'
#define RIGHT 'd'
#define TURN 'w'
#define DOWN 's'
#define ERROR "Error in system call.\n"
#define DRAW_FILENAME "./draw.out"
#define ERROR_EXIT_STATUS 1

/* Typedef */

typedef enum {TRUE, FALSE} Bool;

/* Declarations */

char GetChar();

Bool IsLegalChar(char c);

void UpdateNewChar(int sonPid, int writeFd, char c);

char ListenToKeyboard(int sonPid, int writeFd);

/* Definitions */

int main() {
  int pid;
  int fd[2];
  pipe(fd);

  if ((pid = fork()) < 0){
    write(2, ERROR, strlen(ERROR));
    exit(ERROR_EXIT_STATUS);
  }
  else if (pid == 0){
    dup2(fd[0], 0);
    close(fd[1]);
    execlp (DRAW_FILENAME, DRAW_FILENAME, NULL);
  } else {
    close(fd[0]);
    ListenToKeyboard(pid, fd[1]);
  }
  return 0;
}

char GetChar() {
  char buf = 0;
  struct termios old = {0};
  if (tcgetattr(0, &old) < 0)
    perror("tcsetattr()");
  old.c_lflag &= ~ICANON;
  old.c_lflag &= ~ECHO;
  old.c_cc[VMIN] = 1;
  old.c_cc[VTIME] = 0;
  if (tcsetattr(0, TCSANOW, &old) < 0)
    perror("tcsetattr ICANON");
  if (read(0, &buf, 1) < 0)
    perror ("read()");
  old.c_lflag |= ICANON;
  old.c_lflag |= ECHO;
  if (tcsetattr(0, TCSADRAIN, &old) < 0)
    perror ("tcsetattr ~ICANON");
  return (buf);
}

void UpdateNewChar(int sonPid, int writeFd, char c){
  kill(sonPid, SIGUSR2);
  char buffer[] = {c};
  write(writeFd, buffer, sizeof(buffer));
}

char ListenToKeyboard(int sonPid, int writeFd){
  char pushed = 0;
  while((pushed = GetChar()) != END){
    if (IsLegalChar(pushed) == FALSE){
      continue;
    }
    UpdateNewChar(sonPid, writeFd, pushed);
  }
  UpdateNewChar(sonPid, writeFd, END);
}

Bool IsLegalChar(char c){
  if (c==END || c==LEFT || c==RIGHT || c==TURN || c==DOWN) {
    return TRUE;
  }
  return FALSE;
}
