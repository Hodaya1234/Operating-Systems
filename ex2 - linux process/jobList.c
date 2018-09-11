/**
 *
 * Hodaya Koslowsky
 * Comments are in the header files
 *
 */

#include "jobList.h"
#include <stdio.h>
#include <memory.h>
#include <malloc.h>

Head *CreateHead() {
  Head *head = (Head *) malloc(sizeof(Head));
  if (head == NULL) { return NULL; }
  head->first = NULL;
  head->last = NULL;
  head->numberOfNodes = 0;
  return head;
}


Job *CreateJob(pid_t pid, char **args, int numArgs) {
  static int jobId = 1;

  Job *job = (Job *) malloc(sizeof(Job));
  job->numArgs = numArgs;
  job->pid = pid;
  job->jobId = jobId++;

  // copy the args to the new job
  if (strcmp(args[numArgs - 1], "&") == 0) {
    job->state = bg;
    //free(args[numArgs - 1]);
    job->numArgs--;
  } else {
    job->state = fg;
  }

  char **jobArgs = (char **) malloc(job->numArgs * sizeof(char *));
  int i;
  for(i = 0; i < job->numArgs; i++){
    jobArgs[i] = (char *) malloc(strlen(args[i]) + 1);
    strcpy(jobArgs[i], args[i]);
  }

  job->args = jobArgs;

  return job;
}


void AddJob(Head *head, Job *newJob) {
  if (newJob == NULL || head == NULL) { return; }
  if (head->last == NULL || head->first == NULL) {
    Node *newNode = (Node *) malloc(sizeof(Node));
    if (newNode == NULL) { return; }
    newNode->job = newJob;
    newNode->next = NULL;
    head->first = newNode;
    head->last = newNode;
    head->numberOfNodes = 1;
    return;
  }
  Node *currentLast = head->last;
  Node *newNode = (Node *) malloc(sizeof(Node));
  if (newNode == NULL) { return; }
  newNode->job = newJob;
  newNode->next = NULL;
  currentLast->next = newNode;
  head->last = newNode;
  head->numberOfNodes++;
}


void FreeJob(Job *job) {
  if (job == NULL) { return; }
  if (job->args == NULL) {
    free(job);
    return;
  }
  FreeArgsWithNum(job->args, job->numArgs);
  free(job);
}


void FreeNode(Node *node) {
  if (node == NULL) { return; }
  FreeJob(node->job);
  free(node);
}


void RemoveJobId(Head *head, pid_t pid) {
  if (head == NULL || head->numberOfNodes < 1 || head->first == NULL) { return; }
  if (head->numberOfNodes == 1) {
    FreeNode(head->first);
    head->first = NULL;
    head->last = NULL;
    head->numberOfNodes = 0;
    return;
  }
  Node *currentNode = head->first;
  Node *previousNode = NULL;
  while (currentNode != NULL) {
    if (currentNode->job->pid == pid) { break; }
    previousNode = currentNode;
    currentNode = currentNode->next;
  }
  if (currentNode == NULL) { return; }  // there is no job with that id

  if (previousNode == NULL) {                 //the job to delete is the first one
    head->first = currentNode->next;
  } else if (head->last == currentNode) {    // the job to delete is the last one
    head->last = previousNode;
    previousNode->next = NULL;
  } else {                                    //the job is in the middle
    previousNode->next = currentNode->next;
  }
  FreeNode(currentNode);
  head->numberOfNodes--;

}

void RemoveJob(Head *head, Job *job) {
  if (head == NULL || job == NULL || head->numberOfNodes < 1) { return; }
  RemoveJobId(head, job->pid);
}


void RemoveNode(Head *head, Node *node) {
  if (head == NULL || node == NULL) { return; }
  RemoveJobId(head, node->job->pid);
}

void FreeArgsWithNum(char **args, int numArgs){
  int i = 0;
  if (args == NULL) { return; }
  for(i = 0; i < numArgs; i++){
    if(args[i] != NULL){
      free(args[i]);
    }
  }
  free(args);
}

void FreeMemory(Head *head) {
  Node *current = head->first;
  Node *previous = NULL;
  while (current != NULL) {
    previous = current;
    current = current->next;
    RemoveNode(head, previous);
  }
  free(head);
}
