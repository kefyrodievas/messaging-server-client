#ifndef MSG_H
#define MSG_H

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>

// #include "pack.h"

#define MESSAGE 0
#define NAME    1
#define ROOM    2

#define BUFF_SIZE 4096



char *format_msg(char *buffer, size_t *len, char type) {
    uint16_t msg_len = sizeof(uint16_t) + sizeof(char) + (*len);
    // uint16_t msg_len = *len;
    char *tmp_buff = malloc(msg_len);
    memset(tmp_buff, 0, msg_len);

    char head[3] = { 0 };
    head[0] = (msg_len & 0xff00) >> 8;
    head[1] = msg_len & 0x00ff;
    head[2] = type;

    memcpy(tmp_buff, head, sizeof(head));
    memcpy(tmp_buff + sizeof(head), buffer, *len);

    *len = msg_len;

    return tmp_buff;
}

ssize_t send_all(int fd, char *buf, size_t len) {
    size_t total = 0;   // how many bytes we've sent
    size_t left = len; // how many we have left to send
    ssize_t n = -1;

    while (total < len) {
        n = send(fd, buf + total, left, 0);
        if (n < 0) { break; }
        total += n;
        left -= n;
    }

    if (n == -1 || total != len) {
        return -1;
    }
    return total;
}

ssize_t sendf(int fd, char *buff, size_t len, char type) {
    uint16_t msg_len = sizeof(uint16_t) + sizeof(char) + len;

    char *tmp_buff = malloc(msg_len);
    memset(tmp_buff, 0, msg_len);

    char head[3] = { 0 };
    head[0] = (msg_len & 0xff00) >> 8;
    head[1] = msg_len & 0x00ff;
    head[2] = type;

    memcpy(tmp_buff, head, sizeof(head));
    memcpy(tmp_buff + sizeof(head), buff, len);

    size_t total = 0;   // how many bytes we've sent
    size_t left = msg_len; // how many we have left to send
    ssize_t n = -1;

    while (total < msg_len) {
        n = send(fd, tmp_buff + total, left, 0);
        if (n < 0) { break; }
        total += n;
        left -= n;
    }
    free(tmp_buff);

    if (n == -1 || total != msg_len) {
        return -1;
    }
    return total;
}

ssize_t recvf(int fd, char *buff, char *type) {
    char head[3] = { 0 };
    ssize_t n = -1;

    n = recv(fd, head, sizeof(head), 0);
    if (n <= 0) { return n; }
    if (n < (ssize_t)sizeof(head)) { n += recv(fd, head + n, sizeof(head) - n, 0); }

    uint16_t len = (((uint16_t)head[0] << 8) | head[1]) - sizeof(head);
    *type = head[2];

    memset(buff, 0, len + 1);

    size_t total = 0;
    size_t left = len;

    while (total < len) {
        n = recv(fd, buff + total, left, 0);
        if (n < 0) { break; }
        total += n;
        left -= n;
    }

    if (n == -1 || total != len) {
        return -1;
    }
    return total;
}

#endif