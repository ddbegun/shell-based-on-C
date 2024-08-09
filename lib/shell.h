#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_MAXSIZE 1024
#define DSH_TOK_BUFFER 64
#define DSH_TOK_DELIM " \n\a\t\r"

int dsh_cd(char **args);
int dsh_help();
int dsh_exit();
int author();

char *builtin_str[] = {
    "cd",
    "help",
    "exit",
    "author",
};

int (*builtin_func[]) (char **) = {
    &dsh_cd,
    &dsh_help,
    &dsh_exit,
    &author,
};

int author() {
    printf("this project reference to github project based project\n");
    printf("more informantion please get github\n");
    printf("\n");
    printf("this is my github ddbegun\n");
}

int dsh_num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int dsh_help() {
    printf("this is dsh help command\n");
    printf("type command and arguments then hit it to use\n");

    for (int i = 0; i < dsh_num_builtins(); i++) {
        printf("%s\n",builtin_str[i]);
    }

    printf("more information please use man command\n");
}

int dsh_exit() {
    return 0;
}


int dsh_cd(char **args) {
    if (args[1] == NULL) {
        printf("unexcepted arguments");
        exit(EXIT_FAILURE);
    }
    else{
        if (chdir(args[1]) == -1) {
            perror("dsh");
        }
    }
    return 1;
}

char *dsh_read_line() {

    int buffer_size = BUFFER_MAXSIZE;
    int c;
    int position = 0;

    //向存放字符的地方分配内存
    char *buffer = (char*)malloc(sizeof(char) * buffer_size);
    if (!buffer) {
        printf("eror!");
        exit(EXIT_FAILURE);
    }

    //读取字符
    while (true)
    {
        c = getchar();
        if (c == EOF || c == '\n') {
            buffer[position] = '\0';
            return buffer;
        }
        else{
            buffer[position] = c;
        }
        position++;

        //超出最大范围，重新分配内存    
        if (position >= BUFFER_MAXSIZE) {

            buffer_size += BUFFER_MAXSIZE;
            buffer = (char*)realloc(buffer,buffer_size);

            if (!buffer) {
                printf("alloation error!");
                exit(EXIT_FAILURE);
            }

        }

    }
}

char **dsh_split_line(char *line) {

    char *temp = (char*)malloc(sizeof(char) * (strlen(line) + 1));
    strcpy(temp,line);
    char *token;
    int position = 0;

    char **tokens = (char**)malloc(sizeof(char *) * DSH_TOK_BUFFER);
    if (!tokens) {
        printf("error!");
        exit(EXIT_FAILURE);
    }

    token = strtok(temp,DSH_TOK_DELIM);

    while (token != NULL)
    {
        tokens[position] = token;
        position++;

        if (position >= DSH_TOK_BUFFER) {
            tokens = (char**)realloc(tokens,DSH_TOK_BUFFER * 2);
            if (!token) {
                printf("error!");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, DSH_TOK_DELIM);
    }

    tokens[position] = NULL;
    return tokens;
}



int dsh_lanch(char **args) {
    pid_t pid;
    int status;

    pid = fork();

    if (pid == 0) {
        //子进程
        if (execvp(args[0],args) == -1) {
            printf("child error!\n");
            exit(EXIT_FAILURE);
        }
    }
    else if (pid < 0) {
        //fork出错
        printf("dsh!\n");
        exit(EXIT_FAILURE);
    }
    else {
        //父进程
        do
        {
            waitpid(pid, &status, WUNTRACED);

        } while (!WIFSIGNALED(status) && !WIFEXITED(status));
    }
    return 1;
}

int dsh_execute(char **args) {

    if (args[0] == NULL) {
        return 1;
    }

    for (int i = 0; i < dsh_num_builtins(); i++) {
        if (strcmp(args[0],builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return dsh_lanch(args);
}


void dsh(void) {
    char **args;
    char *line;
    int status;

    do
    {
        printf("dsh$");
        //获取命令输入
        line = dsh_read_line();
        //分割命令和参数
        args = dsh_split_line(line);
        //执行命令
        status = dsh_execute(args);

        free(line);
        free(args);
    } 
    while (status);
}
