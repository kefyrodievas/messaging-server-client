#ifndef MSG_H
#define MSG_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

#define MESSAGE 0
#define NAME    1

#define BUFF_SIZE 4096

char *format_msg(char *buffer, size_t *len, char type)
{
    short msg_len = sizeof(short) + sizeof(char) + (*len);
    char *tmp_buff = malloc(msg_len);
    memset(tmp_buff, 0, msg_len);

    char head[3] = {0};
    head[0] = (msg_len & 0xff00) >> 8;
    head[1] = msg_len & 0x00ff;
    head[2] = type;

    // memcpy(tmp_buff, (void*)msg_len, sizeof(short));
    // memcpy(tmp_buff + sizeof(short), (void *)type, sizeof(char));
    // memcpy(tmp_buff + sizeof(short) + sizeof(char), buffer, *len);

    strcpy(tmp_buff, head);
    // strcat(tmp_buff, type);
    strcat(tmp_buff, buffer);

    return tmp_buff;
}

int send_all(int fd, char *buf, size_t *len)
{
    size_t total = 0;   // how many bytes we've sent
    size_t left = *len; // how many we have left to send
    ssize_t n = -1;

    while (total < *len)
    {
        n = send(fd, buf + total, left, 0);
        if (n == -1)
        {
            break;
        }
        total += n;
        left -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success
}

#endif