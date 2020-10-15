# generation de programme PVM
#  TP algorithmique parallele
#  maitrise
#  LL
#  07/10/97
# 

CC = gcc -g

# executable directory
BDIR = $(PWD)

# Programmes maitre et esclave
PROG = uhm uhs

#PVM_ROOT = /usr/share/pvm3
#PVM_ARCH = LINUXI386
#PVM_ROOT = /home/commun_depinfo/enseignants/lemarchand/TPPVMM1/PvmLinux/pvm3
#PVM_ARCH = LINUX64


#### do not edit #####

EXEC = $(PROG:%=$(BDIR)/%)

#PVM_LIB = $(PVM_ROOT)/lib/$(PVM_ARCH)/libpvm3.a
#CFLAGS =  -I$(PVM_ROOT)/include -D_POSIX_C_SOURCE=2 -DEPATH=\"$(BDIR)\"
#include $(PVM_ROOT)/conf/$(PVM_ARCH).def

PVM_LIB = -lpvm3
CFLAGS =  -DEPATH=\"$(BDIR)\"

#### generation rules #####

all: $(EXEC)
	
$(BDIR)/uhm: point.o uhm.o 
	$(CC) -o $@ uhm.o point.o $(PVM_LIB) $(ARCHLIB)

$(BDIR)/uhs: point.o uhs.o 
	$(CC) -o $@ uhs.o point.o $(PVM_LIB) $(ARCHLIB)

.c.o: point.h
	$(CC) $(CFLAGS) $(ARCHCFLAGS) -c $< -o $@

clean:
	rm -f *.o $(EXEC)
