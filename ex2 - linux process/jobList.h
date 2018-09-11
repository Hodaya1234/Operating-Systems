/**
 *
 * Hodaya Koslowsky
 * Comments are in the header files
 *
 */

#ifndef OS_EX2_JOBLIST_H
#define OS_EX2_JOBLIST_H


#include <unistd.h>


typedef enum {
  bg, fg
} JobState;

typedef struct {
  pid_t pid;
  int jobId;
  char **args; // including the name
  int numArgs;
  JobState state;
} Job;

typedef struct Node {
  Job *job;
  struct Node *next;
} Node;

typedef struct {
  struct Node *first;
  struct Node *last;
  int numberOfNodes;
} Head;

/**
 * Create the head of the list, pointing to the the:
 * 1. first node of the list
 * 2. last node of the list
 * 3. int number of nodes
 * @return A pointer to the head.
 */
Head *CreateHead();

/**
 * Create a new Job
 * @param pid the process ID
 * @param args The arguments of the command, including the command itself.
 * @param numArgs Number of arguments.
 * @return A pointer to the job.
 */
Job *CreateJob(pid_t pid, char **args, int numArgs);

/**
 * Add a new job to the job list.
 * @param head The pointer to the beginning of the job list.
 * @param newJob A pointer to the new job to add.
 */

void AddJob(Head *head, Job *newJob);

/**
 * Free the allocated memory of a job.
 * @param job The job to free.
 */
void FreeJob(Job *job);

/**
 * Free the allocated memory of Node.
 * @param node The node to free.
 */
void FreeNode(Node *node);

/**
 * Free all memory allocated for the job list.
 * @param head The pointer to the beginning of the job list.
 */

void FreeMemory(Head *head);

/**
 * Free the memory allocated for an array of arguments.
 * @param args A pointer to the arguments to free.
 * @param numArgs Number of elements to free.
 */

void FreeArgsWithNum(char **args, int numArgs);

/**
 * Remove a job from the list, using its PID in order to find the right job to remove from the list.
 * @param head The pointer to the beginning of the job list.
 * @param pid The process ID of the job to remove.
 */

void RemoveJobId(Head *head, pid_t pid);

/**
 * Remove a job from the list using a pointer to the job itself.
 * @param head The pointer to the beginning of the job list.
 * @param job A pointer to the job to delete.
 */

void RemoveJob(Head *head, Job *job);

/**
 * Remove a Node from the list.
 * @param head The pointer to the beginning of the job list.
 * @param node A pointer to the node to delete.
 */
void RemoveNode(Head *head, Node *node);

#endif //OS_EX2_JOBLIST_H
