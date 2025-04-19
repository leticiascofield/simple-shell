#include <assert.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

/*

1. Term of Commitment

The group members declare that all code developed for this project is their own.
The group members declare that they have not copied material from the Internet
nor obtained code from third parties.

2. Group Members and Allocation of Effort

Davi Porto Araujo <portodaviporto@gmail.com> 50%
Leticia Scofield Lenzoni <leticialenzoni@gmail.com> 50%

3. Solutions

This project consists of implementing a simplified shell in C, inspired by the xv6 Unix shell. 
The provided skeleton already handled command parsing, while the task was to implement the 
execution logic using system calls. The main components implemented were:

- Task 1: `fork1` Function  
  The function `fork1` wraps the `fork()` system call and exits the program with an error message 
  if the fork fails. This ensures that the shell can safely create child processes.

- Task 2: Simple Command Execution  
  The function `handle_simple_cmd` was implemented using `execvp`, allowing execution of standard 
  commands like `ls`, `cat`, etc.

- Task 3: Input/Output Redirection  
  The function `handle_redirection` handles `<` and `>` symbols, using `open` and `dup2` to 
  redirect `stdin` and `stdout` as needed.

- Task 4: Piping  
  The `handle_pipe` function was implemented using `pipe`, `fork`, `dup2`, and `wait`. 
  It sets up inter-process communication between commands connected by `|`.

- Task 5: Command `cd`  
  The main function includes logic to handle the built-in command `cd`, which is executed 
  in the parent process. A more appropriate error message was added when the directory 
  change fails.

4. Bibliographic References

- man pages for `fork`, `execvp`, `pipe`, `dup2`, `open`, `wait`, and `chdir`:  
  https://linux.die.net/man/

- UFMG Operating Systems slides and class material

*/

/****************************************************************
 * Simplified xv6 Shell
 *
 * This code was adapted from the UNIX xv6 code and material from
 * the MIT Operating Systems course (6.828).
 ***************************************************************/

#define MAXARGS 10

/* Every command has a type. After identifying the command's type,
    the code converts a *cmd into the specific command type. */
struct cmd {
    int type; /* ' ' (exec)
                 '|' (pipe)
                 '<' or '>' (redirection) */
};

struct execcmd {
    int type; // ' ' (exec)
    char *argv[MAXARGS]; // Arguments for the command to be executed
};

struct redircmd {
    int type; // < or > (redirection)
    struct cmd *cmd; // The command to execute (e.g., an execcmd)
    char *file; // The input or output file
    int mode; // The mode in which the file should be opened
    int fd; // The file descriptor number to be used
};

struct pipecmd {
    int type; // | (pipe)
    struct cmd *left; // Left side of the pipe
    struct cmd *right; // Right side of the pipe
};

int fork1(void); // Fork but exit if an error occurs
struct cmd *parsecmd(char *); // Process the command line
void handle_simple_cmd(struct execcmd *ecmd); // Handle simple commands
void handle_redirection(struct redircmd *rcmd); // Handle redirection
void handle_pipe(struct pipecmd *pcmd, int *p, int r); // Handle pipes

/* Execute the command cmd. It never returns. */
void runcmd(struct cmd *cmd) {
    int p[2], r;
    struct execcmd *ecmd;
    struct pipecmd *pcmd;
    struct redircmd *rcmd;

    if (cmd == 0)
        exit(0);

    switch (cmd->type) {
        default:
            fprintf(stderr, "Unknown command type\n");
            exit(-1);

        case ' ':
            ecmd = (struct execcmd *) cmd;
            if (ecmd->argv[0] == 0)
                exit(0);
            handle_simple_cmd(ecmd);
            break;

        case '>':
        case '<':
            rcmd = (struct redircmd *) cmd;
            handle_redirection(rcmd);
            runcmd(rcmd->cmd);
            break;

        case '|':
            pcmd = (struct pipecmd *) cmd;
            handle_pipe(pcmd, p, r);
            break;
    }
    exit(0);
}

