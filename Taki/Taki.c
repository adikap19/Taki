#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

/*
Name: Adi Kapuri
ID: 208184093

Summary of the program: 
In this program we can see a Taki game, which  includes 1-9 regular cards and 5 special cards when each can have different color (yellow, red, blue or green),
but the 'Change color card' which has no color.
Each player can throw a card to the pile \ take another card - according to the game rules.
When we have a player with no cards in his hands, the program will declare his winning.
At the end, the program will show us the statistics of how many times each card was taken from the pile of cards.
*/

#define MAX_NAME_LEN 20
#define CARDS_AMOUNT 14
#define NO_COLOR -1
#define START_CARDS_AMOUNT 4
#define COLORS 4
#define PLUS 10
#define STOP 11
#define DIRECTION 12
#define TAKI 13
#define CHANGE_COLOR 14
#define FINISH_TURN 0
#define NO_CARDS 0
#define TAKE_CARD 0
#define LAST_REGULAR_CARD 9

typedef struct card
{
	int num; //1-9-regular cards, 10-plus, 11-stop, 12-direction, 13-taki, 14-change color
	int color;
} CARD;

struct player_data
{
	CARD* cards;
	int cards_num;
	int phy_cards;
	char first_name[MAX_NAME_LEN + 1];

};
typedef struct player_data PLAYER;

typedef struct statisCard
{
	int numOfCard;
	int appearance;
}statisCard;

typedef struct Game
{
	PLAYER* arrOfPlayers;
	int numOfPlayers;
	CARD upperCard;
	int direction;
	int currentPlayer;
	bool taki;
	bool game_over;
	statisCard stats[CARDS_AMOUNT];

} GAME;

void Welcoming();
void initGame(GAME* game);
void initPlayer(PLAYER* player_data, int i, GAME* game);
void gameLogic(GAME* game);
void printArrOfCards(GAME* game);
void print_card(CARD* res);
CARD create_card(GAME* game);
CARD create_upper_card(GAME* game);
CARD* arrRealloc(int* arr, int size, int newSize);
void takeAnotherCard(GAME* game);
void updateUpperCard_AndRemove(GAME* game, int player_choice);
void throwACard(GAME* game, int player_choice);
void getNextPlayer(GAME* game);
void swapCardsAndRemove(GAME* arr, int cardIndex1, int playerIndex, int cardIndex2);
void specialCards(GAME* game, int player_choice);
int changeColor(GAME* game, int colorChoice);
void plusCard(GAME* game, int player_choice);
void stopCard(GAME* game, int player_choice);
void takiCard(GAME* game, int player_choice);
void changeColorCard(GAME* game, int player_choice);
int checkInput(GAME game, int i);
int checkInputForTaki(GAME game, int i);
void Initial_Statistics(GAME* game);
void updateStatistics(GAME* game, int cardIndex);
void MergeSort(statisCard stat[], int size);
void merge(statisCard stat_arr1[], int size1, statisCard stat_arr2[], int size2, statisCard res[]);
void copyArr(statisCard stat_arr1[], statisCard stat_arr2[], int size);
void cardsStatistics(GAME* game);
void freeArrAfterWin(GAME* game);

void main() 
{
	GAME newGame;
	Initial_Statistics(&newGame);
	srand(time(NULL));
	Welcoming();
	initGame(&newGame);
	gameLogic(&newGame);
}

void Welcoming() //welcoming message
{ 
	printf("************  Welcome to TAKI game !!! ***********\n");
}

void initGame(GAME* game) //This function is getting the amount of players and call a function that Initializing every player
{
	int i;

	printf("Please enter the number of players: ");
	scanf("%d", &game->numOfPlayers);
	
	game->arrOfPlayers = (PLAYER*)malloc(sizeof(PLAYER) * game->numOfPlayers);

	if (game->arrOfPlayers == NULL) //in order to check if the Realloc function succeeded
	{
		printf("ERROR! Memory allocation failed\n");
		exit(1);
	}
	
	for (i = 0; i < game->numOfPlayers; i++)
	{
		initPlayer(&(game->arrOfPlayers[i]),i, game);
	}

	game->direction = 1;
	game->currentPlayer = 0;
}

