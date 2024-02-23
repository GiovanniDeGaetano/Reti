#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>


void printGriglia(char **);
void generateDet(int [],int []);
void Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen);
void Recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen);
int M;

int main(int argc, char **argv)
{
    int sfd,n = 5;
    if(argc != 2){
        printf("Error : %s <Size mat>\n",argv[0]);
        exit(1);
    }
    M = atoi(argv[1]);
    char buffer[10],**griglia = (char **)malloc(M*sizeof(char*));
    short crash = 0,changePos = 0;
    int pos[2],detrito[2];
    struct sockaddr_in server,client;
    socklen_t len = sizeof(client);
    //allocazione e inizializzazione matrice
    for(int i = 0;i < M;i++){
        griglia[i] = (char*)malloc(M*sizeof(char));
        for(int j = 0;j<M;j++){
            griglia[i][j] = '-';
        }
    }

    srand(time(NULL));
    // Creazione del socket
    if((sfd = socket(AF_INET,SOCK_DGRAM,0)) < 0){
        perror("socket");
        exit(1);
    }
    server.sin_family = AF_INET;
    server.sin_port = htons(2309);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(sfd,(struct sockaddr*)&server,sizeof(server)) < 0){
        perror("bind");
        exit(1);
    }
    // Ricezione dei dati dal client
    Recvfrom(sfd,buffer,sizeof(buffer),0,(struct sockaddr*)&client,&len);
    // Invio della dimensione della griglia al client
    Sendto(sfd,&M,sizeof(M),0,(struct sockaddr*)&client,sizeof(client));
    // Ricezione della posizione dal client
    Recvfrom(sfd,pos,sizeof(pos),0,(struct sockaddr*)&client,&len);
    griglia[pos[0]][pos[1]] = 'n';

    while(1){
        for(int i=0;i<n;i++){
            // Se il client ha cambiato la sua posizione genera i detriti nella direzione della navicella
            if(changePos){
                generateDet(pos,detrito);
            }
            else{// Altrimenti genera casualmente i detriti nella matrice
                detrito[0] = rand() % M;
                detrito[1] = rand() % M;
            }
            griglia[detrito[0]][detrito[1]] = 'x';
            // Invio della posizione del detrito al client
            Sendto(sfd,detrito,sizeof(detrito),0,(struct sockaddr*)&client,sizeof(client));
            // Controllo se il detrito ha colpito il client
            if((detrito[0] == pos[0]) && (detrito[1] == pos[1])){
                crash = 1;
            }

        }
        // Se il client è stato colpito, invia un avviso al client
        if(crash){
            crash = 0;
            char alert[10] = "alert";
            Sendto(sfd,alert,strlen(alert),0,(struct sockaddr*)&client,sizeof(client));
            // Ricezione della nuova posizione dal client
            Recvfrom(sfd,pos,sizeof(pos),0,(struct sockaddr*)&client,&len);
            changePos = 1;
            griglia[pos[0]][pos[1]] = 'n';
        }
        sleep(2);
        printGriglia(griglia);
        printf("\n");
        for(int i = 0;i < M;i++){
            for(int j = 0;j<M;j++)
                if(griglia[i][j] != 'n')
                    griglia[i][j] = '-';
        }
    }
    close(sfd);
    return 0;
}


void printGriglia(char **mat){
    for(int i = 0;i < M;i++){
        for(int j = 0;j<M;j++){
            printf("%c",mat[i][j]);
        }
        printf("\n");
    }
}

void Sendto(int sockfd, const void *buf, size_t len, int flags,const struct sockaddr *dest_addr, socklen_t addrlen){
    if(sendto(sockfd,buf,len,flags,dest_addr,addrlen) < 0){
        perror("server sendto");
        exit(1);
    }
}
void Recvfrom(int sockfd, void *buf, size_t len, int flags,struct sockaddr *src_addr, socklen_t *addrlen){
   if(recvfrom(sockfd,buf,len,flags,src_addr,addrlen) < 0){
        perror("server recvfrom");
        exit(1);
    }

}

void generateDet(int pos[],int detrito[]){
    int offsetRow,offsetCol;
    if(pos[0] == 0){
        offsetRow = rand() % 2; // tra 0 e 1;
    }
    else if(pos[0] == M-1){
        offsetRow = (rand() % 2)-1; // tra -1 e 0
    }
    else{
        offsetRow = (rand() % 3) - 1; // tra -1,0 e 1
    }

    if(pos[1] == 0){
       offsetCol = rand() % 2; // tra 0 e 1
    }
    else if(pos[1] == M-1){
       offsetCol = (rand() % 2)-1; // tra -1 e 0
    }
    else{
        offsetCol = (rand() % 3) - 1; // tra -1,0 e 1
    }
    detrito[0] = pos[0] + offsetRow;
    detrito[1] = pos[1] + offsetCol;
}
