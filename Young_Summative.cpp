/*Anastasia Young Summative, December 20, 2017
ISC3U, Mrs Cullum*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>

#define COLUMN_MULTIPLIER 65
#define ROW_MULTIPLIER 95

const int SCREEN_W = 325;       // screen width
const int SCREEN_H = 700;       // screen height
const int BOUNCER_SIZE = 32;

//characteristics of a card
struct CardPerameterType {
    int colour;
    int shape;
    int numShape;
    int shading;
};
enum statusType {
    NotDrawn,
    Drawn,
    Discarded
};
//stores image of card and status
struct CardType {
    statusType status;
    ALLEGRO_BITMAP *image;
};
//all buttons and cards in top row of game(above dealt cards)
struct BannerType {
    ALLEGRO_BITMAP *hintImage;
    ALLEGRO_BITMAP *helpImage;
    ALLEGRO_BITMAP *instructions;
    int firstChosenCard;
    int secondChosenCard;
    int thirdChosenCard;
    bool help;
};
//structure that is the user with their name and highscore
struct Player{
    char name[40];
    float highScore;
};

bool dealCard(int deck[], struct CardType card[], struct BannerType banner, int drawnCards[], int &drawnCardIndex, int &setIndex);
int readCard(int cardNumber, CardPerameterType &cardCharacteristics);
int findMatch(int firstCard, int secondCard);
void shuffleDeck(int deck[]);
void displayCards(struct CardType card[], int drawnCards[], int drawnCardIndex, struct BannerType banner);

int main(int argc, char *argv[]){

    srand(-1);
    FILE *fptr = NULL;

    int userScore;
    fptr = fopen ("Highscores.txt", "r");
    Player bestPlayers[6];
    Player currentPlayer;
    Player swapPlayers[6];
    int playerNum = 0;
    int firstNotDrawn = 81;
    int deck[81];
    char filename[20] = "card";
	CardType card[81];
	BannerType banner ={};
	banner.firstChosenCard = 81;
	banner.secondChosenCard = 81;
	banner.thirdChosenCard = 81;
	ALLEGRO_DISPLAY *display = nullptr;
    int drawnCards[81] = {0};
    int drawnCardIndex = 0;
    int row[18];
    int column[18];
	float bouncer_x = 0;
	float bouncer_y = 0;
    ALLEGRO_EVENT_QUEUE *event_queue = NULL;
    time_t startTime = 0;
    time_t endTime = 0;
    banner.help = false;
    int setIndex = 0;

	// Initialize Allegro
	al_init();

	//install mouse
	if(!al_install_mouse()) {
      fprintf(stderr, "failed to initialize the mouse!\n");
      return -1;
   }

	// Initialize display
	display = al_create_display(SCREEN_W, SCREEN_H);
	if (!display) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize display!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
       	return -1;
	}
	al_set_window_title(display, "SET Summative Game");

   	// need to add image processor
 	if (!al_init_image_addon()) {
    	al_show_native_message_box(display, "Error", "Error", "Failed to initialize image addon!",
                                 nullptr, ALLEGRO_MESSAGEBOX_ERROR);
    	return 0;
	}

    //enables finding mouse click coordinates
	event_queue = al_create_event_queue();
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_mouse_event_source());

    //Input by user for username
    printf("Enter username: ");
    scanf("%s", currentPlayer.name);

    //start timer
    time(&startTime);
    printf("%d\n", startTime);

	//load all bitmaps to an array
	for(int i = 0; i<81; i++){
        sprintf(filename, "card%d.bmp",i);
        printf("%s\n", filename);
        card[i].image = al_load_bitmap(filename);
	}

    //initialize hint image
    banner.hintImage = al_load_bitmap("Hint.bmp");

    //initialize help image
    banner.helpImage = al_load_bitmap("Help.bmp");

    //initialize instructions bitmap
    banner.instructions = al_load_bitmap("instructions.bmp");

    //declare all cards as undrawn
    for (int i = 0; i<81; i++){
        card[i].status = NotDrawn;
        deck[i] = i;
    }

    //shuffle deck
    shuffleDeck(deck);

    //print deck
    for(int i = 0; i<81; i++){
        printf("%d\n", deck[i]);
    }
    /*
    //chosen cards where there is no set to test if the program can deal
    //more cards until there is a set
    deck[1] = 27;
    deck[27] = 1;
    deck[2] = 28;
    deck[28] = 2;
    deck[3] = 36;
    deck[36] = 3;
    deck[4] = 37;
    deck[37] = 4;
    deck[5] = 42;
    deck[42] = 5;
    deck[7] = 43;
    deck[43] = 7;
    deck[8] = 34;
    deck[34] = 8;
    deck[11] = 33;
    deck[33] = 11;
*/
/*  loop that enables the deck to be smaller to test corner cases in the code
    and not have to run through the entire deck each time

    for (int i = 0; i<81; i++){
        if (i>42){
            card[i].status = Drawn; ALLEGRO_EVENT_QUEUE *event_queue = NULL;

        }
        else{
            card[i].status = Discarded;
        }
        //card[i].status = NotDrawn;
        deck[i] = i;
    }*/

    //find number of cards undealth
    for (int i = 0; i < 81; i++){
        if (card[deck[i]].status == NotDrawn){
            firstNotDrawn = i;
            break;
        }
    }
    //print first card in deck that is not drawn
    printf("%d", firstNotDrawn);

    //Loop that executes the entire game - loops until all cards are dealt
    while (dealCard(deck, card, banner, drawnCards, drawnCardIndex, setIndex) == true){

        while(1){

            ALLEGRO_EVENT ev;
            al_wait_for_event(event_queue, &ev);

            //when the mouse is clicked
            if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                bouncer_x = ev.mouse.x;
                bouncer_y = ev.mouse.y;
                //prints coordinates of the mouse
                printf("x position: %f", bouncer_x);
                printf("y position: %f", bouncer_y);

                //check to see that display is displaying cards not the manual
                if(banner.help == false){

                    //check for user clicking help button
                    if((bouncer_x>260 && bouncer_x<325)&&(bouncer_y<95)){
                        banner.help = true;
                    }
                    else{
                        //check for user clicking card - show selected card in banner
                        for(int i = 0; i<drawnCardIndex;i++){
                            row[i] = i / 3;
                            column[i] = i % 3;
                            if ((bouncer_x > (column[i] + 1) * COLUMN_MULTIPLIER && bouncer_x < (column[i]+2) * COLUMN_MULTIPLIER)
                               && (bouncer_y > (row[i]+1) * ROW_MULTIPLIER && bouncer_y < (row[i]+2)*ROW_MULTIPLIER)){
                                if(drawnCards[i]!=banner.firstChosenCard &&
                                   drawnCards[i]!=banner.secondChosenCard &&
                                   drawnCards[i]!=banner.thirdChosenCard){
                                    if(banner.firstChosenCard == 81){
                                        banner.firstChosenCard = drawnCards[i];
                                    }else if(banner.secondChosenCard == 81){
                                        banner.secondChosenCard = drawnCards[i];
                                    }else if(banner.thirdChosenCard == 81){
                                        banner.thirdChosenCard = drawnCards[i];
                                    }
                                    break;
                                }
                            }
                        }

                        //if user clicks a card they have already selected, it returns to it's original place
                        if((bouncer_x>65 && bouncer_x<130) && bouncer_y < 95){
                            banner.firstChosenCard = 81;
                        }else if((bouncer_x>130 && bouncer_x<195) && bouncer_y < 95){
                            banner.secondChosenCard = 81;
                        }else if((bouncer_x>195 && bouncer_x<260) && bouncer_y < 95){
                            banner.thirdChosenCard = 81;
                        }else if((bouncer_x>0 && bouncer_x<65) && bouncer_y < 95){
                            //checks if user clicks hint button
                            //shows first card in the set if user asks for hint
                            banner.firstChosenCard = drawnCards[setIndex];
                            banner.secondChosenCard = 81;
                            banner.thirdChosenCard = 81;
                        }
                    }
                }else{
                    //display is showing instructions - puts it back to game mode
                    banner.help = false;
                }
                break;
            }
        }

        //checks chosen set
        if(banner.firstChosenCard <81 &&
           banner.secondChosenCard<81&&
           banner.thirdChosenCard<81){
            displayCards(card, drawnCards, drawnCardIndex, banner);
            if(findMatch(banner.firstChosenCard, banner.secondChosenCard) == banner.thirdChosenCard){
                //discards cards in set if it is valid
                card[banner.firstChosenCard].status = Discarded;
                card[banner.secondChosenCard].status = Discarded;
                card[banner.thirdChosenCard].status = Discarded;
            }
            //puts cards back into dealt position if set is not valid
            banner.firstChosenCard = 81;
            banner.secondChosenCard = 81;
            banner.thirdChosenCard = 81;
        }
    }

    //end timer
    time(&endTime);
    printf("%d\n", endTime);
    //calculate time it took for user to play game
    currentPlayer.highScore = endTime - startTime;
    printf("%.2f\n", currentPlayer.highScore);

    //error message if textfile does not open
    if (fptr == NULL) {
        printf("Unable to open textfile");
    }else{
        //read in the five players and store info in a structure
        while (fscanf(fptr, "%s", &bestPlayers[playerNum].name) == 1){
            fscanf(fptr, "%f", &bestPlayers[playerNum].highScore);
            playerNum++;
        }
        //close textfile
        fclose(fptr);

        //copies highscores and users to seperate array to swap
        for(int i=0; i<5;i++){
            swapPlayers[i].highScore = bestPlayers[i].highScore;
            strcpy(swapPlayers[i].name, bestPlayers[i].name);
        }

        //compare user's highscore with that of the five players on leaderboard
        for(int i = 0; i<5; i++){
            //printf("i:%d\n", i);
            //when current player has beat a highscore
            if (currentPlayer.highScore<swapPlayers[i].highScore){

                //copies the names and highscores of the users that the player beat into the spot below
                for(int j = i+1; j<5; j++){
                    printf("j:%d\n", j);
                    bestPlayers[j].highScore = swapPlayers[j-1].highScore;
                    strcpy(bestPlayers[j].name, swapPlayers[j-1].name);
                }
                //puts user into the highscores array
                bestPlayers[i].highScore = currentPlayer.highScore;
                strcpy(bestPlayers[i].name, currentPlayer.name);
                break;
            }
        }
    }

    //prints updated highscores
    printf("\nHIGHSCORES:\n");
    for(int i = 0; i<playerNum; i++){
           printf("%d: %s ", (i+1), bestPlayers[i].name);
            printf("%.1f\n", bestPlayers[i].highScore);
    }

    fptr = fopen("Highscores.txt", "w");
    if (fptr == NULL){
        //error message if textfile cannot be opened
        printf("Cannot open textfile");
    }else{
        //rewrites all highscores to textfile
        for (int i = 0; i<5; i++){
            fprintf(fptr, "%s\t", bestPlayers[i].name);
            fprintf(fptr, "%.1f\n", bestPlayers[i].highScore);
        }
    //close textfile
        fclose(fptr);
    }

    //close allegro display
    al_destroy_display(display);
    al_destroy_event_queue(event_queue);

    return 0;
}