void initPlayer(PLAYER* player_data, int i, GAME* game) //This function is getting the players name's, and creating the 4 cards that every player gets by calling another function that raffle the cards 
{
	printf("Please enter the first name of player #%i: \n", i + 1);
	scanf("%s", player_data->first_name); //get player name

	player_data->cards_num = START_CARDS_AMOUNT; //set the logic size of cards that each player gets 
	player_data->phy_cards = START_CARDS_AMOUNT; //set the physical size of the cards that each player gets 

	player_data->cards = (CARD*)malloc(sizeof(CARD) * player_data->cards_num);

	if (player_data->cards == NULL) //In order to check if the Realloc function succeeded
	{
		printf("ERROR! Memory allocation failed\n");
		exit(1);
	}

	for (i = 0; i < player_data->cards_num; i++)
	{
		player_data->cards[i] = create_card(game);
	}
}

void gameLogic(GAME* game) //this function run on the game and let every player choose his next move
{
	int player_choice;
	game->game_over = false;

	game->upperCard = create_upper_card(game);

	while (!(game->game_over))
	{
			printArrOfCards(game);
			player_choice = checkInput(*game, game->currentPlayer); //check valid choise of the player

			if (player_choice == 0)
			{
				takeAnotherCard(game);
				getNextPlayer(game);
			}

			else
				throwACard(game, player_choice);
	}

	printf("\nThe winner is... %s! Congratulations!\n", game->arrOfPlayers[game->currentPlayer].first_name);
	cardsStatistics(game);
	freeArrAfterWin(game);
}

void getNextPlayer(GAME* game) //this function set the direction of the players
{
	int* direction = game->direction, *currentPlayer = &(game->currentPlayer), numOfPlayers = game->numOfPlayers;

	if (*currentPlayer >= 0 && (*currentPlayer) <= numOfPlayers - 2 && direction == 1)
		(*currentPlayer)++;
	
	else if (*currentPlayer >= 1 && *currentPlayer <= numOfPlayers - 1 && direction == -1)
		(*currentPlayer)--;
	
	else // if (*currentPlayer == 0 || *currentPlayer == numOfPlayers) --> 0 && dir =-1 || nOP && dir =1
	{
		if (*currentPlayer == 0)
			*currentPlayer = numOfPlayers - 1;
		
		else
			*currentPlayer = 0;
	}
}

void printArrOfCards(GAME* game) //this function prints the upper card, and for each player his current cards
{
	int i;

	printf("\nUpper card:\n"); 
	print_card(&game->upperCard);
	printf("\n%s's turn:\n", game->arrOfPlayers[game->currentPlayer].first_name);

	for (i = 0; i < game->arrOfPlayers[game->currentPlayer].cards_num; i++) //run for each card that belongs to the player
	{
		printf("\nCard #%d:\n", i + 1);
		print_card(&game->arrOfPlayers[game->currentPlayer].cards[i]);
	}
}

void takeAnotherCard(GAME* game) //this function takes action when the player wants to take another card from the pile, create the card and increase the arr of the cards + logic size of his card number
{
	if (game->arrOfPlayers[game->currentPlayer].phy_cards == game->arrOfPlayers[game->currentPlayer].cards_num)
	{
		game->arrOfPlayers[game->currentPlayer].cards = arrRealloc(game->arrOfPlayers[game->currentPlayer].cards, game->arrOfPlayers[game->currentPlayer].phy_cards, game->arrOfPlayers[game->currentPlayer].phy_cards * 2);
		game->arrOfPlayers[game->currentPlayer].phy_cards *= 2;

		if (game->arrOfPlayers[game->currentPlayer].phy_cards == NULL) //in order to check if the Realloc function succeeded
		{
			printf("ERROR! Memory allocation failed\n");
			exit(1);
		}
	}

	game->arrOfPlayers[game->currentPlayer].cards_num++; //logic size + 1
	game->arrOfPlayers[game->currentPlayer].cards[game->arrOfPlayers[game->currentPlayer].cards_num - 1] = create_card(game); //in order to give the player another card
}

void throwACard(GAME* game, int player_choice) //this function takes action when the player wants to throw a card to the pile according to the card type
{
	if (game->arrOfPlayers[game->currentPlayer].cards[player_choice - 1].num <= LAST_REGULAR_CARD) 
	{
		updateUpperCard_AndRemove(game, player_choice);
		
		if (game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS)
			game->game_over = true;
		else
			getNextPlayer(game);
	}

	else
		specialCards(game, player_choice);
}

