#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080

int main() {
	int client_socket;
	struct sockaddr_in server_addr;
	char buffer[1024];
	int n;
	
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(PORT);
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Connection to the server failed");
		close(client_socket);
		exit(EXIT_FAILURE);
	}
	
	std::cout << "Enter your name: ";
	std::cin.getline(buffer, sizeof(buffer));
	send(client_socket, buffer, strlen(buffer), 0);
	
	std::cout << "Enter your address: ";
	std::cin.getline(buffer, sizeof(buffer));
	send(client_socket, buffer, strlen(buffer), 0);
	
	
	std::cout << "Enter your phone number: ";
	std::cin.getline(buffer, sizeof(buffer));
	send(client_socket, buffer, strlen(buffer), 0);
	
	while (true) {
		std::cout << "\nChoose action (borrow/return/exit): ";
		std::cin.getline(buffer, sizeof(buffer));
		send(client_socket, buffer, strlen(buffer), 0);
		
		if (strcmp(buffer, "borrow") == 0) {
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer;
		std::cin.getline(buffer, sizeof(buffer));
		send(client_socket, buffer, strlen(buffer), 0);
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		
		if (strstr(buffer, "How many days?") != NULL) {
		std::cin.getline(buffer, sizeof(buffer));
		send(client_socket, buffer, strlen(buffer), 0);
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		}
	} else if (strcmp(buffer, "return") == 0) {
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer;
		
		std::cin.getline(buffer, sizeof(buffer));
		send(client_socket, buffer, strlen(buffer), 0);
		recv(client_socket, buffer, sizeof(buffer), 0);
		std::cout << buffer << std::endl;
		} else if (strcmp(buffer, "exit") == 0) {
		break;
		}
	}
	close(client_socket);
	return 0;
}	
