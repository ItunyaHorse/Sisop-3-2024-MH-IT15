#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080

void log_change(const char *type, const char *message) {
    FILE *fp = fopen("/home/rhmnn/soal2/change.log", "a");
    if (fp != NULL) {
        time_t now;
        time(&now);
        struct tm *timeinfo = localtime(&now);
        char timestamp[20];
        strftime(timestamp, sizeof(timestamp), "%d/%m/%y %T", timeinfo);
        fprintf(fp, "[%s] [%s] %s\n", timestamp, type, message);
        fclose(fp);
    }
}

void process_command(int client_socket, const char *command) {
    FILE *anime_file = fopen("/home/rhmnn/soal2/myanimelist.csv", "r+");
    if (anime_file == NULL) {
        perror("Gagal membuka file myanimelist.csv");
        return;
    }

    char response[4096] = {0};
    char line[1024];
    char *token;

    if (strncmp(command, "tampilkan", 9) == 0) {
        while (fgets(line, 1024, anime_file) != NULL) {
            strcat(response, line);
        }
    } else if (strncmp(command, "genre", 5) == 0 || strncmp(command, "hari", 4) == 0 || strncmp(command, "status", 6) == 0) {
        char keyword[512];
        sscanf(command, "%*s %[^\n]", keyword);
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, keyword) != NULL) {
                strcat(response, line);
            }
        }
    } else if (strncmp(command, "add", 3) == 0) {
        char new_anime[1024];
        sscanf(command, "%*s %[^\n]", new_anime);
        fprintf(anime_file, "%s\n", new_anime);
        strcpy(response, "Anime berhasil ditambahkan.");
        log_change("ADD", new_anime);
    } else if (strncmp(command, "edit", 4) == 0) {
        char old_title[100];
        sscanf(command, "%*s %[^\n]", old_title);
        long int pos = ftell(anime_file);
        int found = 0;
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, old_title) != NULL) {
                found = 1;
                fseek(anime_file, pos, SEEK_SET);
                fprintf(anime_file, "%s\n", command + 5); // Skip "edit " part
                strcpy(response, "Anime berhasil diedit.");
                log_change("EDIT", command + 5); // Skip "edit " part
                break;
            }
            pos = ftell(anime_file);
        }
        if (!found) {
            strcpy(response, "Anime tidak ditemukan.");
        }
    } else if (strncmp(command, "delete", 6) == 0) {
        char delete_title[100];
        sscanf(command, "%*s %[^\n]", delete_title);
        long int pos = ftell(anime_file);
        int found = 0;
        while (fgets(line, 1024, anime_file) != NULL) {
            if (strstr(line, delete_title) != NULL) {
                found = 1;
                fseek(anime_file, pos, SEEK_SET);
                fprintf(anime_file, " ");
                strcpy(response, "Anime berhasil dihapus.");
                log_change("DEL", delete_title);
                break;
            }
            pos = ftell(anime_file);
        }
        if (!found) {
            strcpy(response, "Anime tidak ditemukan.");
        }
    } else if (strncmp(command, "exit", 4) == 0) {
        strcpy(response, "Exiting the client");
    } else {
        strcpy(response, "Invalid Command");
    }

    send(client_socket, response, strlen(response), 0);
    fclose(anime_file);
}

int main(int argc, char const *argv[]) {
    int server_socket, client_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char command[2048] = {0};

    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    if ((client_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
         perror("accept");
         exit(EXIT_FAILURE);
    }
    while(1) {
        recv(client_socket, command, 2048, 0);

        printf("Received: %s\n", command);

        process_command(client_socket, command);

    }

    close(server_socket);
    close(client_socket);
    return 0;
}
