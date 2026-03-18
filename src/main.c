#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h> 
#include <sys/types.h>
#include <sys/stat.h>

struct InputBuffer {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
    bool valid_input;
};

// Check if the given command is a builtin command
bool is_builtin(const char *command) {
    const char *builtins[] = {"echo", "exit", "type", "pwd", "cd", NULL};
    for (int i = 0; builtins[i] != NULL; i++) {
        if (strcmp(command, builtins[i]) == 0)
            return true;
    }
    return false;
}

// Implement the "type" builtin command
void builtin_type(const char *command) {
    // First, check if it’s a shell builtin
    if (is_builtin(command)) {
        printf("%s is a shell builtin\n", command);
        return;
    }

    // Get the PATH environment variable
    char *path_env = getenv("PATH");
    if (!path_env) {
        printf("%s: not found\n", command);
        return;
    }

    // Make a copy of PATH because strtok() modifies the string
    char *path_copy = strdup(path_env);
    char *dir = strtok(path_copy, ":");  // PATH is separated by ':' on Linux
    char full_path[512];
    bool found = false;

    // Search each directory in PATH
    while (dir != NULL) {
        // Build the full path: /dir/command
        snprintf(full_path, sizeof(full_path), "%s/%s", dir, command);

        // Check if the file exists and has execute permission
        if (access(full_path, X_OK) == 0) { //X_OK Only works for linux
            printf("%s is %s\n", command, full_path);
            found = true;
            break;
        }

        // Move to the next directory in PATH
        dir = strtok(NULL, ":");
    }

    free(path_copy);

    // If not found in any directory
    if (!found) {
        printf("%s: not found\n", command);
    }
}

// Parse and execute user input
bool check_input(struct InputBuffer *input_buffer) {
    char *command = strtok(input_buffer->buffer, " ");
    if (!command) return false;

    // Handle "echo"
    if (strcmp(command, "echo") == 0) {
        input_buffer->valid_input = true;
        char *args = strtok(NULL, "");
        if (args)
            printf("%s\n", args);
        else
            printf("\n");
        return true;
    }

    // Handle "exit"
    else if (strcmp(command, "exit") == 0) {
        input_buffer->valid_input = true;
        exit(0);
    }

    // Handle "type"
    else if (strcmp(command, "type") == 0) {
        char *arg = strtok(NULL, " ");
        if (arg == NULL) {
            printf("usage: type <command>\n");
        } else {
            builtin_type(arg);
        }
        return true;
    }

    // Handle unknown commands
    else {
        printf("%s: command not found\n", command);
    }

    return false;
}




int main(void) {
    while (1) {
        char input[100];
        setbuf(stdout, NULL);  // Disable output buffering
        printf("$ ");
        if (!fgets(input, sizeof(input), stdin)) break;

        // Remove the trailing newline character
        input[strcspn(input, "\n")] = '\0';

        struct InputBuffer buffer;
        buffer.buffer = input;
        buffer.valid_input = false;

        check_input(&buffer);
    }




    return 0;
}
