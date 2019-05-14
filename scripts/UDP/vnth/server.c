#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <fcntl.h>

#define PACK_SIZE 1440 /*Tamaño de paquetes*/
#define BUFFER_SZ 512   /*Tamaño de buffer*/

int UDP_setup();
void send_();

struct sockaddr_in serv_addr, cli_addr;
socklen_t cli_len;
int sock, stopth = 1;
char buffer[BUFFER_SZ];

int UDP_setup(){
    int rc, n, sock = 0;

    // Inicio:
    printf("=== SERVER ===\n");
    printf("Encargado del envío de paquetes.\n");
    printf("Tamaño de buffer: %d\n", BUFFER_SZ);

    // Creamos el socket
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
      perror("Problema creando el Socket");
      exit(1);
    }
    serv_addr.sin_family = AF_INET; // IPv4
    serv_addr.sin_port = htons(7500);
    serv_addr.sin_addr.s_addr = inet_addr("192.168.1.255");

    rc = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    if (rc < 0){
      perror("Error en bind");
      exit(1);
    }
    cli_len = sizeof(cli_addr);

    // Verificamos conexión:
    if ((rc = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&cli_addr, &cli_len)) < 0){
       perror("Error al intentar conectar ");
       exit(1);
    }
    printf("CONECTADO.\n");
    return sock;
}

void send_data(sock){
    // Enviamos paquetes:
    int rc;
    for (int i=0; i<10000; i++){
      rc = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&cli_addr, cli_len);
      if (rc < 0){
        perror("Error al enviar paquetes");
        exit(1);
      }
    }
    rc = sendto(sock, NULL, 0, 0, (struct sockaddr *)&cli_addr, sizeof(cli_addr));
}

int main(int argc, char **argv)
{
    int data, rt;
    sock = UDP_setup();
    send_data(sock);

    // Cerramos socket:
    close(sock);
    printf("Socket cerrado.\n");
   	return 0;
}
