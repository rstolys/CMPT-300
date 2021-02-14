/*-----------------------------------------------------------------------------
 * shell.c -- Main program for assignment 1
 *
 * 7 Feb 2021	Ryan Stolys
 *
 -----------------------------------------------------------------------------*/

/***INCLUDES******************************************************************/
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pwd.h>


/***DEFINES*******************************************************************/
#define ERROR              -1
#define SUCCESS             0
#define FAILURE             1

#define COMMAND_LENGTH      1024
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)

#define HISTORY_LEN         10

//Supported Commnads 
#define EXIT_CMD            "exit"
#define PWD_CMD             "pwd"
#define CD_CMD              "cd"
#define HELP_CMD            "help"

#define HISTORY_CMD         "history"

//For debugging 
//#define DEBUG

/***GLOBAL VARIABLES**********************************************************/
extern int errno;       //Enables printing of errno messages

struct hist_element { int cmd_num; char cmd[COMMAND_LENGTH]; };        //Structure for history element in array


/***FUNCTION DECLARATIONS*****************************************************/
void myTokenize_command(char *buff, char *tokens[], _Bool *in_bkgnd);

void handleCmd_exit(char *tokens[]);

void handleCmd_pwd(char *tokens[]);

void handleCmd_cd(char *tokens[], char* prev_dir);

void handleCmd_help(char *tokens[]);

void handleCtrl_C();

int addCmdToHistory(int* numCmds, struct hist_element* hist, char *tokens[], char *buffer, _Bool *in_background);

void printHistory(int numCmds, struct hist_element* hist);

void getCommandString(char* cmd, char* tokens[], _Bool in_background);

void displayCmd_Prompt();

void initHistory(struct hist_element* hist);

void myWrite(int fd, char* buf);

void myWrite_errno(int fd);


/***FUNCTION DEFINITIONS******************************************************/


/*******************************************************************
** tokenize_command -- Tokenize the string in 'buff' into 'tokens'.
**
** @param[in]  buff     character array with string to tokenize 
**                      modified so all white space is '\0'
** @param[in]  tokens   array of pointers to i'th token in the sting buff
**                      All returned tokens are non-empty. Ends with null pointer
** @param[out] int      returns the number of tokens
**
********************************************************************/
int tokenize_command(char *buff, char *tokens[])
    {
    int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);

	for (int i = 0; i < num_chars; i++) 
        {
		switch (buff[i]) 
            {
            // Handle token delimiters (ends):
            case ' ':
            case '\t':
            case '\n':
                buff[i] = '\0';
                in_token = false;
                break;

            // Handle other characters (may be start)
            default:
                if (!in_token) 
                    {
                    tokens[token_count] = &buff[i];
                    token_count++;
                    in_token = true;
                    }
            }
	    }
	
    tokens[token_count] = NULL;
	return token_count;
    }


/*******************************************************************
** read_command -- Read a command from the keyboard into the buffer 'buff' and tokenize it
**                  such that 'tokens[i]' points into 'buff' to the i'th token in the command.
**
** @param[in]   buff            Buffer allocated by the calling code. 
**                              At least COMMAND_LENGTH bytes long.
** @param[out]  tokens[]        Array of character pointers which point into 'buff'. Must be at
**                              least NUM_TOKENS long. Will strip out up to one final '&' token.
**                              tokens will be NULL terminated (a NULL pointer indicates end of tokens).
** @param[out]  in_background   pointer to a boolean variable. Set to true if user entered
**                              an & as their last token; otherwise set to false.
**
********************************************************************/
void read_command(char *buff, char *tokens[], _Bool *in_background)
    {
    *in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

    //If read failed and the reason was not the ctrl-c signal
	if(length < 0 && errno != EINTR) 
        {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(ERROR);
        }
    else if(length < 0 && errno == EINTR)
        {
        buff[0] = '\0';         //Terminate the input string and allow new command
        }

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') 
        {
		buff[strlen(buff) - 1] = '\0';
	    }

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);

    //NOTE: Reversed logic from provided code to avoid return statement mid-way through code
	if (token_count != 0) 
        {
        // Extract if running in background:
		if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) 
            {
            *in_background = true;
            tokens[token_count - 1] = 0;
            }
	    }

    return;
    }