bool dealCard(int deck[], CardType card[], BannerType banner, int drawnCards[], int &drawnCardIndex, int &setIndex){
    bool setFound = false;
    int numDrawn = 0;
    int numNotDrawn = 81;
    int firstNotDrawn = 81;
    int requiredDrawnCards = 12;

    //deals until a set is found in the drawn cards or there are no more cards to deal
    while(setFound == false && numNotDrawn > 0){

        numNotDrawn = 0;
        numDrawn = 0;

        //Find out how many cards need to be dealt
        for (int i = 0; i < 81; i++){
            if (card[deck[i]].status == Drawn){
                numDrawn++;
            }
            else if (card[deck[i]].status == NotDrawn){
                numNotDrawn = 81 - i;
                firstNotDrawn = i;
                break;
            }

        }
        printf("Num Drawn: %d\n", numDrawn);
        printf("Num not drawn: %d\n", numNotDrawn);
        printf("first not drawn: %d\n", firstNotDrawn);

        //find number of cards needed to deal
        int cardsToDeal = requiredDrawnCards - numDrawn;

        printf("cards to deal: %d\n", cardsToDeal);
        printf("Required drawn cards: %d\n", requiredDrawnCards);
        printf("num drawn: %d\n\n", numDrawn);

        //For cases where number of cards needed is greater than number of cards left
        if (numNotDrawn < cardsToDeal){
            //deals remaining cards
            cardsToDeal = numNotDrawn;
        }
        printf("Cards to deal: %d\n", cardsToDeal);

        //deals the cards
        for (int i = 0; i<cardsToDeal; i++){
            card[deck[firstNotDrawn+i]].status = Drawn;
        }

        for(int i = 0; i<cardsToDeal; i++){
            printf("card %d: %d\n", i, deck[firstNotDrawn+i]);
            printf("status: %d\n", card[deck[firstNotDrawn+i]].status);
        }

        drawnCardIndex = 0;

        //new array with only cards that are currently drawn
        for (int i = 0; i < 81; i++){
            if (card[i].status == Drawn){
                drawnCards[drawnCardIndex] = i;
                printf("drawn card %d: %d\n", drawnCardIndex, drawnCards[drawnCardIndex]);
                drawnCardIndex++;
            }
        }

        //display the cards drawn
        displayCards(card, drawnCards, drawnCardIndex, banner);

        //checking for set in drawn cards(comparing combination of 2 cards and checking if the third match is drawn)
        int matchCard = 0;
        for (int i = 0; i < (drawnCardIndex - 1); i++){
            for (int j = (i + 1); j < drawnCardIndex; j++){

                //function that returns true if the match card is drawn
                matchCard = findMatch(drawnCards[i], drawnCards[j]);
                if (card[matchCard].status == Drawn){
                    setIndex = i;
                    setFound = true;
                    printf("first card: %d\nsecond card: %d\nmatch card: %d\n", drawnCards[i], drawnCards[j], matchCard);
                    break;
                }
            }
            //stops searching for sets when a set is found
            if (setFound == true){
                break;
            }
        }

        //deals three more cards if there is no set in the current drawn cards
        if (setFound == false){
            requiredDrawnCards += 3;
        }
    }
    return setFound;
}

