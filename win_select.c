#define _CRT_SECURE_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#ifdef __linux__
  #error This program is for Windows only.
#endif
#include <winsock2.h>
#include <ws2tcpip.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "status.h"

#pragma comment(lib, "ws2_32")
#define PORT 23107
#define BUFSIZE 1024

void ErrQuit(const char *msg) {
  LPVOID lpMsgBuf;
  FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, WSAGetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (char *)&lpMsgBuf, 0, NULL);
  MessageBoxA(NULL, (const char *)lpMsgBuf, msg, MB_ICONERROR);
  LocalFree(lpMsgBuf);
  exit(1);
}

void ErrDisplay(const char *msg) {
  LPVOID lpMsgBuf;
  FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, WSAGetLastError(),
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (char *)&lpMsgBuf, 0, NULL);
  printf("[%s] %s\n", msg, (char *)lpMsgBuf);
  LocalFree(lpMsgBuf);
}

void ErrDisplayCode(int errcode) {
  LPVOID lpMsgBuf;
  FormatMessageA(
    FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
    NULL, errcode,
    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
    (char *)&lpMsgBuf, 0, NULL);
  printf("[Error] %s\n", (char *)lpMsgBuf);
  LocalFree(lpMsgBuf);
}

struct SocketInfo {
  SOCKET sock;
  char buf[BUFSIZE + 1];
  char path[1024];
  int recvbytes, sendbytes;
};

typedef enum {
  HTTP_GET,
  HTTP_POST,
  HTTP_HEAD,
  HTTP_PUT,
  HTTP_DELETE,
  HTTP_TRACE,
  HTTP_OPTIONS,
  HTTP_CONNECT,
  HTTP_PATCH
} RequestType;

int nTotalSockets = 0;
struct SocketInfo *SocketInfoArray[FD_SETSIZE];

char *readFile(const char *fname);
bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);
RequestType getReqType(char* buf, char* path);
HTTPStatus getStatusByPhrase(HTTPphrase phrase);
HTTPphrase getPhraseByStatus(HTTPStatus status);
char* makeHeader(char* version, HTTPStatus status, char* contenttype);
void addHeader(char** header, char* key, char* value);
void endHeader(unsigned char** header);
char* reqTypeStr(RequestType t);