/*******************************************************************
** tokenize_command -- Custom implementation of tokenize command 
**                      to ensure the tokens are properly split
**
** @param[in]  buff         character array with string to tokenize 
**                          modified so all white space is '\0'
** @param[in]  tokens       array of pointers to i'th token in the sting buff
**                          All returned tokens are non-empty. Ends with null pointer
** @param[out]  in_bkgnd    pointer to a boolean variable. Set to true if user entered
**                          an & as their last token; otherwise set to false.
** @param[out] int          returns the number of tokens
**
********************************************************************/
void myTokenize_command(char *buff, char *tokens[], _Bool *in_bkgnd)
    {
    // Null terminate and strip \n.
	buff[strlen(buff) - 1] = '\0';

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);

	if(token_count != 0) 
        {
        // Extract if running in background:
		if(token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) 
            {
            *in_bkgnd = true;
            tokens[token_count - 1] = 0;
            }
	    }

    return;
    }

/*******************************************************************
** handleCmd_exit -- handle the exit command for the program
**
** @param[in]  tokens    the tokens of the user command
**
********************************************************************/
void handleCmd_exit(char *tokens[])
    {
    if(tokens[1] == NULL)
        {
        //Clean up any zombie processes that still exist
        while(waitpid(-1, NULL, WNOHANG) > 0) {/*Do nothing*/}

        //Terminate process
        exit(SUCCESS);
        }
    else 
        {
        myWrite(STDOUT_FILENO, "The 'exit' command does not support any arguments.\n");
        }

    return;
    }


/*******************************************************************
** handleCmd_pwd -- handle the print working directory command 
**
** @param[in]  tokens    the tokens of the user command
**
********************************************************************/
void handleCmd_pwd(char *tokens[])
    {
    if(tokens[1] == NULL)
        {
        char wd[COMMAND_LENGTH - 50];
        getcwd(wd, COMMAND_LENGTH - 50);
        char msg[COMMAND_LENGTH];
        sprintf(msg, "The current working directory is: %s\n", wd);

        myWrite(STDOUT_FILENO, msg);     //Print the message
        }
    else 
        {
        myWrite(STDOUT_FILENO, "The 'pwd' command does not support any arguments.\n");
        }
    
    return;
    }


/*******************************************************************
** handleCmd_cd -- will handle the change directory command
**
** @param[in]  tokens       the tokens of the user command
** @param[in]  prev_dir     the previous directory used
**
********************************************************************/
void handleCmd_cd(char *tokens[], char* prev_dir)
    {
    struct passwd* pword = getpwuid(getuid());

    char next_dir[COMMAND_LENGTH];

    _Bool changeDir = false;

    //Save the current_wd
    char cur_wd[COMMAND_LENGTH];    getcwd(cur_wd, COMMAND_LENGTH);

    //Determine the next directory
    if(tokens[1] == NULL)
        {
        changeDir = true;
        strcpy(next_dir, pword->pw_dir);        //Set next_dir to home directory
        }
    else if(tokens[2] == NULL)
        {
        if(tokens[1][0] == '~')
            {
            char dir[COMMAND_LENGTH];
            sprintf(dir, "%s%s", pword->pw_dir, &tokens[1][1]);

            changeDir = true;
            strcpy(next_dir, dir);
            }
        else if(tokens[1][0] == '-')
            {
            if(strcmp(prev_dir, "\0") == 0)
                {
                myWrite(STDOUT_FILENO, "You have no previous directory to go to.\n");
                }
            else
                {
                changeDir = true;
                strcpy(next_dir, prev_dir);
                }
            }
        else 
            {
            changeDir = true;
            strcpy(next_dir, tokens[1]);
            }
        }
    else 
        {
        myWrite(STDOUT_FILENO, "The 'cd' command only supports 1 argument. Your provided 2 or more.\n");
        }

    //If we need to change directories, do so
    if(changeDir)
        {
        if(chdir(next_dir) == -1)
            {
            myWrite_errno(STDOUT_FILENO);
            }
        else 
            {
            strcpy(prev_dir, cur_wd);       //Set the previous directory
            }
        }
    
    return;
    }


