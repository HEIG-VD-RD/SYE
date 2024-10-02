#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <syscall.h>
#include <pthread.h>
#include <string.h>
#include <ctype.h>


#define LETTERS_NB 26
#define A_UPPER_OFFSET 65
#define A_LOWER_OFFSET 97

typedef struct  {
    /* Pointer to position in the text */
	char *text_pointer;
    /* Size of the text chuck to process */
	size_t size;
    /* Letters counters, used to send the results back */
	size_t counters[LETTERS_NB];
} count_param_t;

/**
 * @brief Threads function used to count the letters occurence in a text
 */
void *count_letters(void *args) {
    count_param_t *params = (count_param_t *) args;

    //TO COMPLETE: Count each letter's number of occurences in the part of the text assigned to this thread.
    // Processing must be case-insensitive ('A' and 'a' both count as an occurence of A).
    // Store the results in the given argument structure

    for (size_t i = 0; i < params->size; ++i) {
        if (isalpha(params->text_pointer[i])) {
            params->counters[toupper(params->text_pointer[i]) - A_UPPER_OFFSET]++;
        }
    }


    return NULL;
}

/**
 * @brief Calculate the size of a file
 * 
 * @return The size of the file <fp>
 */
size_t fseek_filesize(FILE *fp) {
    long off;


    if (fseek(fp, 0, SEEK_END) == -1)
    {
        printf("failed to fseek\n");
        exit(EXIT_FAILURE);
    }

    off = ftell(fp);
    if (off == -1)
    {
        printf("failed to ftell\n");
        exit(EXIT_FAILURE);
    }

	fseek(fp, 0, SEEK_SET);

	return off;
}
/**
 * @brief Load a file content into a char buffer
 * 
 * @return A char *buffer with the content of the file <filename>
 */
char *buffer_from_file(char *filename) {

	char *buffer;
	size_t text_size = 0;
	FILE *f = fopen(filename, "r");

	if (f == NULL) {
        printf("failed to fopen %s\n", filename);
        exit(EXIT_FAILURE);
	}

	text_size = fseek_filesize(f);
	printf("text_size %d\n", text_size);

	buffer = malloc(text_size+1);

	if (buffer == NULL) {
        printf("failed to malloc() the buffer\n");
        exit(EXIT_FAILURE);
	} 

	/* Read the whole content into buffer */
	fread(buffer, 1, text_size, f);

	fclose(f);

	/* Add the \0 at the end */
	buffer[text_size] = 0;

	return buffer;
}

/**
 * @brief Main function
 */
int main(int argc, char **argv)
{

	int thread_num;
	size_t result_counters[LETTERS_NB] = { 0 };

	char *filename;


	if(argc != 3)
	{
		printf("Error: arguments number should be 3\n");
		printf("Usage: threads <nb_threads> <filename>\n");
		return EXIT_FAILURE;
	}

	thread_num = atoi(argv[1]);
	filename = argv[2];

	if(thread_num > 0)
	{
		if(thread_num > 15) thread_num = 15;
	}
	else
	{
		printf("Error: The number of threads must be between 0 and 15");
		return EXIT_FAILURE;
	}
    // TO COMPLETE: Load the text and its size

    char *buffer = buffer_from_file(filename);
    size_t fileSize = strlen(buffer);

    //printf("file size: %zu", fileSize);

    // TO COMPLETE: Configure and launch threads
    pthread_t *threads = (pthread_t *) calloc(thread_num, sizeof(pthread_t));

    if (threads == NULL) {
        printf("failed to calloc() the threads\n");
        free(buffer);
        exit(EXIT_FAILURE);
    }
    count_param_t *count_param = (count_param_t *) calloc(thread_num, sizeof(count_param_t));
    if (count_param == NULL) {
        printf("failed to calloc() the count parameters\n");
        free(buffer);
        free(threads);
        exit(EXIT_FAILURE);
    }

    size_t chunk = fileSize / thread_num;
    size_t leftOver = fileSize % thread_num;

    for (size_t i = 0; i < thread_num; i++) {
        count_param[i].text_pointer = buffer + i * chunk;
        count_param[i].size = chunk;

        if (i == (thread_num - 1)) {
            count_param[i].size += leftOver;
        }

		int ret = pthread_create(&threads[i], NULL, count_letters, (void *) &count_param[i]);

		if(ret){
			free(count_param);
			free(buffer);
			printf("Error: pthread_create() failed with errNum : %d ",ret);
			return EXIT_FAILURE;
		}

    }


		// TO COMPLETE: Join the threads and compile the results
		for (int i = 0; i < thread_num; i++) {
			pthread_join(threads[i], NULL);
		}

		for(size_t i = 0; i < thread_num; i++){
			for(size_t j = 0; j < LETTERS_NB; j++){
		result_counters[j] += count_param[i].counters[j];
		}
	}

    /* Display results */
	for (int i = 0; i < LETTERS_NB; ++i) {
		printf("Number of %c: %d\n", (char)(i+A_LOWER_OFFSET), result_counters[i]);
	}

	// TO COMPLETE: Free allocated memory

	free(buffer);
    free(threads);
    free(count_param);


	return EXIT_SUCCESS;
}

