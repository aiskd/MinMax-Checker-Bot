/* Program to print and play checker games.*/

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <assert.h>
#include <string.h>

/* some #define's from my sample solution ------------------------------------*/
#define BOARD_SIZE          8       // board size
#define ROWS_WITH_PIECES    3       // number of initial rows with pieces
#define CELL_EMPTY          '.'     // empty cell character
#define CELL_BPIECE         'b'     // black piece character
#define CELL_WPIECE         'w'     // white piece character
#define CELL_BTOWER         'B'     // black tower character
#define CELL_WTOWER         'W'     // white tower character
#define COST_PIECE          1       // one piece cost
#define COST_TOWER          3       // one tower cost
#define TREE_DEPTH          3       // minimax tree depth
#define COMP_ACTIONS        10      // number of computed actions




/* my #define's -------------------------------------------------------------*/
#define EMPTY_ROWINDEX1      3
#define EMPTY_ROWINDEX2      4
#define TOTAL_PIECES         12
#define TOTAL_CELLS          65
#define LENGTH_OF_CELL       3
#define MAX_MOVES            4
#define ASCII_A              64
#define NEXT_SPACE           1
#define ERROR_1              1
#define ERROR_2              2
#define ERROR_3              3
#define ERROR_4              4
#define ERROR_5              5
#define ERROR_6              6
#define INPUT_LENGTH         6
#define JUMP_SPACE           2
#define END_BROW             0
#define END_WROW             7
#define LOW_NUM              1
#define HIGH_NUM             8
#define ACTION_A             1
#define LINE_A               'A'
#define LINE_P               'P'
#define LINE_LENGTH          1

/* my typedefs --------------------------------------------------*/
typedef char board_t[BOARD_SIZE][BOARD_SIZE];  // board type
typedef struct{
    char src_letter;
    int src_num;
    int src_row;
    int src_col;
    char tgt_letter;
    int tgt_num;
    int tgt_row;
    int tgt_col;
}moves_t;

typedef struct node {
    board_t board;
    char str_move[INPUT_LENGTH];
    int cost;
    int depth;
    struct node *parent;
    struct node *sibling;
    struct node *child;
} node_t;


/*Function Prototypes----------------------------------------------*/

void print_board(board_t pos);
void print_letters();
void board_horizontal();
void fill_pieces(board_t pos);
void fill_row(int *last_piece, char piece_colour, int row_num, board_t pos);
void board_info();
void horizontal_line();
int stage0(board_t piece_pos, int *num_actions, int *black,int *last_action);
int check_errors(moves_t cells,board_t piece_pos,int move_num);
int check_range(char letter, int num);
int check_empty(board_t piece_pos,int row, int col);
void change_pos(board_t piece_pos,moves_t cells);
int check_opponent(int move_num,int row,int col,board_t piece_pos);
int check_illegal(board_t piece_pos, moves_t cells);
int possible_moves(board_t piece_pos,char *move,moves_t A[],int black);
int check_move(moves_t *move, board_t board, int black);
node_t *new_node(board_t piece_pos,int board_cost,node_t *parent,
				 int level, char move_str[]);
node_t *insert_sibling(node_t *root, board_t board,int cost,node_t *parent, 
	                   int level,char move_str[]);
node_t *insert_child(node_t *root, board_t board, int cost, int level, 
	                 char move_str[]);
void remove_node(node_t *root);
void start_node(node_t *root, board_t piece_pos, int black);
void renew_board(board_t og_board,board_t cpy_board);
void renew_array(moves_t next_move[]);
int board_cost(board_t piece_pos);
int find_max(node_t *node,int current_max);
int find_min(node_t *node,int current_min);
void minmax_rule(node_t *root, int level, int black);
int stage1(board_t piece_pos,int black, int *last_action);
void search_node(node_t *root, int move_cost, int *last_action, 
	             board_t piece_pos);
void stage2(board_t piece_pos,int black, int *last_action);
int check_winner(board_t piece_pos,int black);
void print_error(moves_t cells,board_t piece_pos,int move_num);