int fork1(void) {
    /* Task 1: Implement the fork1 function.
    The function is supposed to create a new process using the fork() system call.
    It should print a message if the fork fails, otherwise return the process ID of the child process (or -1 if the fork fails).
    */
    int p_id = fork();

    if (p_id) {
        if (p_id > 0) {
            return p_id;
        }
        fprintf(stderr, "Fork function not implemented\n");
        exit(-1);
    }

    return 0;
    /* END OF TASK 1 */
}

void handle_simple_cmd(struct execcmd *ecmd) {
    /* Task 2: Implement the code below to execute simple commands. */
    execvp(ecmd->argv[0], ecmd->argv);

    fprintf(stderr, "Error executing %s\n", ecmd->argv[0]);
    exit(-1);
    /* END OF TASK 2 */
}

void handle_redirection(struct redircmd *rcmd) {
    /* Task 3: Implement the code below to handle input/output redirection. */

    int file_id = open(rcmd->file, rcmd->mode, 777);

    if (file_id < 0) {
        fprintf(stderr, "could not open file  %s\n", rcmd->file);
        exit(-1);
    }

    if (dup2(file_id, rcmd->fd) < 0) {
        close(file_id);

        fprintf(stderr, "failed to bind files\n");
        exit(-1);
    }
    /* END OF TASK 3 */
}

void handle_pipe(struct pipecmd *pcmd, int *p, int r) {
    /* Task 4: Implement the code below to handle pipes. */
    if(pipe(p) < 0) {
        fprintf(stderr, "failed to stabilish pipe comunication\n");
        exit(-1);
    }

    if(fork1() == 0) {
        close(p[0]);

        dup2(p[1], STDOUT_FILENO);


        runcmd(pcmd->left);
    }

    if(fork1() == 0) {
        close(p[1]);
        dup2(p[0], STDIN_FILENO);

        runcmd(pcmd->right);
    }

    close(p[0]);
    close(p[1]);

    wait(&r);
    wait(&r);
    /* END OF TASK 4 */
}

int getcmd(char *buf, int nbuf) {
    if (isatty(fileno(stdin)))
        fprintf(stdout, "$ ");
    memset(buf, 0, nbuf);
    fgets(buf, nbuf, stdin);
    if (buf[0] == 0) // EOF
        return -1;
    return 0;
}

int main(void) {
    static char buf[100];
    int r;

    // Read and execute commands.
    while (getcmd(buf, sizeof(buf)) >= 0) {
        /* Task 5: Explain the purpose of the if statement below and correct the error message.
        Why is the current error message incorrect? Justify the new message. */
        /* Answer:
         * The code below checks if the "cd " command(changing directory) executed properly, to avoid problems with
         * whitespace and endLine the path is trimmed, removing either of those.
         *
         * The message should be corrected because process does not exist, is far too abstract  because chdir()
         * deals with directory changes, processes can be virtually anything in the program.
         * A more descriptive message of what went wrong could be  "cannot change directory to {required path}",
         * as it reifies the operation that failed, the reasons may involve the directory not existing, permission
         * issues, or even an invalid path.
         *
         */
        if (buf[0] == 'c' && buf[1] == 'd' && buf[2] == ' ') {
            buf[strlen(buf) - 1] = 0;
            if (chdir(buf + 3) < 0)
                fprintf(stderr, "process does not exist\n");
            continue;
        }
        /* END OF TASK 5 */

        if (fork1() == 0)
            runcmd(parsecmd(buf));
        wait(&r);
    }
    exit(0);
}

/****************************************************************
 * Helper functions for creating command structures
 ***************************************************************/

struct cmd *
execcmd(void) {
    struct execcmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = ' ';
    return (struct cmd *) cmd;
}

struct cmd *
redircmd(struct cmd *subcmd, char *file, int type) {
    struct redircmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = type;
    cmd->cmd = subcmd;
    cmd->file = file;
    cmd->mode = (type == '<') ? O_RDONLY : O_WRONLY | O_CREAT | O_TRUNC;
    cmd->fd = (type == '<') ? 0 : 1;
    return (struct cmd *) cmd;
}

