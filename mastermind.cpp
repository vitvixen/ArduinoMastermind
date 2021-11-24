#include <Arduino.h>
#include "mastermind.h"
#include "lcd_wrapper.h"

void turn_on_leds(){
  digitalWrite(LED_BLUE_1, 1);
  digitalWrite(LED_RED_1, 1);
  digitalWrite(LED_BLUE_2, 1);
  digitalWrite(LED_RED_2, 1);
  digitalWrite(LED_BLUE_3, 1);
  digitalWrite(LED_RED_3, 1);
  digitalWrite(LED_BLUE_4, 1);
  digitalWrite(LED_RED_4, 1);
}

char * usrinput(int attempts, char**history, int history_a[10], int history_b[10], char * secret);
void lcd_print_int(int y, int x, int num);
void my_delay();

char* generate_code(bool repeat, int length){
  if (length < 1) return NULL;
  if (repeat == false && length > 10) return NULL;

  int check[10] = { };

  int gen = 0;
  char *result = "abcd";
  
  for (int i = 0; i < length; i++){
    gen = random(10);
    if (repeat == 0 && check[gen] == 1){
      i--;
      continue;
    } else if (repeat == 0 && check[gen] == 0){
        check[gen] = 1;
        result[i] = gen + 48;
        continue;
    } else if (repeat == 1){
        result[i] = gen + 48;
        continue;
    }
  }

  return result;
}

void get_score(const char* secret, const char* guess, int* peg_a, int* peg_b){
  Serial.println(secret);
  *peg_a = 0;
  *peg_b = 0;
  int i = 0;
  int checker[4] = { };
  for (i = 0; i<4; i++){           //getting the number of right positions
    if(guess[i] == secret[i]) {
      *peg_a+=1; 
      checker[i] = 1;
    }
  }
  
  for(int j = 0; j<i; j++){           //getting the number of right guesses wrong (places)
      for(int a = 0; a < i; a++){
        if (guess[j] == secret[a] && checker[a] == 0 ){
          checker[a] = 1;
          if (j!=a) *peg_b+=1;
        }
      }
  }  
}

void turn_off_leds(){
  digitalWrite(LED_BLUE_1, 0);
  digitalWrite(LED_RED_1, 0);
  digitalWrite(LED_BLUE_2, 0);
  digitalWrite(LED_RED_2, 0);
  digitalWrite(LED_BLUE_3, 0);
  digitalWrite(LED_RED_3, 0);
  digitalWrite(LED_BLUE_4, 0);
  digitalWrite(LED_RED_4, 0);
}

void render_leds(const int peg_a, const int peg_b){
  int blue = 0;
  int red = 0;
  for (blue = 0; blue < peg_b; blue++){
    digitalWrite(2*blue+6, 1);
  }
  for (red = 0; red < peg_a; red++){
    digitalWrite(2*red+2*blue+7, 1);
  }
}

void render_history(char* secret, char** history, const int entry_nr){
  int a = 0;
  int b = 0;
  get_score(secret, history[entry_nr], &a, &b);
  render_leds(a, b);
}

void play_game(char* secret){
  lcd_clear();
  lcd_print_at (0, 0, "Welcome to");
  lcd_print_at (1, 0, "Mastermind");
  my_delay();
  lcd_clear();
  lcd_print_at (0, 0, "Your goal is to");
  lcd_print_at (1, 0, "guess my number");
  my_delay();
  lcd_clear();
  
  int attempts = 0;
  int a = 0;
  int b = 0;
  char *guess = "0000";
  char * history[10] = {"aaaa","ииии","cccc","dddd",
                        "eeee","ffff","gggg","hhhh",
                        "jjjj","kkkk" };
  
  int history_a[10];
  int history_b[10];

  lcd_clear();
  //lcd_print_at(1, 0, "Your guess: 0000");

  while (a!= 4 && attempts!= 10){
    Serial.print("The secret is ");
    Serial.println(secret);
    turn_off_leds();
    guess[0] = '0';
    guess[1] = '0';
    guess[2] = '0';
    guess[3] = '0';
    lcd_clear();
    lcd_print_at(0, 0, "Now guess!");
    if(attempts>0){  
      lcd_print_at(0, 0, "0");
      lcd_print_int(0, 1, attempts);
      lcd_print_at(0, 2, ": ");
      lcd_print_at(0, 4, "A");
      lcd_print_int(0, 5, history_a[attempts-1]);
      lcd_print_at(0, 6, "B");
      lcd_print_int(0, 7, history_b[attempts-1]);
      lcd_print_at(0, 8, " ");
      lcd_print_at(0, 9, history[attempts-1]);
    } 
      lcd_print_at(1, 0, "Your guess:");
      lcd_print_at(1, 11, "0000");
    
    
    guess = usrinput(attempts, history, history_a, history_b, secret);
    get_score(secret, guess, &a, &b);
    turn_off_leds();
    render_leds(a, b);

    lcd_print_at(1, 11, guess);
    
    if (a == 4){
      lcd_clear();
      lcd_print_at(0, 0, "Congrats!");
      lcd_print_at(1, 0, "Answer is: ");
      lcd_print_at(1, 12, guess);
      my_delay();
    } else {
        strcpy(history[attempts], (const)guess);
        history_a[attempts]=a;
        history_b[attempts]=b; 
        attempts++;
        lcd_clear();
        lcd_print_at(0,0, "Wrong");
        my_delay();
      } 
      
    if (a!= 4) {
      lcd_clear();
      lcd_print_at(0, 0, "You have ");
      lcd_print_int(0, 9, 10-attempts);
      lcd_print_at (1, 0, "attempts left");
      my_delay();
    }
  }

  if(a == 4) {
    lcd_clear();
    lcd_print_at(0, 0, "You won!"); 
    lcd_print_at(1, 0, "My code is ") ;
    lcd_print_at(1, 11, secret);
    turn_on_leds();
    my_delay();
    turn_off_leds();
  }
  if(a != 4) {
    lcd_clear();
    lcd_print_at(0, 0, "You lost!"); 
    lcd_print_at(1, 0, "My code was ") ;
    lcd_print_at(1, 12, secret);
    turn_off_leds();
    my_delay();
  }

  free(guess);
  for(int i = 0; i < 10; i++) {
    free(history[i]);
    *history[i] = NULL;
    history[i] = NULL;
  }
}