/**************************************************************************/
/*Main program that controls all actions*/
int
main(int argc, char *argv[]) {
	board_t piece_pos={{0}};
	int num_actions=0,black=0,last_action=1;
    
	/*Stage 0 is run*/
    if(!stage0(piece_pos,&num_actions,&black, &last_action)){
		/*If line 'A' is detected and no winner is found yet
		  it will run stage 1*/
		if (num_actions == ACTION_A){
			stage1(piece_pos,black, &last_action);
		} else if (num_actions == COMP_ACTIONS){
			stage2(piece_pos,black, &last_action); /* If line 'P' is detected,
														it will run stage 2*/
		}
    }
    
    return EXIT_SUCCESS;            // exit program with the success code
}

/************************************************************************/
/*(Stage 0) This takes in the input and prints our the board,cost, and
   initial info*/
int
stage0(board_t piece_pos,int *num_actions,int *black, int *last_action){
	char moves[INPUT_LENGTH];
	
	/*This prints out the initial info (board size and pieces) and the board*/
	board_info(); 
    fill_pieces(piece_pos);
    print_board(piece_pos);
    
    /*Takes in the input*/
	while (scanf("%s",moves)==1){
		if(strlen(moves)==LINE_LENGTH){
			if (moves[0]==LINE_A){
				*num_actions = ACTION_A;
			} else if (moves[0] == LINE_P){
				*num_actions = COMP_ACTIONS;
			}
            break;
        }
        
        /*Stores them into structs based on their cells and converts the cells
         to indices (col and row)*/
        moves_t cells = {.src_letter = moves[0],
						 .src_num = atoi(&moves[1]),
						 .tgt_letter = moves[3],
						 .tgt_num = atoi(&moves[4]),
						 .src_col = moves[0]-ASCII_A-1,
						 .src_row = atoi(&moves[1])-1,
						 .tgt_col = moves[3]-ASCII_A-1,
						 .tgt_row = atoi(&moves[4])-1
						 };
        
		/*Checks for any errors in the input and prints 
		  the error if there are any*/
		if (check_errors(cells,piece_pos,*last_action)){
			print_error(cells,piece_pos,*last_action);
			break;
		}
		
		/*If there are no errors, the board and their info is printed*/
		horizontal_line();
		/*If number of actions is not even, that means it is currently
		   black's turn*/
		if (*last_action%2 != 0){ 
			printf("BLACK ACTION");
			*black = 0;
		} else{
			printf("WHITE ACTION");
			*black =1;
		}
		printf(" #%d: %s\n", *last_action, moves);
		change_pos(piece_pos,cells); /*This changes the board 
									   based on their input*/
		printf("BOARD COST: %d\n", board_cost(piece_pos));
		print_board(piece_pos);
		*last_action+=1;
	}
	/*Checks if there are any winners*/
	if(check_winner(piece_pos,*black)){
		return 1;
	}
	return 0;
}
void
print_error(moves_t cells,board_t piece_pos,int move_num){
	if (check_errors(cells,piece_pos,move_num)==ERROR_1){
		printf("ERROR: Source cell is outside of the board.\n");
	} else if (check_errors(cells,piece_pos,move_num)==ERROR_2){
		printf("ERROR: Target cell is outside of the board.\n");
	} else if (check_errors(cells,piece_pos,move_num)==ERROR_3){
		printf("ERROR: Source cell is empty.\n");
	} else if (check_errors(cells,piece_pos,move_num)==ERROR_4){
		printf("ERROR: Target cell is not empty.\n");
	} else if (check_errors(cells,piece_pos,move_num)==ERROR_5){
		printf("ERROR: Source cell holds opponent\'s piece/tower.\n");
	} else if(check_errors(cells,piece_pos,move_num)==ERROR_6){
		printf("ERROR: Illegal action.\n");
	}
}
/************************************************************************/
/*(Stage 1) Finds the next possible move according to the current turn*/
int stage1(board_t piece_pos,int black, int *last_action){
	/*0 is set in because it is currently depth 0 of the node*/
	node_t *root = new_node(piece_pos,board_cost(piece_pos),NULL,0,NULL);
	
	/*This creates the tree for all possible moves*/
	start_node(root,root->board,black);
	start_node(root->child,root->child->board,!black);
	start_node(root->child->child,root->child->child->board,black);
	
	/*Find the min/max according to their turns and depth*/
	minmax_rule(root,TREE_DEPTH,black);    /*Minmax for depth 3*/
	minmax_rule(root,TREE_DEPTH-1,!black); /*Minmax for depth 2*/
	minmax_rule(root,TREE_DEPTH-2,black);  /*Minmax for depth 1*/
	
	/*Finds the best possible action based on their cost*/
	search_node(root->child, root->cost,last_action,piece_pos);
	
	/*Frees up all malloc used*/
	remove_node(root);
	
	/*Checks if there are any winners*/
	if(check_winner(piece_pos,!black)){
		return 1;
	}
	return 0;
}
/**********************************************************************/
/*(Stage 2) Similar to stage 1, however it is repeated 10 times or until 
   a winner is found*/
