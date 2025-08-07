int main(int argc, char **argv)
{
    //Load config files, if any. 

    //running command loop
    lsh_loop(); 

    // perform any shutdown 
    return EXIT_SUCCESS; 
}

void lsh_loop(void){

    char *line;
    char **args;
    int status;

    do {
        print("> ");
        //read function call
        line = lsh_read_line();
        //parse command function call
        args = lsh_split_line(line);
        //run command function call
        status = lsh_execute(args);

        free(line);
        free(args);
    
    } while (status); 
}


//reading a line
#define LSH_RL_BUFSIZE 1024
char *lsh_read_line(void)
{
  int bufsize = LSH_RL_BUFSIZE;
  int position = 0;
  char *buffer = malloc(sizeof(char) * bufsize);
  int c;

  if (!buffer) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  while (1) {
    // Read a character
    c = getchar();

    // If we hit EOF, replace it with a null character and return.
    if (c == EOF || c == '\n') {
      buffer[position] = '\0';
      return buffer;
    } else {
      buffer[position] = c;
    }
    position++;

    // If we have exceeded the buffer, reallocate.
    if (position >= bufsize) {
      bufsize += LSH_RL_BUFSIZE;
      buffer = realloc(buffer, bufsize);
      if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
  }
}

#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{

  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  char *token;

  if (!tokens) {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }
//strtok returns a pointer to the first token
  token = strtok(line, LSH_TOK_DELIM);
  while (token != NULL) {
    //store each pointer into array (buffer) of character pointers
    tokens[position] = token;
    position++;

    //reallocate the array of pointers if necessary
    if (position >= bufsize) {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }

    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int lsh_launch(char **args)
{
  pid_t pid, wpid; //process id number
  int status;
//forks process, saves return value
  pid = fork(); //makes duplicate of the process and starts both running
  if (pid == 0) { 
    // Child process
    //if returns -1 we have an error, perror prints error message
    if (execvp(args[0], args) == -1) { //execvp expects program name and vector(array) 
      perror("lsh");
    }
    exit(EXIT_FAILURE);
  } else if (pid < 0) { //checks if fork() had an error 
    // Error forking
    perror("lsh");
  } else { //fork() executed successfully 
    // Parent process
    do {
      wpid = waitpid(pid, &status, WUNTRACED); //wait until either the processes are exited or killed
    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
  }

  return 1;
}

/*
  Function Declarations for builtin shell commands:
  foward delceratiions, declares but doesnt define 
 */
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

/*
  List of builtin commands, followed by their corresponding functions.
  so builtin commands can be used by modifying these arrays
 */
char *builtin_str[] = {
  "cd",
  "help",
  "exit"
};

int (*builtin_func[]) (char **) = {
  &lsh_cd,
  &lsh_help,
  &lsh_exit
};

int lsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

/*
  Builtin function implementations.
*/
int lsh_cd(char **args)
{
  if (args[1] == NULL) {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else {
    if (chdir(args[1]) != 0) {
      perror("lsh");
    }
  }
  return 1;
}

int lsh_help(char **args)
{
  int i;
  printf("Stephen Brennan's LSH\n");
  printf("Type program names and arguments, and hit enter.\n");
  printf("The following are built in:\n");

  for (i = 0; i < lsh_num_builtins(); i++) {
    printf("  %s\n", builtin_str[i]);
  }

  printf("Use the man command for information on other programs.\n");
  return 1;
}

int lsh_exit(char **args)
{
  return 0;
}

//execute function
int lsh_execute(char **args)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }

  for (i = 0; i < lsh_num_builtins(); i++) {
    if (strcmp(args[0], builtin_str[i]) == 0) { //checks if command equals each builtin
      return (*builtin_func[i])(args);
    }
  }

  return lsh_launch(args);
}