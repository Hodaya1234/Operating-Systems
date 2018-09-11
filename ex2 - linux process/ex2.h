/**
 *
 * Hodaya Koslowsky
 * Comments are in the header files
 *
 */

#include <stdio.h>
#include <memory.h>
#include <malloc.h>
#include <unistd.h>
#include "jobList.h"


#ifndef OS_EX2_EX2_H
#define OS_EX2_EX2_H

#define DELIMITER " \""
#define MAX_ARG_LINE_LENGTH 128
#define MAX_JOBS 128
#define PROMPT "prompt>"
#define NEW_PROCESS_ERROR "Error creating a new process."
#define SYSTEM_CALL_ERROR "Error in system call"
#define HELP_MESSAGE "Type a linux command and then enter, or type exit to leave.\n"

/**
 * Read user input to a buffer and return char* array of args
 * @param numWords a pointer to the int number of words. Tells the calling function how many args there are.
 * @return array of args
 */
char **getUserInput(int *numWords);

/**
 * Execute a command with its args.
 * @param head the pointer to the beginning of the job list
 * @param args the arguments of the command, including the command name itself.
 * @param numArgs number of arguments
 */
void Execute(Head *head, char *args[MAX_ARG_LINE_LENGTH], int numArgs);

/**
 * Print the job list.
 * @param head the pointer to the beginning of the job list.
 */

void PrintJobs(Head *head);

/**
 * Delete the background jobs that finished
 * @param head the pointer to the beginning of the job list.
 */
void CleanJobs(Head *head);

/**
 * The loops that gets commands and sends them to execute.
 */
void RunPrompt();

/**
 * Kill the remaining jobs when need to exit the prompt.
 * @param head the pointer to the beginning of the job list.
 */

void KillAllProcesses(Head *head);

#endif //OS_EX2_EX2_H