/*******************************************************************
** handleCmd_help -- will handle the help command for the program
**
** @param[in]  tokens    the tokens of the user command
**
********************************************************************/
void handleCmd_help(char *tokens[])
    {
    //Check for built in commands
    if(tokens[1] == NULL)
        {
        //List all the commands
        myWrite(STDOUT_FILENO, "The 'help' command requires one argument. You provided no argument. Supported commands are: 'exit', 'pwd' and 'cd'.\n");
        }
    else 
        {
        if(tokens[2] != NULL)
            {
            //Print error message
            myWrite(STDOUT_FILENO, "The 'help' command only supports one argument. You provided 2 or more.\n");
            }
        else if(strcmp(tokens[1], EXIT_CMD) == 0)
            {
            myWrite(STDOUT_FILENO, "'exit' is an builtin command for exitting the current shell program.\n");
            }
        else if(strcmp(tokens[1], PWD_CMD) == 0)
            {
            myWrite(STDOUT_FILENO, "'pwd' is an builtin command for displaying the current working directory.\n");
            }
        else if(strcmp(tokens[1], CD_CMD) == 0)
            {
            myWrite(STDOUT_FILENO, "'cd' is an builtin command for changing the current working directory.\n");
            }
        else if(strcmp(tokens[1], HELP_CMD) == 0)
            {
            myWrite(STDOUT_FILENO, "'help' is an builtin command for obtaining information about the supported commands.\n");
            }
        else 
            {
            char buf[COMMAND_LENGTH]; 
            sprintf(buf, "%s is an external command or application\n", tokens[1]);

            myWrite(STDOUT_FILENO, buf);
            }
        }
    return;
    }


/*******************************************************************
** handleCtrl_C -- will handle the ctrl-c signal
**
** @param[in]  none  
**
********************************************************************/
void handleCtrl_C()
    {
    myWrite(STDOUT_FILENO, "\n'help' is an builtin command for obtaining information about the supported commands.\n");

    //displayCmd_Prompt(); //-- does this need to be printed? Will be printed already on its own

    return;
    }

/*******************************************************************
** addCmdToHistory -- will add the commands to history 
**
** @param[in]  numCmds          the number of commands executed
** @param[in]  hist             pointer to the linked list of history commands
** @param[in]  tokens           the tokens of the user command
** @param[in]  in_background    boolean variable to indicate if task should run in background
**
********************************************************************/
int addCmdToHistory(int* numCmds, struct hist_element* hist, char *tokens[], char *buffer, _Bool *in_background)
    {
    int rc = 1; 
    _Bool addToHistory = true;

    if(tokens[0] != NULL)
        {
        //If there is an exclamation mark in the first position
        if(tokens[0][0] == '!')
            {
            //Get the command number
            int cmdNum = atoi(&tokens[0][1]);

            if(cmdNum == 0 && tokens[0][1] != '0' && tokens[0][1] != '!')
                {
                myWrite(STDOUT_FILENO, "The command number is not valid. It must be a number\n");
                rc = 0;         //We have handled command
                addToHistory = false;
                }
            else if(tokens[0][1] == '!')
                {
                myWrite(STDOUT_FILENO, hist[0].cmd);        //Show the user the command being run 

                strcpy(buffer, hist[0].cmd);                //Copy command to persistent buffer
                myTokenize_command(buffer, tokens, in_background);      //Set the tokens for the command

                rc = 1;         //Indicate the command has not been handled -- main will handle it
                }
            else 
                {
                if(cmdNum < (*numCmds) && cmdNum > (*numCmds) - (HISTORY_LEN + 1) && cmdNum > 0)
                    {
                    //Run the command specified -- (*numCmds) is one ahead of most recent command
                    myWrite(STDOUT_FILENO, hist[(*numCmds) - cmdNum - 1].cmd);                      //Show the user the command being run

                    strcpy(buffer, hist[(*numCmds) - cmdNum - 1].cmd);                      //Copy command to persistent buffer
                    myTokenize_command(buffer, tokens, in_background);      //Set the tokens for the command

                    rc = 1;         //Indicate the command has not been handled -- main will handle it
                    }
                else 
                    {
                    myWrite(STDOUT_FILENO, "The command number is out of the valid range.\n");
                    rc = 0;         //We have handled command
                    addToHistory = false;
                    }
                }
            }

        //Add command to history
        if(addToHistory)
            {
            //Loop through commands starting from the second to last element moving forward
            for(int c = HISTORY_LEN - 2; c >= 0; c--)
                {
                //If command is not empty
                if(hist[c].cmd_num != -1)
                    {
                    hist[c + 1] = hist[c];      //Move all the commands one position back
                    }
                }

            //Add the new command
            strcpy(hist[0].cmd, "\0");      //Reset the character array to empty
            getCommandString(hist[0].cmd, tokens, (*in_background));
            hist[0].cmd_num = (*numCmds)++;
            }

        //Check if command requested is the print command
        if(strcmp(tokens[0], HISTORY_CMD) == 0)
            {
            printHistory(*numCmds, hist);
            rc = 0;         //We have handled command
            }
        }
    else 
        {
        rc = 0;         //Ignore the empty command
        }
    
    return rc;
    }


