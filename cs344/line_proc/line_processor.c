#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

// gifforky

// Citation: used cs344 example consumer-producer program as a template.

// Maximum characters in an input line
#define MAX_CHARS 1000

// Max lines program can handle
#define MAX_LINES 49

// Size of the buffers
#define SIZE ((MAX_CHARS * MAX_LINES) + 2)

// Output line lengths
#define OUT_LEN 80

// For unused args warnings
#define UNUSED(x) (void)(x)

// Buffer 1, shared resource between input thread and next thread.
char buffer_1[SIZE];
// Thread 1 unshared processing buffer
char t1_buff[MAX_CHARS];
// Number of items in the buffer
int count_1 = 0;
// Index where the input thread will put the next item
int prod_idx_1 = 0;
// Index where the next thread will pick up the next item
int con_idx_1 = 0;
// Initialize the mutex for buffer 1
pthread_mutex_t mutex_1 = PTHREAD_MUTEX_INITIALIZER;
// Initialize the condition variable for buffer 1
pthread_cond_t go_1 = PTHREAD_COND_INITIALIZER;

// Buffer 2
char buffer_2[SIZE];
int count_2 = 0;
int prod_idx_2 = 0;
int con_idx_2 = 0;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t go_2 = PTHREAD_COND_INITIALIZER;

// Buffer 3
char buffer_3[SIZE];
int count_3 = 0;
int prod_idx_3 = 0;
int con_idx_3 = 0;
pthread_mutex_t mutex_3 = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t go_3 = PTHREAD_COND_INITIALIZER;

// Buffer 4 (controlled solely by prod 4)
char buffer_4[OUT_LEN + 1] = {0}; // +1 for NULL terminator.
int count_4 = 0;
int prod_idx_4 = 0;

int end_prog = 0;
int get_more_input = 1;


/*
Get input from stdin.
*/
char get_stdin_input(){
    char letter;
    letter = getchar();
    return letter;
}

// thread1 processing buffer(unshared)
int put_1_priv(char letter, int private_idx){
    t1_buff[private_idx] = letter;
    t1_buff[private_idx +1] = 0;
    
    return 0;
}


int t1_empty_b1()
{
    pthread_mutex_lock(&mutex_1);
    while (count_1 > 0)
    {
        pthread_mutex_unlock(&mutex_1);
        pthread_cond_signal(&go_1);
        pthread_cond_wait(&go_1, &mutex_1);
    }
    pthread_mutex_unlock(&mutex_1);   
    return 0;
}

/*
 Function that the input thread will run.
 Get input from stdin.
*/
void *get_input(void *args) // consumes stdin, producer 1 (T1)
{
    UNUSED(args);
    int private_idx = 0;
    prod_idx_1 = 0;
    char *p = buffer_1;
    char letter;


    // get, process, stack, signal

    // wait for consumer ready

    while (1)
    {
        // populating phase
        letter = get_stdin_input();
        put_1_priv(letter, private_idx);
        private_idx++;

        
        if (letter == '\n')
        {
            private_idx = 0;
            if (strcmp(t1_buff, "STOP\n") == 0)
            {
                t1_buff[0] = 3;
                t1_buff[1] = '\n';
                t1_buff[2] = 0;
                strcpy(p, t1_buff);
                count_1 += 2;
                break;
            }
            strcpy(p, t1_buff);
            pthread_mutex_lock(&mutex_1);
            count_1 += strlen(t1_buff);
            pthread_mutex_unlock(&mutex_1);
            p += strlen(t1_buff);
            *p = 0;
            pthread_cond_signal(&go_1);

        }


    }
    pthread_cond_signal(&go_1);
    // t1_empty_b1();
    return NULL;

}

/*
Get the next item from buffer 1
*/
char get_buff_1(){
    pthread_mutex_lock(&mutex_1);
    char letter = buffer_1[con_idx_1];
    // Increment the index from which the letter will be picked up
    con_idx_1 = con_idx_1 + 1;
    count_1--;
    // Unlock the mutex
    pthread_mutex_unlock(&mutex_1);


    // Return the letter
    return letter;
}

/*
 Put an item in buff_2
*/
int put_buff_2(char letter){
    pthread_mutex_lock(&mutex_2);
    buffer_2[prod_idx_2] = letter;

    // Increment the index where the next letter will be put.
    prod_idx_2 = prod_idx_2 + 1;
    buffer_2[prod_idx_2] = 0;
    count_2++;
    


    // Unlock the mutex
    pthread_mutex_unlock(&mutex_2);
    pthread_cond_signal(&go_2);
    
    return (prod_idx_2 - 1);
}


