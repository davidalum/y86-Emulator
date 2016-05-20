#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "y86emul.h"
#include <math.h>

char *memoryBlock;
int PC;
int OF, ZF, SF; 
int registers[8];


char * strdupForText (const char *s) {
  char *d = malloc (strlen (s) + 1);
  if (d == NULL) return NULL;
  strncpy (d,s,2);
  return d;
}


void read(unsigned char *byte) {

unsigned char  lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits of the opCode                                                                                                                             
unsigned char upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits of the opCode                                                                                                                                      
  if( (upper != 0xf) && (lower <0 && lower>8)){
    fprintf(stderr, "Error: ADR error.");
    status = ADR;
    return; 
  }
  int *num = (int *)(memoryBlock+PC+2);

  int inNum;
  unsigned char inChar;
  switch (*byte) {

  case 0xc0:

      OF = 0;
      SF = 0;

      if (scanf("%c", &inChar) == EOF) {
        ZF = 1;
      }

      else {
        unsigned char *byteToRead = (unsigned char *)(memoryBlock+registers[lower]+*num);
        ZF = 0;
        *byteToRead = inChar;
      }

      PC += 6;

    break;

  case 0xc1:

      OF = 0;
      SF = 0;
 if (scanf("%d", &inNum) == EOF) {
        ZF = 1;

      }

      else {

        int *longToRead = (int *)(memoryBlock+registers[lower]+*num);
        ZF = 0;
        *longToRead = inNum;

      }
      PC += 6;
      break;

  }

}


void write(unsigned char *byte) {
unsigned char  lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits of the opCode                                                                                                                              
unsigned char upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits of the opCode                                                                                                                                      
 int lowRegisterVal = registers[lower];



 int *num = (int *)(memoryBlock+PC+2);
  switch (*byte) {

  case 0xd0:
    if( (upper != 0xf) && (lower <0 && lower>8)){
      fprintf(stderr, "Error: ADR error.");
      status = ADR;
      return;
    }


  unsigned char *byteToWrite = (unsigned char *)(memoryBlock+*num+lowRegisterVal);
      printf("%c", *byteToWrite);
      PC += 6;
      break;


  case 0xd1:
    if( (upper != 0xf) && (lower <0 && lower>8)){
      fprintf(stderr, "Error: ADR error.");
      status = ADR;
      return;
    }
  int *longToWrite = (int *)(memoryBlock+*num+lowRegisterVal);
     printf("%d", *longToWrite);
     PC += 6;
     break;
  }

}


void push(int index) {

  int decrEsp = registers[4]-4;

  int *memArray = (int *)(memoryBlock+decrEsp);

  *memArray = registers[index];

  registers[4] = decrEsp;

  PC += 2;

}

void pop(int index) {
int incrEsp = registers[4]+4;

  int *memArray = (int *)(memoryBlock+registers[4]);

  registers[index] = *memArray;

  registers[4] = incrEsp;

  PC += 2;

}

void jump(unsigned char *byte){
  int *value = (int*)(memoryBlock+PC+1);

  switch(*byte){
  case 0x70:

    PC = *value;  
    break;

  case 0x71:
  // printf("initial PC in 0x71 jmp: %d\n", PC);
    if (((SF^OF)|ZF) == 1) 
      PC = *value;
    
    else
      PC= PC+5;
    
    break;

  case 0x72:
  // printf("initial PC in 0x72 jmp: %d\n", PC);
    if ((SF ^ OF) == 1) 
      PC = *value;
    
    else
      PC += 5;
      
    break;

  case 0x73:
  // printf("initial PC in 0x73 jmp: %d\n", PC);
    if (ZF==1) 
      PC = *value;
    
    else
      PC += 5;
    
    break;

  case 0x74:
  // printf("initial PC in 0x74 jmp: %d\n", PC);
    if (ZF==0)
      PC = *value;
    
    else
      PC += 5;
    
  
    break;

  case 0x75:
  // printf("initial PC in 0x75 jmp: %d\n", PC);
    if ((SF^OF)==0) 
      PC = *value;
    
    else
      PC += 5;
    
    break;

  case 0x76:
   //printf("initial PC in 0x76 jmp: %d\n", PC);
    if (((SF^OF)&ZF)==0)
      PC = *value;
    
    else
      PC += 5;
    
   //printf("final PC in 0x76 jmp: %d\n", PC);
    break;

  default:
    status = INS;
    break;
  }
}




