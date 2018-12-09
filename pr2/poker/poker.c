#include <stdio.h>
#include <stdlib.h>


typedef enum suit_s {DIAMONDS, CLUBS, HEARTS, SPADES} Suit;

typedef struct card_s{
  int rank;
  Suit suit;
} *Card;

typedef struct hand_s{
  Card cards[5];
} *Hand;

char * getSuit(Suit s);
Card createCard();
void printCard(Card c);
void destroyCard(Card c);
int isEqual(Card c1, Card c2);
Hand createHand();
void printHand(Hand h);
void destroyHand(Hand h);
int isStraightFlush(Hand h);





char * getSuit(Suit s){
  switch(s){
  case DIAMONDS:
    return "diamonds";
    break;
  case CLUBS:
    return "clubs";
    break;
  case HEARTS:
    return "hearts";
    break;
  case SPADES:
    return "spades";
    break;
  }
}

//creates a random card
//strangely, I had to typecast the void pointer from malloc to a Suit
//it wouldn't compile otherwise
Card createCard(){
  Card c = malloc(sizeof(Card));
  c->rank = 1 + (rand() % 13);
  c->suit =(Suit) malloc(sizeof(Suit));
  c->suit = rand() % 4;
  return c;
}

//prints out rank and suit of the passed card
void printCard(Card c){
  printf("%d : %s\n", c->rank, getSuit(c->suit));
}

void destroyCard(Card c){
  free(c);
}

//returns 1 if c1 == c2, otherwise 0
int isEqual(Card c1, Card c2){
  return (c1->rank == c2->rank) && (c1->suit == c2->suit);
}


//creates a hand that contains 5 random cards that are not duplicates
Hand createHand(){
  Hand h = malloc(sizeof(Hand));
  int cardCount = 0;

  while(cardCount < 5){
    Card c = createCard();
    

    //prevents duplicates from being added to the hand
    //whenever we generate a new card, check all the already added cards for any duplicates
    //if it's a duplicate, generate another card and destroy the bad one
    int isDuplicate = 0;
    int checkDuplicatesPos = 0;
    while(checkDuplicatesPos < cardCount){
      if(isEqual(c, h->cards[checkDuplicatesPos])){
	isDuplicate = 1;
      }
      checkDuplicatesPos++;
    }
    if(isDuplicate == 0){

      h->cards[cardCount] = c;
      cardCount++;
    }
    else{
      destroyCard(c);
    }
  }

  //the first card in the array kept getting overwritten to a seemingly NULL value
  //so this generates a new card and reassigns it
  int firstFixed = 0;
  while(!firstFixed){
    Card c = createCard();
    int n = 1;
    int isDuplicate = 0;
    while(n < 5){
      if(isEqual(c, h->cards[n])){
	isDuplicate = 1;
      }
      n++;
    }

    if(isDuplicate == 0){
      h->cards[0] = c;
      firstFixed = 1;
    }
    else{
      destroyCard(c);
    }
  }

  return h;

}
//prints hand by calling printCard() 5 times
void printHand(Hand h){
  int n = 0;
  while(n < 5){
    printCard(h->cards[n]);
    n++;
  }
}

//deallocates hand by deallocating each card individually via destroyCard()
void destroyHand(Hand h){
  int n = 0;
  while(n < 5){
    destroyCard(h->cards[n]);
    n++;
  }
}

int isStraightFlush(Hand hand){

  //first check if all the suits are the same, if not return 0
  if(hand->cards[0]->suit == hand->cards[1]->suit && hand->cards[0]->suit == hand->cards[2]->suit && hand->cards[0]->suit == hand->cards[3]->suit && hand->cards[0]->suit == hand->cards[4]->suit){
    int n = 0;
    //sort the ranks in ascending order, to make it easier to determine if it's a straight flush
    while(n < 5){
      int m = 0;
      while(m < 5){
	if(hand->cards[n]->rank < hand->cards[m]->rank){
	  Card tmp = hand->cards[n];
	  hand->cards[n] = hand->cards[m];
	  hand->cards[m] = tmp;
	}
	m++;
      }
      n++;
    }

    //if we specifically have an ace, ten, jack, queen, king, we have a straight flush
    if(hand->cards[0]->rank == 1 && hand->cards[1]->rank == 10 && hand->cards[2]->rank == 11 && hand->cards[3]->rank == 12 && hand->cards[4]->rank == 13){
      return 1;
    }
    else{
      //otherwise see if the ranks are in simple incrementing order
      int n = 0;
      while(n < 4){
	if(hand->cards[n]->rank != (hand->cards[n+1]->rank) - 1){
	  return 0;
	}
	n++;
      }
      return 1;
    }

  }
  else{
    return 0;
  }
}

int main(int argc, char *argv[]){

  if(argc == 2){
    int trials = atoi(argv[1]);
    srand(time(NULL));
    int count = 0;
    int trialnum = 0;
    while(trialnum < trials){
      Hand h = createHand();
      if(isStraightFlush(h)){
	count++;
      }
      destroyHand(h);
      trialnum++;
    }


    printf("Total Straight Flushes from %d Trials: %d\n", trials, count);

    double fraction = ((double) count) /((double) trials);

    printf("Fraction of Trials that were Straight Flushes: %f\n\n", fraction);
  }
  else{
    printf("Error: incorrect program commands\n");
    printf("Program usage: [program] [number of trials]\n\n");
  }
}