/*******************************************************************
** printHistory -- will print the history of commands 
**
** @param[in]  numCmds      the number of commands executed
** @param[in]  hist         pointer to the linked list of history commands
**
********************************************************************/
void printHistory(int numCmds, struct hist_element* hist)
    {
    if(numCmds == 0)
        {
        myWrite(STDOUT_FILENO, "There is no history to show\n");
        }
    else 
        {
        //For the most recent 10 commands or until we run out of commands
        for(int c = 0; c < HISTORY_LEN && c < numCmds; c++)
            {
            char recentCmd[COMMAND_LENGTH + 10];

            sprintf(recentCmd, "%d\t%s", hist[c].cmd_num, hist[c].cmd);
            myWrite(STDOUT_FILENO, recentCmd);
            }
        }
   
    return;
    }


/*******************************************************************
** getCommandString -- will collect the command tokens into a single char array 
**
** @param[in]  cmd          character array for the command string
** @param[in]  tokens       the tokens of the user command
** @param[in]  in_bkgnd     indicates if the task is running in the background
**
********************************************************************/
void getCommandString(char* cmd, char* tokens[], _Bool in_bkgnd)
    {
    int i = 0;
    char token[COMMAND_LENGTH];

    while(tokens[i] != NULL) 
        {
        sprintf(token, "%s ", tokens[i]);       //Create one of the tokens in string format
        strcat(cmd, token);                     //Add it to the cmd

        i++;
        }

    //Add ampersand if in the background, gets removed in read_command
    if(in_bkgnd)
        {
        strcat(cmd, " &");
        }

    //Add new line character on end
    strcat(cmd, "\n");

    return;
    }

/*******************************************************************
** displayCmd_Prompt -- displays the command prompt in the console
**
** @param[in] none
**
********************************************************************/
void displayCmd_Prompt()
    {
    char cur_wd[COMMAND_LENGTH];
    char cmd_prompt[COMMAND_LENGTH + 2];
    getcwd(cur_wd, COMMAND_LENGTH);
    sprintf(cmd_prompt, "%s $ ", cur_wd);
    myWrite(STDOUT_FILENO, cmd_prompt);

    return;
    }


