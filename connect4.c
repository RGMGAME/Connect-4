#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
#include <windows.h>

//this has reached an acceptable state 22 hours in.

char grid[6][7];// the game board

int i,y,k; //for fors
int height[7]={0,0,0,0,0,0,0};//the height of each column
int * forbiddenmoves; // array where we store columns where the AI shouldn't play in
int sizeofforbiddenmoves=0;//size of the previous array so we know where to store the next value
char player1input;//player 1 input
char player2input;//player 1 input
int turnplayer=1;//depending on the turn player we put O or X
bool turn1good,turn2good;//checks input to check if the move a player makes is valid

bool win1=false;//if this is true player 1 wins
bool win2=false;//if this is true player 2 wins

bool moveisforbidden=false;//if we decide to play in a column and that column is inside the forbidden
//moves array, we flip this switch and the AI does not play in that column
int input;//input from the player, 1 for playing against humans, 2 against AI
int randommove;//the ai makes a move in a random column
bool aiturndone;//when this is true we found a move and we break the loop where we check every possible
//move
int aichoice;//this stores the column the AI chooses and prints it, in case the player
//missed it
int looptimes;//this guards versus a very specific scenario
bool nolegalmoves;//when the ai loses when all their moves are forbidden

int z=0;//debugging

int aiturn(){
	
	free(forbiddenmoves);/*we empty the forbidden moves array because after every turn
	the forbidden moves change. Maybe the player played in a forbidden column, so we can
	play there now, maybe the player created a new forbidden column that we need to 
	recalculate*/
	
	
	aiturndone=false;//this breaks the for outside of a for
	forbiddenmoves=malloc(sizeof(int));
	sizeofforbiddenmoves=0;
	/*
	Now I have to explain what the hell is goin on with the height and the line formula.
	
	for every column, the formula h+i=5 is true, where h is the height of the column and i
	is the line of the first free spot. For example:
	
	0  .
	1  .
	2  .
	3  .
	4  X
	5  O
	
	the first empty spot, the spot where the piece will go if we choose that column is for
	i=3, and the height of the column is h=2 pieces high, so h+i=5. Consider the following
	scenario:
	   0   1   2   3   4   5   6
	0  .   .   .   .   .   .   .
	1  .   .   .   .   .   .   .
	2  .   .   .   .   .   .   .
	3  .   .   .   O   .   .   .
	4  .   .   O   .   .   .   .
	5  .   O   .   .   .   .   .
	
	we will check the point with coordinates 5,1 for a reverse diagonal win. 
	the code is 
	
	if(grid[i][y]=='O' && grid[i-1][y+1]=='O' && grid[i-2][y+2]=='O'), or for i=5, y=1:
	
	if(grid[5][1]=='O' && grid[4][2]=='O' && grid[3][3]=='O')
	
	now we also need to check the height of column number y+3, in this scenario 4.
	
	the height of column 4 is h+i'=5, where i' is the line column 4 has to have the first
	empty space. i' has to be 2, and the height has to be 3 as you can see in the "picture",
	but how do we tell the computer to check if that is true?
	
	Our i is 5, since we are checking the point 5,1. i' is just i-3. So if we substitute:
	
	h+i'=5 <=> h+i-3=5 <=> h=8-i
	
	so the height of column number y+3 has to be 8-i for us to win, and so the condition
	the if statement has to meet is:
	
	if(grid[i][y]=='O' && grid[i-1][y+1]=='O' && grid[i-2][y+2]=='O' && h[y+3]==8-1)
	
	One last thing. The way items are stored in a two-dimensional array is a line. 
	Let's say we have this grid[2][2]:
	
	grid[0][0]=1
	grid[0][1]=2
	grid[1][0]=3
	grid[1][1]=4
	
	the representation is this:
	
	   0   1
	0  1   2
	1  3   4
	
	but the way they are stored is this: 1 2 3 4,
	so if I ask the computer: print me grid[0][2], it will print me the number stored after
	grid[0][1] which is the number 3, even though there is no column 2 in the grid. This created 
	situations where the game state:
	
	   0   1   2   3   4   5   6
	0  .   .   .   .   .   .   .
	1  .   .   .   .   .   .   .
	2  .   .   .   .   .   O   O
	3  O   O   .   .   .   .   .
	4  .   .   .   .   .   .   .
	5  .   .   .   .   .   .   .	
	
	would win, since the 'O' pieces are techically in a straight line as far as memory is concerned.
	
	also in game states like this:
	
	   0   1   2   3   4   5   6
	0  .   .   .   .   .   .   .
	1  .   .   .   .   .   .   .
	2  .   .   .   .   O   O   O
	3  .   .   .   .   .   .   .
	4  .   .   .   .   .   .   .
	5  .   .   .   .   .   .   .
	
	the computer tried to play in column y=7 so it can win by doing 4 in a line, when the 
	acceptable column numbers are 0-6.
	
	So after we check the pieces on the board and the height of the final column, we also check 
	if the line and column we are about to put the piece in is valid. So for example when
	we want to check for a line win, we shouldn't check column number 6, because there isn't a
	way to put pieces in columns 7,8, and 9 since those column do not exist! So in line wins we
	only check for y<4.
	
	*/
	//these two fors will check all the spaces on the board, and for each space we check all possible
	//ways to win or lose
	for(i=0;i<6;i++){
		for(y=0;y<7;y++){
			//first we check if we can win
			//line win
			if (grid[i][y]=='O' && grid[i][y+1]=='O' && grid[i][y+2]=='O' && height[y+3]==5-i  && y<4){
					/*
					O	O	O	.	
					if this condition is met, we will want to put our move at column y+3*/
					moveincolumn(y+3,2);
					aiturndone=true;
					break;
				}
			if (grid[i][y]=='O' && grid[i][y+1]=='O' && grid[i][y+3]=='O' && height[y+2]==5-i && y<4){
					/*
					O	O	.	O	
					*/
					moveincolumn(y+2,2);
					aiturndone=true;
					break;	
			}
			if (grid[i][y]=='O' && grid[i][y+2]=='O' && grid[i][y+3]=='O' && height[y+1]==5-i && y<4){	
					/*
					O	.	O	O	
					*/			
					moveincolumn(y+1,2);
					aiturndone=true;
					break;	
			}
			if (grid[i][y+1]=='O' && grid[i][y+2]=='O' && grid[i][y+3]=='O' && height[y]==5-i && y<4){
					/*
					.	O	O	O	
					*/	
					moveincolumn(y,2);
					aiturndone=true;
					break;
			}
			//column win		
			if (grid[i][y]=='O' && grid[i+1][y]=='O' && grid[i+2][y]=='O' && height[y]==6-i && height[y]!=6 && i<4){
				moveincolumn(y,2);
				aiturndone=true;
				break;
					
					
			}
			
			//reverse diagonal win
			if (grid[i][y]=='O' && grid[i-1][y+1]=='O' && grid[i-2][y+2]=='O' && height[y+3]==8-i && i>2 && y<4){
					/*
								.
							O
						O
					O		
					*/	
					moveincolumn(y+3,2);
					aiturndone=true;
					break;
				
			}
			if (grid[i][y]=='O' && grid[i-1][y+1]=='O' && grid[i-3][y+3]=='O' && height[y+2]==7-i && i>2 && y<4){
					/*
								O
							.
						O
					O		
					*/	
					moveincolumn(y+2,2);
					aiturndone=true;
					break;
				
			}
			if (grid[i][y]=='O' && grid[i-2][y+2]=='O' && grid[i-3][y+3]=='O' && height[y+1]==6-i && i>2 && y<4){
					/*
								O
							O
						.
					O		
					*/	
					moveincolumn(y+1,2);
					aiturndone=true;
					break;
				
			}
			if (grid[i-1][y+1]=='O' && grid[i-2][y+2]=='O' && grid[i-3][y+3]=='O' && height[y]==5-i && i>2 && y<4){
					/*
								O
							O
						O
					.		
					*/	
					moveincolumn(y,2);
					aiturndone=true;
					break;
				
			}
			//diagonal win
			if (grid[i][y]=='O' && grid[i+1][y+1]=='O' && grid[i+2][y+2]=='O' && height[y+3]==2-i && i<3 && y<4){	
					/*
					O
						O		
							O
								.
					*/	
					moveincolumn(y+3,2);
					aiturndone=true;
					break;		
			}
			if (grid[i][y]=='O' && grid[i+1][y+1]=='O' && grid[i+3][y+3]=='O' && height[y+2]==3-i && i<3 && y<4){	
					/*
					O
						O		
							.
								O
					*/	
					moveincolumn(y+2,2);
					aiturndone=true;
					break;		
			}
			if (grid[i][y]=='O' && grid[i+3][y+3]=='O' && grid[i+2][y+2]=='O' && height[y+1]==4-i && i<3 && y<4){	
					/*
					O
						.		
							O
								O
					*/	
					moveincolumn(y+1,2);
					aiturndone=true;
					break;		
			}
			if (grid[i+3][y+3]=='O' && grid[i+1][y+1]=='O' && grid[i+2][y+2]=='O' && height[y]==5-i && i<3 && y<4){	
					/*
					.
						O		
							O
								O
					*/	
					moveincolumn(y,2);
					aiturndone=true;
					break;		
			}
			//if we cannot win, we check the x's for a player 1 victory and prevent it

			//line win
			
			if (grid[i][y]=='X' && grid[i][y+1]=='X' && grid[i][y+2]=='X' && height[y+3]==5-i  && y<4){
				//this guards vs these game states:	X	X	X	!
				//I don't remember why this if statement is here but I am afraid to delete it
				if( y+3<0 || y+3>6 || height[y+3]>=6);
				else{
					
					moveincolumn(y+3,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
				
			}
			
			if (grid[i][y]=='X' && grid[i][y+1]=='X' && grid[i][y+3]=='X' && height[y+2]==5-i  && y<4){
				//this guards vs these game states:	X	X	!	X
				
				if( y+2<0 || y+2>6 || height[y+2]>=6);
				else{
					
					moveincolumn(y+2,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			
			if (grid[i][y]=='X' && grid[i][y+2]=='X' && grid[i][y+3]=='X' && height[y+1]==5-i  && y<4){
				//this guards vs these game states:	X	!	X	X
				if( y+1<0 || y+1>6 || height[y+1]>=6) ;
				else{
					
					moveincolumn(y+1,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i][y+2]=='X' && grid[i][y+1]=='X' && grid[i][y+3]=='X' && height[y]==5-i  && y<4){
				//this guards vs these game states:	!	X	X	X
				if( y<0 || y>6 || height[y]>=6) ;
				else{
				
					moveincolumn(y,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			
			//column win. 
			
			if (grid[i+3][y]=='X' && grid[i+1][y]=='X' && grid[i+2][y]=='X' && height[y]==5-i && i<3){

				if( y<0 || y>6 || height[y]>=6);
				else{
					
					moveincolumn(y,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			
			//reverse diagonal win
			if (grid[i][y]=='X' && grid[i-1][y+1]=='X' && grid[i-2][y+2]=='X' && height[y+3]==8-i && i>2 && y<4){
				/*this guards vs these game states:
				.  .  .  .  !
				.  .  .  X  .
				.  .  X  .  .
				.  X  .  .  .
				*/
				
				if( y+3<0 || y+3>6 || height[y+3]>=6) ;
				else{
					
					moveincolumn(y+3,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i][y]=='X' && grid[i-1][y+1]=='X' && grid[i-3][y+3]=='X' && height[y+2]==7-i && i>2 && y<4){
				/*this guards vs these game states:
				.  .  .  .  X
				.  .  .  !  .
				.  .  X  .  .
				.  X  .  .  .
				*/
				
				if( y+2<0 || y+2>6 || height[y+2]>=6) ;
				else{
					
					moveincolumn(y+2,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i][y]=='X' && grid[i-2][y+2]=='X' && grid[i-3][y+3]=='X' && height[y+1]==6-i && i>2 && y<4){
				/*this guards vs these game states:
				.  .  .  .  X
				.  .  .  X  .
				.  .  !  .  .
				.  X  .  .  .
				*/
				
				if( y+1<0 || y+1>6 || height[y+1]>=6) ;
				else{
					
					moveincolumn(y+1,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i-1][y+1]=='X' && grid[i-2][y+2]=='X' && grid[i-3][y+3]=='X' && height[y]==5-i && i>2 && y<4){
				/*this guards vs these game states:
				.  .  .  .  X
				.  .  .  X  .
				.  .  X  .  .
				.  !  .  .  .
				*/
				
				if( y<0 || y>6 || height[y]>=6) ;
				else{
					
					moveincolumn(y,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			//diagonal win
			if (grid[i][y]=='X' && grid[i+1][y+1]=='X' && grid[i+2][y+2]=='X' && height[y+3]==2-i && i<3 && y<4){
				/*
				X	.	.	.
				.	X	.	.
				.	.	X	.
				.	.	.	!
				*/
				if( y+3<0 || y+3>6 || height[y+3]>=6) ;
				else{
					
					moveincolumn(y+3,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i][y]=='X' && grid[i+1][y+1]=='X' && grid[i+3][y+3]=='X' && height[y+2]==3-i && i<3 && y<4){
				/*
				X	.	.	.
				.	X	.	.
				.	.	!	.
				.	.	.	X
				*/
				if( y+2<0 || y+2>6 || height[y+2]>=6);
				else{
					
					moveincolumn(y+2,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i][y]=='X' && grid[i+2][y+2]=='X' && grid[i+3][y+3]=='X' && height[y+1]==4-i && i<3 && y<4){
				/*
				X	.	.	.
				.	!	.	.
				.	.	X	.
				.	.	.	X
				*/
				if( y+1<0 || y+1>6 || height[y+1]>=6);
				else{
					
					moveincolumn(y+1,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if (grid[i+1][y+1]=='X' && grid[i+2][y+2]=='X' && grid[i+3][y+3]=='X' && height[y]==5-i && i<3 && y<4){
				/*
				!	.	.	.
				.	X	.	.
				.	.	X	.
				.	.	.	X
				*/
				if( y<0 || y>6 || height[y]>=6);
				else{
					
					moveincolumn(y,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			if(grid[i][y]=='X' && grid[i][y+1]=='X' && height[y+2]==5-i && y<4 && y!=0){
				//now if the player puts two x's next to each other with the front and back open,
				//they might be going for a game state like this .  X  X  X  .  , where no
				//matter where the ai puts an O they lose, so we stop the three in a row before
				//it happens
				if( y+2<0 || y+2>6 || height[y+2]>=6);
				else{
					
					moveincolumn(y+2,2);
					aiturndone=true;//this breaks the i for
					break;//this breaks the y for
				}
			}
			/*if we reached this point we are not in any immidiate danger
			there is one final way we could lose after this. Consider the following game state:
			    1  2  3  4  5
			1	.  .  .  .  .
			2	.  .  .  X  .
			3	.  .  X  ?  ?
			4	.  X  ?  ?  ?
			if the AI plays in column 5 then it will give the player a boost and they will win by
			playing 1,5. when we recognize this pattern we store 5 in an array called
			forbiddenmoves, and if we decide to make a move from now on, we will make sure the
			column we want to move in is not in the forbidden moves array


			1	2	3	4
						.
					X	f
				X
			X			 
			the following stores column 4 in the forbidden moves list
			*/
			if(grid[i][y]=='X' && grid[i-1][y+1]=='X' && grid[i-2][y+2]=='X' && height[y+3]==7-i && i>2 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+3;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
						X
					.
				X	f
			X		  
			*/
			if(grid[i][y]=='X' && grid[i-1][y+1]=='X' && grid[i-3][y+3]=='X' && height[y+2]==6-i && i>2 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+2;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
						X
					X
				.	
			X	f	  
			*/
			if(grid[i][y]=='X' && grid[i-2][y+2]=='X' && grid[i-3][y+3]=='X' && height[y+1]==5-i && i>2 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+1;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
						X
					X
				X	
			.	
			f	  
			*/
			if(grid[i-1][y+1]=='X' && grid[i-2][y+2]=='X' && grid[i-3][y+3]=='X' && height[y]==4-i && i>2 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
			X
			    X
			   		X 
						.  
						f
			*/
			if(grid[i][y]=='X' && grid[i+1][y+1]=='X' && grid[i+2][y+2]=='X' && height[y+3]==1-i && i<3 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+3;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
			X
			    X
			   		. 
					f	X  
						
			*/
			if(grid[i][y]=='X' && grid[i+1][y+1]=='X' && grid[i+3][y+3]=='X' && height[y+2]==2-i && i<3 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+2;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
			X
			    .
			   	f	X 
						X  
						
			*/
			if(grid[i][y]=='X' && grid[i+2][y+2]=='X' && grid[i+3][y+3]=='X' && height[y+1]==3-i && i<3 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+1;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			/*
			.
			f   X
			   		X 
						X  
						
			*/
			if(grid[i+1][y+1]=='X' && grid[i+2][y+2]=='X' && grid[i+3][y+3]=='X' && height[y]==4-i && i<3 && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			// X  X  X  .
			// .  .  .  f
			if(grid[i][y]=='X' && grid[i][y+1]=='X' && grid[i][y+2]=='X' && height[y+3]==4-i  && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+3;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}	
			//X  X  .  X
			//.  .  f  .
			if (grid[i][y]=='X' && grid[i][y+1]=='X' && grid[i][y+3]=='X' && height[y+2]==4-i  && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+2;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			//X  .  X  X
			//.  f  .  .
			if (grid[i][y]=='X' && grid[i][y+2]=='X' && grid[i][y+3]=='X' && height[y+1]==4-i  && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y+1;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			//.  X  X  X
			//f  .  .  .
			if (grid[i][y+2]=='X' && grid[i][y+1]=='X' && grid[i][y+3]=='X' && height[y]==4-i  && y<4){
				forbiddenmoves[sizeofforbiddenmoves]=y;//we add the forbidden column on the array
				sizeofforbiddenmoves++;//we add 1 to the size
				forbiddenmoves=realloc(forbiddenmoves,(sizeofforbiddenmoves+1)*sizeof(int));//and we realloc
				//the array to the new size+1, so it has space for the next
			}
			
			
			//now if we reached this line, we can't win, we are not in danger of losing, and we have 
			//calculated all the moves that make us lose, meaning that we can start trying to win.
			
			
					
		}
		
		if(aiturndone) break;//this breaks the i for prematurely if we picked a move
	}


	//if in this line aiturndone is false then we didnt do any moves so we go in the if statement
	
	if(!aiturndone){
			/*
			
			we can now play moves that make favorable game states
			these next two put moves in this pattern:
			.  .  .
			.  .  O
			.  O  .
			so we can win reverse diagonally more easily
			
			*/
	for(i=0;i<6;i++){
		for(y=0;y<7;y++){
			//reverse diagonally with 1 ally square
			if( grid[i][y]=='O' && grid[i-1][y+1]=='.' && grid[i-2][y+2]=='.' && grid[i-3][y+3]=='.' && height[y+1]==6-i && i>2 && y<4){
				//we reset moveisforbidden
				moveisforbidden=false;
				//we check if move is forbidden
				for(k=0;k<sizeofforbiddenmoves;k++){
					if(forbiddenmoves[k]==y+1){
						moveisforbidden=true;
						break;
					}
				}
				
				if(moveisforbidden);//if it is forbidden we do nothing
				else{
					
					moveincolumn(y+1,2);
					aiturndone=true;
					break;
				}
			}

			//normal diagonal
			if(grid[i][y]=='O' && grid[i+1][y+1]=='.' && grid[i+2][y+2]=='.' && grid[i+3][y+3]=='.' && height[y+1]==4-i  && i<3 && y<4){
				moveisforbidden=false;
				for(k=0;k<sizeofforbiddenmoves;k++){
					if(forbiddenmoves[k]==y-1){
						moveisforbidden=true;
						break;
					}
				}
				if(moveisforbidden);
				else{
					
					moveincolumn(y+1,2);

					aiturndone=true;
					break;
				}
				
			}

			//line
			if(grid[i][y]=='O' && grid[i][y+1]=='.' && grid[i][y+2]=='.' && grid[i][y+3]=='.' && height[y+1]==5-i && y<4){
				moveisforbidden=false;
				for(k=0;k<sizeofforbiddenmoves;k++){
					if(forbiddenmoves[k]==y+1){
						moveisforbidden=true;
						break;
					}
				}
				if(moveisforbidden);
				else{
				
					moveincolumn(y+1,2);

					aiturndone=true;
					break;
				}
				
			}

			//column last because it is the easiest for the player to see and prevent
			if(grid[i][y]=='O' && grid[i+1][y]=='.' && grid[i+2][y]=='.' && grid[i+3][y]=='.' && height[y]==6-i && i>2){
				moveisforbidden=false;
				for(k=0;k<sizeofforbiddenmoves;k++){
					if(forbiddenmoves[k]==y){
						moveisforbidden=true;
						break;
					}
				}
				if(moveisforbidden);
				else{
					
					moveincolumn(y,2);

					aiturndone=true;
					break;
				}
				
			}

		}
		if(aiturndone)break;
	}
	}
	//	if stil nothing is picked, we pick randomly
		if(!aiturndone){
		
			
		looptimes=0;
		
		do{
			
			randommove=rand()%7;
			moveisforbidden=false;
			for(k=0;k<sizeofforbiddenmoves;k++){
				if(forbiddenmoves[k]==randommove){
					moveisforbidden=true;
					break;
				}
			}
			//if the only legal moves are forbidden ones, the AI will never leave this loop, so
			//after it checks for a valid move 100 times and it doesnt find one, it means that
			//the player won
			looptimes++;
			if (looptimes==100){
				
				nolegalmoves=true;
				win1=true;
				break;
			}
			
			
		}while(moveisforbidden || (randommove==0 && height[0]>5) || (randommove==1 && height[1]>5) ||
		(randommove==2 && height[2]>5) || (randommove==3 && height[3]>5) || (randommove==4 && height[4]>5) ||
		(randommove==5 && height[5]>5) || (randommove==6 && height[6]>5));
		//we do this while a move is forbidden, or while the height of the column we chose is full
		moveincolumn(randommove,2);


		aiturndone=true;
	}

}

int gamevsplayer(){
	while(1){
		system("cls");
		//print the board
		for(i=0;i<7;i++) printf(" %d  ",i);
		printf("\n\n");
		for(i=0;i<6;i++){
			printf("\n");
			for(y=0;y<7;y++){
				printf(" %c  ",grid[i][y]);
			}	
		}
		
		checkwin();//this checks the entire board and sees if there are any 4 in a rows,
		//and switches win1 or win2 on if anyone won
		if (win1 || win2) break;//if someone won we break
		//gets input from first player, checks it, inserts it in the move function
		turn1good=false;//this checks inputs
		turnplayer=1;
		printf("\n\nTurn Player: Player %d (X)\n",turnplayer);
		while(!turn1good){
			
		while (scanf("%d",&player1input) != 1){
			//if the scanf doesn't return 1 then an integer was not inputed.
            while (getchar() != '\n');
            printf("Invalid input. Please enter an integer from 0 to 6.\n");
            
        }

		if(player1input<0 || player1input>6) printf("Invalid Input, enter a value from 0 to 6\n");
		else if(player1input==0 && height[0]>5) printf("Invalid Input, column 0 is full\n");
		else if(player1input==1 && height[1]>5) printf("Invalid Input, column 1 is full\n");
		else if(player1input==2 && height[2]>5) printf("Invalid Input, column 2 is full\n");
		else if(player1input==3 && height[3]>5) printf("Invalid Input, column 3 is full\n");
		else if(player1input==4 && height[4]>5) printf("Invalid Input, column 4 is full\n");
		else if(player1input==5 && height[5]>5) printf("Invalid Input, column 5 is full\n");
		else if(player1input==6 && height[6]>5) printf("Invalid Input, column 6 is full\n");
		else{
			moveincolumn(player1input,turnplayer);
			turn1good=true;
		}
		}
		
		system("cls");
		//prints new board, after player 1 plays
		for(i=0;i<7;i++) printf(" %d  ",i);
		printf("\n\n");
		for(i=0;i<6;i++){
			printf("\n");
			for(y=0;y<7;y++){
				printf(" %c  ",grid[i][y]);
			}	
		}
		checkwin();
		if (win1 || win2) break;
		//gets input from second player, checks it, inserts it in the move function
		turn2good=false;
		turnplayer=2;
		printf("\n\nTurn Player: Player %d (O)\n",turnplayer);
		while(!turn2good){
		while (scanf("%d",&player2input) != 1){
            while (getchar() != '\n');
            printf("Invalid input. Please enter an integer from 0 to 6.\n");
        }
		if(player2input<0 || player2input>6) printf("Invalid Input, enter a value from 0 to 6\n");
		else if(player2input==0 && height[0]>5) printf("Invalid Input, column 0 is full\n");
		else if(player2input==1 && height[1]>5) printf("Invalid Input, column 1 is full\n");
		else if(player2input==2 && height[2]>5) printf("Invalid Input, column 2 is full\n");
		else if(player2input==3 && height[3]>5) printf("Invalid Input, column 3 is full\n");
		else if(player2input==4 && height[4]>5) printf("Invalid Input, column 4 is full\n");
		else if(player2input==5 && height[5]>5) printf("Invalid Input, column 5 is full\n");
		else if(player2input==6 && height[6]>5) printf("Invalid Input, column 6 is full\n");
		else{
			moveincolumn(player2input,turnplayer);
			turn2good=true;
		}
		}
		
		}

		
	
	
	if(win1) printf("\nPlayer 1 Wins!");
	if(win2) printf("\nPlayer 2 Wins!");
}
int gamevsai(){
	system("cls");
		//print the board

		for(i=0;i<7;i++) printf(" %d  ",i);

		printf("\n\n");
		for(i=0;i<6;i++){
			printf("\n");
			for(y=0;y<7;y++){
				printf(" %c  ",grid[i][y]);
			}	
		}
		
		printf("\n\n");
	while(1){
		
		
		
		
		//gets input from first player, checks it, inserts it in the move function
		turn1good=false;
		turnplayer=1;
		
		printf("\n\nTurn Player: Player %d (X)\n",turnplayer);
		while(!turn1good){
		while (scanf("%d",&player1input) != 1){
            while (getchar() != '\n');
            printf("Invalid input. Please enter an integer from 0 to 6.\n");
            
        }
		
		if(player1input<0 || player1input>6) printf("Invalid Input, enter a value from 0 to 6\n");
		else if(player1input==0 && height[0]>5) printf("Invalid Input, column 0 is full\n");
		else if(player1input==1 && height[1]>5) printf("Invalid Input, column 1 is full\n");
		else if(player1input==2 && height[2]>5) printf("Invalid Input, column 2 is full\n");
		else if(player1input==3 && height[3]>5) printf("Invalid Input, column 3 is full\n");
		else if(player1input==4 && height[4]>5) printf("Invalid Input, column 4 is full\n");
		else if(player1input==5 && height[5]>5) printf("Invalid Input, column 5 is full\n");
		else if(player1input==6 && height[6]>5) printf("Invalid Input, column 6 is full\n");
		else{
			
			moveincolumn(player1input,turnplayer);
			turn1good=true;
			
		}
		}
		system("cls");
		//print the board

		for(i=0;i<7;i++) printf(" %d  ",i);
		printf("\n\n");
		for(i=0;i<6;i++){
			printf("\n");
			for(y=0;y<7;y++){
				printf(" %c  ",grid[i][y]);
			}	
		}
		printf("\n\n");
		checkwin();
		if (win1 || win2) break;
		//////////////////////
		turnplayer=2;
		sleep(1);
		aiturn();
		
		
		system("cls");
		


		for(i=0;i<7;i++) printf(" %d  ",i);
		printf("\n\n");
		for(i=0;i<6;i++){
			printf("\n");
			for(y=0;y<7;y++){
				printf(" %c  ",grid[i][y]);
			}	
		}
		printf("\n\n");
		if(!nolegalmoves)printf("\nCPU played in column %d\n",aichoice);
		else ("\nThere are no ways the CPU can win from this point on.\n");
		
		checkwin();
		if (win1 || win2) break;
		

	}


	if(win1) printf("\nPlayer 1 Wins!");
	if(win2) printf("\nCPU Wins!");
	
	
	
}
int checkwin(){
	
	for(i=0;i<6;i++){
		for(y=0;y<7;y++){
			//check the x's for a player 1 victory
			//line win
			if (grid[i][y]=='X' && grid[i][y+1]=='X' && grid[i][y+2]=='X' && grid[i][y+3]=='X' && y<4){
				win1=true;
				printf("line win starting from i=%d y=%d\n",i,y);
				break;
			}
			//column win		
			else if (grid[i][y]=='X' && grid[i+1][y]=='X' && grid[i+2][y]=='X' && grid[i+3][y]=='X' && i<3){
				win1=true;
				printf("column win starting from i=%d y=%d\n",i,y);
				break;
			}
			
			//reverse diagonal win
			else if (grid[i][y]=='X' && grid[i-1][y+1]=='X' && grid[i-2][y+2]=='X' && grid[i-3][y+3]=='X' && i>2 && y<4){
				win1=true;
					printf("reverse diagonal win starting from i=%d y=%d\n",i,y);
				break;
			}
			//diagonal win
			else if (grid[i][y]=='X' && grid[i+1][y+1]=='X' && grid[i+2][y+2]=='X' && grid[i+3][y+3]=='X' && i<3 && y<4){
				printf("diagonal win starting from i=%d y=%d\n",i,y);
				win1=true;
				break;
			}
			
			
			//check the o's for a player 2 victory
			//line win
			if (grid[i][y]=='O' && grid[i][y+1]=='O' && grid[i][y+2]=='O' && grid[i][y+3]=='O' && y<4){
				win2=true;
					printf("line win starting from i=%d y=%d\n",i,y);
				break;
			}
			//column win		
			else if (grid[i][y]=='O' && grid[i+1][y]=='O' && grid[i+2][y]=='O' && grid[i+3][y]=='O' && i<3){
				win2=true;
				printf("column win starting from i=%d y=%d\n",i,y);
				break;
			}
			
			//reverse diagonal win
			else if (grid[i][y]=='O' && grid[i-1][y+1]=='O' && grid[i-2][y+2]=='O' && grid[i-3][y+3]=='O' && i>2 && y<4){
				win2=true;
				printf("reverse diagonal win starting from i=%d y=%d\n",i,y);
				break;
			}
			//diagonal win
			else if (grid[i][y]=='O' && grid[i+1][y+1]=='O' && grid[i+2][y+2]=='O' && grid[i+3][y+3]=='O' && i<3 && y<4){
				win2=true;
				printf("diagonal win starting from i=%d y=%d\n",i,y);
				break;
			}
				
			
		}
	}
	
	
}
int moveincolumn(int column,int turnplayer){
	
	/*
	This function takes two arguments, the column where the player wants to play, and 
	which player plays. Depending on which player plays, we put an x or an o, and then
	we set the first empty piece as an x or an o, and we add 1 to the height of the 
	column
	*/
	
	printf("column picked: %d\n",column);
	aichoice=column;
	while(1){
		if(turnplayer==1){
		
	if (column==0){
		//y=0, and as we have said, h+i=5, so i=5-h
		grid[5-height[0]][0]='X';
		height[0]++;
		break;
	}
	else if (column==1){
		grid[5-height[1]][1]='X';
		height[1]++;
		break;
	}
	else if (column==2){
		grid[5-height[2]][2]='X';
		height[2]++;
		break;
	}
	else if (column==3){
		grid[5-height[3]][3]='X';
		height[3]++;
		break;
	}
	else if (column==4){
		grid[5-height[4]][4]='X';
		height[4]++;
		break;
	}
	else if (column==5){
		grid[5-height[5]][5]='X';
		height[5]++;
		break;
	}
	else if (column==6){
		grid[5-height[6]][6]='X';
		height[6]++;
		break;
	}
	
	}
	
	else if(turnplayer==2){
	if (column==0){
		grid[5-height[0]][0]='O';
		height[0]++;
		break;
	}
	else if (column==1){
		grid[5-height[1]][1]='O';
		height[1]++;
		break;
	}
	else if (column==2){
		
		grid[5-height[2]][2]='O';
		height[2]++;
		break;
	}
	else if (column==3){
		grid[5-height[3]][3]='O';
		height[3]++;
		break;
	}
	else if (column==4){
		grid[5-height[4]][4]='O';
		height[4]++;
		break;
		
	}
	else if (column==5){
		grid[5-height[5]][5]='O';
		height[5]++;
		break;
	}
	else if (column==6){
		grid[5-height[6]][6]='O';
		height[6]++;
		break;
	}
	
		
	}
	}
	

}
int main(){
	time_t t; 
	
	srand(time(&t));
	

	//sets the board
	for(i=0;i<6;i++){
		for(y=0;y<7;y++){
			grid[i][y]='.';
		}
	}

	
		printf("Welcome to Connect 4. Do you want to play versus the CPU or another player?\n"
		"1. Vs another player\n2. Vs CPU");
		
		while(1){		
			while(scanf("%d",&input)!=1 || (input!=1 && input!=2)){
				while(getchar()!='\n');
				printf("Please enter 1 or 2\n");
			}
			break;
		}
	//if we are here either 1 or 2 has been inputed
	if(input==1) gamevsplayer();
	if(input==2) gamevsai();
	
	
}