void stage2(board_t piece_pos,int black, int *last_action){
	for (int i =0; i<COMP_ACTIONS; i++){
		if(stage1(piece_pos,black, last_action)){
			break;
		}
		black = !black;
	}
}
/***********************************************************************/
/*Prints initial board info*/
void
board_info(){
	printf("BOARD SIZE: %dx%d\n",BOARD_SIZE,BOARD_SIZE);
    printf("#BLACK PIECES: %d\n",TOTAL_PIECES);
    printf("#WHITE PIECES: %d\n",TOTAL_PIECES);
}
/**********************************************************************/
/*Fills all the pieces required in the board*/
void 
fill_pieces(board_t pos){
	int last_piece = CELL_WPIECE;
	/*Loops through the rows and fills in the pieces depending on their
	  color and row*/
	/*White pieces will be from row 1-3; black pieces for row 6-8*/
	for (int i =0; i<BOARD_SIZE;i++){
		if ( i<EMPTY_ROWINDEX1){ /*No pieces are required in row 4 and 5*/
			fill_row(&last_piece, CELL_WPIECE, i,pos);
		} else if ( i>EMPTY_ROWINDEX2) {
			fill_row(&last_piece, CELL_BPIECE,i,pos);
		} else{
			/*Everything else will be filled with empty cells*/
			for (int j=0; j<BOARD_SIZE;j++){
				pos[i][j] = CELL_EMPTY;
			}
		}
	}
}
/*************************************************************************/
/*Fills the pieces for a row*/
void
fill_row(int *last_piece, char piece_colour, int row_num, board_t pos){
	/*This fills in the pieces after one space from each other*/
	for (int j=0; j<BOARD_SIZE;j++){
		if (*last_piece != CELL_EMPTY){
			pos[row_num][j] = CELL_EMPTY;
			*last_piece = CELL_EMPTY;
		} else{
			pos[row_num][j] = piece_colour;
			*last_piece = piece_colour;
		}
	}
	/*The last piece is changed, since on a new row the piece filled in
	 matches the last piece not the opposite of the last piece*/ 
	if (*last_piece != CELL_EMPTY){
		*last_piece = CELL_EMPTY;
	} else {
		*last_piece = piece_colour;
	}
}
/**************************************************************************/
/*Prints the board*/
void
print_board(board_t pos){
	print_letters(); /*Prints the letters on top of the board*/
	board_horizontal(); /*Prints the line between rows*/
	/*Loops through the cell array and prints it*/
	for (int i=1; i<=BOARD_SIZE; i++){
		printf(" %d ",i);
		for (int j=0; j<BOARD_SIZE; j++){
			if (j==0){
				printf("| %c |",pos[i-1][j]);
			}else{
				printf(" %c |",pos[i-1][j]);
			}
		}
		printf("\n");
		board_horizontal();
	}
}
/**************************************************************************/
/*Checks the errors according to their conditions*/
int
check_errors(moves_t cells,board_t piece_pos,int move_num){
	if (!check_range(cells.src_letter,cells.src_num)){
		return ERROR_1; /*Checks if source cell is in range*/
	} else if (!check_range(cells.tgt_letter,cells.tgt_num)){
		return ERROR_2; /*Checks if target cell is in range*/
	} else if (check_empty(piece_pos,cells.src_row,cells.src_col)){
		return ERROR_3; /*Checks if source cell is empty*/
	} else if (!check_empty(piece_pos,cells.tgt_row,cells.tgt_col)){
		return ERROR_4; /*Checks if target cell is empty*/
	}else if(check_opponent(move_num,cells.src_row,cells.src_col,piece_pos)){
		return ERROR_5;/*Check if the source cell we're moving matches color */
	} else if(check_illegal(piece_pos,cells)){
		return ERROR_6; /*Checks if move is illegal*/
	}
	return 0;
}
/*************************************************************************/
/*Checks if the cell is within A-H and 1-8*/
int
check_range(char letter, int num){
	char letter_str[LENGTH_OF_CELL-1] = {letter}; 
	/*Compares letters first*/
	if ((strcmp(letter_str,"A")*strcmp(letter_str,"H")) >0){
		return 0;
	} else if ((num<LOW_NUM) || (num>HIGH_NUM)){ /*Next, the numbers*/
		return 0;
	}
	return 1;
}
/********************************************************************/
/*Checks if the cell is empty*/
int
check_empty(board_t piece_pos,int row, int col){
	if (piece_pos[row][col] == CELL_EMPTY){
		return 1;
	}
	return 0;
	
}
/******************************************************************/
/*Checks if source matches the current action's color*/
int 
check_opponent(int move_num, int row, int col,board_t piece_pos){
	char src_cell;
	src_cell = piece_pos[row][col];
	if (move_num%2 != 0){ /*If current action number is not divisible by 2
		 					this means it's currently black's turn*/
		if((src_cell!=CELL_BPIECE)&&(src_cell!=CELL_BTOWER)){
			return 1;
		}
	} else {
		if((src_cell!=CELL_WPIECE)&&(src_cell!=CELL_WTOWER)){
			return 1;
		}
	}
	return 0;
}
/*********************************************************************/
/*Checks if current move is illegal or not*/
int 
check_illegal(board_t piece_pos, moves_t cells){
	int letter_space,num_space,mid_row,mid_col;
	char src_cell,mid_cell;
	src_cell = piece_pos[cells.src_row][cells.src_col];
	/*Checks the space between the letters and numbers
		from source to target cell*/
	letter_space = abs(cells.src_letter - cells.tgt_letter);
	num_space = cells.src_num - cells.tgt_num;
	
	/*Pieces are only able to move between 1 or 2 spaces*/
	if (letter_space == NEXT_SPACE || letter_space == JUMP_SPACE){
		/*Makes sure the cell moves diagonally consistently*/
		if (letter_space != abs(num_space)){ 
			return 1;
		}
	} else {
		return 1;
	}
	/*Makes sure white pieces can only move downwards and black pieces move
	  upwards*/
	if (src_cell== CELL_WPIECE){
		if (num_space >=0){ /*Checks if the number is positive/negative*/
			return 1;
		}
	} else if(src_cell == CELL_BPIECE){
		if (num_space <=0){/*Checks if the number is positive/negative*/
			return 1;
		}
	}
	/*If cell moves 2 spaces there has to be an opponent piece between it*/
	if (letter_space == JUMP_SPACE){
		mid_row = (cells.src_row + cells.tgt_row)/2;
		mid_col = (cells.src_col + cells.tgt_col)/2;
		mid_cell = piece_pos[mid_row][mid_col];
		if (mid_cell == CELL_EMPTY){
			return 1;
		} else {
			if (src_cell == CELL_WPIECE || src_cell == CELL_WTOWER){
				if (mid_cell != CELL_BPIECE && mid_cell != CELL_BTOWER){
					return 1;
				}
			} else {
				if (mid_cell != CELL_WPIECE && mid_cell != CELL_WTOWER){
					return 1;
				}
			}
		}
	}
	return 0;
}
/********************************************************************/
/*Changes the board content after a move is taken in*/
void
change_pos(board_t piece_pos,moves_t cells){
	int space,mid_row,mid_col;
	space = abs(cells.src_num - cells.tgt_num);
	
	/*The target cell will now hold the content of the source cell
	  and turns the source cell into an empty cell*/
	piece_pos[cells.tgt_row][cells.tgt_col] = piece_pos[cells.src_row]
												[cells.src_col];
	piece_pos[cells.src_row][cells.src_col] = CELL_EMPTY;
	
	/*If the cell moves 2 spaces, it will capture 
	  the opponent cell between it*/
	if (space == JUMP_SPACE){
		/*Finds the cell between source and target cell*/
		mid_row = (cells.src_row+cells.tgt_row)/2; 
		mid_col = (cells.src_col+cells.tgt_col)/2;
		piece_pos[mid_row][mid_col] = CELL_EMPTY;
	}
	
	/*If any of the pieces reach the end it will convert into a tower*/
	if (piece_pos[cells.tgt_row][cells.tgt_col]== CELL_WPIECE){
		if (cells.tgt_row == END_WROW){
			piece_pos[cells.tgt_row][cells.tgt_col] = CELL_WTOWER;
		}
	} else if (piece_pos[cells.tgt_row][cells.tgt_col]==CELL_BPIECE){
		if (cells.tgt_row == END_BROW){
			piece_pos[cells.tgt_row][cells.tgt_col] = CELL_BTOWER;
		}
	}
	
}
/***********************************************************************/
/*Calculates the board cost*/
int
board_cost(board_t piece_pos){
	int wpiece=0,bpiece=0,wtower =0,btower=0;
	int cost;
	/*Counts the pieces in the current board*/
	for(int i=0; i<BOARD_SIZE;i++){
		for(int j=0; j<BOARD_SIZE;j++){
			if(piece_pos[i][j]==CELL_WPIECE){
				wpiece += 1;
			} else if(piece_pos[i][j]==CELL_BPIECE){
				bpiece +=1;
			} else if(piece_pos[i][j]==CELL_WTOWER){
				wtower +=1;
			} else if(piece_pos[i][j]==CELL_BTOWER){
				btower += 1;
			}
		}
	}
	/*Calculates board cost*/
	cost = bpiece+COST_TOWER*btower-wpiece-COST_TOWER*wtower;
	return cost;
}
/***************************************************************************/
/*Creates a new node to point to*/
node_t 
*new_node(board_t piece_pos,int board_cost,node_t *parent,int level, 
		  char move_str[]){
    node_t *new = malloc(sizeof(node_t));
    /*Fills all the data in the struct node*/
    if(new != NULL){
        new->sibling = NULL;
        new->child = NULL;
        for (int i =0; i<BOARD_SIZE; i++){
        	for (int j=0; j<BOARD_SIZE; j++){
        		new->board[i][j] = piece_pos[i][j];
        	}
        }
        new->depth = level; 
        new->parent = parent;
        new->cost = board_cost;
        if (move_str != NULL){
        	strncpy (new->str_move, move_str, INPUT_LENGTH);
        }   
    }

    return new;
}
/**********************************************************************/
/*Inserts a new node as a child*/
node_t 
*insert_child(node_t *root, board_t board, int cost,int level,char move_str[]){
    if(root==NULL){
        return NULL;
    }
    /*If a root already has a child, the new node 
      will be assigned as a sibling*/
    if(root->child){
        return insert_sibling(root->child, board,cost,root,level,move_str);
    }else{
        return (root->child = new_node(board,cost,root,level,move_str));
    }
}
/****************************************************************/
/*Inserts a new node as a sibling*/
node_t 
*insert_sibling(node_t *root, board_t board,int cost,node_t *parent,int level,
				char move_str[]){
    if (root==NULL){
        return NULL;
    }
    /*Node is added at the end of all the other siblings*/
    while(root->sibling){
        root= root->sibling;
    }
    root->sibling = new_node(board,cost,parent,level,move_str);
    return (root->sibling);
}
/**************************************************************/
/*Starts making a tree from one node*/
void start_node(node_t *root, board_t piece_pos,int black){
	/*This holds all the cells in the board*/
    char all_cells[TOTAL_CELLS][LENGTH_OF_CELL] =
    {"A1","B1","C1","D1","E1","F1","G1","H1",
     "A2","B2","C2","D2","E2","F2","G2","H2",
     "A3","B3","C3","D3","E3","F3","G3","H3",   
     "A4","B4","C4","D4","E4","F4","G4","H4",
     "A5","B5","C5","D5","E5","F5","G5","H5",
     "A6","B6","C6","D6","E6","F6","G6","H6",
     "A7","B7","C7","D7","E7","F7","G7","H7",
     "A8","B8","C8","D8","E8","F8","G8","H8"};
    int cost;
    char tmp_str[INPUT_LENGTH];
    moves_t next_move[MAX_MOVES]; 
    board_t pos_cpy;
    /*Checks if root is empty*/
    if (root==NULL){
    	return;
    }
	/*Finds all the possible moves in every cell of the board depending 
	  on their color*/
	for (int i=0; i<(BOARD_SIZE*BOARD_SIZE);i++){
		renew_array(next_move);
		renew_board(piece_pos,pos_cpy);
		possible_moves(pos_cpy, all_cells[i], next_move,black);
		for (int k=0; k<MAX_MOVES; k++){
			/*This converts the cells from possible moves into a string
			  e.g "H3-F1" and puts it in a struct node*/
			if(next_move[k].src_letter != 0){
				tmp_str[0] = next_move[k].src_letter;
				tmp_str[1] = next_move[k].src_num + '0';
				tmp_str[2] = '-';
				tmp_str[3] = next_move[k].tgt_letter;
				tmp_str[4] = next_move[k].tgt_num +'0';
				tmp_str[5] = '\0';
				renew_board(piece_pos,pos_cpy);
				change_pos(pos_cpy,next_move[k]);
				cost = board_cost(pos_cpy);
				/*Puts all the data as a child node of the original node*/
				insert_child(root, pos_cpy,cost,root->depth+1,tmp_str);
			}
		}
	}
    
    /*Function repeats for the node's siblings*/
    if (root->sibling){
        start_node(root->sibling,root->sibling->board,black);
    }
    if(root->parent != NULL){
    	if(root->parent->sibling){
    		start_node(root->parent->sibling,
    					root->parent->sibling->board,black);
    	}
    }
}
/*************************************************************************/
/*Applies the minmax decision rule*/
void minmax_rule(node_t *root, int level,int black){
	/*The node will start applying minmax depending on the color when the 
	  level is reached*/
	if(root->depth != level){
		minmax_rule(root->child,level,black);
	} else {
		if (black){
			find_max(root,root->cost);
		} else {
			find_min(root,root->cost);
		}
		/*Applies minmax to all nodes with the same level*/
		if (root->parent->sibling != NULL){
			minmax_rule(root->parent->sibling,level,black);
		}
	}
}
/************************************************************************/
/*Finds the minimum board cost out of all the children or basically 
  the siblings of the node*/