void executeMemory(char *memoryBlock){  //lower is the first 4 bits and upper is the last 4 bits of the byte

   unsigned char upper;
   unsigned char lower;

  while(status == AOK){

    unsigned char *byte =(unsigned char*) (memoryBlock+PC);                                                                                                                                             
                                                                                                                                                                               
    
    switch(*byte) {
    case 0x00: //NOP
    case 0x10: //HLT

      if(*byte == 0x00){
	PC++;

	break;
      }
      status = HLT;
      break;
      




      /*RRMOVL 
	Moves a value in a register to another register 
      */
      case 0x20:
     
      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits of the opCode 
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits of the opCode
 
     if(upper < 0 || upper> 8 || lower <0 || lower >8){

       status = ADR; 
       break;
     }

     registers[upper] = registers[lower];

     PC = PC + 2 ; //moves PC past the register byte to the next command


     break;






     /*  IRMOVL
	 Moves an immediate value into a register
     */
	 case 0x30:

      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4;
      upper = (memoryBlock[PC+1] & 0x0f);    
      if( upper <0 || upper >8 || lower != 0xf){
	fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }
      int * value = (int *) (memoryBlock + PC + 2);

      PC = PC + 6;
  
      registers[upper] = *value;


      break;





      /*  RMMOVL 
      Moves a value in register to memory
      */
    case 0x40: 


	lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits
	upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits

      if(upper < 0 || upper>8 || lower <0 || lower >8){
	fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }

      int * val = (int *) (memoryBlock + PC + 2);
      int address = registers[upper] + *val;
      int * memory = (int *) (memoryBlock + address);

 *memory = registers[lower];

	PC = PC + 6;
      break;





      /* MRMOVL
	 Moves a value in memory to a register
       */
    case 0x50:{
    
      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                   
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits      
      if(upper < 0 || upper> 8 || lower <0 || lower >8){
        fprintf(stderr,"Error: ADR error.\n");
        status = ADR;
        break;
      }


  int * value = (int *) (memoryBlock + PC + 2);

 int addr = registers[upper] + *value;

int *memoryAddr = (int *) (memoryBlock+addr); 

 registers[lower] = *memoryAddr;   //CHECK THIS

 PC = PC + 6;

      break;
    }



      /*   ADDL
       */
    case 0x60:
    
    lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                   
    upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits 
    
    int sum = registers[upper] + registers[lower]; 
    registers[upper] = sum;

    if(upper < 0 || upper> 7 || lower <0 || lower >7){
      fprintf(stderr,"Error: ADR error.\n");
      status = ADR;
      break;
    }
    if(sum == 0)
      ZF = 1;
    else
      ZF = 0;
    if( (registers[upper] >0 && registers[lower]>0 && sum <0) || (registers[upper]<0 && registers[lower]<0 && sum>0)){
      OF = 1;
    }
    else
      OF = 0;
    if(sum<0)
      SF = 1;
    else 
      SF = 0;

    PC = PC + 2;

    break;


    /*SUBL
     */ 
    case 0x61:
      //printf("In SUBL\n");

      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                    
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits                                                                                                                                                           
     // printf("initial PC in subl = %d\n", PC);
      if(upper < 0 || upper> 8 || lower <0 || lower >8){
        fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }
      int diff = registers[upper] - registers[lower];
      registers[upper] = diff;

      if(diff == 0)
	ZF = 1;
      else
	ZF = 0;
      if( (registers[upper]>0 &&registers[lower]<0 && diff <0) || (registers[upper]<0 &&registers[lower]>0 && diff>0)){
	OF = 1;
      }
      else
	OF = 0;
      if(diff<0)
	SF = 1;
      else
	SF = 0;

      PC = PC+ 2;

      break;




      /*   ANDL
       */
    case 0x62:


      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                    
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits                                                                                                                                                           
      if(upper < 0 || upper> 8 || lower <0 || lower >8){
        fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }
//printf("before register up: %d\n", registers[upper]);
      int and = registers[upper] & registers[lower];
      registers[upper] = and;
      //printf("after register up: %d\n", registers[upper]);
      OF = 0;

      if(and == 0)
	ZF = 1;
      else
	ZF = 0;
      if(and <0) 
	SF = 1;
      else 
	SF = 0;
      PC = PC+2;

      break ;





      /*   XORL
       */
    case 0x63:


      if(upper < 0 || upper> 8 || lower <0 || lower >8){
        fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }
      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                    
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits  
      int exor = registers[upper] ^ registers[lower];
      registers[upper] = exor;
      OF = 0;

      if(exor == 0)
	ZF = 1;
      else 
	ZF = 0;
      if(exor <0)
	SF = 1;
      else
	SF = 0;
	
	PC = PC +2;
      break;





      /*   IMULL
       */
    case 0x64:


      if(upper < 0 || upper> 7 || lower <0 || lower >7){
        fprintf(stderr,"Error: ADR error.\n");
        status = ADR;
        break;
      }

      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                    
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits      
      int product = registers[upper] * registers[lower];
registers[upper] = product; 
      if( (registers[lower] > 0 && registers[upper] > 0 && product < 0) || (registers[lower] < 0&& registers[upper] < 0&& product < 0) ||
          ( registers[lower] < 0 && registers[upper] > 0 && product >0) || (registers[lower] > 0 && registers[upper] < 0 && product >0) ){
	OF = 1;
      }
      else
	OF = 0;
      if(product <0)
	SF = 1;
      else 
	SF = 0;
     
      if(product == 0)
	ZF = 1;
      else
	ZF = 0;
          
	 PC = PC+2;
      break;


      /*   CMPL
       */

    case 0x65:

 if(upper < 0 || upper> 7 || lower <0 || lower >7){
        fprintf(stderr,"Error: ADR error.\n");
        status = ADR;
        break;
      }
      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                    
      upper = (memoryBlock[PC+1] & 0x0f);
	
	int compare = registers[upper] - registers[lower];
	if (compare == 0)
	ZF = 1;
	else 
	ZF = 0;

	if (compare > 0)
	SF = 0;
	else 
	SF = 1;

PC = PC + 2;

break;
	


    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
    case 0x76:
      //printf("In a JMP\n");

      jump(byte);
      break;


      /*   CALL
       */
    case 0x80:{
      //printf("In CALL (calls push)p\n");
   
	int *value=(int*)(memoryBlock+PC+1);
//printf("value after PC= %d\n",*value);
    registers[8] = PC+5;
      push(8);

      PC = *value;


      break;
    }

      /*   RET
       */
    case 0x90:
     // printf("In RET\n");

      pop(8);  //changed to 7 from 8
      PC = registers[8];
      break;

      /*   PUSH
       */
    case 0xa0:


      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                 
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits                                                                                                                                                          
                                                                                                                                   
      if ((upper == 0xf) && (lower >= 0 && lower< 9 )){ 

     push(lower);
    } 
      else {
        fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	 
      }

      break;


      /* POP
       */
    case 0xb0:
     // printf("In POP\n");

      lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits                                                                                                                                                   
      upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits      

      if ((upper == 0xf) && (lower >= 0 && lower< 9 )){
	pop(lower);
      }
      else{
	fprintf(stderr,"Error: ADR error.\n");
        status = ADR;
      }

      break;

    case 0xc0:
    case 0xc1:
     // printf("In READ\n");

      read(byte);
      break;

    case 0xd0:
    case 0xd1:
      //printf("In WRITE\n");

      write(byte);
      break;

/*MOVSBL
*/

   case 0xe0:

	lower = (memoryBlock[PC+1] & 0xf0 ) >> 4; //first 4 bits
	upper = (memoryBlock[PC+1] & 0x0f); //last 4 bits


      if(upper < 0 || upper>8 || lower <0 || lower >8){
	fprintf(stderr,"Error: ADR error.\n");
	status = ADR;
	break;
      }

	 char *hexStr = malloc (sizeof(char) *4);
char a, b, c, d;
a = memoryBlock[PC + 2];
b = memoryBlock[PC + 3];
c = memoryBlock[PC + 4];
d = memoryBlock[PC + 5];

sprintf(hexStr, "%02x %02x %02x %02x", d, c, b, a);
int result = strtol(hexStr, NULL, 16);

int shiftingByte;
shiftingByte = (int) memoryBlock[registers[upper]+result];  int leadingByte;
leadingByte = shiftingByte & 0x000000ff;  int shiftedValue;   if(leadingByte >= 0) 
    shiftedValue = leadingByte ^ 0x00000000;  else     shiftedValue = leadingByte ^ 0xffffff00;  registers[lower] = shiftedValue;


	PC = PC + 6;
break;

 default:
      fprintf(stderr,"Error: INS error.\n");
      status = INS;
	break;
    } //end of switch
    
if(status == HLT){
      fprintf(stderr, "Halt statment entered\n");
      return ;
    }


  }//end of while status=AOK 

}



