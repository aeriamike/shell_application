// Shell starter file
// You may make any changes to any part of this file.

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

#define COMMAND_LENGTH 1028
#define NUM_TOKENS (COMMAND_LENGTH / 2 + 1)
#define HISTORY_DEPTH 10


/**
 * Command Input and Processing
 */

/*
 * Tokenize the string in 'buff' into 'tokens'.
 * buff: Character array containing string to tokenize.
 *       Will be modified: all whitespace replaced with '\0'
 * tokens: array of pointers of size at least COMMAND_LENGTH/2 + 1.
 *       Will be modified so tokens[i] points to the i'th token
 *       in the string buff. All returned tokens will be non-empty.
 *       NOTE: pointers in tokens[] will all point into buff!
 *       Ends with a null pointer.
 * returns: number of tokens.
 */

char *current;

char*temp;

char* cwd;
char history[HISTORY_DEPTH][COMMAND_LENGTH];

int num=0;


int tokenize_command(char *buff, char *tokens[])
{
	int token_count = 0;
	_Bool in_token = false;
	int num_chars = strnlen(buff, COMMAND_LENGTH);
	for (int i = 0; i < num_chars; i++) {
		switch (buff[i]) {
		// Handle token delimiters (ends):
		case ' ':
		case '\t':
		case '\n':
			buff[i] = '\0';
			in_token = false;
			break;

		// Handle other characters (may be start)
		default:
			if (!in_token) {
				tokens[token_count] = &buff[i];
				token_count++;
				in_token = true;
			}
		}
	}
	tokens[token_count] = NULL;
	

	return token_count;
}

/**
 * Read a command from the keyboard into the buffer 'buff' and tokenize it
 * such that 'tokens[i]' points into 'buff' to the i'th token in the command.
 * buff: Buffer allocated by the calling code. Must be at least
 *       COMMAND_LENGTH bytes long.
 * tokens[]: Array of character pointers which point into 'buff'. Must be at
 *       least NUM_TOKENS long. Will strip out up to one final '&' token.
 *       tokens will be NULL terminated (a NULL pointer indicates end of tokens).
 * in_background: pointer to a boolean variable. Set to true if user entered
 *       an & as their last token; otherwise set to false.
 */
void read_command(char *buff, char *tokens[], _Bool *in_background)
{
	*in_background = false;

	// Read input
	int length = read(STDIN_FILENO, buff, COMMAND_LENGTH-1);

	if (length < 0 && (errno !=EINTR)) {
		perror("Unable to read command from keyboard. Terminating.\n");
		exit(-1);
	}

	// Null terminate and strip \n.
	buff[length] = '\0';
	if (buff[strlen(buff) - 1] == '\n') {
		buff[strlen(buff) - 1] = '\0';
	}

	// Tokenize (saving original command string)
	int token_count = tokenize_command(buff, tokens);
	if (token_count == 0) {
		return;
	}

	// Extract if running in background:
	if (token_count > 0 && strcmp(tokens[token_count - 1], "&") == 0) {
		*in_background = true;
		tokens[token_count - 1] = 0;
	}
	
}


void pwd(){

	char pwd_address[128];				//set the space of address to 128 bytes
	cwd = getcwd(pwd_address,sizeof(pwd_address));	//get and print the the current address
	if(cwd == NULL)
	{
		printf("Error\n");
		exit(0);
	}
	
}

void cd(char *input){
	int flag = 0;
	current = malloc(sizeof(*current));
	current=(getcwd(NULL,0));
	//write(STDOUT_FILENO, "Directory changed to: ", strlen("directory changed to: "));
	if(input == NULL){
		chdir(getenv("HOME"));		//if there is no input, send the directory to home.
	}else if(input == ".."){
		
		chdir(getenv(".."));		//if the directory input is "..", get to the one top level directory using get ennvrionment
	
	}
	else{					//if there is something in the input, concatenate "/" after the current directory,
						//and then add the name of the next directory if it exists.
		
		temp = strcat(current, "/");// add the target directory after adding "/"
		input = strcat(temp, input);

		flag = chdir(input);	//check if the next directory exists

		if(flag == -1){	//if there is no such directory according to flag, print the error message.
			write(STDOUT_FILENO, "No directory found.", strlen("No directory found."));
			write(STDOUT_FILENO, "\n", strlen("\n"));
		}
	
	}
	
	free(current);
}
int c = 1;
void record(char* tokens[], char* num){

	int i = num;
	int a,b = 0;
	
	if (strcmp(tokens[0],"")!=0 && strlen(tokens[0])!=NULL){
		
		for(a=0;a<COMMAND_LENGTH;a++){
			if(i>9){
				
				i=i-10;		//restrict the range of history records to having only the last ten records
						//shown after the recent command
				memset(history[i],0,strlen(history[i]));
				for(c ; c<i; c++){
					free(history[c]);
					free(tokens[c]);
				}
				c++;
			}


			if(tokens[0][a]=='\0'){		//rearrange the order of history
				history[i][a]=' ';
				b++;		
			}
			else{
				
				history[i][a] = tokens[0][a];	//move the previous history away to have spaces for the new history
			}

	
			if(tokens[b]==NULL){
				break;
				}

		}
	}	

}



