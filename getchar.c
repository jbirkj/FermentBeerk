/* Program for reading a key on STDIN without waiting */
/* Copyleft (c) 2014 - Christian Paulsen              */

#define _BSD_SOURCE

#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

int main(int argc, char **argv) 
{
  long flg;
  struct termios term, oldterm;
  long counter=0;
  char lastchar=' ';

  if ((flg = fcntl(STDIN_FILENO, F_GETFL)) == (long)-1) {
    perror("fcntl(STDIN_FILENO, F_GETFL)");
    return -1;
  }
  flg |= O_NDELAY;
  if (fcntl(STDIN_FILENO, F_SETFL, flg) == -1) {
    perror("fcntl(STDIN_FILENO, F_SETFL)");
    return -1;
  }

  if (isatty(STDIN_FILENO)) {
    if (tcgetattr(STDIN_FILENO, &term)==-1) {
      perror("tcgetattr(STDIN_FILENO, F_SETFL)");
      return -1;
    }

    oldterm=term;
    cfmakeraw(&term);
    if (tcsetattr(STDIN_FILENO, TCSANOW, &term)==-1) {
      perror("tcgetattr(STDIN_FILENO, F_SETFL)");
      return -1;
    }
  }

  while ( 1 ) {

    int ch;

    ch = getchar();

    if (ch == 'q' ) {
      break;
    }

    if ( ch == EOF ) {
      counter++;
      printf("%ld HEJ - lastchar = %c\r", counter, lastchar);
      fflush(stdout);
      usleep(1);
    } else {
      lastchar = (char)ch;
    }
  }

  tcsetattr(STDIN_FILENO, TCSANOW, &oldterm);

  printf("\n");

  return 0;
}
