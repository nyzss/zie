#include <fileapi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ZIE_BUFSIZE 1024
#define ZIE_TOK_BUFSIZE 64
#define ZIE_TOK_DELIM " \t\r\n\a"

char *zie_read_line() {

  int buffsize = ZIE_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * buffsize);

  int c;

  if (!buffer) {
    fprintf(stderr, "zie: couldn't allocate");
    exit(EXIT_FAILURE);
  }

  while (1) {
    c = getchar();

    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;
  }

  if (position >= buffsize) {
    buffsize += 2;
    buffer = (char *)realloc(buffer, sizeof(char) * buffsize);
  }
  if (!buffer) {
    fprintf(stderr, "zie: re-allocation error\n");
  }
};

char **zie_split_line(char *line) {
  int buffsize = ZIE_TOK_BUFSIZE, position = 0;

  char **tokens = (char **)malloc(buffsize * sizeof(char *));
  char *token;

  if (!tokens) {
    fprintf(stderr, "zie: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(line, " ");
  while (token != NULL) {
    tokens[position] = token;
    position++;

    if (position >= buffsize) {
      buffsize += ZIE_TOK_BUFSIZE;
      tokens = (char **)realloc(tokens, buffsize * sizeof(char *));
      if (!tokens) {
        fprintf(stderr, "zie: re-allocation error\n");
      }
    }
    token = strtok(NULL, ZIE_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

// in-progress 'ls' implementation.
void list_folders() {
  WIN32_FIND_DATA ffd;
  HANDLE file = FindFirstFile("*", &ffd);
  printf("filename: %s\n", ffd.cFileName);

  while (FindNextFile(file, &ffd)) {
    printf("filename: %s\n", ffd.cFileName);
  }
}

// the easy way lol
int zie_execute_args(char **args) {

  if (strcmp(args[0], "clear") == 0) {
    system("cls");
    return 1;
  }
  if (strcmp(args[0], "ls") == 0) {
    printf("files: \n");
    list_folders();
    return 1;
  }
  if (strcmp(args[0], "exit") == 0) {
    exit(EXIT_SUCCESS);
    return 1;
  }

  // shellexecute approach, which works without issues lol
  if (ShellExecute(NULL, "open", args[0], NULL, NULL, SW_HIDE)) {
    return 1;
  }
  return 0;
}

void zie_loop(void) {
  char *line;
  char **args;
  int status;

  do {
    printf("> ");
    line = zie_read_line();
    args = zie_split_line(line);
    status = zie_execute_args(args);

    free(line);
    free(args);
  } while (status);
}

int main(int argc, char **argv) {

  zie_loop();
  return EXIT_SUCCESS;
}