void completeText(int hexAddr, char *tok){

  int length = strlen(tok);

  int tokIndex = 0;
  int curr_index = hexAddr;
  
while(tokIndex < length ){    

     char *low = strdupForText(tok+tokIndex);
     memoryBlock[curr_index] =strtol(low, NULL, 16);
    
    free(low);
    tokIndex += 2;

    curr_index++;

    }
}

int main(int argc, char **argv){
  int size; //size directive as a decimal. 
  char line [2000]; 
  char *tok;
  int textBool = 0; 
  
  //printf("Test\n");
  if(argc != 2){ //check for proper # of arguments
    fprintf(stderr, "Error: Need 2 arguemnts.\n");
    return 0;
  }
  if(strcmp (argv[1], "-h")==0){ //check if help flag entered 
    printf("Proper usage: y86emul <y86 input file>\n");
    return 0;
  }

  FILE *fp = fopen(argv[1], "r");
  if(fp == NULL){
    fprintf(stderr, "No file given\n");
    return 0;
  }
  int lineNum = 1; 
  char tempStr [10];
  char directive[15];
  int hexAddr;
  int value;
  char charNum;
  while(fgets(line, 2000, fp) != NULL){

    if(lineNum == 1){ //scanning and entering size information. 
      sscanf(line, "%s %x\n", tempStr, &size); 
      //printf("directive name: %s\n", tempStr);    
      if(strcmp(tempStr, ".size") != 0){
	fprintf(stderr, "Error: Size not given.\n");
	return 0;
      }
      //printf("size: %d\n", size);
      memoryBlock =(char *) malloc( sizeof(char) * size); //creating the memory block 
    }
    else{
      tok = strtok(line, " \n\t"); //tok is what directive should be

      //Start of directive checks
      if ( (strcmp(tok, ".string") == 0 || strcmp(tok, ".long") == 0 || strcmp(tok, ".bss") == 0 || 
	      strcmp(tok, ".byte") == 0 || strcmp(tok, ".text") == 0) ) //directive is non-size valid directive 
      { //directive is either long, string, byte, bss, or text
	
      strcpy(directive, tok);
      
     
      tok = strtok(NULL, "\n\t"); //moves to next string (hex address) 
      sscanf(tok, "%x", &hexAddr);//puts the hex address in int hex Addr
      

           if(strcmp(directive, ".long") == 0){  //check if its a .long directive
	     //printf("entered here!\n"); 
	     tok = strtok(NULL, "\n\t");   
	     sscanf(tok, "%d", &value); //gets the long value (3rd arg)

	     memoryBlock[hexAddr] = value;

	   }

	   else if(strcmp(directive, ".byte") == 0){ //check if its a .byte directive 
             tok = strtok(NULL, "\n\t");
	     sscanf(tok, "%2x", &value); //take val of char and puts in value
	     //printf("Value: %d\n", value);
	     charNum = value;

	     memoryBlock[hexAddr] = charNum;

	   }

	   else if(strcmp(directive, ".string") == 0){ //check if its a .string directive
	     tok = strtok(NULL, "\n\t");
	     int length = strlen(tok);
	     char arg[length];
	     //arg = strdup(tok);
	     strcpy(arg, tok);
	     //printf("string: %s\n", arg);
	     int i=1;
	     while (i<length-1){
     	       //printf("Arg char: %c\n", arg[i]);
	       memoryBlock[hexAddr] = (unsigned char) arg[i];
	       //printf("Mem block char: %c\n", memoryBlock[hexAddr]);
	       i++;
	       hexAddr++;
	     }
	   }

	   else if(strcmp(directive, ".bss") == 0){ //check if its a .bss directive
             tok = strtok(NULL, "\n\t");
	     sscanf(tok, "%d", &value);
	     for (; value > 0; value--) {
	       memoryBlock[hexAddr] = (unsigned char)0; 
	       hexAddr++;
	     }
	   }

	   else if(strcmp(directive, ".text") == 0){
	     textBool = 1;
	     PC = hexAddr; //initializing the program counter
             tok = strtok(NULL, "\n\t");
	     completeText(hexAddr, tok);
	   }
	   else{
	     fprintf(stderr, "Error: Directive is incorrect.\n");
	     return 0;
	   }
      } 

    else{ //unknown directive entered
      status = INS;
      fprintf(stderr, "Error: INS status because invalid directive.\n");
      return 0;
         }
    }
    lineNum =0;
  } //end of all directives being stored 
  if(textBool == 0){
    fprintf(stderr, "Error: Text file not found.\n");
    return 0;
  }
  if(status != INS || status != ADR)
    status = AOK;

  fclose(fp);
  
  executeMemory(memoryBlock);

  free (memoryBlock);
return 0;
}
