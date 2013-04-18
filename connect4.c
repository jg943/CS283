#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <ctype.h>

int w;
void display(int ** Board,int rows, int columns);
int** build_board(int N);
int makeMove(int** Board, int player,int r[]);
int checkVictory(int** Board);
int checkHorr(int** Board);
void AI_move(int** Board,int player, int player2);
void freeBoard(int **Board);

//Function to check if the input is a number
int isnumeric(char *str)
{
  while(*str)
  {
    if(!isdigit(*str))
      return 0;
    str++;
  }

  return 1;
}

// Function to check if the Board is full
// Return 0 is not and 1 if it is full
int checkFull(int ** Board){
	int i, j;

	for(i=0; i<=w; i++){
		for(j=0; j<=w; j++){
			if(Board[i][j]==0)
				return 0;
		}
	}

	return 1;
}

int main(int argc, char* argv[]){
    
    srand((int) time(NULL));

	 if(argc < 3){
		 puts("Not enough args... Program will Now exit");
		 exit(-2);
	 }

	 if(!(isnumeric(argv[1]) && isnumeric(argv[2]))){
		 puts("One argument is not a number ... Program will exit");
		 exit(-2);
	 }

 	 int width= atoi(argv[2]);
    int i;
    int fd[atoi(argv[1])][2];
    pid_t pid;
    int readbuffer[2];
    int winCount=0;
    w=width-1;

    // Creates 2 pipes per child
	 for (i=0; i<atoi(argv[1]); i++) {
        pipe(fd[i]);
    }

	// Do the fork for the number of games requested Child just gives random
	// move parent takes care of the logical parts.
    for(i = 0; i < atoi(argv[1]); i++) {
        pid = fork();
        int r[2];
        int check=0;
		  int child_pid=0;
        srand(getpid());
		  int** Board=build_board(width);
       
		  if(pid < 0) {
            printf("Error");
            exit(1);
        } else if (pid == 0) {
            printf("Child (%d): %d\n", i + 1, getpid());
            child_pid=getpid();

            while(1){
            
            makeMove(Board, 2,r);
            
            close(fd[i][0]);
            
                if (write(fd[i][1], r, sizeof(r))!=sizeof(r))
                    puts("ERROR");
                sleep(.2);
            }
            exit(0);
        } else  {
            printf("Parent (%d): %d\n", i + 1, getpid());
            
            while (1) {
                
                if (check==0) {
            
                    close(fd[i][1]);
                test:read(fd[i][0], readbuffer, sizeof(readbuffer));
                    
							if(checkFull(Board)){
								kill(pid, SIGKILL);                                                                                                                                               
								puts("Board Full");
                        display(Board, width, width);                                                                                                                                     
                        freeBoard(Board);            
								break;
							}

                    if(Board[readbuffer[0]][readbuffer[1]]==0){
         				   printf("Child placed peice at: %d,%d\n", readbuffer[0],readbuffer[1]);
                        Board[readbuffer[0]][readbuffer[1]]=2;
						  }
                    else{
                        goto test;
                    }
                    
							if(checkFull(Board)){
								kill(pid, SIGKILL);                                                                                                                                               
								puts("Board Full");
                        display(Board, width, width);                                                                                                                                     
                        freeBoard(Board);            
								break;
							}
  
                  check=checkVictory(Board);
                    
                    if (check!=0) {
                        if(check==1){
                            puts("Winner Parent");
                            printf("Winning Parent (%d): %d\n", i + 1, getpid());
                        }
                        else{
                            puts("Winner Child");
                            printf("Winning Child (%d): %d\n", i + 1, child_pid);
                        }
                        
                        kill(pid, SIGKILL);
                        display(Board, width, width);
								freeBoard(Board);
                        winCount++;
                        break;
                    }

                    AI_move(Board,1,2);

			        
                    check=checkVictory(Board);
                    
                    if (check!=0) {
                        if(check==1){
                            puts("Winner Parent");
                            printf("Winning Parent (%d): %d\n", i + 1, getpid());
                        }
                        else{
                            puts("Winner Child");
                            printf("Winning Child (%d): %d\n", i + 1, child_pid);
                        }
                        
                        kill(pid, SIGKILL);
                        display(Board, width, width);
								freeBoard(Board);
                        winCount++;
                        break;
                    }

		  				if(checkFull(Board)){
								kill(pid, SIGKILL);                                                                                                                                               
								puts("Board Full");
                        display(Board, width, width);                                                                                                                                     
                        freeBoard(Board);            
								break;
					   }
            
                    
                }   
            }
            if (winCount==atoi(argv[1])) {
                break;
            }
           // exit(0);
        }
    }
    
    return 0;
}

// Function to free the Board that was created using malloc
void freeBoard(int** Board){

	int i;
	for (i = 0; i <= w; i++){
   	free(Board[i]);
	}	
	free(Board);
}