int main(int argc, char *argv[]) {
  int retval;
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    return 1;
  SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
  if (listen_sock == INVALID_SOCKET) ErrQuit("socket()");
  struct sockaddr_in serveraddr;
  memset(&serveraddr, 0, sizeof(serveraddr));
  serveraddr.sin_family = AF_INET;
  serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
  serveraddr.sin_port = htons(PORT);
  retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
  if (retval == SOCKET_ERROR) ErrQuit("bind()");
  retval = listen(listen_sock, SOMAXCONN);
  if (retval == SOCKET_ERROR) ErrQuit("listen()");
  u_long on = 1;
  retval = ioctlsocket(listen_sock, FIONBIO, &on);
  if (retval == SOCKET_ERROR) ErrDisplay("ioctlsocket()");
  fd_set rset, wset;
  int nready;
  SOCKET client_sock;
  struct sockaddr_in clientaddr;
  int addrlen;
  while (1) {
    FD_ZERO(&rset);
    FD_ZERO(&wset);
    FD_SET(listen_sock, &rset);
    for (int i = 0; i < nTotalSockets; i++) {
      if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes) FD_SET(SocketInfoArray[i]->sock, &wset);
      else FD_SET(SocketInfoArray[i]->sock, &rset);
    }
    nready = select(0, &rset, &wset, NULL, NULL);
    if (nready == SOCKET_ERROR) ErrQuit("select()");
    if (FD_ISSET(listen_sock, &rset)) {
      addrlen = sizeof(clientaddr);
      client_sock = accept(listen_sock, (struct sockaddr *)&clientaddr, &addrlen);
      if (client_sock == INVALID_SOCKET) {
        ErrDisplay("accept()");
        break;
      } else {
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        printf("[TCP Server] Client Incoming: %s:%d - The Guest Has Arrived!\n", addr, ntohs(clientaddr.sin_port));
        if (!AddSocketInfo(client_sock)) closesocket(client_sock);
      }
      if (--nready <= 0) continue;
    }
    for (int i = 0; i < nTotalSockets; i++) {
      struct SocketInfo *ptr = SocketInfoArray[i];
      if (FD_ISSET(ptr->sock, &rset)) {
        retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
        if (retval == SOCKET_ERROR) {
          ErrDisplay("recv()");
          RemoveSocketInfo(i);
        } else if (retval == 0) {
          RemoveSocketInfo(i);
        } else {
          ptr->recvbytes = retval;
          addrlen = sizeof(clientaddr);
          getpeername(ptr->sock, (struct sockaddr *)&clientaddr, &addrlen);
          ptr->buf[ptr->recvbytes] = '\0';
          char addr[INET_ADDRSTRLEN];
          inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
          // printf("[TCP/%s:%d] %s\n", addr, ntohs(clientaddr.sin_port), ptr->buf);
        }
      } else if (FD_ISSET(ptr->sock, &wset)) {
        memset(ptr->path, 0, 1024);
        addrlen = sizeof(clientaddr);
        getpeername(ptr->sock, (struct sockaddr *)&clientaddr, &addrlen);
        ptr->buf[ptr->recvbytes] = '\0';
        char addr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
        RequestType reqtype = getReqType(ptr->buf, ptr->path);
        printf("[%s/%s]: %s \n",reqTypeStr(reqtype),addr,ptr->path);
        unsigned char* buf = NULL;
        unsigned char* header = NULL;
        if (strcmp(ptr->path, "/") == 0) {
          buf = readFile("login.html");
        } else if (strcmp(ptr->path, "/gender") == 0) {
          buf = readFile("birthdaygender.html");
        } else if (strcmp(ptr->path, "/f.js") == 0) {
          buf = readFile("f.js");
          header = makeHeader("HTTP/1.0", HttpOk, "text/javascript; charset=utf-8");
        } else if (strcmp(ptr->path, "/sg.js") == 0) {
          buf = readFile("sg.js");
          header = makeHeader("HTTP/1.0", HttpOk, "text/javascript; charset=utf-8");
        } else {
          buf = readFile("404.html");
          header = makeHeader("HTTP/1.0", HttpNotFound, "text/html; charset=utf-8");
        }
        if (header == NULL) header = makeHeader("HTTP/1.0", HttpOk, "text/html; charset=utf-8");
        endHeader(&header);
        send(ptr->sock, header, strlen(header), 0);
        retval = send(ptr->sock, buf, strlen(buf), 0);
        free(buf);
        if (retval == SOCKET_ERROR) {
          ErrDisplay("send()");
          RemoveSocketInfo(i);
        } else {
          ptr->sendbytes += retval;
          if (ptr->recvbytes == ptr->sendbytes) {
            ptr->recvbytes = ptr->sendbytes = 0;
          }
        }
        RemoveSocketInfo(i);
      }
    }
  }
  closesocket(listen_sock);
  WSACleanup();
  return 0;
}

bool AddSocketInfo(SOCKET sock)
{
  if (nTotalSockets >= FD_SETSIZE) {
    printf("[Error] Unable to add socket information.\n");
    return false;
  }
  struct SocketInfo *ptr = (struct SocketInfo *)calloc(1,sizeof(struct SocketInfo));
  if (ptr == NULL) {
    printf("[Error] Memory allocation failed – When memory goes MIA!\n");
    return false;
  }
  ptr->sock = sock;
  ptr->recvbytes = 0;
  ptr->sendbytes = 0;
  SocketInfoArray[nTotalSockets++] = ptr;
  return true;
}

void RemoveSocketInfo(int nIndex)
{
  struct SocketInfo *ptr = SocketInfoArray[nIndex];
  struct sockaddr_in clientaddr;
  int addrlen = sizeof(clientaddr);
  getpeername(ptr->sock, (struct sockaddr *)&clientaddr, &addrlen);
  char addr[INET_ADDRSTRLEN];
  inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
  printf("[TCP Server] Client Disconnected: %s:%d\n",
    addr, ntohs(clientaddr.sin_port));
  closesocket(ptr->sock);
  free(ptr);
  if (nIndex != (nTotalSockets - 1))
    SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
  --nTotalSockets;
}

