#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

/*************** STRUCTURES *******************************/

struct Sayilar {
	int data;
	struct Sayilar *next;
};
struct Islemler {
	char data; // '+', '-', '*'
	struct Islemler *next;
};

struct StackOps {
	char op; 
	struct StackOps *next;
};
struct StackVals {
	int value;
	struct StackVals *next;
};

/*************** STRUCTURES - END *************************/


/*************** CONSTANTS ********************************/

#define RED     "\x1b[31m"
#define GREEN   "\x1b[32m"
#define YELLOW  "\x1b[33m"
#define BLUE    "\x1b[34m"
#define MAGENTA "\x1b[35m"
#define CYAN    "\x1b[36m"
#define RESET   "\x1b[0m"

/*************** CONSTANtS - END *************************/


/*************** GAME CONFIGS  ***************************/
	

struct Sayilar *sayilar = NULL;
struct Islemler *islemler = NULL;
struct StackOps *ops = NULL;   		// Operatorler
struct StackVals *vals = NULL;

const char randomIslem[3] = {'+','-','*'};
const int falseAnswer = 3; // yanlis cevap skor azalt 
const int trueAnswer = 7; // dogru cevap skor arttir
int HAK = 3; // Max Yanlış yapma hakkı
int result = 0;

void randomSayiIslem(void);
void startGame(void);
void gameMenu(void);
time_t start, end;

int rear = -1, front = 0, counter = 0; // queue variables
int const QUEUE_SIZE = 50;
int queue[QUEUE_SIZE];



/*************** GAME CONFIGS - END ************************/


/*************** USER CONFIGS ******************************/

int currentLevel = 1;
int currentScore = 0;
int LevelMaxScores[3] = {25,50,99999};
char mIslem[3]; // Kullanıcının cevapları


/************** USER CONFIGS - END ************************/


void addSayi(int sayi) {
	struct Sayilar * cursor = sayilar;
	struct Sayilar *yenisayi = (struct Sayilar*)malloc(sizeof(struct Sayilar));
	yenisayi -> data = sayi;
		
	if(cursor == NULL) { //insertFirst
		yenisayi -> next = sayilar;
		sayilar = yenisayi;
	}
	else { // insertLast
		yenisayi -> next = NULL;
		while(cursor -> next != NULL) cursor = cursor -> next;
		cursor -> next = yenisayi;
	}
}
void addIslem(char islem) {
	struct Islemler * cursor = islemler;
	struct Islemler *yeniislem = (struct Islemler*)malloc(sizeof(struct Islemler));
	yeniislem -> data = islem;
		
	if(cursor == NULL) { //insertFirst
		yeniislem -> next = islemler;
		islemler = yeniislem;
	}
	else { // insertLast
		yeniislem -> next = NULL;
		while(cursor -> next != NULL) cursor = cursor -> next;
		cursor -> next = yeniislem;
	}
}
void printIslem() {
	struct Islemler *cursor = islemler;
	while(cursor != NULL) {
		printf("%c ", cursor -> data);
		cursor = cursor -> next;
	}
	printf("\n");
}
void printSayi() {
	struct Sayilar *cursor = sayilar;
	while(cursor != NULL) {
		printf("%d ", cursor -> data);
		cursor = cursor -> next;
	}
	printf("\n");
}

void pushVals(int value) { 
	struct StackVals* tmp = (struct StackVals*)malloc(sizeof(struct StackVals));

	tmp -> value = value;
	tmp -> next = vals;
	vals = tmp;
}
int popVals() {
	struct StackVals* tmp = vals;
    int output = vals -> value;
    vals = vals -> next;
    free(tmp);
    return output;
}