// Function to check for a victory on the connect four board
// Will return 1 for the parent winning, 2 for the child winning, or zero if
// there is no winner.
int checkVictory(int** Board){
    
    int i,j;
	//check horizontally
	for (i=0; i<=w; i++) {
        for (j=w; j>=0; j--) {
            if ( j > 2 && Board[i][j]==1 && Board[i][j-1]==1 && Board[i][j-2]==1 && Board[i][j-3]==1) {
                return 1;
            }
            if ( j > 2 && Board[i][j]==2 && Board[i][j-1]==2 && Board[i][j-2]==2 && Board[i][j-3]==2) {
                return 2;
            }
        }
    }
    
    //check vertically
    for (i=0; i<=w; i++) {
        for (j=w; j>=0; j--) {
            if ( i < (w-2) && Board[i][j]==1 && Board[i+1][j]==1 && Board[i+2][j]==1 && Board[i+3][j]==1) {
                return 1;
            }
            if ( i < (w-2) && Board[i][j]==2 && Board[i+1][j]==2 && Board[i+2][j]==2 && Board[i+3][j]==2) {
                return 2;
            }
        }
    }

    
    //check diagonally
    for (i=0; i<=w; i++) {
        for (j=w; j>=0; j--) {
            if (i < (w-2) && j > 2 && Board[i][j]==1 && Board[i+1][j-1]==1 && Board[i+2][j-2]==1 && Board[i+3][j-3]==1) {
                return 1;
            }
            if (i > 2 && j < 5 && Board[i][j]==1 && Board[i-1][j+1]==1 && Board[i-2][j+2]==1 && Board[i-3][j+3]==1) {
                return 1;
            }
        }
    }
    
    for (i=0; i<=w; i++) {
        for (j=w; j>=0; j--) {
            if (i < (w-2) && j > 2 && Board[i][j]==2 && Board[i+1][j-1]==2 && Board[i+2][j-2]==2 && Board[i+3][j-3]==2) {
                return 2;
            }
            if (i > 2 && j < 5 && Board[i][j]==2 && Board[i-1][j+1]==2 && Board[i-2][j+2]==2 && Board[i-3][j+3]==2) {
                return 2;
            }
        }
    }

    return 0;
}

// Function that will return a Board with the requried dimenson
// This function does this by using malloc
int** build_board(int N){
  
    int i,j;
    
    int **Board = (int**) malloc(N*sizeof(int*));
    for (i = 0; i < N; i++)
        Board[i] = (int*) malloc(N*sizeof(int));
    for (i = 0; i < N; i++) {
        for (j = 0; j < N; j++) {
            Board[i][j] = 0;
        }
    }
    
    return Board;
}

// AI for the parent
// Will return the move that it chose to take
// There are two ways this is accomplished
// 1.) Searches the board for a place where it would instantly win
// 2.) If no winning move found now find a move that blocks the child
// 3.) If the two above cases fail, just choose a random move
void AI_move(int**Board,int player, int player2){
    
    int i,j,a;
    
    for (j=0; j<=w; j++) {
        for (i=w; i>=0; i--) {
            // printf("I: %d\n", i);
            if ( j < w && Board[j][i]==0 && Board[j+1][i]!=0) {
                Board[j][i]=player;
                if(checkVictory(Board)==1){
                    printf("Parent placed peice at: %d,%d\n", j,i);
                    return;
                }
                else
                    Board[j][i]=0;
            }
        }
    }
    
    for (j=0; j<=w; j++) {
        for (i=w; i>=0; i--) {
            if ( j < w && Board[j][i]==0 && Board[j+1][i]!=0) {
                Board[j][i]=player2;
                if(checkVictory(Board)==2){
                    Board[j][i]=player;
                    return;
                }
                else
                    Board[j][i]=0;
            }
        }
    }
    
    start:a= rand()%(w+1);
    for (i=w; i>=0; i--) {
        if ((Board[i][a])==0) {
            Board[i][a]=player;
            printf("Parent placed peice at: %d,%d\n", i,a);
            return;
        }
    }
    
    goto start; // If you get here you did not find a move for that row go try again
}

// This is the AI for the child
// This just generates a random move and return that
int makeMove(int** Board,int player, int r[]){
    
    int a;
    start:a= rand()%(w+1);
    int i;
    for (i=w; i>=0; i--) {
        if ((Board[i][a])==0) {
            Board[i][a]=player;
            r[0]=i;
            r[1]=a;
            return 1;
        }
    }
    
    goto start;    
}

// Function to print the Board to stdout and replace the numbers I use with
// more logical characters R (red) B (blue) - (not used)
void display(int** Board,int rows, int columns){
    
    int i,j;
    
    for (i=0; i <= w;i++){
        for (j=0;j <=w;j++){
            if (Board[i][j]==1) {
                printf(" R ");
            }
            else if(Board[i][j]==2)
                printf(" B ");
            else
                printf(" - ");
        }
        printf("\n");
    }
    
}