void updateUpperCard_AndRemove(GAME* game, int player_choice) //This function is being called when the player has thrown a regular 1-9 card
{
	game->upperCard.color = game->arrOfPlayers[game->currentPlayer].cards[player_choice - 1].color; //set the upper card color as the thrown card
	game->upperCard.num = game->arrOfPlayers[game->currentPlayer].cards[player_choice - 1].num; //set the upper card number as the thrown card
	swapCardsAndRemove(game, player_choice - 1, game->currentPlayer, game->arrOfPlayers[game->currentPlayer].cards_num - 1); //swap the card that the player chose with the last card in the arr
}

void specialCards(GAME* game, int player_choice) //This function is responsible to the special game cards and how each card impact the game
{
	int card_num = game->arrOfPlayers[game->currentPlayer].cards[player_choice - 1].num;

	switch (card_num)
	{
	case PLUS:
	{
		plusCard(game, player_choice);
		break;
	}
	case STOP:
	{
		stopCard(game, player_choice);
		break;
	}
	case DIRECTION:
	{
		updateUpperCard_AndRemove(game, player_choice);
		game->direction *= -1;
		getNextPlayer(game);
		break;
	}
	case TAKI:
	{
		takiCard(game, player_choice);
		break;
	}
	case CHANGE_COLOR:
	{
		changeColorCard(game, player_choice);
		break;
	}
	}
}

void plusCard(GAME* game, int player_choice) //this function takes action when the player has thrown PLUS card and gives him another turn
{
	updateUpperCard_AndRemove(game, player_choice);

	if (game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS)
	{
		takeAnotherCard(game);
		getNextPlayer(game);
	}
}

void stopCard(GAME* game, int player_choice) //this function takes action when the player has thrown STOP card and skip the next player turn
{
	updateUpperCard_AndRemove(game, player_choice);

	if (game->numOfPlayers == 2 && game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS)
	{
		takeAnotherCard(game);
		getNextPlayer(game);
		getNextPlayer(game);
	}

	else if (game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS)
		game->game_over = true;
	
	else
	{
		getNextPlayer(game);
		getNextPlayer(game);
	}
}

void takiCard(GAME* game, int player_choice) //this function takes action when the player has thrown TAKI card and let him throw cards with the same color only
{
	game->taki = false;

	updateUpperCard_AndRemove(game, player_choice); //update the upper card as TAKI card
	printArrOfCards(game); 

	while (game->taki == false && game->game_over == false)
	{
		player_choice = checkInputForTaki(*game, game->currentPlayer);

		if (player_choice != FINISH_TURN)
		{
			if (game->arrOfPlayers[game->currentPlayer].cards[player_choice - 1].num == CHANGE_COLOR)
			{
				changeColorCard(game, player_choice);
				game->taki = true;
			}

			else //if the player throw a card that is not CHANGE COLOR
			{
				updateUpperCard_AndRemove(game, player_choice); //update the upper card as the new card that the player throw 

				if (game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS && game->upperCard.num != PLUS)
				{
					game->game_over = true;
					game->taki = true;
				}

				else
					printArrOfCards(game);
			}
		}
		
		else //if the player choose to finish his turn
		{
			if (game->upperCard.num > LAST_REGULAR_CARD)
				specialCards(game, player_choice);
			
			else
				getNextPlayer(game);
			
			game->taki = true;
		}
	}
} 

void changeColorCard(GAME* game, int player_choice) //This function set the upper card as the change color card according to the color that the player chose with 'change color' card
{
	int colorChoice;
	printf("Please enter your color choice:\n1 - Yellow\n2 - Red\n3 - Blue\n4 - Green\n");
	scanf("%d", &colorChoice);
	game->upperCard.color = changeColor(&game, colorChoice);
	game->upperCard.num = CHANGE_COLOR;
	swapCardsAndRemove(game, player_choice - 1, game->currentPlayer, game->arrOfPlayers[game->currentPlayer].cards_num - 1); //swap the card that the player chose with the last card in the arr
	
	if (game->arrOfPlayers[game->currentPlayer].cards_num == NO_CARDS)
		game->game_over = true;
	else
		getNextPlayer(game);
}

