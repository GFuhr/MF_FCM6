
#!/usr/bin/env python
import shutil
import os.path
import shlex, subprocess,sys


__author__="Guillaume"
__date__ ="$19 nov. 2010 10:04:00$"



config_options=['-DUSED_ALIGN_MALLOC'];



ICC_CFLAGS=['-xHOST',
            '-Wall',
            '-g',
            '-O3',
            '-std=c89 -unroll16',
            '-ansi_alias',
            '-align',
            '-ipo',
            '-openmp',
            '-vec_report5',
            '-par-report3',
            '-static','-DH5Gcreate_vers=2 -DH5Dcreate_vers=2 -DH5Gopen_vers=2 -DH5Dopen_vers=2'
            ]


ICC_CFLAGS_12=['-xHOST',
            '-Wall',
            '-g',
            '-O3',
            '-std=c89',
            ' -unroll16',
            '-strict-ansi',
            '-ansi-alias',
            '-ansi-const',
            '-fargument-noalias-global',
            '-opt-calloc',
            '-simd',
            '-align',
            '-ipo',
            '-openmp',
            #'-guide',
            #'-guide-data-trans',
            #'-guide-par',
            #'-guide-vec',
            '-vec-report5',
            '-opt-report',
            '-parallel',
            '-opt-report-file=report_vect_icc.txt',
            '-par-report3',
            #'-static',
            '-finline',
            '-restrict',
            '-check-uninit',
            '-w2',
            '-Wall',
            '-Wcheck',
            '-Wfatal-errors',
            '-Wmissing-declarations',
            '-Wmissing-prototypes',
            '-Wstrict-aliasing',
            '-Wuninitialized',
            '-Wunused-function',
            '-Wshadow',
            '-Wwrite-strings',
            '-falign-functions',
            '-fargument-noalias','-DH5Gcreate_vers=2 -DH5Dcreate_vers=2 -DH5Gopen_vers=2 -DH5Dopen_vers=2'
            ]

ICC_LIB=['-limf','-lsvml']
ICC_LIB_12=['-limf', '-lz']

GCC_CFLAGS=[ '-O2  -g -ggdb',
             '-fstrict-aliasing -ffast-math',
             '-std=c99 -ansi -pedantic',
             '-Wchar-subscripts -Wcomment',
             '-Wformat=2 -Wimplicit-int',
             '-Werror-implicit-function-declaration',
             '-Wmain -Wparentheses -Wsequence-point',
             '-Wreturn-type -Wswitch -Wtrigraphs -Wunused',
             '-Wuninitialized -Wunknown-pragmas',
             '-Wfloat-equal -Wundef -Wshadow -Wpointer-arith',
             '-Wbad-function-cast -Wwrite-strings',
             '-Wconversion -Wsign-compare -Waggregate-return',
             '-Wstrict-prototypes -Wmissing-prototypes',
             '-Wmissing-declarations -Wmissing-noreturn',
             '-Wformat -Wmissing-format-attribute',
             '-Wno-deprecated-declarations -Wpacked',
             '-Wredundant-decls -Wnested-externs',
             '-Winline -Wlong-long',
             '-Wunreachable-code -fopenmp','-DH5Gcreate_vers=2 -DH5Dcreate_vers=2 -DH5Gopen_vers=2 -DH5Dopen_vers=2'
             ]


GCC_LIB = ['-lm']

externallib=('fftw');
externallib_info=[['fftw','/usr/local/include/','/usr/lib/']];




FILE_LIST_H2D=['eule.c','clock.h','common.h','config_version.h','H2D.h','H2D_Func.h',
		'H2D_types.h','libraries.h','memory_alloc.h','invmat.c','commonhead.h','C_Norm.h',
		'my_macro.h','myfiles.h','myfiles.c','input.h','input.c','alloc.h','MemAlloc.h',
		'alloc.c','MemAlloc.c','mystrings.h','mystrings.c','boundary.c','H2D.c',
		'init.c','step.c','functions.c','params.h','RK4.c','euli.c','cn.c'];