//Calculate all characteristics of a card based only on it's card number
int readCard(int cardNumber, CardPerameterType &cardCharacteristics){
    cardCharacteristics.colour = ((cardNumber / 9) % 3);
    cardCharacteristics.numShape = cardNumber % 3;
    cardCharacteristics.shape = ((cardNumber / 3)% 3);
    cardCharacteristics.shading = ((cardNumber / 27)% 3);
    return 0;
}

int findMatch(int firstCard, int secondCard){

    CardPerameterType firstCardType = {0};
    CardPerameterType secondCardType = {0};
    CardPerameterType matchCardType = {0};

    readCard(firstCard, firstCardType);
    readCard(secondCard, secondCardType);

    //Compare numerical characteristics to calculate the characteristics of the match card

    //Find colour of match card
    if (firstCardType.colour == secondCardType.colour){
        matchCardType.colour = firstCardType.colour;
    }
    else{
        matchCardType.colour = (3 - firstCardType.colour - secondCardType.colour);
    }

    //find number of shapes on match card
    if (firstCardType.numShape == secondCardType.numShape){
        matchCardType.numShape = firstCardType.numShape;
    }
    else{
        matchCardType.numShape = (3 - firstCardType.numShape - secondCardType.numShape);
    }

    //find shape type on match card
    if (firstCardType.shape == secondCardType.shape){
        matchCardType.shape = firstCardType.shape;
    }
    else{
        matchCardType.shape = (3 - firstCardType.shape - secondCardType.shape);
    }

    //find shading type on match card
    if (firstCardType.shading == secondCardType.shading){
        matchCardType.shading = firstCardType.shading;
    }
    else{
        matchCardType.shading = (3 - firstCardType.shading - secondCardType.shading);
    }

    //Use match card's calculated characteristics to convert it to it's card number
    int matchCard = (matchCardType.shading * 27) +
                    (matchCardType.colour * 9) +
                    (matchCardType.shape * 3) +
                    (matchCardType.numShape);

    return matchCard;
}

