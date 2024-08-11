#include <iostream>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>


#define PORT 8080
#define MAX_CLIENTS 10
#define MAX_BOOKS 15



struct Book {
	char title[50];
	char author[50];
	int available; 
	int book_id;
};

struct Client {
	char name[50];
	char address[100];
	char phone[15];
	int client_socket;
	int borrowed_book_id;
};

Book books[MAX_BOOKS] = {
	{"The Great Gatsby", "F. Scott Fitzgerald", 1, 1001},
	{"1984", "George Orwell", 1, 2002},
	{"To Kill a Mockingbird", "Harper Lee", 1, 3003},
	{"Moby-Dick", "Herman Melville", 0, 4004},
	{"War and Peace", "Leo Tolstoy", 1, 5005},
	{"Pride and Prejudice", "Jane Austen", 1, 6006},
	{"The Catcher in the Rye", "J.D.Salinger", 1, 7007},
	{"The Hobbit", "J.R.R.Tolkien", 1, 8008},
	{"Ulysses", "James Joyce", 0, 9009},
	{"The Odyssey", "Homer", 1 , 1010},
	{"Brave New World", "Aldous Huzley", 1, 1100},
	{"The Lord of the Rings", "J.R.R.Tolkien", 1, 1200},
	{"Fahrenheit 451", "Ray Bradbury", 1, 1300},
	{"The picture of Dorain Gray", "Oscar Wilde", 1, 1400},
	{"Crime and Punishment", "Fyodor Dostoevsky", 1, 1500}
};

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *handle_client(void *arg) {
	Client *client = (Client *)arg;
	char buffer[1024];
	int n;
	
	std::cout << "Client connected: " << client->name << ", " << client->address << ", " << client->phone << std::endl;
	
	while ((n = recv(client->client_socket, buffer, sizeof(buffer), 0)) > 0) {
		buffer[n] = '\0';
		std::cout << "Message from " << client->name << ": " << buffer << std::endl;
		
		if (strcmp(buffer, "borrow") == 0) {
		
	send(client->client_socket, "Enter book title:", 18, 0);
		n = recv(client->client_socket, buffer, sizeof(buffer), 0);
		buffer[n] = '\0';
		
		int book_found = 0;
pthread_mutex_lock(&lock);
	for (int i =0; i < MAX_BOOKS; i++) {
		if (strcmp(books[i].title, buffer) == 0) {
			book_found = 1;
			if (books[i].available) {
			books[i].available = 0;
			client->borrowed_book_id = books[i].book_id;
			
			send(client->client_socket, "How many days? (Max 7): ", 25, 0);
			n = recv(client->client_socket, buffer, sizeof(buffer), 0);
			buffer[n] = '\0';
			int days = atoi(buffer);
			
			snprintf(buffer, sizeof(buffer), "Book borrowed successfully. Book ID: %d. Please return within %d days.", client->borrowed_book_id, days);
			
			send(client->client_socket, buffer, strlen(buffer), 0);
			} else {
			send(client->client_socket, "Book is not available. It will be available soon.", 50, 0);
			 }
			 break;
			}
		}
pthread_mutex_unlock(&lock);
	if (!book_found) {
	
	send(client->client_socket, "Book not found.", 15, 0);
}
}else if (strcmp(buffer, "return") == 0) {

send(client->client_socket, "Enter book ID to return: ", 25, 0);
	n = recv(client->client_socket, buffer, sizeof(buffer), 0);
	buffer[n] = '\0';
	int return_book_id = atoi(buffer);
	
pthread_mutex_lock(&lock);
	for (int i = 0; i < MAX_BOOKS; i++) {
		if (books[i].book_id == return_book_id && return_book_id == client->borrowed_book_id) {
		books[i].available = 1;
		client->borrowed_book_id = 0;
		
		send(client->client_socket, "Book returned successfully", 27, 0);
		break;
		}
	}
	pthread_mutex_unlock(&lock);
	} else if (strcmp(buffer, "exit") == 0) {
	break;
	}
	}
	std::cout << "Client disconnected: " << client->name << std::endl;
	std::cout << std::endl;
	close(client->client_socket);
	pthread_mutex_lock(&lock);
	client_count--;
	pthread_mutex_unlock(&lock);
	return NULL;
}

int main() {
	int server_socket, client_socket;
	struct sockaddr_in server_addr, client_addr;
	socklen_t addr_size;
	pthread_t tid;
	
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0) {
		perror("Socket creation failed");
		exit(EXIT_FAILURE);
	}
	
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = INADDR_ANY;
	server_addr.sin_port = htons(PORT);
	
	if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		perror("Bind failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	
	if (listen(server_socket, MAX_CLIENTS) < 0) {
		perror("Listen failed");
		close(server_socket);
		exit(EXIT_FAILURE);
	}
	
	std::cout << "Server is listening on port " << PORT << std::endl;
	
	while (1) {
		addr_size = sizeof(client_addr);
		client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
		if (client_socket < 0) {
			perror("Accept failed");
			continue;
		}
		
		pthread_mutex_lock(&lock);
		Client *client = &clients[client_count++];
		client->client_socket = client_socket;
		pthread_mutex_unlock(&lock);
		
		recv(client_socket, client->name, sizeof(client->name), 0);
		recv(client_socket, client->address, sizeof(client->address), 0);
		recv(client_socket, client->phone, sizeof(client->phone), 0);
		
		pthread_create(&tid, NULL, handle_client, (void *)client);
	}
	close(server_socket);
	return 0;
}
			
			
			
			
	
