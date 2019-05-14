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

    // rc = bind(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    // if (rc < 0){
    //   perror("Error en bind");
    //   exit(1);
    // }
    cli_len = sizeof(cli_addr);

    // Verificamos conexión:
    // if ((rc = recvfrom(sock, buffer, 256, 0, (struct sockaddr *)&cli_addr, &cli_len)) < 0){
    //    perror("Error al intentar conectar ");
    //    exit(1);
    // }
    // printf("CONECTADO.\n");
    return sock;
}

void *send_data(){
    // Enviamos paquetes:
    int rc;
    while (stopth){
      rc = sendto(sock, buffer, strlen(buffer), 0, (struct sockaddr *)&cli_addr, cli_len);
      if (rc < 0){
        perror("Error al enviar paquetes");
        pthread_exit(NULL);
      }
    }
    pthread_exit(NULL);
}

void *func_stop(void *inp)
{
    fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
    while (1) {
      char __tbuf[2];
      if ( read(0, &__tbuf,1) > 0 )
        break;
      sleep(1);
    }
    stopth = 0;
    sleep(1);
    //munmap((void *) gpio, getpagesize() );
    pthread_exit(NULL);
}

int main(int argc, char **argv)
{
    pthread_t thread_send, thread_stop;
    int data, rt;
    sock = UDP_setup();

    rt = pthread_create(&thread_send, NULL, send_data, (void *) &data);
    if (rt < 0){
      printf("Error creando thread.\n");
    }

    rt = pthread_create(&thread_stop, NULL, func_stop, (void *) &data);
    if (rt < 0){
      printf(" Error in stop\n");
    }
    pthread_join(thread_send, NULL);
    pthread_join(thread_stop, NULL);

    // Cerramos socket:
    close(sock);
    printf("Socket cerrado.\n");
   	return 0;
}
