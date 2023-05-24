#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>

#define TRUE 1
#define FALSE 0

void add_student_to_csv(char *class, char *div, char *name, char *roll, char *attendance)
{
    // Open the CSV file in append mode
    FILE *fp = fopen("students.csv", "a");

    // Check if file opened successfully
    if (fp == NULL)
    {
        printf("Error opening file\n");
        return;
    }

    // Write the data to the CSV file
    fprintf(fp, "%s,%s,%s,%s,%s\n", class, div, name, roll, attendance);

    // Close the file
    fclose(fp);
}

void split_string(char str[], char classs[], char div[], char nam[], char roll[], char attendance[])
{
    char *token;
    int i = 0;
    token = strtok(str, "_");
    while (token != NULL)
    {
        switch (i)
        {
        case 0:
            strcpy(classs, token);
            break;
        case 1:
            strcpy(div, token);
            break;
        case 2:
            strcpy(nam, token);
            break;
        case 3:
            strcpy(roll, token);
            break;
        case 4:
            strcpy(attendance, token);
            break;
        }
        i++;
        token = strtok(NULL, "_");
    }
}

int main()
{
    /* define variables here */
    const char *port = "65001";     /* same port as the client */
    const int clientname_size = 32; /* store client's IPv4 address */
    char clientname[clientname_size];
    char buffer[BUFSIZ], sendstr[BUFSIZ];
    const int backlog = 10;                    /* also max connections */
    char connection[backlog][clientname_size]; /* storage for IPv4 connections */
    socklen_t address_len = sizeof(struct sockaddr);
    struct addrinfo hints, *server;
    struct sockaddr address;
    int r, max_connect, fd, x, done;
    fd_set main_fd, read_fd;
    int serverfd, clientfd;

    char classs[8], div[8], nam[20], roll[5], attendance[10];

    int details_counter = 0;
    char name[60];
    char namee[60];

    /* setup the server */
    memset(&hints, 0, sizeof(struct addrinfo)); /* use memset_s() */
    hints.ai_family = AF_INET;                  /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;            /* TCP */
    hints.ai_flags = AI_PASSIVE;                /* accept any connection */
    r = getaddrinfo(0, port, &hints, &server);
    if (r != 0)
    {
        perror("failed");
        exit(1);
    }

    /* create a socket */
    serverfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (serverfd == -1)
    {
        perror("failed");
        exit(1);
    }

    /* bind to a port */
    r = bind(serverfd, server->ai_addr, server->ai_addrlen);
    if (r == -1)
    {
        perror("failed");
        exit(1);
    }

    /* listen for a connection*/
    puts("Server is Live now...");
    r = listen(serverfd, backlog);
    if (r == -1)
    {
        perror("failed");
        exit(1);
    }

    /* deal with multiple connections */
    max_connect = backlog;      /* maximum connections */
    FD_ZERO(&main_fd);          /* initialize file descriptor set */
    FD_SET(serverfd, &main_fd); /* set the server's file descriptor */
    /* endless loop to process the connections */
    done = FALSE;
    while (!done)
    {
        /* backup the main file descriptor set into a read set for processing */
        read_fd = main_fd;

        /* scan the connections for any activity */
        r = select(max_connect + 1, &read_fd, NULL, NULL, 0);
        if (r == -1)
        {
            perror("failed");
            exit(1);
        }

        /* loop to check for active connections */
        for (fd = 1; fd <= max_connect; fd++)
        {
            /* filter only active or new clients */
            if (FD_ISSET(fd, &read_fd))
            {
                /* check the server for a new connection */
                if (fd == serverfd)
                {
                    /* add the new client */
                    clientfd = accept(
                        serverfd,
                        (struct sockaddr *)&address,
                        &address_len);
                    if (clientfd == -1)
                    {
                        perror("failed");
                        exit(1);
                    }
                    /* connection accepted, get IP address */
                    r = getnameinfo(
                        (struct sockaddr *)&address,
                        address_len,
                        clientname,
                        clientname_size,
                        0,
                        0,
                        NI_NUMERICHOST);
                    /* update array of IP addresses */
                    strcpy(connection[clientfd], clientname);

                    /* add new client socket to the file descriptor list */
                    FD_SET(clientfd, &main_fd);

                    /* welcome the new user: create welcome string and send */
                    /* welcome string: "SERVER> Welcome xxx.xxx.xxx.xxx to the chat server\n"
                       "Type 'close' to disconnect; 'shtudown' to stop\n" */
                    strcpy(buffer, "\nVISHWAKRAMA LABS> Welcome ");
                    strcat(buffer, connection[clientfd]);
                    strcat(buffer, " to Vishwakarma Labs!");
                    // strcat(buffer, " Enter your Name, Class and Roll no. [AI&DS_A_Siddhesh_27]");
                    send(clientfd, buffer, strlen(buffer), 0);

                    /*ask details of the student and store them in an array*/
                    // char name[60];
                    // strcpy(buffer, "Enter your name: ");
                    // send(clientfd, buffer, strlen(buffer), 0);
                    // r = recv(fd, buffer, BUFSIZ, 0);
                    // if (r >= 1)
                    // {
                    //     buffer[r - 1] = " ";
                    //     strcpy(name, buffer);
                    //     printf("%s", name);
                    // }

                    // send(clientfd, buffer, strlen(buffer), 0);

                    /* tell everyone else about the new user */
                    /* build the string: "SERVER> xxx.xxx.xxx.xxx has joined the server" */
                    strcpy(buffer, "\nVISHWAKARMA LABS> ");
                    strcat(buffer, connection[clientfd]);
                    strcat(buffer, " has joined the server\n");
                    /* loop from the server's file descriptor up,
                       sending the string to each active connection */
                    for (x = serverfd + 1; x < max_connect; x++)
                    {
                        // if (FD_ISSET(x, &main_fd))
                        // send(x, buffer, strlen(buffer), 0);
                    }
                    /* output the string to the local console as well */
                    printf("%s\n", buffer);

                } /* end if to add new client */
                /* deal with incoming data from an established connection */
                else
                {
                    /* check input buffer for the current fd */
                    r = recv(fd, buffer, BUFSIZ, 0);
                    /* if nothing received, disconnect them */
                    if (r < 1)
                    {
                        FD_CLR(fd, &main_fd); /* clear the file descriptor */
                        close(fd);            /* close the file descriptor/socket */

                        /* tell others that the user has disconnected */
                        /* build the string: "SERVER> xxx.xxx.xxx.xxx, disconnected" */
                        strcpy(buffer, "\nVISHWAKARMA LABS> ");
                        strcat(buffer, connection[fd]);
                        strcat(buffer, " disconnected\n");
                        /* loop through all connections (not the server) to send the string */
                        for (x = serverfd + 1; x < max_connect; x++)
                        {
                            if (FD_ISSET(x, &main_fd))
                            {
                                send(x, buffer, strlen(buffer), 0);
                            }
                        }
                        /* output the string locally */
                        printf("%s\n", buffer);
                    }
                    /* at this point, the connected client has text to share */
                    /* share the incoming text with all connections */
                    else
                    {
                        buffer[r] = '\0'; /* cap the received string */
                        /* first check to see whether the "shutdown\n" string was sent */
                        if (strcmp(buffer, "shutdown\n") == 0)
                        {
                            done = TRUE; /* if so, set the loop's terminating condition */
                        }
                        /*if they are details*/
                        // else if(details_counter==0){
                        //     strcpy(name,buffer);
                        //     details_counter++;
                        //     printf("Joined - %s", name);
                        // }
                        // else if(details_counter==1){
                        //     strcpy(namee,buffer);
                        //     details_counter++;
                        //     printf("Completed - %s",namee);
                        // }
                        /* otherwise, echo the received string to all connected fds */
                        else
                        {
                            /* build the string: "xxx.xxx.xxx.xxx> [text]" */
                            strcpy(sendstr, connection[fd]);
                            strcat(sendstr, "> ");
                            strcat(sendstr, buffer);

                            split_string(buffer, classs, div, nam, roll, attendance);
                            add_student_to_csv(classs, div, nam, roll, attendance);

                            strcpy(classs, "");
                            strcpy(div, "");
                            strcpy(nam, "");
                            strcpy(roll, "");
                            strcpy(attendance, "");
                            // classs, div[1], nam[20], roll[5], attendance[10];
                            /* loop through all connections, but not the server */
                            for (x = serverfd + 1; x < max_connect; x++)
                            {
                                /* check for an active file descriptor */
                                if (FD_ISSET(x, &main_fd))
                                {
                                    /* send the built string */
                                    // send(x, sendstr, strlen(sendstr), 0);
                                }
                            }
                            /* echo the string to the server as well */
                            printf("%s\n", sendstr);
                        } /* end else for connected client */
                    }     /* end else after disconnect */
                }         /* end else to send/recv from client(s) */
            }             /* end if */
        }                 /* end for loop through connections */
    }                     /* end while */

    /* generate local message: "SERVER> Shutdown issued; cleaning up" */
    puts("\nVISHWAKRMA LABS> Shutdown issued; cleaning up");
    /* close the socket and free any allocated memory */
    close(serverfd);
    freeaddrinfo(server);
    return (0);
}