RequestType getReqType(char* buf, char* path) {
  for (int i = 0; i < strlen(buf); i++) {
    if (buf[i] == 'G' && buf[i + 1] == 'E' && buf[i + 2] == 'T' && buf[i + 3] == ' ') {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      return HTTP_GET;
    } else if (buf[i] == 'P' && buf[i + 1] == 'O' && buf[i + 2] == 'S' && buf[i + 3] == 'T' && buf[i + 4] == ' ') {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      return HTTP_POST;
    }
  }
  return HTTP_GET;
}

char* reqTypeStr(RequestType t) {
  switch (t){
    case HTTP_GET: return "GET";
    case HTTP_POST: return "POST";
    case HTTP_HEAD: return "HEAD";
    case HTTP_PUT: return "PUT";
    case HTTP_DELETE: return "DELETE";
    case HTTP_TRACE: return "TRACE";
    case HTTP_OPTIONS: return "OPTIONS";
    case HTTP_CONNECT: return "CONNECT";
    case HTTP_PATCH: return "PATCH";
    default: return NULL;
  }
}

char* makeHeader(char* version, HTTPStatus status, char* contenttype) {
  char* header = (char*)malloc(1024);
  if (header == NULL) {
    perror("Memory allocation failed");
    return NULL;
  }
  char* statusphrase = getPhraseByStatus(status);
  if (statusphrase == NULL) {
    perror("Status phrase not found");
    return NULL;
  }
  sprintf(header, "%s %d %s", version, status, statusphrase);
  addHeader(&header, "Content-Type", contenttype);
  return header;
}

HTTPphrase getPhraseByStatus(HTTPStatus status) {
  for (int i = 0; i < sizeof(HttpStatusTable) / sizeof(HttpStatusTable[0]); i++) 
    if (HttpStatusTable[i].status == status) return HttpStatusTable[i].phrase;
  return NULL;
}

HTTPStatus getStatusByPhrase(HTTPphrase phrase) {
  for (int i = 0; i < sizeof(HttpStatusTable) / sizeof(HttpStatusTable[0]); i++)
    if (HttpStatusTable[i].phrase == phrase) return HttpStatusTable[i].status;
  return -1;
}

void addHeader(char** header, char* key, char* value) {
  size_t len = strlen(*header);
  size_t key_len = strlen(key);
  size_t value_len = strlen(value);
  size_t new_len = len + key_len + value_len + 4; // 4 for ": ", "\r\n", and null terminator
  char* new_header = realloc(*header, new_len);
  if (new_header == NULL) {
    return;
  }
  *header = new_header;
  strcat(*header, key);
  strcat(*header, ": ");
  strcat(*header, value);
  strcat(*header, "\r\n");
}

void endHeader(unsigned char** header) {
  size_t len = strlen(*header);
  size_t new_len = len + 2; // 2 for "\r\n" and null terminator
  char* new_header = realloc(*header, new_len);
  if (new_header == NULL) {
    return;
  }
  *header = new_header;
  strcat(*header, "\r\n");
} 

char *readFile(const char *fname) {
  FILE *file = fopen(fname, "rb");
  if (file == NULL) {
    perror("File opening failed");
    return NULL;
  }
  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);
  rewind(file);
  char *content = (char *)malloc(file_size + 1);
  if (content == NULL) {
    fclose(file);
    perror("Memory allocation failed");
    return NULL;
  }
  size_t total_read = 0;
  size_t bytes_read;
  while (total_read < file_size) {
    bytes_read = fread(content + total_read, 1, file_size - total_read, file);
    if (bytes_read <= 0) {
      perror("File read failed");
      fclose(file);
      free(content);
      return NULL;
    }
    total_read += bytes_read;
  }
  content[total_read] = '\0';
  fclose(file);
  return content;
}