void pushOps(char op) {

    struct StackOps* tmp = (struct StackOps*)malloc(sizeof(struct StackOps));
   
	tmp -> op = op;
    tmp -> next = ops;
    ops = tmp;
}
char popOps() {
    struct StackOps* tmp = ops;
    char output = ops -> op;
    ops = ops -> next;
    free(tmp);
    return output;
}
int evaluate(char String[32]) {
	int i = 0;
	char c = String[0];
	while(c != '\0') {
	
		if(c == '(' || c == ' ');
		else if(c == '+') pushOps(c);
		else if(c == '-') pushOps(c);
		else if(c == '*') pushOps(c);
		else if(c == ')') { // Buraya kadarki sonucu hesapla
			
			char op = popOps();
			int value = popVals();
			if(op == '+') value = popVals() + value;
			else if(op == '-') value = popVals() - value;
			else if(op == '*') value = popVals() * value;
			pushVals(value);
		}
		else pushVals(c - '0'); 

		c = String[++i];
	}
	return popVals();
}
bool isFullQueue() {
	return (counter == QUEUE_SIZE);
}
bool isEmptyQueue()  {
	return (counter == 0);
}
void enqueue(int value) {
	
	if(!isFullQueue()) {
		if(rear == QUEUE_SIZE-1) rear = -1;
		queue[++rear] = value;
		counter++;
	}
}
int dequeue() {
	
	if(!isEmptyQueue()) {
		if(front == QUEUE_SIZE-1)
			front = 0;
		int data = queue[front++];
		counter--;
		return data;
	}
	else {
		return NULL;
	}
}
int peekQueue()
{
	if(!isEmptyQueue())
		return queue[front];
	else return NULL;
}
void gameOver() {

	int min = int(difftime(end,start)/60);
	int sec = int(difftime(end,start)) % 60;
	enqueue(currentScore);
	system("cls");
	printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n\n");
	printf(CYAN "   SKOR : %d \t\t   GECEN ZAMAN : %.2d:%.2d\n\n", currentScore, min,sec);
	printf(GREEN "\n\n================================================\n");
	printf(RED "Ana Menuye donmek icin bir tusa basiniz.\n");
	_getch();

}
void checkAnswer() {
	bool block = false;
	for(int i=0;i<currentLevel;i++) { // 0 i 2 olana kadar 
		if(!(mIslem[i] == '+' ||  mIslem[i] == '-' || mIslem[i] == '*')) {
			//currentScore -= falseAnswer;
			//HAK--;
			block = true;
			startGame();
		}
	}
	if(!block) {
		int userResult = 0;
		char tmpSoru[13];
		switch(currentLevel) {
			case 1: {
				sprintf(tmpSoru,"(%d %c %d)", sayilar -> data, mIslem[0], sayilar -> next -> data);
				userResult = evaluate(tmpSoru);
				break;
			}
			case 2: {
				sprintf(tmpSoru,"(%d %c ( %d %c %d ))",sayilar -> data, mIslem[0], sayilar -> next -> data, 
				mIslem[1], sayilar -> next -> next -> data);
				userResult = evaluate(tmpSoru);
				break;
			}
			case 3: {
				sprintf(tmpSoru,"((%d %c (%d %c %d)) %c %d)",sayilar -> data, mIslem[0], sayilar -> next -> data,
				mIslem[1], sayilar -> next -> next -> data, mIslem[2], sayilar -> next -> next -> next -> data);
				userResult = evaluate(tmpSoru);
				break;
			}
			default:break;
		}
		if(result == userResult) { // Doğru Cevap

			system("cls");
			printf(CYAN "\n\n\n=================  ISLEMI BUL ==================\n\n\n");
				
			currentScore += trueAnswer;
			if(currentScore >= LevelMaxScores[currentLevel-1]) { // LevelUp
				printf(GREEN "\tTEBRIKLER! Ust seviyeye ulastiniz\n\n");
				currentLevel++;
				printf(CYAN "\n=============================================\n");
				sleep(3);
			}
			else {
				printf(GREEN "\t\tDogru Cevap!\n");
				printf(CYAN "\n\n================================================\n");
				sleep(1);
			}
			randomSayiIslem();
			startGame();
		}
		else { // Yanlış Cevap
			currentScore -= falseAnswer;
			HAK--;
			if(HAK <= 0) {
				time(&end);
				gameOver();
			}
			else startGame();
		}
	}
}
void createSayi() {
	static int i = 0;

	addSayi(rand()%9+1);
	if(i < currentLevel +1) {
		i++;
		createSayi();
	}
	else {
		i = 0;
	}
}
void createIslem() {
	static int j = 0;
	addIslem(randomIslem[rand()%3]);
	if(j < currentLevel) {
		j++;
		createIslem();
	}
	else {
		j = 0;
	}
}
void randomSayiIslem() {
	sayilar = NULL; // Sayilar listesini bosalt.
	islemler = NULL; // Islemler listesini bosalt.
	
	createSayi();
	createIslem();
	char Soru[13];
	switch(currentLevel) {
		case 1: {

			sprintf(Soru,"(%d %c %d)", sayilar -> data, islemler -> data, sayilar -> next -> data);
			result = evaluate(Soru);
			break;
		}
		case 2: {
			sprintf(Soru,"(%d %c ( %d %c %d ))",sayilar -> data, islemler -> data, sayilar -> next -> data, 
				islemler -> next -> data, sayilar -> next -> next -> data);
			result = evaluate(Soru);
			break;
		}
		case 3: {
			sprintf(Soru,"((%d %c (%d %c %d)) %c %d)",sayilar -> data, islemler -> data, sayilar -> next -> data,
				islemler -> next -> data, sayilar -> next -> next -> data, islemler -> next -> next -> data,
				sayilar -> next -> next -> next -> data);
			result = evaluate(Soru);
			break;
		}
	}
}
void startGame() {
	system("cls");
	printf(GREEN "\n\n\n===============  ISLEMI BUL ===============\n");
	printf(CYAN "   SEVIYE : %d \t\t\tSKOR : %d\n\n", currentLevel, currentScore);
	
	switch(currentLevel) {
		case 1: {
			printf(RESET "\t\t(%d [?] %d) = %d\n", sayilar -> data, sayilar -> next -> data, result);
			printf(CYAN "\n   Ust Seviye icin gereken puan : %d\n", LevelMaxScores[currentLevel-1] - currentScore);
			break;
		}
		case 2: {
			printf(RESET "\t(%d [?] ( %d [?] %d )) = %d\n", sayilar -> data, sayilar -> next -> data, sayilar -> next -> next -> data, result);
			printf(CYAN "\n   Ust Seviye icin gereken puan : %d\n", LevelMaxScores[currentLevel-1] - currentScore);
			break;
		}
		case 3: {
			printf(RESET "     ((%d [?] (%d [?] %d)) [?] %d) = %d\n", sayilar -> data, sayilar -> next -> data, 
				sayilar -> next -> next -> data , sayilar -> next -> next -> next -> data,result);
			printf(CYAN "\n\tSon Seviyeye ulastiniz\n");
			break;
		}
		default:break;
	}
	printf(RESET "\n\tYANLIS YAPMA HAKKINIZ : ");printf(RED "%d\n" RESET,HAK);
	printf(GREEN "\n===========================================\n");
	printf(YELLOW "Cevap : " RESET);
	char cevap[3];
	scanf("%s",&cevap);
	for(int i=0;i<3;i++) mIslem[i] = cevap[i];
	checkAnswer();
}
void prepareGame() {
	HAK = 3;
	currentLevel = 1;
	currentScore = 0;
	randomSayiIslem();
	for(int i=5;i>=0;i--) {
		system("cls");
		printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n\n");
		printf(CYAN "SAYILAR ARASINDAKI ISLEMLERI BUL, BOLUMLERI GEC!");

		if(i != 0)	{
			printf("\n         OYUN ");printf(RED "%d",i);printf(CYAN " SANIYE SONRA BASLAYACAK" RESET);
		}
		else printf(YELLOW "\n\t         OYUN BASLIYOR!", i);

		printf(GREEN "\n\n\n================================================\n");
		sleep(1);
	}
	time(&start);
	startGame();
}
void firstScores() {

	system("cls");
	printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n");
	if(isEmptyQueue()) printf(RED "Su anda kayitli veri yok.\n\n");
	else {
		printf(CYAN "\tSKOR : %d\n", peekQueue());
	}
	printf(CYAN "1. Bir Sonraki Skora bak\n");
	printf("2. Ana Menuye Don\n");

	printf(GREEN "\n================================================\n");
	int choosen;
	scanf("%d",&choosen);
	
	if(choosen == 1) {
		dequeue();
	}
	
	else gameMenu();
	printf(RED "Ana menuye donmek icin bir tusa basiniz.\n" RESET);
	_getch();
}
void gameTutorial() {
	system("cls");
	printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n");
	printf(CYAN "Ana Menuden 'Oyuna Basla' secenegini secin.\n");
	sleep(2);
	printf(RED "\nBir sonrakine gecmek icin herhangi bir tusa basiniz.\n");
	printf(GREEN "\n================================================\n");
	_getch();
	system("cls");
	printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n");
	printf(CYAN "Uygun islemleri bulup yazip enter tusuna basin.\nOrnek cevap : +-\n");
	sleep(3);
	printf(RED "\nAna menuye icin herhangi bir tusa basiniz.\n" RESET);
	printf(GREEN "\n================================================\n");
	_getch();
}
void gameMenu() {

	while(1) {

		system("cls");
		printf(GREEN "\n\n\n=================  ISLEMI BUL ==================\n\n" RESET);
		printf(YELLOW   "   1. Oyuna Basla\n");
		printf(CYAN "   2. Nasil Oynanir ?\n");
		printf("   3. Ilk Skorlarini Gor\n");
		printf(RED"   4. Oyundan Cikis Yap\n");
		printf(GREEN "\n================================================\n" RESET);

		int choosen;
		scanf("%d",&choosen);
		switch(choosen) {
			case 1:	{
				prepareGame();
				break;
			}
			case 2: {

				gameTutorial();
				break;
			}
			case 3: {
				firstScores();
				break;
			}
			case 4: {
			
				for(int i=3;i>0;i--) {
					system("cls");
					printf(RED "\n\n\n\t\tOyundan cikis yapiliyor... (%d)\n",i);
					printf(RESET);
					sleep(1);
				}
				return;
			}
			default : {
				break;
			}
		}
	}
}

int main() {
	srand(time(NULL));
	gameMenu();
}