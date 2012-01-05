MAKE      = make --no-print-directory
CC        = gcc
CLINKER   = $(CC)
PATHFILE  = ./
SHELL = /bin/sh

CFLAGS = -O2  -Wchar-subscripts -Wcomment -Wformat=2 -Wimplicit-int -Werror-implicit-function-declaration -Wmain -Wparentheses -Wsequence-point -Wreturn-type -Wswitch -Wtrigraphs -Wunused \
			 -Wuninitialized -Wunknown-pragmas -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wbad-function-cast -Wwrite-strings \
			-Wconversion -Wsign-compare -Waggregate-return -Wstrict-prototypes -Wmissing-prototypes -Wmissing-declarations -Wmissing-noreturn \
			-Wformat -Wmissing-format-attribute -Wno-deprecated-declarations -Wpacked -Wredundant-decls -Wnested-externs -Winline -Wlong-long \
			-Wunreachable-code  

CLIBS = -lm


all	: advdiff

clean:
	-rm -f $(PATHFILE)*.o $(PATHFILE)advdif.exe


# code ADVDIF 
# ----------
ADVDIFF_O =  $(PATHFILE)/advdiff.o 


advdiff : $(ADVDIFF_O)
	$(CC) $(CFLAGS) $(ADVDIFF_O) -o $(PATHFILE)/advdiff.exe  $(CLIBS)

	


#------------ advdiff -----------------------
$(PATHFILE)advdiff.o	: $(PATHFILE)/advdiff.c $(PATHFILE)advdiff.h
		  $(CC) $(CFLAGS) -c $(PATHFILE)/advdiff.c


# cleaning
# ----------