int
find_min(node_t *node,int current_min){
	int min_cost;
	min_cost = current_min;
	if (node->sibling != NULL){
		if (min_cost > node->sibling->cost){
			min_cost = node->sibling->cost;
		}
		find_min(node->sibling,min_cost);
	}
	node->parent->cost = min_cost; /*Changes the parent node's cost into the 
									 the minimum cost*/
	return min_cost;
}
/**********************************************************************/
/*Finds the minimum board cost out of all the children or basically 
  the siblings of the node*/
int
find_max(node_t *node,int current_max){
	int max_cost;
	max_cost = current_max;
	
	if (node->sibling != NULL){
		if (max_cost < node->sibling->cost){
			max_cost = node->sibling->cost;
		}
		find_max(node->sibling,max_cost);
	}
	node->parent->cost = max_cost;
	return max_cost;
}
/***********************************************************************/
/*Searches a node's children that matches the board cost,prints out the
  searched node and is chosen as the next move of the board */
void search_node(node_t *root, int move_cost, int *last_action,
				 board_t piece_pos){
	if (root->cost == move_cost){
		horizontal_line();
		printf("*** ");
		/*If the current num action is not an even number, 
		  this means it's black's turn*/
		/*This prints out all the info for the board*/
		if(*last_action%2 != 0){ 
			printf("BLACK ");
		} else {
			printf("WHITE ");
		}
		printf("ACTION #%d: %s\n",*last_action,root->str_move);
		printf("BOARD COST:%2d\n",board_cost(root->board));
		print_board(root->board);
		/*This changes the contents of the board based on the new move*/
		for (int i=0; i<BOARD_SIZE; i++){
			for (int j=0; j<BOARD_SIZE; j++){
				piece_pos[i][j] =root->board[i][j];
			}
		}
		*last_action += 1;
	} else {
		search_node(root->sibling, move_cost,last_action,piece_pos);
	}
}
/********************************************************************/
/*Empties the contents of the array*/
void renew_array(moves_t next_move[]){
	for (int i=0; i<MAX_MOVES; i++){
		next_move[i].src_letter = 0;
	}
}
/******************************************************************/
/*This converts the board back to its original state 
  before any moves were made*/
