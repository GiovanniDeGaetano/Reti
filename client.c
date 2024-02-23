#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>

void changePos();
void Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
void Recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);

int pos[2],M,sfd;
struct sockaddr_in server;

int main(int argc, char **argv)
{
    char buffer[10];
    socklen_t len = sizeof(server);
    srand(time(NULL));

    if(argc != 2){
        printf("Error : %s <IP Address>\n",argv[0]);
        exit(1);
    }
    // Creazione del socket
    if((sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(2309);

    if(inet_pton(AF_INET,argv[1],&server.sin_addr) <= 0){
        perror("inet_pton");
        exit(1);
    }
    // Invio di un messaggio iniziale al server
    Sendto(sfd,buffer,sizeof(buffer),0,(struct sockaddr*)&server,sizeof(server));
    // Ricezione della dimensione della griglia dal server
    Recvfrom(sfd,&M,sizeof(M),0,(struct sockaddr*)&server,&len);

    pos[0] = rand() % M;
    pos[1] = rand() % M;
    // Invio della posizione al server
    Sendto(sfd,pos,sizeof(pos),0,(struct sockaddr*)&server,sizeof(server));
    while(1){
        Recvfrom(sfd,buffer,sizeof(buffer),0,(struct sockaddr*)&server,&len);
        // Se il messaggio ricevuto è "alert", cambia posizione
        if((strcmp(buffer,"alert")) == 0){
            changePos();
            printf("La navicella si sposta\n");
            // Invio della nuova posizione al server
            Sendto(sfd,pos,sizeof(pos),0,(struct sockaddr*)&server,sizeof(server));
        }
    }
    close(sfd);
    return 0;
}
// Funzione per cambiare posizione
void changePos(){
    int offsetRow;
    // Calcolo dell'offset in base alla posizione corrente
    if(pos[0] == 0){
    // Se la navicella si trova nella prima riga si crea un offset per le righe scegliendo casualmente un nemero tra 0 e 1
        offsetRow = rand() % 2; // tra 0 e 1;
    }
    else if(pos[0] == M-1){
    // Se la navicella si trova nell'ultima riga si crea un offset per le righe scegliendo casualmente un nemero tra -1 e 0
        offsetRow = (rand() % 2)-1; // tra -1 e 0
    }
    else{
        offsetRow = (rand() % 3) - 1; // tra -1,0 e 1
    }
    pos[0] += offsetRow;
    //Si controlla se "offsetRow" non sia uguale a 0 
    //in modo tale di non avere la possibilità che l'offset per le righe che per le colonne siano entrambi uguali a 0
    if(pos[1] == 0){//analogo al if precedente
        if(offsetRow == 0)
            pos[1] += 1;
        else
            pos[1] += rand() % 2; // tra 0 e 1
    }
    else if(pos[1] == M-1){
        if(offsetRow == 0)
            pos[1] += -1;
        else
            pos[1]+= (rand() % 2)-1; // tra -1 e 0
    }
    else{
        if(offsetRow == 0)
            pos[1] += 2 * (rand() % 2) -1; //tra -1 e 1
        else
            pos[1] += (rand() % 3) - 1; // tra -1,0 e 1
    }
}

void Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
    if(sendto(sockfd,buf,len,flags,dest_addr,addrlen) < 0){
        perror("client sendto");
        exit(1);
    }
}
void Recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen){
   if(recvfrom(sockfd,buf,len,flags,src_addr,addrlen) < 0){
        perror("client recvfrom");
        exit(1);
    }

}