MAKE_ENV=[['LIB_PERS'   , '$(SRC)/libs/'],
          ['HEADERS'    , '$(SRC)/include'],
          #['PATH_SRC'       , './']
          ];


class Config:
    compil_name ='';
    compil_path='';
    path_lib={};
    needed_lib={};
    config_include='';
    config_lib=[];
    config_flags=[];
    configure=[];

    def GenerateLibDic(self):
        for values in externallib:
            for libinfo in externallib_info:
                if values == libinfo[0]:
                  self.path_lib[values]={'lib':libinfo[2],'include':libinfo[1]};

    def getLib(self):
        return self.config_lib;

    def getInclude(self):
        return self.config_include;

    def generateCLIBS(self):
        if '--with-fftw' in self.configure:
            self.config_lib.append('-lfftw3');



    def necessarylibs(self):
        for libs in externallib:
            self.needed_lib[libs]=False;



        if '--with-fftw' in config_options:
            self.needed_lib['fftw']=True;



    def __init__(self,config=config_options,path='./src',CC='gcc',
    CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB):
        self.configure=config;
        self.compil_name=CC;
        self.compil_options='';
        self.config_flags=CFLAGS;
        self.config_lib=CLIBS;
        self.compil_path=os.path.normpath(path);
        self.necessarylibs();
        self.GenerateLibDic();
        self.generateCLIBS();


class FileParser(Config):
    header_list=[];
    source_path='';
    filename='';

    def __init__(self,filename,config=config_options,path='./src',CC='gcc',
    CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB):
        Config.__init__(self,config,path,CC,CFLAGS,CLIBS);
        if filename!='':
            self.analyse(filename);
            

    def analyse(self,filename):
        self.header_list=[];
        [self.source_path,self.filename]=os.path.split(filename);
        self.source_path=os.path.normpath(self.source_path);
        self.FindInclude();
        return self.header_list;

    def printDependencies(self):
        for elem in self.header_list:
            print '[',elem,']';


    def pathHeader(self,header):
        line_full = os.path.join(self.source_path, header.replace(' ', '').replace('\"', ''));
        return os.path.normpath(line_full).replace('\r\n','').replace('\n','');

    def correctPath(self,buffer_line,curPath):
        tmp_line=buffer_line;
        if (tmp_line.find('libraries')>0)&(tmp_line.find('libraries.h')<0):
            tmp_line=tmp_line.replace(tmp_line[0:tmp_line.find('libraries')+10],
            '$(LIB_PERS)/');
        else:
            if tmp_line.find('include')>0:
                tmp_line=tmp_line.replace(tmp_line[0:tmp_line.find('include')+8]
                ,'$(HEADERS)/');
            else:
                tmp_line=tmp_line.replace(curPath,'$(SRC)/');

        tmp_line=tmp_line[0:len(tmp_line)]+' ';
        return tmp_line;


    def FindInclude(self):
        sourcefile=os.path.join(self.source_path,self.filename);
        rootPath=self.source_path;
        headers='';
        skip=0;
        comment=0;
        fileIn=open(sourcefile,'r');
        for line in fileIn:
            if (line.find('/*')>=0):
                comment=1;
                skip=1;
            if (line.find('*/')>=0):
                if (comment==1):
                    skip=0;
                    continue;


            if line.find('#endif')>=0:
                    skip=0;
                    continue;

            if (((line.find('#include \"')>=0)or (line.find('#include\"')>=0)) and (skip==0)):
                res=self.pathHeader(line.replace('#include','').replace('\"',''));
                self.header_list.append(res);
                headers=headers+self.correctPath(res,rootPath);

        headers=headers.replace('\"','');
        headers=headers.replace('\0',' ');
        headers=headers.replace('\n',' ');
        fileIn.close();

        if self.header_list==[]:
            print 'Warning : no headers found for file : ', self.filename;


