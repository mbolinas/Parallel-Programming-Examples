#include <stdio.h>
#include <stdlib.h>

//lengths just stores a single int, not an array of ints from argv
//lengths was supposed to point to the start of an array of ints, but it's easier just to access argv now and use that as the array
int *lengths;
int **data;

int main(int argc, char **argv){

  lengths = malloc(sizeof(int*));
  *lengths = argc - 1;//don't count name of the program as an argument
  data = malloc(*lengths * sizeof(int*));
  
  //for each argument, allocate an amount of memory equal to the size of argument
  //if argv = 1, 2, 3 then we allocate a size of 1 to data[0], size of 2 to data[1], etc
  //then we fill those arrays with incrementing ints
  int argnum = 1;
  int currentint = 0;
  while(argnum <= *lengths){
    char *tmp = *(argv + argnum);
    int rowlength = *tmp - '0';

    data[argnum - 1] = malloc(rowlength * sizeof(int*));
    int count = 0;
    while(count < rowlength){
      *(data[argnum - 1] + count) = currentint;

      count++;
      currentint++;
    }
    
    

    argnum++;
  }

  //print statements
  int row = 0;
  while(row < *lengths){
    int arroffset = 0;

    char *tmp = *(argv + row + 1);
    int arrlength = *tmp - '0';
    
    while(arroffset < arrlength){
      int n = *(data[row] + arroffset);
      printf("%d ", n);
      arroffset++;
    }
    printf("\n");
    row++;
  }


  //memory deallocation
  argnum = 1;
  while(argnum <= *lengths){
    free(data[argnum - 1]);
    argnum++;
  }  
  free(lengths);
  free(data);

  return 0;

}
