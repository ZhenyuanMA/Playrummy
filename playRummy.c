#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

int transfer1(char suit, char rank){
	char suits[] = {'S', 'H', 'D', 'C'};
	char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
	int num;

	for (int a = 0; a < 4; a++){
		for (int b = 0; b < 13; b++){
			if (suit == suits[a] && rank == ranks[b]){
				num = b * 4 + a + 1;
			}
		}
	}
	return num;
}

char transfer2_1(int num){
	char suits[] = {'S', 'H', 'D', 'C'};
	char suit;

	suit = suits[(num - 1) % 4];
	return suit;
}

char transfer2_2(int num){
	char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
	char rank;

	rank = ranks[(num - 1) / 4];
	return rank;
}

void printrummy(int a, int b, int c, int d, int e, int f, int g, int h, int i){
	int cards[9] = {a, b, c, d, e, f, g, h, i};
	int card;

	for(int k = 0; k < 8; k++){
		for(int j = 1; j < 9; j++){
			if (cards[k] < cards[k]){
				card = cards[k];
				cards[k] = cards[j];
				cards[j] = card;
			}
		}
	}

	for (int i = 0; i < 7; i++){
		if ((transfer2_2(cards[i]) == transfer2_2(cards[i + 1])) && (transfer2_2(cards[i]) == transfer2_2(cards[i + 2])) && cards[i] != 0 && cards[i + 1] != 0 && cards[i + 2] != 0){
			printf(" <%c%c %c%c %c%c>", transfer2_1(cards[i]), transfer2_2(cards[i]), transfer2_1(cards[i + 1]), transfer2_2(cards[i + 1]), transfer2_1(cards[i + 2]), transfer2_2(cards[i + 2]));
			cards[i] = 0;
			cards[i + 1] = 0;
			cards[i + 2] = 0;
		}
		else if ((((cards[i] - 1) / 4) == ((cards[i + 1] - 1) / 4) + 1) && (((cards[i + 1] - 1) / 4) == ((cards[i + 2] - 1) / 4) + 1) && cards[i] != 0 && cards[i + 1] != 0 && cards[i + 2] != 0){
			printf(" <%c%c %c%c %c%c>", transfer2_1(cards[i]), transfer2_2(cards[i]), transfer2_1(cards[i + 1]), transfer2_2(cards[i + 1]), transfer2_1(cards[i + 2]), transfer2_2(cards[i + 2]));
			cards[i] = 0;
			cards[i + 1] = 0;
			cards[i + 2] = 0;
		}
	}


}