void printer(){
	
	int a = 1;
	
	if(num>9){				//restrict the range of history records to having only the last ten records
						//printed, including the "history" command which triggers this function
		a = num -9;

	}

	char string[a];
	int sizecommand = sizeof(history[(a-1)%10]);
	char string2[sizecommand];
	
	while(a<=num){		

		sprintf(string, "%d\t",a);
		write(STDOUT_FILENO, string, strlen(string));	//write the label number

	int j = 0;

	for(j=0; j<COMMAND_LENGTH;j++){

		string2[j]=history[(a-1)%10][j];	//rearrange the order of each historical commands
		}
	
			
		write(STDOUT_FILENO, string2, strlen(string2));	//print the last ten command of each order
		printf("\n");
		a++;
	}
	

}

void sigint_change(int a){
	write(STDOUT_FILENO, "\n", strlen("\n"));
	printer();			//change signal function to print history
	write(STDOUT_FILENO, current, strlen(current)); //reprint the current directory after the signal is executed
	write(STDOUT_FILENO, "> ", strlen("> "));	// as well as the ">" in the end

}

void histerror(){
	
		write(STDOUT_FILENO, "Error: unknown history command", strlen("Error: unknown history command"));
		write(STDOUT_FILENO, "\n", strlen("\n"));
			

}


/**
 * Main and Execute Commands
 */
int main(int argc, char* argv[])
{
	char input_buffer[COMMAND_LENGTH];
	char *tokens[NUM_TOKENS];
	

	
	while (true) {

		// Get command
		// Use write because we need to use read() to work with
		// signals, and read() is incompatible with printf().
		signal(SIGINT,sigint_change);
		current = getcwd(NULL,0);
		write(STDOUT_FILENO, current, strlen(current));
		write(STDOUT_FILENO, "> ", strlen("> "));
		_Bool in_background = false;
		read_command(input_buffer, tokens, &in_background);
		
		

		if(tokens[0] != NULL){
			
			
			
	
			if(strcmp(tokens[0], "!!")==0){			//run previous command
				if(num == 0){
					write(STDOUT_FILENO, "No history", strlen("No history"));
					write(STDOUT_FILENO, "\n", strlen("\n"));
				
				}
				else if(history[num-1] <0 ){
					histerror();
				}else{
					write(STDOUT_FILENO,history[num-1] , strlen(history[num-1]));
					write(STDOUT_FILENO, "\n", strlen("\n"));
					tokenize_command(history[(num-1)],tokens);
					

					}
				}


			else if(strchr(tokens[0], '!')){			//run historical command
				int histnum  = 0;
		histnum = atoi(&tokens[0][1]);
				
				if(histnum <=0 || histnum<num-9|| histnum>num){	// Error:command does not exist
					histerror();
					
				}else{
				

				write(STDOUT_FILENO, history[(histnum-1)%10] , strlen( history[(histnum-1)%10] ) );
				write(STDOUT_FILENO, "\n", strlen("\n"));
				tokenize_command(history[(histnum-1)%10],tokens);

				}
		
			}


				

			if(strcmp(tokens[0], "!!")!=0){		//if none of !! or !n applied, continue with history record set
				
				
				record(tokens, num);
				num++;
			
			}

			
			if(strcmp(tokens[0], "pwd") == 0){	//execute present current directory command if the first token is "pwd"
				
				pwd();
	
			}

			if(strcmp(tokens[0], "cd")==0){	//execute change directory command if the first token is "pwd"
				
				cd(tokens[1]);		//and bring the second token to the function
			
			}
			
			if(strcmp (tokens[0], "exit") == 0){	//exit program if the first token is "exit"
			
				return 0;
			}
			if(strcmp (tokens[0], "history")==0){	//print last ten commands if the first token is "history"
				
				printer();
			}

			

		}


		// DEBUG: Dump out arguments:
		// CHECK THE STUFF YOU JUST TYPED
		for (int i = 0; tokens[i] != NULL; i++) {
			//write(STDOUT_FILENO, "   Token: ", strlen("   Token: "));
			//write(STDOUT_FILENO, tokens[i], strlen(tokens[i]));

		}
		if (in_background) {
			write(STDOUT_FILENO, "Run in background.", strlen("Run in background."));
			write(STDOUT_FILENO, "\n", strlen("\n"));
				
		}
		


		/**
		 * Steps For Basic Shell:
		 * 1. Fork a child process
		 * 2. Child process invokes execvp() using results in token array.
		 * 3. If in_background is false, parent waits for
		 *    child to finish. Otherwise, parent loops back to
		 *    read_command() again immediately.
		 */
		
		//Doing fork() process
		//Strcutre of fork() references to lecture sildes on Process

		int final;
		pid_t child = fork();
		
		if(child<0){
			write(STDOUT_FILENO, "Fork() process failed", strlen("Fork process failed"));
			write(STDOUT_FILENO, "\n", strlen("\n"));
			exit(0);
			

		}
		else if (child==0){
			if(execvp(tokens[0],tokens)<0){	//execvp: command represents the command to be preformed
							//and params stores the parameters to this command
				if(strcmp(tokens[0], "cd")!=0 && strcmp(tokens[0], "history")!=0 ){

				write(STDOUT_FILENO, tokens[0], strlen(tokens[0]));
				write(STDOUT_FILENO, ": exec failed", strlen(" : exec failed"));
				write(STDOUT_FILENO, "\n", strlen("\n"));
				}
				}
			
			exit(-1);
			
		}
		else{
			
			if(!in_background){
			waitpid(child,&final,0);
			//write(STDOUT_FILENO, "Fork Success.", strlen("Fork Success."));
			//write(STDOUT_FILENO, "\n", strlen("\n"))
			}
			
		}
		
		//clean up zombie children with WNOHANG
		while(waitpid(-1,NULL,WNOHANG)>0){
			;
		}


	}
	return 0;
}