class DirectoryListing(FileParser):
    extension_list=['.c'];
    fichiers=[];
    FILE_LIST=[];
    dep_list=[];
    unique_header_list=[];
    dir='';

    def __init__(self,path='../../../src',list=FILE_LIST,config=config_options,CC='gcc',CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB):
        FileParser.__init__(self,'',config,path,CC,CFLAGS,CLIBS);
        self.FILE_LIST=list;
        self.dir=path;
        self.listDirectory();
        self.generateDependencies();



    def addExtension(self,ext):
        self.extension_list.append(ext);

    def listDirectory(self):
        path=self.dir;
        self.fichiers=[];
        for root, dirs, files in os.walk(path):
            for i in files:
                (res,ext)=os.path.splitext(i);
                if ((ext in self.extension_list) and (ext!='')):
                    if i in self.FILE_LIST:
                        self.fichiers.append(os.path.join(root, i));

    def getListFile(self):
        return self.fichiers;

    def printFileDep(self):
        for elem in self.dep_list:
            print '[',elem,']';

        print 'Used headers';
        for elem in self.unique_header_list:
            print 'include [',elem,']';

    def isValid(self,list_headers):
        Valid=True;
#        for headers in list_headers:
#	    print headers;
#            for nlibs in self.needed_lib:
#		print '['+nlibs+']';
#                if ((headers.find(nlibs)>0) and self.needed_lib[nlibs]==False):
#                    Valid=False;

        return Valid;


    def generateDependencies(self):
        i=0;
        self.dep_list=[];
        for files in self.fichiers:
            [path,name]=os.path.split(files);
            valHeader=self.analyse(files);

            if self.isValid(valHeader)==False:
                print 'verification failed for file',files;
                self.fichiers.remove(files);
                continue;

            self.dep_list.append([[],[]]);
            self.dep_list[i][0].append(files.replace('\\','/'));
            self.dep_list[i][1].append(valHeader);
            for headers in valHeader:
                [path_head,name_head]=os.path.split(headers);

                if path_head not in self.unique_header_list:
                    self.unique_header_list.append(path_head);
            i=i+1;


    def printListFile(self):
        print 'number of files found : ', len(self.fichiers);
        print self.fichiers;



class CompilParam(DirectoryListing):
    compil_include='';
    compil_lib='';
    compil_options='';


    def __init__(self,config=config_options,path='./src',CC='gcc',CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB,FILE_LIST=FILE_LIST_H2D):
        DirectoryListing.__init__(self,path,FILE_LIST,config,CC,CFLAGS,CLIBS);
        self.generateCFLAGS();
        self.compilPath();


    def compilPath(self):
        self.compil_include='';
        self.compil_lib='';
        for header in self.unique_header_list:
            [path,name]=os.path.split(header);
            self.compil_include=self.compil_include+' -I'+path;
#        for libs in self.needed_lib:
#	    print libs;
#            if self.needed_lib[libs]==True:
#                self.compil_include=self.compil_include+' -I'+self.path_lib[libs]['include'];
#                self.compil_lib=self.compil_lib+' -L'+self.path_lib[libs]['lib'];
        for flags in self.config_lib:
            self.compil_lib=self.compil_lib+' '+flags;

    def generateCFLAGS(self):
        self.compil_options=''
        for flags in self.config_flags:
            self.compil_options=self.compil_options+' '+flags;


