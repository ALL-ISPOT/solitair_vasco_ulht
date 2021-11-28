#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#define MSG_WIN "You Win! Congratulations!"
#define MSG_LOSE "You Lost! Better luck next time!"
#define DEFAULT_SUM 10
#define DEFAULT_SEED 7
#define DEFAULT_NROWS 12
#define DEFAULT_NHOLDS 2
#define DEFAULT_LEVEL 3
#define SCORELIMIT 21
#define DEBUG 0	

typedef 
struct _board
{
	int * left;
	int * right;
	int * rhold;
    int * lhold;
    int leftsize, rightsize, lholdsize, rholdsize;
    int score;
    int nholds;
} 
Board;

#define LEFT 'a'
#define RIGHT 's'
#define LMOVE 'q'
#define RMOVE 'w'
#define REVLHOLD 'z'
#define REVRHOLD 'x'
#define EXIT 'e'
#define PROMPT ">"

/* needed functions */
double uniRand(void);
double randn(double, double);
int randn_sat(double, double, int, int);
void testargs(int argc, char ** argv, int levels, long int seed, int nrows, int nholds);
void initboard(int nrows, int nholds, Board *board, int level, long seed);
void alocationerror();
void printout(int nrows, Board board);
void initargs(int *nholds, int *nrows, long *seed, int *levels, int argc, char **argv);
char get_user_command();
void play(char cmd, Board *board);


const char* HEADER =    "|HOLD |LEFT |///|RIGHT|HOLD |";
const char* SEPARATOR = "+-----+-----+---+-----+-----+";
const char* TERMINATOR = "+---------------------------+";


int main(int argc, char ** argv) {
    int level = DEFAULT_LEVEL, nrows = DEFAULT_NROWS, nholds = DEFAULT_NHOLDS;
    long seed = DEFAULT_SEED;
    Board board;
    char cmd = ' ';
	
	/*verificar args*/
    initargs(&nholds, &nrows, &seed, &level, argc, argv);
    /*if (DEBUG) testargs(argc, argv, level, seed, nrows, nholds);*/

	/* initialize random seed */
    initboard(nrows, nholds, &board, level, seed);
	
	while(cmd != EXIT) {
        printout(nrows, board);   
        
        if (board.rightsize == 0 && board.leftsize == 0 && board.lholdsize == 0 && board.rholdsize == 0) {
            printf("%s\n", MSG_WIN);
            break;
        }
        else if (board.score >= SCORELIMIT || board.score <= 0) {
            printf("%s\n", MSG_LOSE);
            break;
        }

        cmd = get_user_command();
        
        play(cmd, &board);
    }
	return 0;
}

void play(char cmd, Board *board){
    switch (cmd){
        case LEFT:
            board->score += board->left[board->leftsize-1];
            board->leftsize--;
            break;

        case RIGHT:
            board->score += board->right[board->rightsize-1];
            board->rightsize--;
            break;
        
        case LMOVE:
            if (board->lholdsize < board->nholds && board->leftsize > 0){
                board->lhold[board->lholdsize] = board->left[board->leftsize-1];
                board->leftsize--;
                board->lholdsize++;
            }
            break;
        
        case RMOVE:
            if (board->rholdsize < board->nholds && board->rightsize > 0){
                board->rhold[board->rholdsize] = board->right[board->rightsize-1];
                board->rightsize--;
                board->rholdsize++;
            }
            break;

        case REVLHOLD:
            if (board->lholdsize > 0){
                board->score += board->lhold[board->lholdsize-1];
                board->lholdsize--;
            }
            break;

        case REVRHOLD:
            if (board->rholdsize > 0){
                board->score += board->rhold[board->rholdsize-1];
                board->rholdsize--;
            }
            break;

        default: break;
    }
}

char get_user_command(){
    char c;
    fflush(stdin);
    printf("%s", PROMPT);
    do {
        c = getchar();
    } while( isspace(c) );
    
    return c;
}

void initargs(int *nholds, int *nrows, long *seed, int *levels, int argc, char **argv){
    if (argc == 5) *nholds = atoi( argv[4] );
    if (argc >= 4) *nrows = atoi( argv[3] );
    if (argc >= 3) *seed = atoi( argv[2] );
    if (argc >= 2) *levels = atoi( argv[1] );
}

