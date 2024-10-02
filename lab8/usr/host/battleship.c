#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <signal.h>

#define SIZE_GRID 16

int socketSend(int sock_fd, char *buf, const size_t size);

int forfeit = 0, sock_fd;

typedef enum
{
	DEFAULT = 0,
	BOAT,
	MISSED_SHOT,
	SUNKEN_BOAT
} gridState;

typedef enum
{
	SERVER_LISTEN = 0,
	SERVER_CLIENT_CONNECT,
	CLIENT_CONNECTING,
	CLIENT_CONNECTED,
	BOAT_PLACE,
	OPPONENT_WAIT,
	ATTACK_CELL,
	USER_ATTACK,
	OPPONENT_ATTACK,
	MISS,
	HIT,
	USER_GRID,
	OPPONENT_GRID,
	USER_WON,
	USER_LOST,
	INV_CELL_ATTACKED,
	INV_CELL_VAL,
	INV_BOAT_STACK,
	USER_FORFEIT,
	OPPONENT_FORFEIT
} stringsIdx;

const char *strings[] = {
	"Server listening on port", // 0
	"Client connected",
	"Connecting to server",
	"Connected to server successfully",
	"Please enter 3 grid cells to place your boats into",
	"Waiting for opponent...", // 5
	"Please enter a grid cell to attack your opponent",
	"You attacked",
	"Your opponent attacked",
	"MISS",
	"HIT", // 10
	"User grid",
	"Opponent grid",
	"YOU WON ^_^",
	"YOU LOST T_T",
	"Invalid ! You already attacked this cell", // 15
	"Invalid ! Please enter a grid cell index between 0-F",
	"Invalid ! You cannot place 2 boats on the same grid cell",
	"You forfeited the game",
	"Your opponent forfeited the game"};

/**
 * @brief Forfeits the game when receiving SIGSTP (Ctrl+Z) signal
 *
 * Signal routine to override default behaviour of SIGSTP. When this routine is called due to the reception of SIGSTP (Ctrl+Z in the shell) it sends a message on the socket to warn that the user wants to forfeit the game and then closes the socket file descriptor before exiting
 */
void quitGame(int sig)
{
	if (sig == SIGTSTP)
	{
		// TO COMPLETE: step 2
		forfeit = 1;
		char data = OPPONENT_FORFEIT;
		
		if (socketSend(sock_fd, &data, sizeof(char) < 0))
		{
			printf("Error sending data \n");
			exit(1);
		}
		
		printf(" %s: %s \n", strings[USER_FORFEIT], strings[USER_LOST]);
		if(close(sock_fd) < 0){
			puts("socket close");
			exit(1);
		}
		exit(0);
	}
}

/**
 * @brief Converts a single hexadecimal character to its decimal value
 *
 * The hexadecimal digit may be lowercase or uppercase
 *
 * @param digit the hexadecimal digit to convert
 *
 * @returns The decimal value of the hexadecimal digit given or -1 if the given digit isn't hexadecimal
 */
int charToHex(const int digit)
{
	if (digit >= '0' && digit <= '9')
		return digit - '0';

	int lowerDigit = tolower(digit);
	if (lowerDigit >= 'a' && lowerDigit <= 'f')
		return 10 + (lowerDigit - 'a');

	return -1;
}

/**
 * @brief Recovers a hexadecimal digit representing a grid cell from the user and returns its decimal value
 *
 * Only the first input hexadecimal digit is returned. All other input characters are safely discarded. If the given hex digit is invalid, an error message is displayed and a new input is required
 *
 * @returns The decimal value of the selected grid cell
 */
char promptCell()
{
	char userInput = 0;
	// TO COMPLETE

	while (1)
	{

		userInput = getchar();
		while (getchar() != '\n' && !feof(stdin))
			;

		userInput = charToHex(userInput);
		

		if (userInput == -1)
		{
			printf("%s \n", strings[INV_CELL_VAL]);

			continue;
		}
		else
		{
			return userInput;
		}
	}
}

/**
 * @brief Prints a grid
 *
 * Prints the given board with the following pattern and symbols:
 * ~ ~ ~ 0
 * ~ o ~ ~
 * ~ 0 ~ o
 * ~ ~ ~ X
 *
 * ~: Default (water)
 * 0: Boat
 * o: Missed shot
 * X: Sunken boat
 */
void printBoard(const char *grid)
{
	const char symbols[] = "~0oX";
	// TO COMPLETE
	for (int i = 0; i < SIZE_GRID; i++)
	{
		if (i % 4 == 0 && i != 0)
		{
			printf("\n");
		}
		printf("%c ", symbols[grid[i]]);
	}
	printf("\n");
}