/*******************************************************************
** MAIN FUNCTION -- test program
**
** @param[in]  argc    number of command line arguements
** @param[in]  argv    array of command line arguements
**
********************************************************************/
int main(int argc, char **argv)
    {
    char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];

    //Setup the history array
    struct hist_element hist[HISTORY_LEN];      //Define the history array
    int cmd_num = 0;
    initHistory(hist);

    //Setup the user information for navigation
    char prev_dir[COMMAND_LENGTH];              //Define history character array for previous directory  
    prev_dir[0] = '\0';                         //Set the current previous directory to empty string

    //Set up the signal handler 
    struct sigaction handler; 
    handler.sa_handler = handleCtrl_C; 
    handler.sa_flags = 0; 
    sigemptyset(&handler.sa_mask); 
    sigaction(SIGINT, &handler, NULL);

    //Loop until termination
	while (true) 
        {
		// Print command prompt to screen to collect user command
        displayCmd_Prompt();

		_Bool in_background = false;

        // Call function to read the command from the input
		read_command(input_buffer, tokens, &in_background);

       
#ifdef DEBUG
		// DEBUG: Dump out arguments:
		for (int i = 0; tokens[i] != NULL; i++) 
            {
			write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));
			write(STDOUT_FILENO, "\n", strlen("\n"));
            }

		if (in_background) 
            {
			write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
		    }
#endif  /*DEBUG*/


        //If history returns 1 then the command still needs to be handled. 
        //      function will add command to the history is applicable
        if(addCmdToHistory(&cmd_num, hist, tokens, input_buffer, &in_background))
            {
            //Check if current command is one we can handle 
            if(strcmp(tokens[0], EXIT_CMD) == 0)
                {
                handleCmd_exit(tokens);
                }
            else if(strcmp(tokens[0], PWD_CMD) == 0)
                {
                handleCmd_pwd(tokens);
                }
            else if(strcmp(tokens[0], CD_CMD) == 0)
                {
                handleCmd_cd(tokens, prev_dir);
                }
            else if(strcmp(tokens[0], HELP_CMD) == 0)
                {
                handleCmd_help(tokens);
                }
            else    //Fork process to handle command
                {
                pid_t var_pid;      //Retrun value from fork used to identify the thread
                var_pid = fork();

                if(var_pid < 0)     //If error occured in fork
                    {
                    myWrite_errno(STDOUT_FILENO);
                    exit(ERROR);
                    }
                else if(var_pid == 0)       //Child process
                    {
                    if(execvp(tokens[0], tokens) == -1)
                        {
                        //Print errno message
                        myWrite_errno(STDOUT_FILENO);
                        exit(ERROR);        //Will exit from child process (hopefully)
                        }
                    }
                else        //Parent process
                    {
                    if(!in_background)
                        {
                        waitpid(var_pid, NULL, 0);         //Wait until child has completed
                        }
                    // else process will complete on its own, continue to accept next command
                    }
                }
            }

        //Clean up any zombie processes that still exist
        while(waitpid(-1, NULL, WNOHANG) > 0) {/*Do nothing*/}
	    }

    return SUCCESS;
    }


/**
 * Steps For Basic Shell:
 * 1. Fork a child process
 * 2. Child process invokes execvp() using results in token array.
 * 3. If in_background is false, parent waits for
 *    child to finish. Otherwise, parent loops back to
 *    read_command() again immediately.
 */



/*******************************************************************
** myWrite -- will print message to screen
**
** @param[in]  fd       file descriptor to write to
** @param[in]  buf      the character array to be printed
**
********************************************************************/
void myWrite(int fd, char* buf) 
    {
    write(fd, buf, strlen(buf));

    return;
    }


/*******************************************************************
** myWrite_errno -- will print message to screen
**
** @param[in]  fd       file descriptor to write to
** @param[in]  buf      the character array to be printed
**
********************************************************************/
void myWrite_errno(int fd)
    {
    char buf[100];

    sprintf(buf, "%s\t errno: %d\n", strerror(errno), errno);

    write(fd, buf, strlen(buf));

    return;
    }


/*******************************************************************
** initHistory -- will initalize the history array
**
** @param[in]  fd       file descriptor to write to
** @param[in]  buf      the character array to be printed
**
********************************************************************/
void initHistory(struct hist_element* hist)
    {
    for(int h = 0; h < HISTORY_LEN; h++)
        {
        hist[h].cmd_num = -1;
        strcpy(hist[h].cmd, "\0");
        }

    return;
    }