void initboard(int nrows, int nholds, Board *board, int level, long seed){
    int i;
    srand(seed);
	board->left  = (int *) malloc( sizeof(int) * nrows);
	board->right = (int *) malloc( sizeof(int) * nrows);
    board->rhold = (int *) malloc( sizeof(int) * nholds);
    board->lhold = (int *) malloc( sizeof(int) * nholds);
    if (board->left  == (int *)0) alocationerror();
    if (board->right == (int *)0) alocationerror();
    if (board->rhold == (int *)0) alocationerror();
    if (board->lhold == (int *)0) alocationerror();
    board->leftsize = nrows;
    board->rightsize = nrows;
    board->lholdsize = 0;
    board->rholdsize = 0;
    board->score = DEFAULT_SUM;
    board->nholds = nholds;
	
	for (i = nrows-1 ; i >= 0 ; i--)
		board->left[i] = randn_sat(0, level, -21, 21);

	for (i = nrows-1 ; i >= 0 ; i--)
		board->right[i] = randn_sat(0, level, -21, 21);
}

void alocationerror(){
    printf("Memory alocation error");
    exit(0);
}

#define STRSIZE 4

void printout(int nrows, Board board){
    int i;
    char lholdString[STRSIZE], rholdString[STRSIZE], leftString[STRSIZE], rightString[STRSIZE];
    printf("%s\n", SEPARATOR);
    printf("%s\n", HEADER); 
    printf("%s\n", SEPARATOR);

    for(i = 0; i < nrows; i++) {
        
        if (i<board.lholdsize){
            snprintf(lholdString, STRSIZE, "%-3d", board.lhold[i]);
        } else {
            snprintf(lholdString, STRSIZE, "   ");
        }
        
        if (i<board.rholdsize){
            snprintf(rholdString, STRSIZE, "%-3d", board.rhold[i]);
        } else {
            snprintf(rholdString, STRSIZE, "   ");
        }
        
        if (i == board.leftsize - 1 || DEBUG){
            snprintf(leftString, STRSIZE, "%-3d", board.left[i]);
        } 
        else if(i<board.leftsize) {  
            snprintf(leftString, STRSIZE, " * ");
        }
        else {
            snprintf(leftString, STRSIZE, "   ");
        }
        
        if (i == board.rightsize - 1 || DEBUG){
            snprintf(rightString, STRSIZE, "%-3d", board.right[i]);
        } 
        else if(i<board.rightsize) {
            snprintf(rightString, STRSIZE, " * ");
        }
        else {
            snprintf(rightString, STRSIZE, "   ");
        }
        
        printf("| %3s | %3s |///| %3s | %3s |\n", lholdString, leftString, rightString, rholdString); 
    }
    printf("%s\n", SEPARATOR);
    printf("|SUM = %-3d                  |\n", board.score); 
    printf("%s\n", TERMINATOR);
}

void testargs(int argc, char ** argv, int levels, long int seed, int nrows, int nholds)
{
    int i;
    for (i = 0; i< argc; i++) printf("%d %s\n", i, argv[i]);
    printf("nholds = %d \n", nholds);
    printf("nrows = %d \n", nrows);
    printf("seed = %ld \n", seed);
    printf("levels = %d \n", levels);
}




/* function: uniRand()
 * return: double
 * parameters: none
 * description:
 * 	generate random number between within [0 - 1] 
 */
double uniRand()
{
	return (double) rand() / RAND_MAX;
}


/* function: randn()
 * return: double
 * parameters:
 * 	double mean - the mean of the normal distribution
 * 	double std - the standard deviation of the normal distribution
 * description:
 * 	generate random number following a normal distribuition using
 *	Box–Muller transform.
 */
#define PI 3.14159265358979
double randn(double mean, double std)
{
	double u1, u2, z;

	u1 = uniRand();
	u2 = uniRand();

	z =  sqrt(-2*log(u1))*cos(2*PI*u2);

	/* Multiply by the standard deviation and add the mean.*/
	return std * z + mean;
}


/* function: randn_sat()
 * return: int
 * parameters:
 * 	double mean - the mean of the normal distribution
 * 	double std - the standard deviation of the normal distribution
  * 	int min - the minimum value
 * 	int max - the maximum value
 * description:
 * 	generate random number following a normal distribuition and saturated
 * 	to the interval [min, max]
 */
int randn_sat(double mean, double std, int min, int max)
{
	double r = randn(mean, std);
	return (int) (r > max ? max : r < min ? min : r);
}
