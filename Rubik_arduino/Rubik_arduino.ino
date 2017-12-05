/****************************************************************************************************
 * Code créer par Toisoul Lionel élève de robotique à la haute école de la ville de liège (ISET)
 * 2017/2018
 ****************************************************************************************************/
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>    // Bibliothèque a télécharger sur adafruit 

// Utilisation de l'adresse par défaut 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

// Suivant le type de servo utilisé, la largeur d'impulsion minimale et maximale 
// peut changer. L'idéal est de sélectionner des valeurs aussi petites et aussi
// grande que possible sans jamais atteindre la butée de blocage du servo-moteur. 
// Vous aurrez peut-etre besoin de modifier ces valeurs pour les faire correspondrent 
// à votre type de servo!
#define SERVOMIN_all  150 // La longueur d'impulsion 'minimale' (valeur du compteur, max 4096)
#define SERVOMAX_all  600 // La longueur d'impulsion 'maximale' (valeur du compteur, max 4096)

// Servos Commandés - Numéro de sorties sur le PCA9685
#define servoA  0
#define servoa  1
#define servoE  2
#define servoe  3
#define servoC  4
#define servoc  5
#define servoG  6
#define servog  7

#define A 1
#define E 2
#define C 3
#define G 4

#define SortirA 11
#define RentrerA 12
#define zeroA 13
#define nonanteA 14
#define fullA 15

#define SortirE 21
#define RentrerE 22
#define zeroE 23
#define nonanteE 24
#define fullE 25

#define SortirC 31
#define RentrerC 32
#define zeroC 33
#define nonanteC 34
#define fullC 35

#define SortirG 41
#define RentrerG 42
#define zeroG 43
#define nonanteG 44
#define fullG 45

int t=700; // delay entre chaques mouvements ne pas mettre <600
int trans_delay=100;
int rot_delay=100;

int SERVOMIN[8] = { 150, 150, 150, 150, 150, 150, 150, 150 };
int SERVOMAX[8] = { 600, 600, 600, 600, 600, 600, 600, 600 };

void setup() 
{
  Serial.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(60);  // Les servo sont mis-à-jour à ~60 Hz  //1000 max
}

//-----------------------------------------------------------------------------------------------------
// Deplace un servo "BRAS", avant arrière, 
// pos varie de 0 à 100
// s = numero du servo
void move_arm(uint8_t s,int pos ){
   int longueurImpulsion;
   longueurImpulsion = map( pos, 0, 100, SERVOMIN[s], SERVOMAX[s]);    // Transformation de % en pulse
   pwm.setPWM(s, 0, longueurImpulsion);
   delay(trans_delay);
}

// Tourne un servo "MAIN"
// pos varie de 0 à 270
// s = numero du servo
void turn_arm(uint8_t s,int pos ){
   int longueurImpulsion;
   longueurImpulsion = map( pos, 0, 270, SERVOMIN[s], SERVOMAX[s]);    // Transformation de degré en pulse
   pwm.setPWM(s, 0, longueurImpulsion);
   delay(rot_delay);
}

/****************************************************************************************************
 * translations arm = A,E,C,G
 ****************************************************************************************************/

void Rentrer (uint8_t arm){ 
  if(arm==A) move_arm(servoA,100 );
  if(arm==E) move_arm(servoE,100 );
  if(arm==G) move_arm(servoG,100 );
  if(arm==C) move_arm(servoC,100 );
}

void Sortir (uint8_t arm){ 
  if(arm==A) move_arm(servoA,0 );
  if(arm==E) move_arm(servoE,0 );
  if(arm==G) move_arm(servoG,0 );
  if(arm==C) move_arm(servoC,0 );
}

/****************************************************************************************************
 * rotations arm = A,E,C,G
 ****************************************************************************************************/
void zero (uint8_t arm) { 
  if(arm==A) turn_arm(servoa,0 );
  if(arm==E) turn_arm(servoe,0 );
  if(arm==G) turn_arm(servog,0 );
  if(arm==C) turn_arm(servoc,0 );
}
void nonante (uint8_t arm) { 
   if(arm==A) turn_arm(servoa,130 );
   if(arm==E) turn_arm(servoe,130 );
   if(arm==G) turn_arm(servog,130 );
   if(arm==C) turn_arm(servoc,130 );
}
void full (uint8_t arm) {  
   if(arm==A) turn_arm(servoa,270 );
   if(arm==E) turn_arm(servoe,270 );
   if(arm==G) turn_arm(servog,270 );
   if(arm==C) turn_arm(servoc,270 );
}