struct cmd *
pipecmd(struct cmd *left, struct cmd *right) {
    struct pipecmd *cmd;

    cmd = malloc(sizeof(*cmd));
    memset(cmd, 0, sizeof(*cmd));
    cmd->type = '|';
    cmd->left = left;
    cmd->right = right;
    return (struct cmd *) cmd;
}

/****************************************************************
 * Command Line Processing
 ***************************************************************/

char whitespace[] = " \t\r\n\v";
char symbols[] = "<|>";

int gettoken(char **ps, char *es, char **q, char **eq) {
    char *s;
    int ret;

    s = *ps;
    while (s < es && strchr(whitespace, *s))
        s++;
    if (q)
        *q = s;
    ret = *s;
    switch (*s) {
        case 0:
            break;
        case '|':
        case '<':
            s++;
            break;
        case '>':
            s++;
            break;
        default:
            ret = 'a';
            while (s < es && !strchr(whitespace, *s) && !strchr(symbols, *s))
                s++;
            break;
    }
    if (eq)
        *eq = s;

    while (s < es && strchr(whitespace, *s))
        s++;
    *ps = s;
    return ret;
}

int peek(char **ps, char *es, char *toks) {
    char *s = *ps;
    while (s < es && strchr(whitespace, *s)) s++;
    *ps = s;
    return *s && strchr(toks, *s);
}

struct cmd *parseline(char **, char *);

struct cmd *parsepipe(char **, char *);

struct cmd *parseexec(char **, char *);

/* Copy characters from the input buffer, starting from s to es.
 * Place a null terminator at the end to create a valid string. */
char *mkcopy(char *s, char *es) {
    int n = es - s;
    char *c = malloc(n + 1);
    assert(c);
    strncpy(c, s, n);
    c[n] = 0;
    return c;
}

struct cmd *
parsecmd(char *s) {
    char *es;
    struct cmd *cmd;

    es = s + strlen(s);
    cmd = parseline(&s, es);
    peek(&s, es, "");
    if (s != es) {
        fprintf(stderr, "leftovers: %s\n", s);
        exit(-1);
    }
    return cmd;
}

struct cmd *
parseline(char **ps, char *es) {
    struct cmd *cmd;
    cmd = parsepipe(ps, es);
    return cmd;
}

struct cmd *
parsepipe(char **ps, char *es) {
    struct cmd *cmd;

    cmd = parseexec(ps, es);
    if (peek(ps, es, "|")) {
        gettoken(ps, es, 0, 0);
        cmd = pipecmd(cmd, parsepipe(ps, es));
    }
    return cmd;
}

struct cmd *
parseredirs(struct cmd *cmd, char **ps, char *es) {
    int tok;
    char *q, *eq;

    while (peek(ps, es, "<>")) {
        tok = gettoken(ps, es, 0, 0);
        if (gettoken(ps, es, &q, &eq) != 'a') {
            fprintf(stderr, "missing file for redirection\n");
            exit(-1);
        }
        switch (tok) {
            case '<':
                cmd = redircmd(cmd, mkcopy(q, eq), '<');
                break;
            case '>':
                cmd = redircmd(cmd, mkcopy(q, eq), '>');
                break;
        }
    }
    return cmd;
}

struct cmd *
parseexec(char **ps, char *es) {
    char *q, *eq;
    int tok, argc;
    struct execcmd *cmd;
    struct cmd *ret;

    ret = execcmd();
    cmd = (struct execcmd *) ret;

    argc = 0;
    ret = parseredirs(ret, ps, es);
    while (!peek(ps, es, "|")) {
        if ((tok = gettoken(ps, es, &q, &eq)) == 0)
            break;
        if (tok != 'a') {
            fprintf(stderr, "syntax error\n");
            exit(-1);
        }
        cmd->argv[argc] = mkcopy(q, eq);
        argc++;
        if (argc >= MAXARGS) {
            fprintf(stderr, "too many args\n");
            exit(-1);
        }
        ret = parseredirs(ret, ps, es);
    }
    cmd->argv[argc] = 0;
    return ret;
}