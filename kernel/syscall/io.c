 /* @file: io.c
  * 
  * Contains read and write syscall handlers 
  * and utility functions required by them.
  * 
  * @author: Group Member 1: Soumyaroop Dutta <soumyard>
  *          Group Member 2: Pooja Mangla <pmangla>
  *          Group Number: 18
  *          Date: Dec 3, 2015
  */



#include "CswiHandler.h"


/* Read SWI Handler: Invoked when the programmer wants the
 *         program to read an input from the user
 * 
 * Arguments: reg : the buffer in which user input is read
 */
int read_swi_handler(unsigned* reg){

    int FD = reg[0];
    char* buffer =  (char*)reg[1]; 
    int count = reg[2];
    int result=0;

    if(FD != STDIN_FILENO){
      return -EBADF;
    }

     //check for user stack area limit.
    if((unsigned)(buffer+count) > SDRAM_UP || 
	(unsigned)(buffer) < SDRAM_DOWN){
       return -EFAULT;
    }

     
    while(count>0){
    
       buffer[result] = (char) getc();
       
       //check for new line character.   
       if(buffer[result] == '\n' || buffer[result] == '\r')
         {
           buffer[result] = '\n';
           putc(buffer[result]);
           return (result+1);
         }
       //check for backspace and delete
       if(buffer[result] == '\b' || buffer[result] == DELETE)
       {
           if(result != 0){
           puts("\b \b");
       //in case of a bacckspace, remove the last element and delete the
       //last one from the screen
           result--;
           count++;
          }
           continue;	
       }
 
       if(buffer[result] == ENDOFTRANS)
       {
           return result;
       }
  
       //print it to the screen. 
       putc(buffer[result]);
       count--;

       result++;
    }
    
    return result;	
}

/* Write SWI Handler: Invoked when the programmer wants the
 *         program to write an output to the console
 * 
 * Arguments: reg : the buffer in which the output is stored
 */

int write_swi_handler(unsigned *reg){
    int FD = reg[0];
    char* buffer =  (char*)reg[1]; 
    int count = reg[2];
    int result=0;

    if(FD != STDOUT_FILENO){
      return -EBADF;
    }

     //check for user stack area limit.
    if((((unsigned)(buffer+count) > SDRAM_UP ||
	(unsigned)(buffer) < SDRAM_DOWN)) &&
	((unsigned)(buffer) > STRATA_FLASH)){
       
       return -EFAULT;
    }
    
    //print the buffer, till the amount asked to be written
    //has been completed, or the buffer gets over.
    while(count>0 && buffer[result] != '\0'){
       putc(buffer[result]);
       result++;
       count--;
    }
    
    return result; 
}
    
