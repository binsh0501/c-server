// 참고자료로 쓰세요 참고로 이거 제대로 동작안하긴함
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <tchar.h>

WSADATA wsaData;
SOCKET sock0;
struct sockaddr_in client;
struct sockaddr_in addr;
#define PORT 23117

#define IF(c,d) if(c){d;}

void HandleWinsockError(const char *message) {
  int error_code = WSAGetLastError();
  char *error_msg = NULL;
  FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
  NULL, error_code, 0, (LPSTR)&error_msg, 0, NULL);
  fprintf(stderr, "%s: %s (Error Code: %d)\n", message, (error_msg)?error_msg:"Unknown Error", error_code);
  IF(error_msg, LocalFree(error_msg))
  exit(1);
}

void sendPage(char* content, SOCKET* sock) {
  if (send(*sock, content, strlen(content), 0) < 1) HandleWinsockError("send(0)");
  free(content);
  content = NULL;

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
  size_t read_size = fread(content, 1, file_size, file);
  content[read_size] = '\0';
  fclose(file);
  return content;
}

int getHostAddr(void) {
  int i;
  HOSTENT *lpHost;
  IN_ADDR inaddr;
  char szBuf[256],szIP[16];
  if (WSAStartup(MAKEWORD(1,1), &wsaData)) {
    printf("WSAStartup failed\n");
    return -1;
  }
  gethostname(szBuf, (int)sizeof(szBuf));
  lpHost = gethostbyname(szBuf);
  for (i=0;lpHost->h_addr_list[i];i++) memcpy(&inaddr, lpHost->h_addr_list[i], 4);
  strcpy(szIP, inet_ntoa(inaddr));
  printf("build server: http://%s:%d\n",szIP,PORT);  
  WSACleanup();
  return 0;
}

DWORD WINAPI handleRequest(LPVOID lpParam) {
  SOCKET sockw = *(SOCKET*)lpParam;
  int len, sockaddr_in_size = sizeof(struct sockaddr_in), recv_len = 0;
  unsigned char buf[1024], path[1024];
  unsigned char* html = NULL;
  html = (unsigned char*)malloc(1024);
  memset(path, 0, 1024);
  memset(html, 0, 1024);
  recv_len = recvfrom(sockw, buf,1024, 0, (struct sockaddr *)&client, &sockaddr_in_size);
  buf[recv_len - 1] = 0;
  if (buf[0] == '\0') strcpy(buf, NULL);
  printf("%s \n",buf);
  for (int i = 0; i < strlen(buf); i++) {
    printf("%d\n",i);
    if (buf[i] == 'G' && buf[i + 1] == 'E' && buf[i + 2] == 'T' && buf[i + 3] == ' ') {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      break;
    } else if (buf[i] == 'P' && buf[i + 1] == 'O' && buf[i + 2] == 'S' && buf[i + 3] == 'T' && buf[i + 4] == ' ') {
      for (int j = 0; buf[i + 4 + j] != ' '; j++) path[j] = buf[i + 4 + j];
      break;
    }
  }
  printf("request: %s \n",path);
  unsigned char *header =  "HTTP/1.0 200 OK\nContent-type: text/html\n\n";
  send(sockw, header, strlen(header), 0);
  if (strcmp(path, "/page1") == 0) {
    sendPage(readFile("page2.html"), &sockw);
  }else if (strcmp(path, "/page2") == 0) {
    sendPage(readFile("page2.html"), &sockw);
  }else {
    sendPage(readFile("page2.html"), &sockw);
  }
  printf("the program was stopped this line\n");
  closesocket(sockw);
  free(html);
  return 0;
}

int main() {
  int len, n, sockaddr_in_size = sizeof(struct sockaddr_in), recv_len = 0;
  HANDLE hThread;
  DWORD dwThreadId;
  unsigned char buf[1024], path[1024];
  unsigned char* html = NULL;
  if (getHostAddr() != 0) HandleWinsockError("get IP address failed");
  if (WSAStartup(MAKEWORD(2, 0), &wsaData)) HandleWinsockError("reset winsock failed");
  sock0 = socket(AF_INET, SOCK_STREAM, 0);
  if (sock0 == INVALID_SOCKET) HandleWinsockError("socket failed");
  addr.sin_family = AF_INET;
  addr.sin_port = htons(PORT);
  addr.sin_addr.S_un.S_addr = INADDR_ANY;
  if (bind(sock0, (struct sockaddr *)&addr, sizeof(addr)) != 0) HandleWinsockError("bind failed");
  if (listen(sock0, 5) != 0) HandleWinsockError("listen()");
  html = (unsigned char*)malloc(1024);
  if (html == NULL) {
    perror("Memory allocation failed");
    return NULL;
  }
  while (1) {
    SOCKET sockw = accept(sock0, (struct sockaddr *)&client, &sockaddr_in_size);
    if (sockw == INVALID_SOCKET) HandleWinsockError("accept()");
    hThread = CreateThread(NULL, 0, handleRequest, (LPVOID)&sockw, 0, &dwThreadId);
    if (hThread == NULL) HandleWinsockError("CreateThread failed");
    CloseHandle(hThread);
  }
  closesocket(sock0);
  WSACleanup();
  return 0;
}