int t2_empty_b2()
{
    pthread_mutex_lock(&mutex_2);
    while (count_2 > 2)
    {
        pthread_mutex_unlock(&mutex_2);
        pthread_cond_signal(&go_2);
        pthread_cond_wait(&go_2, &mutex_2);

    }
    pthread_mutex_unlock(&mutex_2); 
    return 0;  
}

int t2_ask_b1()
{
    pthread_mutex_lock(&mutex_1);
    while (count_1 < 1)
    {
        pthread_mutex_unlock(&mutex_1);
        pthread_cond_signal(&go_1);
        pthread_cond_wait(&go_1, &mutex_1);

    }
    pthread_mutex_unlock(&mutex_1);   
    return 0;
}

void *check_newlines(void *args) // consumer 1 producer 2 (T2)
{
    UNUSED(args);
    char letter = 0;
    con_idx_1 = 0; // where i'm consuming from
    prod_idx_2 = 0; // where I'm sending to



    while (1)
    {

        // shove phase
        pthread_mutex_lock(&mutex_1);
        if (count_1 < 1)
            {pthread_mutex_unlock(&mutex_1);
            t2_ask_b1();}
        else
        {
            pthread_mutex_unlock(&mutex_1);
        }

        // shove phase
        pthread_mutex_lock(&mutex_1);

        
        while (count_1 > 0)
        {
            pthread_mutex_unlock(&mutex_1);
            letter = get_buff_1();
            if (letter == 3)
            {
                put_buff_2(3);
                pthread_mutex_lock(&mutex_1);
                count_1 = 0;
                pthread_mutex_unlock(&mutex_1);
                
                
                pthread_cond_signal(&go_1);
                pthread_cond_signal(&go_2);
                t2_empty_b2();
                return NULL;
            }
            if (letter == '\n')
            {
                put_buff_2(' ');
            }
            else
            {
                put_buff_2(letter);
            }
            pthread_mutex_lock(&mutex_1);
        }

        pthread_mutex_unlock(&mutex_1);

        

    }

    
    pthread_cond_signal(&go_1);
    pthread_cond_signal(&go_2);
    return NULL;
}

/*
Get the next item from buffer 2
*/
char get_buff_2(){

    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_2);
    char item = buffer_2[con_idx_2];
    // Increment the index from which the item will be picked up
    con_idx_2 = con_idx_2 + 1;
    count_2--;
    pthread_mutex_unlock(&mutex_2);
    pthread_cond_signal(&go_2);

    // Return the item
    return item;
}

int put_buff_3(char item)
{

    // Lock the mutex before putting the item in the buffer
    pthread_mutex_lock(&mutex_3);
    // Put the item in the buffer
    buffer_3[prod_idx_3] = item;
    // Increment the index where the next item will be put.
    prod_idx_3 = prod_idx_3 + 1;
    buffer_3[prod_idx_3] = 0;
    count_3++;

    // Signal to the consumer that the buffer is no longer empty.

    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
    pthread_cond_signal(&go_2);
    pthread_cond_signal(&go_3);


    return (prod_idx_3 - 1);
}


int t3_empty_b3()
{
    pthread_mutex_lock(&mutex_3);
    while (count_3 > 0)
    {
        pthread_mutex_unlock(&mutex_3);
        pthread_cond_signal(&go_3);
        pthread_cond_wait(&go_3, &mutex_3);

    }
    pthread_mutex_unlock(&mutex_3); 
    return 0;  
}

int t3_ask_b2()
{
    pthread_mutex_lock(&mutex_2);
    while (count_2 < 1)
    {
        pthread_mutex_unlock(&mutex_2);
        pthread_cond_signal(&go_2);
        pthread_cond_wait(&go_2, &mutex_2);

    }
    pthread_mutex_unlock(&mutex_2);   
    return 0;
}


void *check_plus_signs(void *args) // consumer 2 producer 3 (T3)
{
    UNUSED(args);
    char letter = 0;
    char space_1 = 0;
    char space_2 = 0;
    con_idx_2 = 0; // where i'm consuming from
    prod_idx_3 = 0; // where I'm sending to


    while (1)
    {
        
        // shove phase
        pthread_mutex_lock(&mutex_2);

        if (count_2 < 1)
            {pthread_mutex_unlock(&mutex_2);
            pthread_cond_signal(&go_2);
            t3_ask_b2();}
        else
        {
            pthread_mutex_unlock(&mutex_2);
        }
        pthread_cond_signal(&go_2);

        // shove phase
        pthread_mutex_lock(&mutex_2);


        while (count_2 > 0)
        {
            pthread_mutex_unlock(&mutex_2);
            pthread_cond_signal(&go_2);
            letter = get_buff_2();
            if (letter == 3)
            {
                if (space_2 != 0)
                {
                    put_buff_3(space_2);
                    
                }
                put_buff_3(letter);
                pthread_cond_signal(&go_2);
                pthread_cond_signal(&go_3);
                return NULL;
                
                
            }
            if (space_2 == 0)
            {
                space_2 = letter;
            }
            else
            {

                space_1 = space_2;
                space_2 = letter;


                if ((space_2 == '+') && (space_1 == '+'))
                {
                    put_buff_3('^');
                    space_1 = 0;
                    space_2 = 0;
                }

                else
                {
                    put_buff_3(space_1);
                }


            }
            pthread_mutex_lock(&mutex_2);
        }

        pthread_mutex_unlock(&mutex_2);
        pthread_cond_signal(&go_2);

    }

    pthread_cond_signal(&go_2);
    pthread_cond_signal(&go_3);
    return NULL;
}