int changeColor(GAME * game, int colorChoice) //This function get the color that the player chose and change the upper card color accordingly when got the 'Change Color' card
{
		char color[COLORS] = { 'R', 'B', 'G', 'Y' };

		switch (colorChoice)
		{
		case 1:
			game->upperCard.color = color[3];
			break;
		case 2:
			game->upperCard.color = color[0];
			break;
		case 3:
			game->upperCard.color = color[1];
			break;
		case 4:
			game->upperCard.color = color[2];
			break;
		}

		return  game->upperCard.color;
}

void swapCardsAndRemove(GAME* arr, int cardIndex1, int playerIndex, int cardIndex2) //Swap function between 2 indexes in an arr
{
	CARD tmp = arr->arrOfPlayers[playerIndex].cards[cardIndex1];
	arr->arrOfPlayers[playerIndex].cards[cardIndex1] = arr->arrOfPlayers[playerIndex].cards[cardIndex2];
	arr->arrOfPlayers[playerIndex].cards[cardIndex2] = tmp;

	arr->arrOfPlayers[arr->currentPlayer].cards_num--; //logic size - 1
}

int checkInput(GAME game, int i) //This function makes sure that the player chose a valid card according to the game orders
{
	bool valid = false;
	int player_choice;
	
	while (valid == false)
	{

		printf("\nPlease enter 0 if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle:\n", game.arrOfPlayers[i].cards_num);
		scanf("%d", &player_choice);

		while (player_choice < 0 || player_choice > game.arrOfPlayers[i].cards_num)
		{
			printf("Invalid choice! Try again.");
			printf("Please enter 0 if you want to take a card from the deck\nor 1 - %d if you want to put one of your cards in the middle:\n", game.arrOfPlayers[i].cards_num);
			scanf("%d", &player_choice);
		}

		if (player_choice == TAKE_CARD)
			return player_choice;

		else
		{

			if (game.arrOfPlayers[i].cards[player_choice - 1].color != game.upperCard.color && game.arrOfPlayers[i].cards[player_choice - 1].num != game.upperCard.num && game.arrOfPlayers[i].cards[player_choice - 1].num != CHANGE_COLOR)
				printf("Invalid card! Try again.");
			
			else
				valid = true;

		}
	}
	return player_choice;
}

int checkInputForTaki(GAME game, int i) //This function makes sure that the player chose a valid card when he throws cards after getting TAKI card 
{
	bool valid = false;
	int player_choice;

	while (valid == false)
	{
		printf("Please enter %d if you want to finish your turn\nor 1 - %d if you want to put one of your cards in the middle:\n", FINISH_TURN, game.arrOfPlayers[i].cards_num);
		scanf("%d", &player_choice);

		while (player_choice < 0 || player_choice > game.arrOfPlayers[i].cards_num)
		{
			printf("Invalid card! Try again.");
			printf("Please enter %d if you want to finish your turn\nor 1 - %d if you want to put one of your cards in the middle:\n", FINISH_TURN, game.arrOfPlayers[i].cards_num);
			scanf("%d", &player_choice);
		}

		if (player_choice == FINISH_TURN)
			return player_choice;

		else
		{
			if (game.arrOfPlayers[i].cards[player_choice - 1].color != game.upperCard.color && game.arrOfPlayers[i].cards[player_choice - 1].num != CHANGE_COLOR)
				printf("Invalid card! Try again.");

			else
				valid = true;
		}
	}
	return player_choice;
}

void print_card(CARD* card) //This function hold the pattern of each card
{
	printf("*********\n*       *\n");

	if (card->num == CHANGE_COLOR)
		printf("* COLOR *\n");
	else if (card->num <= LAST_REGULAR_CARD)
		printf("*  %2d   *\n", card->num);
	else if (card->num == PLUS)
		printf("*   +   *\n");
	else if (card->num == STOP)
		printf("*  STOP *\n");
	else if (card->num == DIRECTION)
		printf("*  <->  *\n");
	else if (card->num == TAKI)
		printf("*  TAKI *\n");

	printf("*   %c   *\n", card->color);
	printf("*       *\n");
	printf("*********\n");
}

CARD create_upper_card(GAME* game) //This function draw the upper card from the package randomly
{
	char color[COLORS] = { 'R', 'B', 'G', 'Y' };
	CARD res;

	res.num = (rand() % LAST_REGULAR_CARD) + 1;
	res.color = color[rand() % 4];

	updateStatistics(game, (res.num) - 1);
	return res;
}