void renew_board(board_t og_board,board_t cpy_board){
	for (int i=0; i<BOARD_SIZE;i++){
		for(int j=0; j<BOARD_SIZE; j++){
			cpy_board[i][j] = og_board[i][j];
		}
	}
}
/********************************************************************/
/*Finds all the possible moves for 1 cell and puts it into an array*/
int possible_moves(board_t piece_pos, char *move, moves_t A[],int black){
	int num_possible =0;
	/*Structs with all the possible moves for 1 cell*/
	/*Col and rows are the indices of the cells*/
	moves_t first = {.src_letter = move[0],
					 .src_num = atoi(&move[1]),
					 .tgt_letter = move[0] + NEXT_SPACE,
					 .tgt_num = atoi(&move[1]) - NEXT_SPACE,
					 .src_col = move[0]-ASCII_A-1,
					 .src_row = atoi(&move[1])-1,
					 .tgt_col = move[0]-ASCII_A-1 + NEXT_SPACE,
					 .tgt_row = atoi(&move[1])-1 - NEXT_SPACE
					 };
	moves_t second = {.src_letter = move[0],
					 .src_num = atoi(&move[1]),
					 .tgt_letter = move[0] + NEXT_SPACE,
					 .tgt_num = atoi(&move[1]) + NEXT_SPACE,
					 .src_col = move[0]-ASCII_A-1,
					 .src_row = atoi(&move[1])-1,
					 .tgt_col = move[0]-ASCII_A-1 + NEXT_SPACE,
					 .tgt_row = atoi(&move[1])-1+ NEXT_SPACE
					 };
	moves_t third = {.src_letter = move[0],
					 .src_num = atoi(&move[1]),
					 .tgt_letter = move[0] - NEXT_SPACE,
					 .tgt_num = atoi(&move[1]) + NEXT_SPACE,
					 .src_col = move[0]-ASCII_A-1,
					 .src_row = atoi(&move[1])-1,
					 .tgt_col = move[0]-ASCII_A-1 - NEXT_SPACE,
					 .tgt_row = atoi(&move[1])-1 + NEXT_SPACE
					 };
	moves_t fourth = {.src_letter = move[0],
					 .src_num = atoi(&move[1]),
					 .tgt_letter = move[0] - NEXT_SPACE,
					 .tgt_num = atoi(&move[1]) - NEXT_SPACE,
					 .src_col = move[0]-ASCII_A-1,
					 .src_row = atoi(&move[1])-1,
					 .tgt_col = move[0]-ASCII_A-1 - NEXT_SPACE,
					 .tgt_row = atoi(&move[1])-1 - NEXT_SPACE
					 };
	/*Checks if the moves are valid*/
	/*If yes, it will be put into an array of possible moves*/
	if (!check_move(&first, piece_pos, black)){
		A[0] = first;
		num_possible += 1;
	}
	if (!check_move(&second, piece_pos, black)){
		A[1] = second;
		num_possible += 1;
	}
	if (!check_move(&third, piece_pos, black)){
		A[2] = third;
		num_possible += 1;
	}
	if (!check_move(&fourth, piece_pos, black)){
		A[3] = fourth;
		num_possible += 1;
	}
	return num_possible;
}
/********************************************************************/
/*Checks if move is valid and converts the move into a jump if an
  opponent is on target cell*/
