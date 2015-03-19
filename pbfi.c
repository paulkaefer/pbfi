/*
 * pbfi.c
 *
 * Written by: Paul Kaefer
 * 3/16/2012
 *
 * Paul's BF interpretter.
 *
 * More information on the BF (Brainfuck) programming language
 * may be found at http://en.wikipedia.org/wiki/Brainfuck
 */

#include <stdio.h>  /* for printf, etc.      */
#include <ctype.h>  /* needed for getch      */
#include <string.h> /* for strlen() function */

#define ARRAY_LENGTH 1000

#define AFTER   1
#define BEFORE -1

struct tape
{
    struct tape *next;
    struct tape *prev;
    unsigned int bytes[ARRAY_LENGTH];
};

struct tape *t_malloc(struct tape *parent, int placement)
{
    struct tape *newtape;
    newtape = (struct tape *)malloc(sizeof(struct tape));
    if (newtape==NULL) {
        /* No memory was allocated. */
        printf("t_malloc was unable to allocate more memory.\n");
        return NULL;
    }//END IF
    if (placement == -1) 
    { // place the new tape BEFORE the old one
        parent->prev  = newtape;
        newtape->next = parent;
        newtape->prev = NULL;
    } 
    else
    { // place the new tape AFTER the old one
        parent->next  = newtape;
        newtape->prev = parent;
        newtape->next = NULL;
    }
    return newtape;
}//END p_malloc function

void cleartape(struct tape *tapeptr)
{
    int i = 0;
    for (i=0; i<ARRAY_LENGTH; i++)
    {
        tapeptr->bytes[i] = 0;
    }
}

void main(void)
{
    // TODO: dynamically allocate memory for the following array
    char program_string[100000];
    char ch = '0';
    char input_ch = '\n';
    int stop = 0;
    int i = 0;
    int pos = -1;
    int index = 0;

    for (i=0; i<1000; i++)
    {
        program_string[i] = EOF;
    }

    // store the BF code (minus extraneous characters) as a string:
    while ((ch = getchar()) != EOF)
    {
        //TODO: check if the program exceeds 1000 characters. Use malloc as needed.
        if ( ('+'==ch) || ('-'==ch) || ('>'==ch) || ('<'==ch) || ('['==ch) || (']'==ch) || ('.'==ch) || (','==ch) )
        {
            pos++;
            program_string[pos] = ch;
        }
        // TODO (optional): if encounters a '!' character, create a data string and change input from keyboard
        // to the data string. Change the "," case, below, accordingly.
    }

    // More information on strlen() may be found at:
    //      http://www.gnu.org/software/libc/manual/html_node/String-Length.html

    struct tape tape1;
    tape1.prev = NULL;
    tape1.next = NULL;
    cleartape(&tape1);

    struct tape *currtape = &tape1;
    
    // position in program
    pos = 0;

    //pid_t pid = fork();

    //if (pid != 0) {

    while (!stop)
    {
        ch = program_string[pos];
        if (ch == "!")
        {
            printf("\n'!' functionality is not available at this time.\n");
        }
        if (ch == '+')
        {
            currtape->bytes[index]++;
        }
        else if (ch == '-')
        {
            currtape->bytes[index]--;
        }
        else if (ch == '.')
        {
            printf("%c",currtape->bytes[index]);
        }
        else if (ch == ',')
        {
            printf("Enter a character: ");
            while ( (input_ch = getchar()) != '\n' )
            {
            }
            currtape->bytes[index] = input_ch;
        }
        else if (ch == '>')
        {
            //TODO: handle malloc forwards (if no next exists)
            // Do I have to free() in order?
            if ( (index == (ARRAY_LENGTH - 1)) && (currtape->next != NULL) )
            {
                currtape = currtape->next;
                index = 0;
            }
            else if ( (index == (ARRAY_LENGTH - 1)) && (currtape->next == NULL) )
            {
                currtape = t_malloc(currtape, AFTER);
                index = 0;
            }
            else
            {
                index++;
            }
        }// end '>' case
        else if (ch == '<')
        {
            //TODO: handle malloc backwards (if no prev exists)
            // *** if going backwards, set pos = ARRAY_LENGTH - 1
            // Do I have to free() in order?
            if ( (index == 0) && (currtape->prev != NULL) )
            {
                currtape = currtape->prev;
                index = (ARRAY_LENGTH - 1);
            }
            else if ( (index == 0) && (currtape->prev == NULL) )
            {
                currtape = t_malloc(currtape, BEFORE);
                index = (ARRAY_LENGTH - 1);
            }
            else
            {
                index--;
            }
        }// end '<' case
        else if (ch == '[')
        {
            //TODO: if *ptr!=0, continue. Else, find CORRESPONDING ]
            if (currtape->bytes[index] == 0)
            {
            // tape (array) value at this position is zero, so skip to the corresponding ']' character
                int found = 0;
                int temp_pos = pos;
                char temp_ch = 'x';
                int count = 0;// count of nested []-pairs
                while (!found)
                {
                    temp_pos++;
                    temp_ch = program_string[temp_pos];
                    if (temp_ch == '[')
                    {
                        count++;
                    }
                    else if (temp_ch == ']')
                    {
                        if (count==0)
                        {
                            found = 1;
                            pos = temp_pos;// the next command after the ']' will be executed next
                        }
                        else
                        {
                            count--;
                        }
                    }
                }// end while (!found)
            }// end if
        }// end '[' case
        else if (ch == ']')
        {
            //TODO: if *ptr==0, continue. Else, find CORRESPONDING [
            if (currtape->bytes[index] != 0)
            {
            // tape (array) value here is not zero, so go back to the beginning of this loop
            // GOING BACKWARDS THIS TIME!!!
                int found = 0;
                int temp_pos = pos;
                char temp_ch = 'x';
                int count = 0;// count of nested []-pairs
                while (!found)
                {
                    temp_pos--;
                    temp_ch = program_string[temp_pos];
                    if (temp_ch == ']')
                    {
                        count++;
                    }
                    else if (temp_ch == '[')
                    {
                        if (count == 0)
                        {
                            found = 1;
                            pos = temp_pos;
                        }
                        else
                        {
                            count--;
                        }
                    }
                }// end while (!found)
            }// end if
        }// end ']' case
        else if ((ch==EOF))// || (ch=='\n'))
        {
            // int value of EOF is -1 (even if cast to an unsigned int)
            stop = 1;
        }

        // increment the program counter
        pos++;

    }// END WHILE LOOP (goes through bf program code)

    //TODO: free() all malloc()'d memory

    if ( (tape1.prev==NULL) && (tape1.next==NULL) )
    {
        // no need to free() anything
    }
    else
    {
        if (tape1.prev != NULL)
        {
            currtape = tape1.prev;
            while (currtape->prev != NULL)
            {
                currtape = currtape->prev;
            }
            stop = 0;
            while (!stop)
            {
                currtape = currtape->next;
                free(currtape->prev);
                if (currtape == &tape1)
                {
                    stop = 1;
                }
            }
        } // end free()-ing prev tapes
        if (tape1.next != NULL)
        {
            currtape = tape1.next;
            while (currtape->next != NULL)
            {
                currtape = currtape->next;
            }
            stop = 0;
            while (!stop)
            {
                currtape = currtape->prev;
                free(currtape->next);
                if (currtape == &tape1)
                {
                    stop = 1;
                }
            }
        } // end free()-ing next tapes

    } // end if-else

    //END OF PROGRAM CODE

}// END MAIN