int put_buff_4(char item)
{
    // Put the item in the buffer
    buffer_4[prod_idx_4] = item;
    buffer_4[prod_idx_4 + 1] = 0;
    // Increment the index where the next item will be put.
    prod_idx_4 = prod_idx_4 + 1;
    count_4++;

    return (prod_idx_4 - 1);
}

char get_buff_3(){


    // Lock the mutex before checking if the buffer has data
    pthread_mutex_lock(&mutex_3);

    char letter = buffer_3[con_idx_3];

    // Increment the index from which the letter will be picked up
    // if (letter == ' ')
    //     con_idx_3 = 0;
    count_3--;


    // Unlock the mutex
    pthread_mutex_unlock(&mutex_3);
    con_idx_3 = con_idx_3 + 1;
    pthread_cond_signal(&go_3);


    // Return the letter
    return letter;
}



int t4_ask_b3()
{
    pthread_mutex_lock(&mutex_3);
    while (count_3 < 1)
    {
        pthread_mutex_unlock(&mutex_3);
        pthread_cond_signal(&go_3);
        pthread_cond_wait(&go_3, &mutex_3);

    }
    pthread_mutex_unlock(&mutex_3);   
    return 0;
}



/*
 Function that the output thread will run. 
 Consume an item from the buffer shared with the square root thread.
 Print the item.
*/
void* write_output(void *args) // consumer 3
{
    UNUSED(args);
   
    char *p = buffer_3;

    while (1)
    {
        pthread_cond_signal(&go_3);
        pthread_mutex_lock(&mutex_3);
        if (count_3 < 1)
            {pthread_mutex_unlock(&mutex_3);
            pthread_cond_signal(&go_3);
            t4_ask_b3();}
        else
        {
            pthread_mutex_unlock(&mutex_3);
        }

        pthread_cond_signal(&go_3);
        pthread_mutex_lock(&mutex_3);

        while (count_3 > 0)
        {
            pthread_mutex_unlock(&mutex_3);
            pthread_cond_signal(&go_3);
            if (p[0] == 3)
                {
                    pthread_mutex_lock(&mutex_3);
                    count_3 = 0;
                    pthread_mutex_unlock(&mutex_3);
                    pthread_cond_signal(&go_3);
                    return NULL;
                }
            buffer_4[count_4] = p[0];
            pthread_mutex_lock(&mutex_3);
            count_3--;
            pthread_mutex_unlock(&mutex_3);
            count_4++;
            p++;
            pthread_cond_signal(&go_3);
            if (count_4 > (OUT_LEN - 1))
            {
                pthread_cond_signal(&go_3);
                printf("%s\n", buffer_4);
                fflush(stdout);
                count_4 = 0;
            }
            pthread_mutex_lock(&mutex_3);
        }
        pthread_mutex_unlock(&mutex_3);
        
    }

}

int main()
{
    int i = 0;
    i = i + 2;
    
    pthread_t input_t, newline_t, plus_sign_t, output_t;
    buffer_1[0] = 0;
    buffer_2[0] = 0;
    buffer_3[0] = 0;
    buffer_4[0] = 0;

    // get_input(buffer_1);
    // check_newlines(buffer_1);
    // check_plus_signs(buffer_1);
    // write_output(buffer_1);
    // Create the threads 
    pthread_create(&input_t, NULL, get_input, NULL);
    pthread_create(&newline_t, NULL, check_newlines, NULL);
    pthread_create(&plus_sign_t, NULL, check_plus_signs, NULL);
    pthread_create(&output_t, NULL, write_output, NULL);

    // Wait for the threads to terminate

    
    pthread_join(output_t, NULL);
    pthread_join(plus_sign_t, NULL);
    pthread_join(newline_t, NULL);
    pthread_join(input_t, NULL);

    // Mutex teardown
    pthread_mutex_destroy(&mutex_1);
    pthread_mutex_destroy(&mutex_2);
    pthread_mutex_destroy(&mutex_3);
    pthread_cond_destroy(&go_1);
    pthread_cond_destroy(&go_2);
    pthread_cond_destroy(&go_3);

    return 0;

}