/**
 * @brief Recovers size bytes from the socket_fd file descriptor and stores them in the buf buffer
 *
 * @returns -1 if an error occured, 0 otherwise
 */
int socketRecv(const int sock_fd, char *buf, const size_t size)
{
	// TO COMPLETE


	
	if (read(sock_fd, buf, size) < 0)
	{
		puts("Error socket recv");
		return -1;
	}
	
	return 0;
}

/**
 * @brief Sends size bytes from the buf buffer to the sock_fd file descriptor
 *
 * @returns -1 if an error occured, 0 otherwise
 */
int socketSend(const int sock_fd, char *buf, const size_t size)
{
	// TO COMPLETE


	
	if (write(sock_fd, buf, size) < 0)
	{
		puts("Error socket send");
	}
	
	return 0;
}

/**
 * @brief Creates a server that waits for a client before returning
 *
 * The server is created and then waits for a client to connect with accept. Once a client is connected, the server's file descriptor is closed and the client's file descriptor is returned
 *
 * @returns -1 if an error occured, the client's file descriptor as returned by accept otherwise
 */
int server_create(const int port)
{
	// TO COMPLETE
	int listen_fd = 0, conn_fd = 0, bindVal = 0;

	struct sockaddr_in serv_addr;

	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_fd < 0)
	{
		printf("Error creating socket \n");
		return -1;
	}

	memset(&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(port);

	bindVal = bind(listen_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
	if (bindVal < 0)
	{
		printf("Error binding \n");
		close(listen_fd);
		return -1;
	}

	// Listen for incoming connections and accept one. There's no need to listen for more than one connection
	if (listen(listen_fd, 1) < 0)
	{
		printf("Error listening \n");
		close(listen_fd);
		return -1;
	}

	printf("%s %d... \n", strings[SERVER_LISTEN], port);
	conn_fd = accept(listen_fd, (struct sockaddr *)NULL, NULL);
	if (conn_fd < 0)
	{
		printf("Error accepting \n");
		close(listen_fd);
		return -1;
	}

	printf("%s \n", strings[SERVER_CLIENT_CONNECT]);

	return conn_fd;
}

/**
 * @brief Creates a client that connects to a server before returning
 *
 * @returns -1 if an error occured, the file descriptor of the server as returned by connect otherwise
 */
int client_connect(const char *ip_addr, const int port)
{
	// TO COMPLETE
	int socket_fd = 0;

	// Create a variable sockadd_in to store the server address and other information
	struct sockaddr_in serv_addr;

	// Create socket with Internet domain and TCP protocol
	if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("Error creating socket \n");
		return -1;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));

	// Configure server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port);

	if (inet_pton(AF_INET, ip_addr, &serv_addr.sin_addr) <= 0)
	{
		printf("Error inet_pton \n");
		return -1;
	}
	printf("%s on %s:%d \n", strings[CLIENT_CONNECTING], ip_addr, port);

	// Try connecting to server

	if (connect(socket_fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("Error connecting \n");
		return -1;
	}
	// Connected to server
	printf("%s to %s:%d \n", strings[CLIENT_CONNECTED], ip_addr, port);

	return socket_fd;
}
/**
 * @brief Converts a "hexadecimal" value to its Ascii representation
 * 
 * @returns The Ascii decimal value of the hexadecimal digit given or -1 if the given digit isn't hexadecimal
 */
int hexToChar(const int hexValue)
{
	if (hexValue >= 0 && hexValue <= 9)
	{
		return hexValue + '0';
	}
	else if (hexValue >= 10 && hexValue <= 15)
	{
		return hexValue - 10 + 'A';
	}
	else
	{
		return -1;
	}
}