char * usrinput(int attempts, char**history, int history_a[10], int history_b[10], char * secret){
  int enter = analogRead(A0);
  boolean b1 = !digitalRead(2);
  boolean b2 = !digitalRead(3);
  boolean b3 = !digitalRead(4);
  boolean b4 = !digitalRead(5);
  char * guess1 = "0000";

  int one = 0;
  int two = 0;
  int three = 0;
  int four = 0;
    
  unsigned long last_press = 0;
  
  lcd_print_at(1, 11, guess1);
  
  while (1){
    
    enter = analogRead(A0);
    b1 = !digitalRead(2);
    b2 = !digitalRead(3);
    b3 = !digitalRead(4);
    b4 = !digitalRead(5);
    int marker = 0;

    if (enter < 20 && millis() - last_press > 250) {
      lcd_clear();
      lcd_print_at(0, 0, "Your guess:");
      lcd_print_at(1, 11, guess1);
      my_delay();
      marker = 1;
      last_press = millis();
    }else if (marker == 0) {                    //кнопки с цифрами
                                                                                //scrolling through history
      if(b1 == 1 && b2 == 1 && millis() - last_press > 150 && attempts!=0)  {
        lcd_print_at(1, 11, "0000");
        int getter = attempts-1;
        while(1){
          enter = analogRead(A0);
          b1 = !digitalRead(2);
          b2 = !digitalRead(3);
          b3 = !digitalRead(4);
          b4 = !digitalRead(5);
          
          int check = 0;
  
          if (enter < 20 && millis() - last_press > 250) {
            turn_off_leds();
            check = 1;
            last_press = millis();
          } else if (check != 1){
              if(b1 == 1 && millis() - last_press > 250){
                turn_off_leds();
                if(getter == 0) getter = attempts;
                getter--;

                lcd_print_int(0, 1, getter+1);
                lcd_print_int(0, 5, history_a[getter]);
                lcd_print_int(0, 7, history_b[getter]);
                lcd_print_at(0, 9, history[getter]);
                lcd_print_at(0, 15, "A");
                render_leds(history_a[getter], history_b[getter]);
                
                last_press = millis();
              }
              if(b2 == 1 && millis() - last_press > 150){
                turn_off_leds();
                if(getter == attempts-1) getter = -1;
                getter++;

                lcd_print_int(0, 1, getter+1);
                lcd_print_int(0, 5, history_a[getter]);
                lcd_print_int(0, 7, history_b[getter]);
                lcd_print_at(0, 9, history[getter]);
                lcd_print_at(0, 15, "A");
                render_leds(history_a[getter], history_b[getter]);
                
                last_press = millis();
              }
          }
          if(check == 1) {
            strcpy(guess1, "0000");
            strcpy(guess1, history[getter]);\
            one = history[getter][0] - '0';
            two = history[getter][1] - '0';
            three = history[getter][2] - '0';
            four = history[getter][3] - '0';
            lcd_print_at(1, 11, guess1);
            lcd_print_at(0, 15, " ");
            break;
          }
        }
      }
      
      if(b1 == 1 && millis() - last_press > 150){                  //кнопки по одной
        one++;
        one = one%10;
        *guess1 = (char)48+one;
        lcd_print_at (1, 11, guess1);
        last_press = millis();
      }
      if(b2 == 1 && millis() - last_press > 150){
        two++;
        two = two%10;
        guess1[1] = (char)48+two;
        lcd_print_at (1, 11, guess1);
        last_press = millis();
      }
      if(b3 == 1 && millis() - last_press > 150){
        three++;
        three = three%10;
        guess1[2] = (char)48+three;
        lcd_print_at (1, 11, guess1);
        last_press = millis();
      }
      if(b4 == 1 && millis() - last_press > 150){
        four++;
        four = four%10;
        guess1[3] = (char)48+four;
        lcd_print_at (1, 11, guess1);
        last_press = millis();
      }
      
    }
    if (marker == 1) break;
  }

  return guess1;
}

void my_delay(){
  unsigned long one = millis();
  while(millis()-one < 1500){
    continue;
  }
}
