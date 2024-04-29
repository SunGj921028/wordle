#include<stdio.h>
#include<stdint.h>
#include<string.h>
#include<time.h>
#include<stdlib.h>
#include<stdbool.h>
#include<ctype.h>

bool judge(char word[256]){
    char *suffix = NULL;
    //Ex: Ant's
    suffix = strstr(word,"'");
    if(suffix!=NULL){
        return false;
    }
    //0->ing 1->ed 2->s
    uint8_t judge[4] = {0};
    uint8_t count = 0;
    char *upper = word;
    while(*upper!='\n'){
        if(*upper>'z'||*upper<'a'){
            return false;
        }
        if((count==2&&*upper=='i') || (count==3&&*upper=='n') || (count==4&&*upper=='g')) judge[0] += 1;
        if((count==3&&*upper=='e') || (count==4&&*upper=='d')) judge[1] += 1;
        if(count==4&&*upper=='s') judge[2] += 1;
        if(count==3&&*upper=='s') judge[3] += 1;
        if(judge[2]==1&&judge[3]==1) return true;
        if(judge[0]==3 || judge[1]==2 ||judge[2]==1) return false;
        count++;
        upper++;
    }
    return true;
}

void print(char guess[6], uint8_t color[5]){
    for(int i=0;i<5;i++){
        if(color[i]==0) printf("%c",guess[i]);
        else if(color[i]==1) printf("\033[33m%c\033[0m",guess[i]);
        else if(color[i]==2) printf("\033[32m%c\033[0m",guess[i]);
        else printf("\033[1;30m%c\033[0m",guess[i]);
    }
    printf("\n");
    return;
}

void gray(char name[6], char guess[6], uint8_t color[5]){
    bool check_name = false;
    char check[] = "abcdefghijklmnopqrstuvwxyz";
    uint8_t repeat[26] = {0};
    uint8_t repeat_word[5] = {0};
    uint8_t count_repeat = 0;
    for(int i=0;i<5;i++){
        for(int j=0;j<26;j++){
            //repeat letters
            if(check[j]==guess[i]){
                repeat[j] += 1;
                //repear over or equal twice
                if(repeat[j]>=2){
                    check_name = true;
                    repeat_word[count_repeat++] = j;
                }
            }
        }
    }
    //match over or equal twice
    if(check_name){
        for(int i=0;i<count_repeat;i++){
            uint8_t count = 0;
            //read through answer
            for(int j=0;j<5;j++){
                if(name[j]==check[repeat_word[i]]) {
                    count++;
                }
            }
            uint8_t count_not_green = 0;
            //change to gray
            if(count!=repeat[repeat_word[i]] && count!=0){
                for(int j=0;j<5;j++){
                    if(guess[j]==check[repeat_word[i]] && color[j]!=2) count_not_green++, color[j] = 3;
                }
            }
            //
            if(count_not_green!=(repeat[repeat_word[i]]-count)){
                uint8_t count_over = 0;
                for(int j=0;j<5;j++){
                    if(guess[j]==check[repeat_word[i]] && color[j]!=2) count_over++, color[j] = 1;
                    if(count_over == (repeat[repeat_word[i]]-count)) break;
                }
            }
            //The repeat letters not match in answer, then change back to white
            if(count==0){
                for(int m=0;m<5;m++){
                    if(guess[m]==check[repeat_word[i]]) color[m] = 0;
                }
            }
        }
    }
    return;
}

bool wordle(char name[6],char guess[6]){
    uint8_t color[5] = {0};
    //0->white 1->yellow 2->green 3->gray
    for(int i=0;i<5;i++){
        for(int j=0;j<5;j++){
            if(guess[j]==name[i]){
                if(i==j) {
                    color[j] = 2;
                    break;
                }else if(color[j]!=2) color[j] = 1;
            }
        }
    }
    gray(name,guess,color);
    print(guess,color);
    //if one not green, return false
    for(int i=0;i<5;i++){
        if(color[i]!=2){
            return false;
        }
    }
    return true;
}

int main(){
    FILE *file = NULL;
    //path of american-english
    char pathname[] = "/usr/share/dict/american-english";
    char wordname[256] = {0};
    char game_name[100000][6] = {0};
    uint32_t count_name = 0;
    if((file = fopen(pathname,"rb"))==NULL){
        printf("Cannot open file!!\n");
        return -1;
    }
    //find five letters words
    while(fgets(wordname,sizeof(wordname),file)!=NULL){
        if(strlen(wordname)==6){
            if(judge(wordname)){
                strncpy(game_name[count_name],wordname,strlen(wordname)-1);
                game_name[count_name][strlen(wordname)] = '\0';
                count_name++;
            }
        }
    }
    fclose(file);
    srand(time(NULL));
    //random th answer
    uint32_t number = rand() % count_name;
    printf("Answer: %s\n",game_name[number]);
    uint8_t count_time = 1;
    char guess[6] = {0};
    //six chances
    while(count_time<7){
        printf("Guess %d: ",count_time++);
        scanf("%s",guess);
        //wrong input
        if(strlen(guess)!=5){
            printf("Wrong Input!!\n");
            continue;
        }
        bool good_name = false;
        for(int i=0;i<count_name;i++){
            if(strncmp(guess,game_name[i],5)==0){
                good_name = true;
                break;
            }
        }
        if(!good_name){
            printf("Wrong Input!!\n");
            continue;
        }
        //win
        if(wordle(game_name[number],guess)){
            printf("Congratulations!\n");
            return 0;
        }
    }
    //lose
    printf("Good Luck\n");
    return 0;
}