CARD create_card(GAME* game) //This function draw the card from the package randomly
{
	char color[COLORS] = { 'R', 'B', 'G', 'Y' };
	CARD res;

	res.num = (rand() % CARDS_AMOUNT) + 1;
	if (res.num == CHANGE_COLOR)
		res.color = NO_COLOR;
	else
		res.color = color[rand() % 4];

	updateStatistics(game, (res.num) - 1);
	return res;
}

CARD* arrRealloc(CARD* arr, int size, int newSize) //This function change the size of the dynamic arr to a new size
{

	CARD* newArr = (CARD*)malloc(newSize * sizeof(CARD));
	int i;

	if (newArr)
	{
		for (i = 0; i < size && i < newSize; i++)
		{
			newArr[i].color = arr[i].color;
			newArr[i].num = arr[i].num;
		}
		free(arr);
	}

	return newArr;
}

void Initial_Statistics(GAME* game) //initial the data of the statistics
{
	int i;

	for (i = 0; i < CARDS_AMOUNT; i++)
	{
		game->stats[i].numOfCard = i + 1;
		game->stats[i].appearance = 0;
	}
}

void updateStatistics(GAME* game, int cardIndex) //updating the cards appearances
{
	game->stats[cardIndex].appearance++;
}

void MergeSort(statisCard stat[], int size) //this function sort the merged arr that we merged in 'merge' function
{
	statisCard* temp = NULL;
	if (size <= 1)
		return;

	MergeSort(stat, size / 2);
	MergeSort(stat + size / 2, size - size / 2);
	temp = (statisCard*)malloc(size * sizeof(statisCard));

	if (temp)
	{
		merge(stat, size / 2, stat + size / 2, size - size / 2, temp);
		copyArr(stat, temp, size);
		free(temp);
	}

	else
	{
		printf("ERROR! Memory allocation failed\n");
		exit(1);
	}

}

void merge(statisCard stat_arr1[], int size1, statisCard stat_arr2[], int size2, statisCard res[]) //this function is merging 2 arrs into 1 arr
{
	int ind1 = 0, ind2 = 0, resind = 0;

	while ((ind1 < size1) && (ind2 < size2))
	{
		if (stat_arr1[ind1].appearance > stat_arr2[ind2].appearance)
		{
			res[resind] = stat_arr1[ind1];
			ind1++;
		}
		else
		{
			res[resind] = stat_arr2[ind2];
			ind2++;
		}
		resind++;
	}
	while (ind1 < size1)
	{
		res[resind] = stat_arr1[ind1];
		ind1++;
		resind++;
	}
	while (ind2 < size2)
	{
		res[resind] = stat_arr2[ind2];
		ind2++;
		resind++;
	}
}

void copyArr(statisCard stat_arr1[], statisCard stat_arr2[], int size) //this function Copies one arr to another
{
	int i;

	for (i = 0; i < size; i++)
	{
		stat_arr1[i] = stat_arr2[i];
	}
}

void cardsStatistics(GAME* game) //this function shows us the statistics of how many times each card appeared
{
	int i;

	MergeSort(game->stats, CARDS_AMOUNT);
	printf("\n************ Game Statistics ************\nCard # | Frequency\n__________________\n");

	for (i = 0; i < CARDS_AMOUNT; i++)
	{
		switch (game->stats[i].numOfCard)
		{
		case PLUS:
		{
			printf("   +   |   %d  \n", game->stats[i].appearance);
			break;
		}
		case STOP:
		{
			printf(" STOP  |   %d  \n", game->stats[i].appearance);
			break;
		}
		case DIRECTION:
		{
			printf("  <->  |   %d  \n", game->stats[i].appearance);
			break;
		}
		case TAKI:
		{
			printf(" TAKI  |   %d  \n", game->stats[i].appearance);
			break;
		}
		case CHANGE_COLOR:
		{
			printf(" COLOR |   %d  \n", game->stats[i].appearance);
			break;
		}
		default: //regular card case
			printf("   %d   |   %d  \n", game->stats[i].numOfCard, game->stats[i].appearance);
			break;
		}
	}
}

void freeArrAfterWin(GAME* game) //free the memory of the dynamic arrs that we Assigned
{ 
	int i, j;
	for (i = 0; i < game->numOfPlayers; i++)
	{
		free(game->arrOfPlayers[i].cards);
	}

	free(game->arrOfPlayers);
}