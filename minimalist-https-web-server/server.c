
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <string.h>

void main() {
	//Create socket 
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
	    perror("Socket creation failed");
	    return -1;
    }		

    //Address setup
    struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = INADDR_ANY;

	//bind socket

if (bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
perror("bind failed");
	return -1;

}

	//Listen for connections
    listen(sockfd, 10);

    //Accept client connection
    int clientfd = accept(sockfd, NULL, NULL);
    if (clientfd < 0) {
perror("CLient connection failed");
return -1;
    }

//SSL setup
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());
    SSL*ssl = SSL_new(ctx);
    SSL_set_fd(ssl,clientfd);
    SSL_use_certificate_chain_file(ssl,"fullChain");
    SSL_use_PrivateKey_file(ssl, "theKey", SSL_FILETYPE_PEM);

if (SSL_accept(ssl) <= 0) {
perror("SSL accept failed");
SSL_free(ssl);
return -1;
}

//Read from client
    char buffer[1024] = {0};
    SSL_read(ssl, buffer,1023);

//File request handling
char* file_request = buffer + 5;// assuming GET/file.......

char response[1024] = {0};
char* metadata = "HTTP/1.0.200 OK\r\nContent-Type: text/html\r\n\r\n";
                                        
memcpy(response, metadata, strlen(metadata));
if (strncmp(file_request, "index.html ", 11) ==0){
    FILE* f = fopen("index.html", "r");
    fread(response + strlen(metadata), 1024 - strlen(metadata) - 1,1,f);
    fclose(f);
} else {
    char* error = "No page4 found";
    memcpy(response + strlen(metadata), error, strlen(error));
}

//Send response
SSL_write(ssl,response,strlen(response));

//Cleanup
SSL_shutdown(ssl);
SSL_free(SSL);
SSL_CTX_free(ctx);
close(clientfd);
close(sockfd);

return 0;
}


