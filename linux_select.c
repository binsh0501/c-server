#ifndef __linux__
#error This program is for Linux only!
#endif
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <poll.h>
#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdbool.h>
#include "status.h"

typedef int SOCKET;
#define SOCKET_ERROR   -1
#define INVALID_SOCKET -1

void err_quit(const char *msg) {
	char *msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
	exit(1);
}

void err_display(const char *msg) {
	char *msgbuf = strerror(errno);
	printf("[%s] %s\n", msg, msgbuf);
}

void err_display_code(int errcode) {
  char *msgbuf = strerror(errcode);
  printf("[에러] %s\n", msgbuf);
}

#define SERVERPORT 23107
#define BUFSIZE    1024

struct SOCKETINFO {
	SOCKET sock;
	char buf[BUFSIZE + 1];
  char path[1024];
	int recvbytes;
	int sendbytes;
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
struct SOCKETINFO *SocketInfoArray[FD_SETSIZE];

bool AddSocketInfo(SOCKET sock);
void RemoveSocketInfo(int nIndex);
int GetMaxFDPlus1(SOCKET s);
char *readFile(const char *fname);
RequestType getReqType(char* buf, char* path);
HTTPStatus getStatusByPhrase(HTTPphrase phrase);
HTTPphrase getPhraseByStatus(HTTPStatus status);
char* makeHeader(char* version, HTTPStatus status, char* contenttype);
void addHeader(char** header, char* key, char* value);
void endHeader(unsigned char** header);
char* reqTypeStr(RequestType t);
bool cmpMethod(char* buf, int start, char* method);

int main(int argc, char *argv[]) {
	int retval;
	SOCKET listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock == INVALID_SOCKET) err_quit("socket()");
	struct sockaddr_in serveraddr;
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(SERVERPORT);
	retval = bind(listen_sock, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if (retval == SOCKET_ERROR) err_quit("bind()");
	retval = listen(listen_sock, SOMAXCONN);
	if (retval == SOCKET_ERROR) err_quit("listen()");
	int flags = fcntl(listen_sock, F_GETFL);
	flags |= O_NONBLOCK;
	fcntl(listen_sock, F_SETFL, flags);
	fd_set rset, wset;
	int nready;
	SOCKET client_sock;
	struct sockaddr_in clientaddr;
	socklen_t addrlen;
	while (1) {
		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(listen_sock, &rset);
		for (int i = 0; i < nTotalSockets; i++) {
			if (SocketInfoArray[i]->recvbytes > SocketInfoArray[i]->sendbytes) FD_SET(SocketInfoArray[i]->sock, &wset);
			else FD_SET(SocketInfoArray[i]->sock, &rset);
		}
		nready = select(GetMaxFDPlus1(listen_sock), &rset, &wset, NULL, NULL);
		if (nready == SOCKET_ERROR) err_quit("select()");
		if (FD_ISSET(listen_sock, &rset)) {
			addrlen = sizeof(clientaddr);
			client_sock = accept(listen_sock,
				(struct sockaddr *)&clientaddr, &addrlen);
			if (client_sock == INVALID_SOCKET) {
				err_display("accept()");
				break;
			}
			else {
				int flags = fcntl(client_sock, F_GETFL);
				flags |= O_NONBLOCK;
				fcntl(client_sock, F_SETFL, flags);
				char addr[INET_ADDRSTRLEN];
				inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
				printf("\n[클라이언트 접속] %s:%d\n",
					addr, ntohs(clientaddr.sin_port));
				if (!AddSocketInfo(client_sock))
					close(client_sock);
			}
			if (--nready <= 0)
				continue;
		}
		for (int i = 0; i < nTotalSockets; i++) {
			struct SOCKETINFO *ptr = SocketInfoArray[i];
			if (FD_ISSET(ptr->sock, &rset)) {
				retval = recv(ptr->sock, ptr->buf, BUFSIZE, 0);
				if (retval == SOCKET_ERROR) {
					err_display("recv()");
					RemoveSocketInfo(i);
				}
				else if (retval == 0) {
					RemoveSocketInfo(i);
				} else {
					ptr->recvbytes = retval;
					addrlen = sizeof(clientaddr);
					getpeername(ptr->sock, (struct sockaddr *)&clientaddr, &addrlen);
					ptr->buf[ptr->recvbytes] = '\0';
					char addr[INET_ADDRSTRLEN];
					inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
					printf("[TCP/%s:%d] %s\n", addr,
						ntohs(clientaddr.sin_port), ptr->buf);
				}
			}
			else if (FD_ISSET(ptr->sock, &wset)) {
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
        }
				if (header == NULL) header = makeHeader("HTTP/1.0", HttpOk, "text/html; charset=utf-8");
        endHeader(&header);
        send(ptr->sock, header, strlen(header), 0);
        retval = send(ptr->sock, buf, ((buf==NULL)?0:strlen(buf)), 0);
        free(buf);
				if (retval == SOCKET_ERROR) {
					err_display("send()");
					RemoveSocketInfo(i);
				}
				else {
					ptr->sendbytes += retval;
					if (ptr->recvbytes == ptr->sendbytes) {
						ptr->recvbytes = ptr->sendbytes = 0;
					}
				}
				RemoveSocketInfo(i);
			}
		}
	}
	close(listen_sock);
	return 0;
}