//-----------------------------------------------------------------------------------------------------
// execute une scéquence de mouvements,
// la séquence codée est dans un tableau seq, le nombres de pas est nbstep

void do_sequence(uint8_t s[],int nb){
  for(int i=0;i<nb;i++){
    int code=s[i];
    uint8_t code_diz=code/10;   //1,2,3 ou 4 = A,E,C ou G
    uint8_t code_unit=code-(code_diz*10);
    
    switch(code_unit)
    {
      case 1:Sortir(code_diz);break;
      case 2:Rentrer(code_diz);break;
      case 3:zero(code_diz);break;
      case 4:nonante(code_diz);break;
      case 5:full(code_diz);break;
    } 
    delay(t); 
  } 
}

//-----------------------------------------------------------------------------------------------------
// Execute une commande reçue sur le port série
// c1: code commande, toujours une lettre
// c2: paramètre de la commande
// Il y a des commandes simples = un seul mouvement A,a,E,e...
// et des commandes complexes = plusieurs mouvements X,P,F,R ...
//   mettre toute la liste ici ds le commentaire

void do_command(char c1,char c2){

int nb_step=0;

  switch(c1)
      {
      case 'A':
        if(c2 == '0')Rentrer(A);       
        if(c2 == '1')Sortir(A); 
        break;
    
      case 'a':
        if(c2 == '2')zero(A);
        if(c2 == '3')nonante(A);
        if(c2 == '4')full(A);
        break;
        
      case 'E':
        if(c2 == '0')Rentrer(E);
        if(c2 == '1')Sortir(E);
        break;
    
      case 'e':
        if(c2 == '2')zero(E); 
        if(c2 == '3')nonante(E);
        if(c2 == '4')full(E);
        break;
               
      case 'C':
        if(c2 == '0')Rentrer(C);
        if(c2 == '1')Sortir(C);
        break;
       
      case 'c':
        if(c2 == '2')zero(C); 
        if(c2 == '3')nonante(C);
        if(c2 == '4')full(C); 
        break;
        
      case 'G':
        if(c2 == '0')Rentrer(G);
        if(c2 == '1')Sortir(G); 
        break;
          
      case 'g':
        if(c2 == '2')zero(G); 
        if(c2 == '3')nonante(G);
        if(c2 == '4')full(G); 
        break;
        
      case 'X':
        if(c2 == 'X') //ouvre A et E et ferme C et G
        {
          Rentrer(A);Rentrer(E);Rentrer(C);Rentrer(G);
          delay(t);
          zero(A);zero(E);zero(C);zero(G);
        }
        if(c2 == 'Y') //ferme tout les servos
        {
          Sortir(A);Sortir(E);Sortir(C);Sortir(G);
        }
        break;
      
      case 'P':
      
        if(c2 == '1')  //présente la face 1  (ROUGE)
        {   
          Rentrer(E);Rentrer(G);
          delay(t);
          nonante(E);nonante(G);
          delay(t);
          Sortir(E);Sortir(G);
          delay(t);
          Rentrer(A);Rentrer(C);
        }
        
        if(c2 == '2')  //présente la face 2,3,4  (VERT)puis(ORANGE)puis(BLEU)
        {
          zero(G);full(E);
          delay(t);
          Sortir(A);Sortir(C);
          delay(t);
          Rentrer(G);Rentrer(E);
          delay(t);
          nonante(G);nonante(E);
          delay(t);
          Sortir(G);Sortir(E);
          delay(t);
          Rentrer(A);Rentrer(C);
        }
        
        if(c2 == '3')  //présente la face 5 (JAUNE) 
        {
          zero(A);full(C);
          delay(t);
          zero(G);full(E);
          delay(t);
          Sortir(A);Sortir(C);
          delay(t);
          Rentrer(G);Rentrer(E);
          delay(t);
          nonante(A);nonante(C);
        }
        
        if(c2 == '4')  //présente la face 6 (BLANC)  
        {
          zero(A);full(C);
          delay(t);
          Sortir(G);Sortir(E);
          delay(t);
          Rentrer(A);Rentrer(C);
          delay(t);
          nonante(A);nonante(C);
          delay(t);
          Sortir(A);Sortir(C);
          delay(t);
          Rentrer(G); Rentrer(E);
          delay(t);
          zero(A); full(C);
          delay(t);
          nonante(G); nonante(E);
          delay(t);
          Sortir(G); Sortir(E);
          delay(t);
          Rentrer(A);Rentrer(C);
        }
        
        if(c2 == '5')  //remet le cube en position initiale 
        {
          Sortir(A); Sortir(C);
          delay(t);
          Rentrer(G); Rentrer(E);
          delay(t);
          zero(G); zero(E);
          delay(t);
          nonante(A); nonante(C);
          delay(t);
          Sortir(G); Sortir(E);
          delay(t);
          Rentrer(A); Rentrer(C);
          delay(t);
          zero(A); zero(C);
          delay(t);
          Sortir(A); Sortir(C);
        }
        
        break;
/****************************************************************************************************
 * Résolution face F
 ****************************************************************************************************/
        case 'F':
        if(c2 == '1')
        {
            uint8_t seq[32]={
            RentrerE,
            fullE,            
            SortirE,            
            RentrerA,
            RentrerC,            
            nonanteG,
            nonanteE,            
            SortirA,
            SortirC,            
            RentrerG,            
            RentrerE,            
            zeroG,
            fullE,            
            SortirG,
            SortirE,            
            nonanteC,            
            RentrerC,            
            zeroC,            
            SortirC,            
            RentrerG,
            RentrerE,            
            nonanteG,
            nonanteE,            
            SortirG,
            SortirE,            
            RentrerA,
            RentrerC,            
            zeroG,
            fullE,            
            SortirA,
            SortirC    
            };
            do_sequence(seq,31);   
        }
        if(c2 == '2')
        {
            uint8_t seq[32]={
            RentrerE,
            fullE,
            SortirE,
            RentrerA,
            RentrerC,
            nonanteG,
            nonanteE,
            SortirA,
            SortirC,
            RentrerG,
            RentrerE,
            zeroG,
            fullE,
            SortirG,
            SortirE,
            fullC,
            RentrerC,
            zeroC,
            SortirC,
            RentrerG,
            RentrerE,
            nonanteG,
            nonanteE,
            SortirG,
            SortirE,
            RentrerA,
            RentrerC,
            zeroG,
            fullE,
            SortirA,
            SortirC
            };
            do_sequence(seq,31);
        }
        if(c2 == '3')
        {
            uint8_t seq[32]={
            RentrerE,
            fullE,
            SortirE,
            RentrerA,
            RentrerC,
            nonanteG,
            nonanteE,
            SortirA,
            SortirC,
            RentrerG,
            RentrerE,
            zeroG,
            fullE,
            SortirG,
            SortirE,
            RentrerC,
            nonanteC,
            SortirC,
            zeroC,
            RentrerG,
            RentrerE,
            nonanteG,
            nonanteE,
            SortirG,
            SortirE,
            RentrerA,
            RentrerC,
            zeroG,
            fullE,
            SortirA,
            SortirC
          };
            do_sequence(seq,31);
        }
        break;
/****************************************************************************************************
 * Résolution face R
 ****************************************************************************************************/
        case 'R':
        if(c2 == '1')
        {
           uint8_t seq[32]={nonanteC,RentrerC,zeroC,SortirC};
           do_sequence(seq,4);
        }
        if(c2 == '2')
        {
           uint8_t seq[32]={fullC,RentrerC,zeroC,SortirC};
           do_sequence(seq,4);
        }
        if(c2 == '3')
        {
          uint8_t seq[32]={RentrerC,nonanteC,SortirC,zeroC};
          do_sequence(seq,4);
        }
/****************************************************************************************************
 * Résolution face U
 ****************************************************************************************************/
        case 'U':
        if(c2 == '1')
        { 
           uint8_t seq[32]={nonanteE,RentrerE,zeroE,SortirE};         
           do_sequence(seq,4);
        }
        if(c2 == '2')
        {
           uint8_t seq[32]={fullE,RentrerE,zeroE,SortirE};
           do_sequence(seq,4);
        }
        if(c2 == '3')
        {
          uint8_t seq[32]={RentrerE,nonanteE,SortirE,zeroE};
          do_sequence(seq,4);
        }
/****************************************************************************************************
 * Résolution face L
 ****************************************************************************************************/
        case 'L':
        if(c2 == '1')
        {
           uint8_t seq[32]={nonanteA,RentrerA,zeroA,SortirA};         
           do_sequence(seq,4);
        }
        if(c2 == '2')
        {
           uint8_t seq[32]={fullA,RentrerA,zeroA,SortirA};
           do_sequence(seq,4);
        }
        if(c2 == '3')
        {  
           uint8_t seq[32]={RentrerA,nonanteA,SortirA,zeroA};
           do_sequence(seq,4);
        }
/****************************************************************************************************
 * Résolution face B
 ****************************************************************************************************/
      case 'B':
        if(c2 == '1')
        {
          uint8_t seq[32]={
            RentrerE,
            fullE,
            SortirE,
            RentrerA,
            RentrerC,
            nonanteG,
            nonanteE,
            SortirA,
            SortirC,
            RentrerG,
            RentrerE,
            zeroG,
            fullE,
            SortirG,
            SortirE,
            nonanteA,
            RentrerA,
            zeroA,
            SortirA,
            RentrerG,
            RentrerE,
            nonanteG,
            nonanteE,
            SortirG,
            SortirE,
            RentrerA,
            RentrerC,
            zeroG,
            fullE,
            SortirA,
            SortirC};
            do_sequence(seq,31);
        }
        if(c2 == '2')
        {
           uint8_t seq[32]={
            RentrerE,
            fullE,
            SortirE,
            RentrerA,
            RentrerC,
            nonanteG,
            nonanteE,
            SortirA,
            SortirC,
            RentrerG,
            RentrerE,
            zeroG,
            fullE,
            SortirG,
            SortirE,
            fullA,
            RentrerA,
            zeroA,
            SortirA,
            RentrerG,
            RentrerE,
            nonanteG,
            nonanteE,
            SortirG,
            SortirE,
            RentrerA,
            RentrerC,
            zeroG,
            fullE,
            SortirA,
            SortirC};
            do_sequence(seq,31);
        }
        if(c2 == '3')
        {
          uint8_t seq[32]={
            RentrerE,
            fullE,
            SortirE,
            RentrerA,
            RentrerC,
            nonanteG,
            nonanteE,
            SortirA,
            SortirC,
            RentrerG,
            RentrerE,
            zeroG,
            fullE,
            SortirG,
            SortirE,
            RentrerA,
            nonanteA,
            SortirA,
            zeroA,
            RentrerG,
            RentrerE,
            nonanteG,
            nonanteE,
            SortirG,
            SortirE,
            RentrerA,
            RentrerC,
            zeroG,
            fullE,
            SortirA,
            SortirC};
            do_sequence(seq,31);
        }
/****************************************************************************************************
 * Résolution face D
 ****************************************************************************************************/
        case 'D':
        if(c2 == '1')
        {
           uint8_t seq[32]={nonanteG,RentrerG,zeroG,SortirG};         
           do_sequence(seq,4);
        }
        if(c2 == '2')
        {
           uint8_t seq[32]={fullG,RentrerG,zeroG,SortirG};         
           do_sequence(seq,4);
        }
        if(c2 == '3')
        {
           uint8_t seq[32]={RentrerG,nonanteG,SortirG,zeroG};         
           do_sequence(seq,4);
        }
      } 
}

//-----------------------------------------------------------------------------------------------------
void loop() 
{ 
    if (Serial.available() > 0)   // Serial.available permet de recupérer les données envoyer sur le port serie de l'arduino
    {
      // On va décomposer les données recues en deux caractères distincts (car nous avons choisi de nommer toutes nos commandes par deux lettres)
      // On va utiliser A,B,C et D pour les quatres servos qui permettent de faire avancer les bras, 0 ou 1 en fonction de si ils sont sortis ou non
      // On va utiliser a,b,c et d pour les quatres servos qui permettent de faire tourner les bras, 0, 90 ou 180 en fonction du degré que l'on veut 
      // les faire tourner
      char c1 = Serial.read();
      delay(50);
      char c2 = Serial.read();   
      // delay(50); // inutile
      do_command(c1,c2);
         
     }
}