int main(int argc, char const *argv[])
{
	char userGrid[16], advGrid[16];
	const char *ip_addr = "127.0.0.1";
	int server = 0, userInput, port = 5000;

	// Verify arguments and configure
	for (size_t i = 0; i < argc; i++)
	{
		if (i == 0)
			continue;
		if (strcmp(argv[i], "-s") == 0)
			server = 1;
		else if (strcmp(argv[i], "-a") == 0)
		{
			ip_addr = argv[i + 1];
			i++;
		}
		else if (strcmp(argv[i], "-p") == 0)
		{
			port = atoi(argv[i + 1]);
			i++;
		}
		else
		{
			printf("Unknown argument\n");
			return -1;
		}
	}

	// TO COMPLETE: Init game boards and ask user for boat placement
	memset(userGrid, DEFAULT, SIZE_GRID);
	memset(advGrid, DEFAULT, SIZE_GRID);

	printf("%s \n", strings[BOAT_PLACE]);
	for (int i = 0; i < 3; i++)
	{
		userInput = promptCell();
		if (userGrid[userInput] == BOAT)
		{
			printf("%s \n", strings[INV_BOAT_STACK]);
			i--;
		}
		else
		{
			userGrid[userInput] = BOAT;
		}
	}

	// TO COMPLETE: Print board
	printf("%s \n", strings[USER_GRID]);
	printBoard(userGrid);

	// TO COMPLETE: Init socket communication according to role
	if (server)
	{
		sock_fd = server_create(port);
		if (sock_fd < 0)
		{
			printf("Error creating server \n");
			exit(1);
		}
	}
	else
	{
		sock_fd = client_connect(ip_addr, port);
		if (sock_fd < 0)
		{
			printf("Error connecting to server \n");
			exit(1);
		}
	}

	// TO COMPLETE (part 2): Setup signal handler for forfeit
	signal(SIGTSTP, quitGame);

	// Run game according to role
	int userBoats = 3, advBoats = 3;
	char data = 0;
	int validInput = 0;
	char usrInput = 0;
	int firstIter = server ? 1 : -1;

	while (1)
	{
		// TO COMPLETE
		if (firstIter > 0)
		{
			printf("%s\n", strings[ATTACK_CELL]);

			while (validInput == 0)
			{
				usrInput = promptCell();
				if (advGrid[usrInput] == MISSED_SHOT || advGrid[usrInput] == SUNKEN_BOAT)
				{
					printf("%s \n", strings[INV_CELL_ATTACKED]);
				}
				else
				{
					validInput = 1;
				}
			}
			validInput = 0;

			//printf("%c\n", hexToChar(usrInput));
			if (socketSend(sock_fd, &usrInput, sizeof(usrInput)) < 0)
			{
				printf("Error sending data \n");
				exit(1);
			}

			puts(strings[OPPONENT_WAIT]);

			if (socketRecv(sock_fd, &data, sizeof(data)) < 0)
			{
				printf("Error receiving data \n");
				exit(1);
			}


			if(data == OPPONENT_FORFEIT){
				puts(strings[OPPONENT_FORFEIT]);
				forfeit = 1;
				break;
			}
			else if (data == HIT)
			{
				advGrid[usrInput] = SUNKEN_BOAT;
				advBoats--;
			}
			else if (data == MISS)
			{
				advGrid[usrInput] = MISSED_SHOT;
			}
			
			printf("%s %c: %s\n", strings[USER_ATTACK], hexToChar(usrInput), strings[data]);

			puts(strings[OPPONENT_GRID]);
			printBoard(advGrid);

			firstIter = -firstIter;
		}
		else
		{
			printf("%s\n", strings[OPPONENT_WAIT]);
			if (socketRecv(sock_fd, &data, sizeof(data)) < 0)
			{
				printf("Error receiving data \n");
				exit(1);
			}
			
			
			
			
			//printf("Data: %d\n", data);
			//Issue with the reception 	of data when SIGSTOP is called since we receive 0 and not 19. I don't know why
			if(data == OPPONENT_FORFEIT){
				
				forfeit = 1;
				break;
			}


			if(userGrid[data] == BOAT){
				userBoats--;
				userGrid[data] = SUNKEN_BOAT;
				usrInput = HIT;
			} else if(userGrid[data] == DEFAULT){
				userGrid[data] = MISSED_SHOT;
				usrInput = MISS;
			} 

			if (socketSend(sock_fd, &usrInput, sizeof(usrInput)) < 0)
			{
				printf("Error sending data \n");
				exit(1);
			}

			printf("%s %c: %s\n",strings[OPPONENT_ATTACK],  hexToChar(data), strings[usrInput]);
			printBoard(userGrid);
			firstIter = -firstIter;
		}
		if (userBoats == 0) {
            puts(strings[USER_LOST]);
            break;
        } else if (advBoats == 0) {
            puts(strings[USER_WON]);
            break;
        }
	}

	// TO COMPLETE (part 2): Check forfeit (out of game loop without all user or opponent's boats destroyed)
		//Doesn't work completely and I don't know why
		if(forfeit == 1){
			printf("\n%s : %s\n", strings[OPPONENT_FORFEIT], strings[USER_WON]);
		}
	// TO COMPLETE: Close socket file descriptor
		
	    if (close(sock_fd) < 0) {
        puts("socket close");
        return -1;
    }

	return 0;
}