bool AddSocketInfo(SOCKET sock)
{
	if (nTotalSockets >= FD_SETSIZE) {
		printf("[오류] 소켓 정보 추가 실패.\n");
		return false;
	}
	struct SOCKETINFO *ptr = (struct SOCKETINFO *)calloc(1,sizeof(struct SOCKETINFO));
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
	struct SOCKETINFO *ptr = SocketInfoArray[nIndex];
	struct sockaddr_in clientaddr;
	socklen_t addrlen = sizeof(clientaddr);
	getpeername(ptr->sock, (struct sockaddr *)&clientaddr, &addrlen);
	char addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &clientaddr.sin_addr, addr, sizeof(addr));
	printf("[클라이언트 연결 끊김] IP 주소=%s, 포트 번호=%d\n",
		addr, ntohs(clientaddr.sin_port));
	close(ptr->sock);
	free(ptr);
	if (nIndex != (nTotalSockets - 1))
		SocketInfoArray[nIndex] = SocketInfoArray[nTotalSockets - 1];
	--nTotalSockets;
}

int GetMaxFDPlus1(SOCKET s)
{
	int maxfd = s;
	for (int i = 0; i < nTotalSockets; i++) {
		if (SocketInfoArray[i]->sock > maxfd) {
			maxfd = SocketInfoArray[i]->sock;
		}
	}
	return maxfd + 1;
}

bool cmpMethod(char* buf, int start, char* method) {
  for (int i = 0; i < strlen(method); i++)
    if (buf[start + i] != method[i]) return false;
  return true;
}

RequestType getReqType(char* buf, char* path) {
  for (int i = 0; i < strlen(buf); i++) {
    if (cmpMethod(buf, i, "GET")) {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      return HTTP_GET;
    } else if (cmpMethod(buf, i, "POST")) {
      for (int j = 0; buf[i + 5 + j] != ' '; j++) path[j] = buf[i + 5 + j];
      return HTTP_POST;
    } else if (cmpMethod(buf, i, "HEAD")) {
      for (int j = 0; buf[i + 5 + j] != ' '; j++) path[j] = buf[i + 5 + j];
      return HTTP_HEAD;
    } else if (cmpMethod(buf, i, "PUT")) {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      return HTTP_PUT;
    } else if (cmpMethod(buf, i, "DELETE")) {
      for (int j = 0; buf[i + 7 + j] != ' '; j++) path[j] = buf[i + 7 + j];
      return HTTP_DELETE;
    } else if (cmpMethod(buf, i, "TRACE")) {
      for (int j = 0; buf[i + 5 + j] != ' '; j++) path[j] = buf[i + 5 + j];
      return HTTP_TRACE;
    } else if (cmpMethod(buf, i, "OPTIONS")) {
      for (int j = 0; buf[i + 7 + j] != ' '; j++) path[j] = buf[i + 7 + j];
      return HTTP_OPTIONS;
    } else if (cmpMethod(buf, i, "CONNECT")) {
      for (int j = 0; buf[i + 7 + j] != ' '; j++) path[j] = buf[i + 7 + j];
      return HTTP_CONNECT;
    } else if (cmpMethod(buf, i, "PATCH")) {
      for (int j = 0; buf[i + 5 + j] != ' '; j++) path[j] = buf[i + 5 + j];
      return HTTP_PATCH;
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
