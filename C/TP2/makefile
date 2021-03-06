include libconfig.in
MAKE = make  --no-print-directory
CC = gcc
CLINKER = $(CC)
SHELL = /bin/sh
MV = mv
SRC = .
PATHFILE = .
OBJ = $(PATHFILE)/obj/$(CC)
EXE = $(PATHFILE)/bin
ADD_LIB = $(ADD_LIB_CFG) -L$(LIB_PERS)
ADD_INC = $(ADD_INC_CFG) -I$(INC_PERS) 
HEADERS = $(SRC)/include
LIB_PERS = $(SRC)/libs/
HEADERS = $(SRC)/include
CFLAGS= -O2  -g -ggdb -fstrict-aliasing -ffast-math -std=c99 -ansi -pedantic -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Werror-implicit-function-declaration -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings -Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wmissing-noreturn -Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long -Wunreachable-code -DH5Gcreate_vers=2 -DH5Dcreate_vers=2 -DH5Gopen_vers=2 -DH5Dopen_vers=2
LDFLAGS=$(ADD_LIB) -lm 
PRGM_O =  $(OBJ)/RK4.o $(OBJ)/eule.o $(OBJ)/euli.o $(OBJ)/invmat.o $(OBJ)/myfiles.o $(OBJ)/mystrings.o $(OBJ)/input.o $(OBJ)/MemAlloc.o $(OBJ)/alloc.o $(OBJ)/H2D.o $(OBJ)/init.o $(OBJ)/boundary.o $(OBJ)/step.o $(OBJ)/cn.o $(OBJ)/functions.o

all	: h2d_gcc.exe
clean:
	-rm $(OBJ)/*.o $(EXE)/h2d_gcc.exe
h2d_gcc.exe : $(PRGM_O)
	$(CC) $(CFLAGS) $(PRGM_O) $(CLIBS) $(LDFLAGS) -o $(EXE)/h2d_gcc.exe

#-------- RK4.c --------#
$(OBJ)/RK4.o	 : include/H2D_Func.h include/schemes.h include/H2D_types.h 
		$(CC) $(CFLAGS) -c ./RK4/RK4.c
		$(MV) RK4.o $(OBJ)/RK4.o

#-------- eule.c --------#
$(OBJ)/eule.o	 : include/H2D_Func.h include/schemes.h include/H2D_types.h 
		$(CC) $(CFLAGS) -c ./eule/eule.c
		$(MV) eule.o $(OBJ)/eule.o

#-------- euli.c --------#
$(OBJ)/euli.o	 : include/H2D_types.h include/H2D_Func.h params/params.h 
		$(CC) $(CFLAGS) -c ./euli/euli.c
		$(MV) euli.o $(OBJ)/euli.o

#-------- invmat.c --------#
$(OBJ)/invmat.o	 : include/H2D_types.h include/libraries.h include/common.h include/H2D_Func.h 
		$(CC) $(CFLAGS) -c ./libs/invmat.c
		$(MV) invmat.o $(OBJ)/invmat.o

#-------- myfiles.c --------#
$(OBJ)/myfiles.o	 : libs/files/headers/myfiles.h libs/strings/headers/mystrings.h libs/MemAlloc/headers/MemAlloc.h 
		$(CC) $(CFLAGS) -c ./libs/files/src/myfiles.c
		$(MV) myfiles.o $(OBJ)/myfiles.o

#-------- mystrings.c --------#
$(OBJ)/mystrings.o	 : libs/strings/headers/mystrings.h 
		$(CC) $(CFLAGS) -c ./libs/strings/src/mystrings.c
		$(MV) mystrings.o $(OBJ)/mystrings.o

#-------- input.c --------#
$(OBJ)/input.o	 : libs/inputs/headers/input.h libs/MemAlloc/headers/MemAlloc.h 
		$(CC) $(CFLAGS) -c ./libs/inputs/src/input.c
		$(MV) input.o $(OBJ)/input.o

#-------- MemAlloc.c --------#
$(OBJ)/MemAlloc.o	 : libs/MemAlloc/headers/alloc.h libs/MemAlloc/headers/MemAlloc.h 
		$(CC) $(CFLAGS) -c ./libs/MemAlloc/src/MemAlloc.c
		$(MV) MemAlloc.o $(OBJ)/MemAlloc.o

#-------- alloc.c --------#
$(OBJ)/alloc.o	 : libs/C_Norm/commonhead.h libs/MemAlloc/headers/alloc.h 
		$(CC) $(CFLAGS) -c ./libs/MemAlloc/src/alloc.c
		$(MV) alloc.o $(OBJ)/alloc.o

#-------- H2D.c --------#
$(OBJ)/H2D.o	 : include/H2D.h params/params.h 
		$(CC) $(CFLAGS) -c ./main/H2D.c
		$(MV) H2D.o $(OBJ)/H2D.o

#-------- init.c --------#
$(OBJ)/init.o	 : include/H2D_types.h include/H2D_Func.h params/params.h 
		$(CC) $(CFLAGS) -c ./main/init.c
		$(MV) init.o $(OBJ)/init.o

#-------- boundary.c --------#
$(OBJ)/boundary.o	 : include/H2D_types.h include/H2D_Func.h 
		$(CC) $(CFLAGS) -c ./main/boundary.c
		$(MV) boundary.o $(OBJ)/boundary.o

#-------- step.c --------#
$(OBJ)/step.o	 : include/H2D_types.h include/H2D_Func.h 
		$(CC) $(CFLAGS) -c ./main/step.c
		$(MV) step.o $(OBJ)/step.o

#-------- cn.c --------#
$(OBJ)/cn.o	 : include/H2D_types.h include/H2D_Func.h params/params.h 
		$(CC) $(CFLAGS) -c ./cranck/cn.c
		$(MV) cn.o $(OBJ)/cn.o

#-------- functions.c --------#
$(OBJ)/functions.o	 : include/common.h include/H2D_Func.h params/params.h 
		$(CC) $(CFLAGS) -c ./params/functions.c
		$(MV) functions.o $(OBJ)/functions.o