int
check_move(moves_t *move, board_t board, int black){
	/*This converts the move into a jump if an opponent is detected on
	  target cell*/
	if (check_errors(*move,board,black) == ERROR_4){
		if (move->tgt_letter<move->src_letter){
			move->tgt_letter -= NEXT_SPACE;
			move->tgt_col -= NEXT_SPACE;
		} else {
			move->tgt_letter += NEXT_SPACE;
			move->tgt_col += NEXT_SPACE;
			
		}
		if (move->tgt_num<move->src_num){
			move->tgt_num -= NEXT_SPACE;
			move->tgt_row -= NEXT_SPACE;
		} else {
			move->tgt_num += NEXT_SPACE;
			move->tgt_row += NEXT_SPACE;
			
		}
	}
	/*Checks if move is valid*/
	if(check_errors(*move,board,black) != 0){
		return 1;
	}
	return 0;
}
/***********************************************************************/
/*Checks if there are any winners*/
int check_winner(board_t piece_pos,int black){
	moves_t tmp_str[INPUT_LENGTH];
	char all_cells[TOTAL_CELLS][LENGTH_OF_CELL] =
    {"A1","B1","C1","D1","E1","F1","G1","H1",
     "A2","B2","C2","D2","E2","F2","G2","H2",
     "A3","B3","C3","D3","E3","F3","G3","H3",   
     "A4","B4","C4","D4","E4","F4","G4","H4",
     "A5","B5","C5","D5","E5","F5","G5","H5",
     "A6","B6","C6","D6","E6","F6","G6","H6",
     "A7","B7","C7","D7","E7","F7","G7","H7",
     "A8","B8","C8","D8","E8","F8","G8","H8"};

	/*Checks if all the pieces are unable to move depending on their color*/
	for(int i=0; i<TOTAL_CELLS; i++){
	   if (possible_moves(piece_pos, all_cells[i],tmp_str,black)){
		   return 0;
	   } 
   }
   /*Winner will be printed if all pieces are unable to move*/
   if(black){
	   printf("WHITE WIN!\n");
   }else{
	   printf("BLACK WIN!\n");
   }
   return 1;
}
/*************************************************************************/
/*Frees up all the memory after malloc is used*/
void remove_node(node_t *root){
    if(root->sibling != NULL)
        remove_node(root->sibling);

    if((root->child != NULL))
        remove_node(root->child);

    free(root);
}
/************************************************************************/

void
horizontal_line(){
	printf("=====================================\n");
}
void
print_letters(){
	printf("     A   B   C   D   E   F   G   H\n");
}

void
board_horizontal(){
	printf("   +---+---+---+---+---+---+---+---+\n");
}
/************************************************************************/
/*algorithms are fun*/
/* THE END -------------------------------------------------------------------*/