class CommandLine(DirectoryListing,CompilParam):
    list_files='';
    exec_cmd='';

    def __init__(self,path='./src',CC='gcc',config=config_options,CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB,FILE_LIST=FILE_LIST_H2D):
        CompilParam.__init__(self,config,path,CC,CFLAGS,CLIBS,FILE_LIST);

    def generateCommandLine(self):
        self.exec_cmd=self.compil_name+' '; #nom du compilo
        self.exec_cmd=self.exec_cmd+self.compil_options+' ' ; #flags associes
        #liste des fichiers c
        for files in self.fichiers:
            self.exec_cmd=self.exec_cmd+' '+files;

        #chemins a inclure pour les entetes et librairies
        self.exec_cmd=self.exec_cmd+' '+self.compil_include;
        self.exec_cmd=self.exec_cmd+' '+self.compil_lib;

    def _execCmd(self,cmd,filename='soft.exe'):
        args=shlex.split(cmd);
        try:
            proc = subprocess.Popen(args, shell=False,stdout=subprocess.PIPE,stderr=subprocess.STDOUT);
            stdout_value = proc.communicate()[0];

            while True:
                retcode=proc.wait();
                if retcode != None: break;
                
            if retcode != 0:
                print >>sys.stderr, 'compilation error for file {0}'.format(filename)
                print 'See {0}.log for details'.format(filename)

        except OSError, e:
            print >>sys.stderr, "Execution failed:", e

        #log of compilation outputs
        logfile=filename+'.log';
        flog=open(logfile,'w');
        flog.write(stdout_value);
        flog.close();
        return retcode;

    def Compil(self,filename='soft.exe'):
        self.generateCommandLine();
        self.saveExecLine(self.compil_name+'_compil_'+filename+'.sh');

        print 'Compilation command used :\n'
        print '--'*30+'\n'
        self.printExecLine();
        print '\n'+'--'*30+'\n';
        
        if (self._execCmd(self.exec_cmd+' -o '+filename,filename)!=0): #compilation error =>executable not found
            #check compilation of each object
            self._compilObj();

    def _compilObj(self):
        exec_cmd_obj=self.compil_name+' '; #nom du compilo
        exec_cmd_obj=exec_cmd_obj+self.compil_options+' ' ; #flags associes
        #liste des fichiers c

        for obj in self.fichiers:
            shortname = os.path.split(obj)[1];
            shortname_obj = shortname.replace('.c','.o');
            cmd=exec_cmd_obj+' -c '+obj;
            retval=self._execCmd(cmd,shortname_obj);
            if (os.path.isfile(shortname_obj)==True):
                os.remove(shortname_obj);

    def printExecLine(self):
        print self.exec_cmd;

    def saveExecLine(self,filename='gcc_compil.sh'):
        filehandle=open(filename,'w');
        filehandle.write(self.exec_cmd);
        filehandle.close();



