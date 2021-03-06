/*
COPYRIGHT (C) 2009  Roberto Bucher (roberto.bucher@supsi.ch)

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA.
*/

#include <pyblock.h>
#include <nuttx/config.h>

#include <sys/ioctl.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include <nuttx/sensors/qencoder.h>

static void init(python_block *block)
{
  int * intPar    = block->intPar;
  int fd;

  fd = open(block->str, O_RDONLY);
  if(fd<0) {
    fprintf(stderr,"Error opening device: %s\n", block->str);
    exit(1);
  }
  intPar[1] = fd;
  if(intPar[0]){
    int ret = ioctl(fd, QEIOC_RESET, 0);
    if (ret < 0){
      fprintf(stderr,"qe_main: ioctl(QEIOC_RESET) failed: %d\n", errno);
      close(fd);
      exit(1);
    }
  }
}

static void inout(python_block *block)
{
  int * intPar    = block->intPar;
  double * realPar = block->realPar;
  double *y = block->y[0];
  int ret;
  int fd = intPar[1];
  int32_t position;

  ret = ioctl(fd, QEIOC_POSITION, (unsigned long)((uintptr_t) &position));
  if (ret < 0){
    fprintf(stderr, "qe_main: ioctl(QEIOC_POSITION) failed");
    close(fd);
    exit(1);
  }
  
  y[0] = 1.0*position/realPar[0]; 
}

static void end(python_block *block)
{
  int * intPar    = block->intPar;
  close(intPar[1]);
}

void nuttxENC(int flag, python_block *block)
{
  if (flag==CG_OUT){          /* get input */
    inout(block);
  }
  else if (flag==CG_END){     /* termination */ 
    end(block);
  }
  else if (flag ==CG_INIT){    /* initialisation */
    init(block);
  }
}


