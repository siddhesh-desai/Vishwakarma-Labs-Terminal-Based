#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <time.h>

#define TRUE 1
#define FALSE 0

void replace(char s[])
{
    int i = 0;
    while (s[i] != '\0')
    {
        if (s[i] == '\n')
        {
            s[i] = '_';
        }
        i++;
    }
}

void wait_45_min()
{
    int remaining_time = 10; // 60 seconds countdown
    time_t start_time = time(NULL);

    while (remaining_time > 0)
    {

        printf("\rTime remaining: %d seconds", remaining_time);
        fflush(stdout);
        sleep(1);
        // Wait for 1 second
        time_t current_time;
        do
        {
            current_time = time(NULL);
        } while (current_time == start_time); // Wait for 1 second
        remaining_time--;
    }

    printf("\rYou are done!                       \n"); // clear remaining time and print final message
}

int main(int argc, char *argv[])
{
    const char *port = "65001"; /* available for our use */
    char *host;
    struct addrinfo hints, *server;
    int r, sockfd, done;
    char buffer[BUFSIZ];
    fd_set read_fd;

    int counter = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Format: client hostname\n");
        exit(1);
    }
    host = argv[1];

    /* obtain and convert server name and port */
    printf("\nLooking for Vishwakarma Labs on %s...", host);
    memset(&hints, 0, sizeof(hints)); /* use memset_s() */
    hints.ai_family = AF_INET;        /* IPv4 */
    hints.ai_socktype = SOCK_STREAM;  /* TCP */
    r = getaddrinfo(host, port, &hints, &server);
    if (r != 0)
    {
        perror("failed");
        exit(1);
    }
    puts("\nVishwakarma Labs found!");
    puts("\nEnter your Name, Class and Roll no. [AI&DS_A_Siddhesh_27]");

    /* create a socket */
    sockfd = socket(server->ai_family, server->ai_socktype, server->ai_protocol);
    if (sockfd == -1)
    {
        perror("failed");
        exit(1);
    }

    /* connect to the socket */
    r = connect(sockfd, server->ai_addr, server->ai_addrlen);
    freeaddrinfo(server);
    if (r == -1)
    {
        perror("failed");
        exit(1);
    }

    /* loop to interact with the server */
    done = FALSE;
    while (!done)
    {
        /* initialize file descriptor set */
        FD_ZERO(&read_fd);
        FD_SET(sockfd, &read_fd); /* add the socket */
        /* this step must be done to ensure that the fgets() call doesn't block */
        FD_SET(0, &read_fd); /* add standard input */

        r = select(sockfd + 1, &read_fd, NULL, NULL, 0);
        if (r == -1)
        {
            perror("failed");
            exit(1);
        }

        /* remote input */
        if (FD_ISSET(sockfd, &read_fd))
        {
            sleep(1);
            r = recv(sockfd, buffer, BUFSIZ, 0);
            /* check for zero input, disconnection */
            if (r < 1)
            {
                puts("Connection closed by peer");
                break;
            }
            /* otherwise, cap the buffer and output it */
            buffer[r] = '\0';
            printf("%s", buffer);
        }

        /* local input */
        if (FD_ISSET(0, &read_fd))
        {
            /* don't send an empty line */
            if (fgets(buffer, BUFSIZ, stdin) == NULL)
            {
                putchar('\n');
            }
            /* if 'done' is input, close the loop and quit */
            else if (strcmp(buffer, "close\n") == 0)
            {
                done = TRUE;
            }
            /* otherwise, send the input string - including the newline */
            else
            {
                if (counter == 0)
                {
                    replace(buffer);
                    send(sockfd, buffer, strlen(buffer), 0);
                    counter++;
                    printf("\nYour timer has started!");
                    wait_45_min();
                    replace(buffer);
                    strcat(buffer, "Present\n");
                    send(sockfd, buffer, strlen(buffer), 0);
                    printf("\nYour Attendance has been marked! Bye!\n");
                    break;
                }
                else
                {
                    printf("Chat Disabled!!");
                }
            }
        }
    } /* end while loop */

    /* all done, clean-up */
    printf("\nDisconnected\n");
    close(sockfd);

    return (0);
}