void shuffleDeck(int deck[]){
    int cardsLeft[81];
    int maxIndex = 0;
    int swapIndex = 0;

    //array of numbers to choose a random number from
    for (int i = 0; i<81; i++){
        cardsLeft[i] = i;
    }

    //pick random cards from cardsLeft and place in deck
    for (int i = 0; i<81; i++){
        maxIndex = 81 - i;
        swapIndex = rand() %maxIndex;
        deck[i] = cardsLeft[swapIndex];
        cardsLeft[swapIndex] = cardsLeft[maxIndex-1];
        maxIndex--;
    }
}

void displayCards(struct CardType card[], int drawnCards[], int drawnCardIndex, struct BannerType banner){
    int row[18];
    int column[18];

    //clear screen
    al_clear_to_color(al_map_rgb(0,0,0));
    if(banner.help == false){

        //display help and hint button
        al_draw_bitmap(banner.helpImage, 260, 0, 0);
        al_draw_bitmap(banner.hintImage, 0, 0, 0);

        //display three chosen cards
        if (banner.firstChosenCard<81){
            al_draw_bitmap(card[banner.firstChosenCard].image, 65, 0, 0);
        }
        if(banner.secondChosenCard<81){
            al_draw_bitmap(card[banner.secondChosenCard].image, 130, 0, 0);
        }
        if(banner.thirdChosenCard<81){
            al_draw_bitmap(card[banner.thirdChosenCard].image, 195, 0, 0);
        }

        //calculate row and column of each drawn card
        for (int i = 0; i < drawnCardIndex; i++){
            if (drawnCards[i] != banner.firstChosenCard &&
                drawnCards[i] != banner.secondChosenCard &&
                drawnCards[i] != banner.thirdChosenCard ) {
                row[i] = i / 3;
                column[i] = i % 3;
                //display drawn cards
                al_draw_bitmap(card[drawnCards[i]].image,(column[i]+1) * COLUMN_MULTIPLIER, (row[i]+1) * ROW_MULTIPLIER, 0);
                al_flip_display();
            }
        }
    }else{
        //display bitmap with instructions on how to play game
        al_draw_bitmap(banner.instructions, 15, 95, 0);
        al_flip_display();
    }
    al_rest(1);
}