int main(int argc, char *argv[]){
	char str[2];
	char buf[80], buf2[80];
	int num, temp, num_cards = 0, i, j, k, n, card, count;
	int cards[9];
	int tem[3];
	int input[52], ifdiscard = 0, discard;
	int num_child = atoi(argv[1]);
	int hands[num_child][8];
	int fd[num_child][2];
	int id[num_child];
	int rummy = 0;
	int rummychild;
	int readyrummy[num_child][3];


	//initialize
	for (i = 0; i < num_child; i++){
		readyrummy[i][0] = 0;
		readyrummy[i][1] = 13;
		readyrummy[i][2] = 13;
	}

	//read from the file
	scanf("%s ", str);
	num = transfer1(str[0], str[1]);
	input[num_cards] = num;
	num_cards++;

	scanf("%s ", str);
	num = transfer1(str[0], str[1]);
	while (num != input[num_cards - 1]){
		input[num_cards] = num;
		scanf("%s ", str);
		num = transfer1(str[0], str[1]);
		num_cards++;
	}
	
	//deliver cards
	temp = 0;
	for (j = 0; j < 8; j++){
		for (i = 0; i < num_child; i++){
			hands[i][j] = input[temp];
			temp++;
		}
	}

	//get all pid
	for (i = 0; i < num_child; i++){
		int pid = fork();
		if (pid < 0){
			printf("Fork failed!\n");
			exit(1);
		}
		if (pid == 0){
			exit(0);
		}
		else{
			id[i] = pid;
		}
	}

	printf("Parent: the child palyers are ");
	for (i = 0; i < num_child; i++){
		printf("%d ", id[i]);
	}
	printf("\n");

	for (i = 0; i < num_child; i++){
		printf("Child %d, pid %d: ", i + 1, id[i]);
		for (j = 0; j < 8; j++){
			str[0] = transfer2_1(hands[i][j]);
			str[1] = transfer2_2(hands[i][j]);
			printf("%s ", str);
		}
		printf("\n");
	}

	//arrangement
	for (i = 0; i < num_child; i++){
		for(k = 0; k < 7; k++){
			for(j = k + 1; j < 8; j++){
				if (hands[i][k] < hands[i][j]){
					card = hands[i][k];
					hands[i][k] = hands[i][j];
					hands[i][j] = card;
				}
			}
		}
	}

	//pipe
	while(temp < num_cards && rummy == 0){
		for (i = 0; i < num_child; i++){
			if (temp < num_cards && rummy == 0){
				if (pipe(fd[i]) < 0){
					printf("Pipe creation error!\n");
					exit(1);
				}
				int pid = fork();
				if (pid < 0){
					printf("Fork failed!\n");
					exit(1);
				}
				if (pid == 0){
					if (ifdiscard == 0){ //parent deal new card
						if (readyrummy[i][0] == 1){ //ready to rummy
							close(fd[i][1]);
							buf[0] = 0;
							n = read(fd[i][0], buf, 80);
							buf[++n] = 0;
							card = transfer1(buf[0], buf[1]);
							k = 1;
							while (readyrummy[i][k] != 13){
								if ((card - 1) / 4 == readyrummy[i][k]){
									printf("Child %d: rummy \n", i + 1);
									rummy = 1;
									rummychild = i + 1;
									printf("Child %d:", i + 1);
									printrummy(hands[i][0], hands[i][1], hands[i][2], hands[i][3], hands[i][4], hands[i][5], hands[i][6], hands[i][7], card);
									close(fd[i][0]);
									exit(0);

								}
							}
							discard = card; // determine which to discard
							buf2[0] = transfer2_1(card);
							buf2[1] = transfer2_2(card);
							buf2[2] = 1;
							buf2[3] = 0;
							printf("Child %d: discard %s \n", i + 1, buf2);
							write(fd[i][1], buf2, 3);
							close(fd[i][0]);
							exit(0);
						}
						else{ //not ready to rummy
							close(fd[i][1]);
							buf[0] = 0;
							n = read(fd[i][0], buf, 80);
							buf[++n] = 0;
							// determine which to discard
							count = 0;
							for (k = 0; k < 8; k++){
								cards[k] = hands[i][k];
							}
							cards[8] = transfer1(buf[0], buf[1]);
							//arrangment
							for(k = 0; k < 8; k++){
								for(j = k + 1; j < 9; j++){
									if (cards[k] < cards[j]){
										card = cards[k];
										cards[k] = cards[j];
										cards[j] = card;
									}
								}
							}
							for (k = 0; k < 7; k++){
								if ((transfer2_2(cards[k]) == transfer2_2(cards[k + 1])) && (transfer2_2(cards[k]) == transfer2_2(cards[k + 2])) && cards[k] != 0 && cards[k + 1] != 0 && cards[k + 2] != 0){
									count++;
									cards[k] = 0;
									cards[k + 1] = 0;
									cards[k + 2] = 0;
								}
								else if ((((cards[k] - 1) / 4) == ((cards[k + 1] - 1) / 4) + 1) && (((cards[k + 1] - 1) / 4) == ((cards[k + 2] - 1) / 4) + 1) && cards[k] != 0 && cards[k + 1] != 0 && cards[k + 2] != 0){
									count++;
									cards[k] = 0;
									cards[k + 1] = 0;
									cards[k + 2] = 0;
								}
							}
							if (count == 2){
								j = 0;
								for (k = 0; k < 9; k++){
									if (cards[k] != 0){
										tem[j] = cards[k];
										j++;
									}
								}
								if (((tem[0] - 1) / 4) == ((tem[1] - 1) / 4) + 1){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[1] - 1) / 4) + 1;
								}
								else if (((tem[0] - 1) / 4) == ((tem[2] - 1) / 4) + 1){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[2] - 1) / 4) + 1;
								}
								else if (((tem[1] - 1) / 4) == ((tem[2] - 1) / 4) + 1){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[2] - 1) / 4) + 1;
								}
								else if (((tem[1] - 1) / 4) == ((tem[0] - 1) / 4) + 1){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[0] - 1) / 4) + 1;
								}
								else if (((tem[2] - 1) / 4) == ((tem[0] - 1) / 4) + 1){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[2] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[0] - 1) / 4) + 1;
								}
								else if (((tem[2] - 1) / 4) == ((tem[1] - 1) / 4) + 1){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[2] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[1] - 1) / 4) + 1;
								}
								else if (((tem[0] - 1) / 4) == ((tem[1] - 1) / 4)){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4);
								}
								else if (((tem[1] - 1) / 4) == ((tem[2] - 1) / 4)){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4);
								}
								else if (((tem[0] - 1) / 4) == ((tem[2] - 1) / 4)){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4);
								}
							}
							else {
								card = cards[8];
							}
							num = 9;
							for (k = 0; k < 8; k++){
								if (hands[i][k] == card){
									num = k;
								}
							}
							if (num != 9){
								hands[i][num] = transfer1(buf[0], buf[1]);
							}

							discard = card; // determine which to discard
							buf2[0] = transfer2_1(card);
							buf2[1] = transfer2_2(card);
							buf2[2] = 1;
							buf2[3] = 0;
							printf("Child %d: discard %s \n", i + 1, buf2);
							write(fd[i][1], buf2, 3);
							close(fd[i][0]);
							exit(0);
						}
					}
					else if (ifdiscard == 1){ // parent deal card discard by last child
						if (readyrummy[i][0] == 1){ //ready to rummy
							close(fd[i][1]);
							buf[0] = 0;
							n = read(fd[i][0], buf, 80);
							buf[++n] = 0;
							card = transfer1(buf[0], buf[1]);
							k = 1;
							while (readyrummy[i][k] != 13){
								if (card == readyrummy[i][k]){
									printf("Child %d: rummy \n", i + 1);
									rummy = 1;
									rummychild = i + 1;
									printf("Child %d:", i + 1);
									printrummy(hands[i][0], hands[i][1], hands[i][2], hands[i][3], hands[i][4], hands[i][5], hands[i][6], hands[i][7], card);
									close(fd[i][0]);
									exit(0);

								}
							}
							discard = card; // determine which to discard
							buf2[0] = transfer2_1(discard);
							buf2[1] = transfer2_2(discard);
							buf2[2] = 1;
							buf2[3] = 0;
							write(fd[i][1], buf2, 3);
							printf("Child %d: skip %s \n", i + 1, buf2);
							close(fd[i][0]);
							exit(0);
						}
						else{ // not ready to rummy
							close(fd[i][1]);
							buf[0] = 0;
							n = read(fd[i][0], buf, 80);
							buf[++n] = 0;

							// determine which to discard
							count = 0;
							for (k = 0; k < 8; k++){
								cards[k] = hands[i][k];
							}
							cards[8] = transfer1(buf[0], buf[1]);
							//arrangment
							for(k = 0; k < 8; k++){
								for(j = k + 1; j < 9; j++){
									if (cards[k] < cards[j]){
										card = cards[k];
										cards[k] = cards[j];
										cards[j] = card;
									}
								}
							}
							for (k = 0; k < 7; k++){
								if ((transfer2_2(cards[k]) == transfer2_2(cards[k + 1])) && (transfer2_2(cards[k]) == transfer2_2(cards[k + 2])) && cards[k] != 0 && cards[k + 1] != 0 && cards[k + 2] != 0){
									count++;
									cards[k] = 0;
									cards[k + 1] = 0;
									cards[k + 2] = 0;
								}
								else if ((((cards[k] - 1) / 4) == ((cards[k + 1] - 1) / 4) + 1) && (((cards[k + 1] - 1) / 4) == ((cards[k + 2] - 1) / 4) + 1) && cards[k] != 0 && cards[k + 1] != 0 && cards[k + 2] != 0){
									count++;
									cards[k] = 0;
									cards[k + 1] = 0;
									cards[k + 2] = 0;
								}
							}
							if (count == 2){
								j = 0;
								for (k = 0; k < 8; k++){
									if (cards[k] != 0){
										tem[j] = cards[k];
										j++;
									}
								}
								if (((tem[0] - 1) / 4) == ((tem[1] - 1) / 4) + 1){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[1] - 1) / 4) + 1;
								}
								else if (((tem[0] - 1) / 4) == ((tem[2] - 1) / 4) + 1){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[2] - 1) / 4) + 1;
								}
								else if (((tem[1] - 1) / 4) == ((tem[2] - 1) / 4) + 1){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[2] - 1) / 4) + 1;
								}
								else if (((tem[1] - 1) / 4) == ((tem[0] - 1) / 4) + 1){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[0] - 1) / 4) + 1;
								}
								else if (((tem[2] - 1) / 4) == ((tem[0] - 1) / 4) + 1){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[2] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[0] - 1) / 4) + 1;
								}
								else if (((tem[2] - 1) / 4) == ((tem[1] - 1) / 4) + 1){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[2] - 1) / 4) - 1;
									readyrummy[i][2] = ((tem[1] - 1) / 4) + 1;
								}
								else if (((tem[0] - 1) / 4) == ((tem[1] - 1) / 4)){
									card = tem[2];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4);
								}
								else if (((tem[1] - 1) / 4) == ((tem[2] - 1) / 4)){
									card = tem[0];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[1] - 1) / 4);
								}
								else if (((tem[0] - 1) / 4) == ((tem[2] - 1) / 4)){
									card = tem[1];
									readyrummy[i][0] = 1;
									readyrummy[i][1] = ((tem[0] - 1) / 4);
								}
							}
							else {
								card = cards[8];
							}
							num = 9;
							for (k = 0; k < 8; k++){
								if (hands[i][k] == card){
									num = k;
								}
							}
							if (num != 9){ // can take
								hands[i][num] = transfer1(buf[0], buf[1]);
								printf("Child %d: take %s \n", i + 1, buf);

								discard = card; // determine which to discard
								buf2[0] = transfer2_1(card);
								buf2[1] = transfer2_2(card);
								buf2[2] = 1;
								buf2[3] = 0;
								write(fd[i][1], buf2, 3);
								printf("Child %d: discard %s \n", i + 1, buf2);
								close(fd[i][0]);
								exit(0);
							}
							else{ // cannot take
								printf("Child %d: skip %s \n", i + 1, buf);
								close(fd[i][0]);
								ifdiscard = 0;
								exit(0);
							}							
						}
					}
				}
				else{ //parent
					if (ifdiscard == 0){ //if last child skip
						close(fd[i][0]);
						buf[0] = 0;
						buf[0] = transfer2_1(input[temp]);
						buf[1] = transfer2_2(input[temp]);
						buf[2] = 0;
						printf("Parent: dealing %s to child %d \n", buf, i + 1);
						write(fd[i][1], buf, 2);
						temp++;			
						waitpid(pid, 0, 0);
						buf2[0] = 0;
						n = read(fd[i][0], buf2, 80);
						buf2[++n] = 0;
						ifdiscard = buf2[2];
						if (ifdiscard == 1){
							discard = transfer1(buf2[0], buf2[1]);
						}
						close(fd[i][1]);
					}
					else if (ifdiscard == 1){ //if last child discard
						if (i == 0){
							close(fd[i][0]);
							buf[0] = 0;
							buf[0] = transfer2_1(discard);
							buf[1] = transfer2_2(discard);
							buf[2] = 0;
							printf("Parent: child %d discards %s \n", num_child, buf);
							write(fd[i][1], buf, 2);	
							waitpid(pid, 0, 0);
							buf2[0] = 0;
							n = read(fd[i][0], buf2, 80);
							buf2[++n] = 0;
							ifdiscard = buf2[2];
							if (ifdiscard == 1){
								discard = transfer1(buf2[0], buf2[1]);
							}
							close(fd[i][1]);
						}
						else{
							close(fd[i][0]);
							buf[0] = 0;
							buf[0] = transfer2_1(discard);
							buf[1] = transfer2_2(discard);
							buf[2] = 0;
							printf("Parent: child %d discards %s \n", i, buf);
							write(fd[i][1], buf, 2);	
							waitpid(pid, 0, 0);
							buf2[0] = 0;
							n = read(fd[i][0], buf2, 80);
							buf2[++n] = 0;
							ifdiscard = buf2[2];
							if (ifdiscard == 1){
								discard = transfer1(buf2[0], buf2[1]);
							}
							close(fd[i][1]);
						}
					}
				}
			}
			else{
				break;
			}
			
		}
	}

	//no one rummy
	if (rummy == 0){
		printf("Parent: no more card, it is a draw \n");
		for (i = 0; i < num_child; i++){
			printf("Child %d: ", i + 1);
			for (j = 0; j < 8; j++){
				str[0] = transfer2_1(hands[i][j]);
				str[1] = transfer2_2(hands[i][j]);
				printf("%s ", str);
			}
			printf("\n");
		}
	}
	else if (rummy == 1){
		printf("Parent: child %d rummy \n", rummychild);
		for (i = 0; i < num_child; i++){
			if (i != rummychild - 1){
				printf("Child %d: ", i + 1);
				for (j = 0; j < 8; j++){
					str[0] = transfer2_1(hands[i][j]);
					str[1] = transfer2_2(hands[i][j]);
					printf("%s ", str);
				}
				printf("\n");
			}		
		}
	}


}