class pyMake(CommandLine):
    make_name='';
    make_var_list_path=[[]];
    make_var_list_opt=[[]];
    softname ='';

    def __init__(self,makefilename='makefile',exename='emedge.exe',path='./src',CC='gcc',config=config_options,CFLAGS=GCC_CFLAGS,CLIBS=GCC_LIB,FILE_LIST=FILE_LIST_H2D):
        CommandLine.__init__(self,path,CC,config,CFLAGS,CLIBS,FILE_LIST);
        self.make_name=makefilename;
        self.softname=exename;
        self.make_var_list=MAKE_ENV;

    def _genCommonVarEnv(self):
        comm_var_en=[['MAKE'   , 'make  --no-print-directory'],
          ['CC'                , self.compil_name],
          ['CLINKER'           , '$(CC)'],
          ['SHELL'             , '/bin/sh'],
          ['MV'                , 'mv'],
          ['SRC'               , self.compil_path],
          ['PATHFILE'          , os.path.normpath(os.path.join('$(SRC)'      ,'../'))],
          ['OBJ'               , os.path.normpath(os.path.join('$(PATHFILE)' ,'obj/$(CC)'))],
          ['EXE'               , os.path.normpath(os.path.join('$(PATHFILE)' ,'bin'))]
          ];

        fconfig = open('libconfig.in','w');
        cfgaddL='ADD_LIB_CFG = ';
        cfgaddI='ADD_INC_CFG = ';
        for libs in self.needed_lib:
            if self.needed_lib[libs]==True:
                #comm_var_en.append(['INC_'+libs.upper(),self.path_lib[libs]['include']]);
                #comm_var_en.append(['LIB_'+libs.upper(),self.path_lib[libs]['lib']]);
                string='INC_'+libs.upper()+' = '+ self.path_lib[libs]['include']+'\n';
                string=string+'LIB_'+libs.upper()+' = '+ self.path_lib[libs]['lib']+'\n';
                cfgaddL=cfgaddL+'-L$('+'LIB_'+libs.upper()+') ';
                cfgaddI=cfgaddI+'-I$('+'INC_'+libs.upper()+') ';
                fconfig.write(string);
        fconfig.write(cfgaddI+'\n');
        fconfig.write(cfgaddL+'\n');
        fconfig.close();                

        comm_var_en.append(['ADD_LIB'    , '$(ADD_LIB_CFG) -L$(LIB_PERS)']);
        comm_var_en.append(
            ['ADD_INC'    , '$(ADD_INC_CFG) -I$(INC_PERS) ']);

        comm_var_en.append(['HEADERS'    ,'$(SRC)/include']);

        return comm_var_en;

    def _VarEnvToString(self):
        comm_list=self._genCommonVarEnv();
        string_var='';
        for var in comm_list:
            string_var=string_var+var[0]+' = '+var[1]+'\n';

        for var in self.make_var_list:
            string_var=string_var+var[0]+' = '+var[1]+'\n';

        return string_var;


    def _genFlags(self):
        fmake=open(self.make_name,'w');
        fmake.write('include libconfig.in\n');
        str_env=self._VarEnvToString();
        fmake.write(str_env);
        strCFlags='CFLAGS=';
        fmake.write(strCFlags+self.compil_options+'\n');
        strLDFlags='LDFLAGS=$(ADD_LIB) ' ;
        for libs in self.config_lib:
            strLDFlags=strLDFlags+libs+' ';
        fmake.write(strLDFlags+'\n');

        strOBJ='PRGM_O = ';
        for obj in self.fichiers:
            shortname = os.path.split(obj)[1];
            shortname_obj = shortname.replace('.c','.o');
            strOBJ=strOBJ+' $(OBJ)/'+shortname_obj;

        fmake.write(strOBJ);
        buffer='\n\nall\t: {0}\nclean:\n\t-rm $(OBJ)/*.o $(EXE)/{0}\n'.format(self.softname);
        fmake.write(buffer);

        buffer='{0} : $(PRGM_O)\n\t$(CC) $(CFLAGS) $(PRGM_O) $(CLIBS) $(LDFLAGS) -o $(EXE)/{0}\n\n'.format(self.softname);
        fmake.write(buffer);
        fmake.close();
        self._genObjRules();

    def _genObjRules(self):
        fmake=open(self.make_name,'a');
        for entries in self.dep_list:
            filename=entries[0][0];
            try :
                filedeps=entries[1][0];
            except IndexError:
                filedeps=[];
                print 'no dependencies for :'
                print entries

            shortname = os.path.split(filename)[1];
            shortname_obj = shortname.replace('.c','.o');
            buffer='#-------- %s --------#\n' %shortname;
            fmake.writelines(buffer);
            buffer='$(OBJ)/'+shortname_obj+'\t : ';#+line_modif+'  '+checkInclude(line,repcour)+'\n'
            for deps in filedeps:
                buffer=buffer+ os.path.normpath(deps)+' ';
            buffer=buffer+'\n';
            fmake.writelines(buffer);
            buffer='\t\t$(CC) $(CFLAGS) -c '+filename+'\n';
            fmake.writelines(buffer);
            buffer='\t\t$(MV) '+shortname_obj+' $(OBJ)/'+shortname_obj+'\n\n';
            fmake.writelines(buffer)

        fmake.close();
        print 'a makefile with name {0} has been generated.'.format(self.make_name);
        print 'type : \n make -f -j4 {0}\n for execution.'.format(self.make_name);
        print 'executable {0} will be generated'.format(self.softname);


    def generate(self):
        self._genFlags();

    def compile(self,filename='emedge'):
        self.Compil(filename)
        
if __name__ == "__main__":

    make_test1=pyMake('makefile','h2d_gcc.exe','./','gcc',config_options,GCC_CFLAGS,GCC_LIB,FILE_LIST_H2D);
    make_test1.generate();

    make_test4=pyMake('make_icc','h2d_icc.exe','./','icc',config_options,ICC_CFLAGS_12,ICC_LIB_12,FILE_LIST_H2D);
    make_test4.generate();




    
