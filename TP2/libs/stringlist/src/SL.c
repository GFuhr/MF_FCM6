/**
*\file SL.c
*\author G. Fuhr
*\version 2.0
*\date 16 Mars 2008
*
* fichier source du parser de fichier ini
*
*/


#include "../headers/SL.h"



/**
*\fn static char *mfgets(char *buffer, size_t size)
*\brief fonction securisee pour remplacer get
*\param buffer contient les donnees recues
*\param size la taille du buffer
*\return : le buffer ainsi modifie ou NULL en cas d'erreur (taille trop petite)
*/
static char *mfgets(char *buffer, size_t const size)
{
    char *eol = NULL;

    if(!fgets(buffer, size, stdin))
        return NULL;

    eol = strrchr(buffer, '\n');

    if(eol)
        *eol = '\0';

    return buffer;
}

/**
*\fn SL_CODE SLE_init(psSLelement const p)
*\brief initialise un element 
*\param p adresse de l'element
*\return : SL_OK
*/
static SL_CODE SLE_init(psSLelement const p)
{
    SL_CODE sl_er=SL_OK;

    p->NextElm=NULL;
    p->PrevElm=NULL;
    p->sVarName=NULL;

    return sl_er;
}

/**
*\fn void SLE_free(psSLelement p)
*\brief efface un element
*\param p : pointeur 
*/

static void SLE_free(psSLelement p)
{
    if (p!=NULL)
    {

        /*fprintf(stdout,"effacement sle %p\t%p\t%p\t[%s]\n",p,p->NextElm,p->PrevElm,p->sVarName);*/
        if (p->PrevElm !=NULL)
            p->PrevElm->NextElm = p->NextElm;
        if (p->NextElm !=NULL)
            p->NextElm->PrevElm = p->PrevElm;

        p->NextElm=NULL;
        p->PrevElm=NULL;
        if (p->Var.type==SL_STRING)
        {
            free(p->Var.value.slString),p->Var.value.slString=NULL;
        }

        free(p->sVarName),p->sVarName=NULL;
        free(p);
    }
}


/**
*\fn static SLelement *SLE_malloc(size_t count)
*\brief allocation pour un nouvel element 
*\param count : nombre d'elements a allouer
*\return : pointeur vers le premier element
*/

static SLelement *SLE_malloc(size_t count)
{
    size_t i=0;
    SLelement *new_elem= malloc(count * sizeof(*new_elem));
    memset(new_elem,0,count*sizeof(*new_elem));

    if (new_elem != NULL)
    {
        for(i=0;i<count;i++)
        {
            new_elem[i].NextElm = NULL;
            new_elem[i].PrevElm = NULL;
        }
    }

    return new_elem;
}

/**
*\fn static SL_CODE SL_init(sSL *const pssl)
*\brief initialise une instande de stringlist
*\param pssl : pointeur vers la stringlist
*\return : SL_OK
*/

static SL_CODE SL_init(sSL *const pssl)
{
    SL_CODE sl_er=SL_OK;

    if (pssl != NULL)
    {
        pssl->pSLfirst    = NULL;
        pssl->pSLlast     = NULL;
        pssl->pSLcurrent  = NULL;
        pssl->file        = NULL;
    }
    else
        sl_er = SL_ERR0R_SL_NOT_INITIALISED;

    return sl_er;
}




/**
*\fn static SL_CODE SL_addVar(psSL const pssl,SLelement const *const SLe)
*\brief ajoute un element a la liste
*\param pssl pointeur vers la stringlist
*\param SLe adresse de l'element a ajouter
*\return : SL_OK, SL_ERROR_UNKNOWN_DATA
*/
static SL_CODE SL_addVar(psSL const pssl,SLelement const *const SLe)
{
    SL_CODE sl_er=SL_OK;
    psSLelement pSLnew = SLE_malloc(1);
    size_t size_elem=0;
    if (pSLnew==NULL)
    {
        sl_er = SL_ERROR_MEMORY;
        return sl_er;
    }


    pSLnew->Var.type  = SLe->Var.type;
    pSLnew->sVarName = malloc((mystrlen(SLe->sVarName)+1)*sizeof(*pSLnew->sVarName));


    mystrcpy(pSLnew->sVarName,SLe->sVarName,(mystrlen(SLe->sVarName)+1)*sizeof(*pSLnew->sVarName));

    switch(SLe->Var.type)
    {
    case SL_INT :
        pSLnew->Var.value.slInt       = SLe->Var.value.slInt;
        break;
    case SL_LONG :
        pSLnew->Var.value.slLong      = SLe->Var.value.slLong;
        break;
    case SL_DOUBLE:
        pSLnew->Var.value.slDouble    = SLe->Var.value.slDouble;
        break;
    case SL_SIZE_T:
        pSLnew->Var.value.slSize_t    = SLe->Var.value.slSize_t;
        break;        
    case SL_FLOAT:
        pSLnew->Var.value.slFloat     = SLe->Var.value.slFloat;
        break;
    case SL_CHAR:
        pSLnew->Var.value.slChar      = SLe->Var.value.slChar;
        break;
    case SL_STRING:
        size_elem = (mystrlen(SLe->Var.value.slString)+1)*sizeof(*pSLnew->Var.value.slString);
        pSLnew->Var.value.slString=malloc(size_elem);
        mystrcpy(pSLnew->Var.value.slString,SLe->Var.value.slString,size_elem);
        break;
    case SL_UNDEF :
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }

    if (pssl->pSLcurrent==NULL) /*first element*/
    {
        pssl->pSLcurrent          = pSLnew;
        pssl->pSLcurrent->NextElm = NULL;
        pssl->pSLcurrent->PrevElm = NULL;
        pssl->pSLfirst            = pssl->pSLcurrent;
        pssl->pSLlast             = pssl->pSLcurrent;
    }
    else
    {
        pSLnew->PrevElm           = pssl->pSLcurrent;
        pSLnew->NextElm           = pssl->pSLcurrent->NextElm;
        pssl->pSLcurrent          = pSLnew;
        pssl->pSLlast->NextElm    = pssl->pSLcurrent;
        pssl->pSLcurrent->PrevElm = pssl->pSLlast;
        pssl->pSLlast             = pssl->pSLcurrent;
    }

    return sl_er;
}




/**
*\fn static SL_CODE SL_putVar(psSL const pssl,char const * const cVarName,SL_Var const slVar)
*\brief ajoute le contenu d'une variable a la stringlist
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param slVar : valeur et type de la variable
*\return : SL_OK, SL_ERROR_MEMORY ou SL_ERROR_UNKNOWN_DATA
*/
SL_CODE SL_putVar(psSL const pssl,char const * const cVarName,SL_Var const slVar)
{
    SL_CODE sl_er=SL_OK;
    SLelement *sle=NULL;
    size_t size_elem = (mystrlen(cVarName)+1)*sizeof(*cVarName);

    sle=SLE_malloc(1);
    if(sle==NULL)
    {
        sl_er = SL_ERROR_MEMORY;
        return sl_er;
    }

    sle->sVarName= malloc(size_elem);
    mystrcpy(sle->sVarName,cVarName,size_elem);
    sle->Var.type=slVar.type;

    switch(sle->Var.type)
    {
    case SL_INT :
        sle->Var.value.slInt     = slVar.value.slInt;
        break;
    case SL_LONG :
        sle->Var.value.slLong    = slVar.value.slLong;
        break;
    case SL_SIZE_T   : 
        sle->Var.value.slSize_t = slVar.value.slSize_t;
        break;
    case SL_DOUBLE:
        sle->Var.value.slDouble  = slVar.value.slDouble;
        break;
    case SL_FLOAT:
        sle->Var.value.slFloat   = slVar.value.slFloat;
        break;
    case SL_CHAR:
        sle->Var.value.slChar    = slVar.value.slChar;
        break;
    case SL_STRING:
        sle->Var.value.slString=malloc((mystrlen(slVar.value.slString)+1)*sizeof(*sle->Var.value.slString));
        mystrcpy(sle->Var.value.slString,slVar.value.slString,(mystrlen(slVar.value.slString)+1)*sizeof(*sle->Var.value.slString));
        break;
    case SL_UNDEF :
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }

    if (sl_er == SL_OK)
    {
        SL_addVar(pssl,sle);
    }

    SLE_free(sle),sle=NULL;
    return sl_er;
}


/**
*\fn static SL_CODE SL_writeSLE(psSL const pssl,SLelement const * const sle)
*\brief ecrit un element de la liste dans le fichier
*\param pssl pointeur vers la stringlist
*\param sle adresse de l'element a ecrire
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND, SL_ERROR_TYPE_MISMATCH ou SL_ERROR_UNKNOWN_DATA
*/
static SL_CODE SL_writeSLE(psSL const pssl,SLelement const * const sle)
{
    int iRes=0;
    SL_CODE sl_er=SL_OK;

    if (pssl->file==NULL)
        sl_er = SL_ERROR_FILE_NOT_OPENED;
    else
    {
        switch(sle->Var.type)
        {
        case SL_INT    : iRes=myfprintf(pssl->file,"int\t%s = %d;\n",sle->sVarName,sle->Var.value.slInt);
            break;
        case SL_LONG   : iRes=myfprintf(pssl->file,"long\t%s = %ld;\n",sle->sVarName,sle->Var.value.slLong);
            break;
        case SL_SIZE_T   : iRes=myfprintf(pssl->file,"size_t\t%s = %ld;\n",sle->sVarName,sle->Var.value.slSize_t);
            break;
        case SL_DOUBLE : iRes=myfprintf(pssl->file,"double\t%s = %f;\n",sle->sVarName,sle->Var.value.slDouble);
            break;
        case SL_FLOAT  : iRes=myfprintf(pssl->file,"float\t%s = %f;\n",sle->sVarName,sle->Var.value.slFloat);
            break;
        case SL_CHAR   : iRes=myfprintf(pssl->file,"char\t%s = %c;\n",sle->sVarName,sle->Var.value.slChar);
            break;
        case SL_STRING : iRes=myfprintf(pssl->file,"string\t%s = %s;\n",sle->sVarName,sle->Var.value.slString);
            break;
        case SL_UNDEF:
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;
        }
    }
    if (iRes<0)
        sl_er = SL_ERROR_WRITE;

    return sl_er;
}



/**
*\fn static SL_CODE SL_writeVar(psSL const pssl,char const * const cVarName,SL_Var sVar)
*\brief ecrit une variable dans le fichier
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param sVar : type et valeur de la variable
*\return : SL_OK, SL_ERROR_MEMORY, SL_ERROR_UNKNOWN_DATA
*/
static SL_CODE SL_writeVar(psSL const pssl,char const * const cVarName,SL_Var sVar)
{
    SL_CODE sl_er = SL_OK;
    SLelement *psle = NULL;
    size_t size_elem = (mystrlen(cVarName)+1)*sizeof(*psle->sVarName);

    psle=SLE_malloc(1);

    if (psle ==NULL)
    {
        sl_er = SL_ERROR_MEMORY;
        return sl_er;
    }


    psle->Var.type=sVar.type;
    psle->sVarName=malloc(size_elem);
    mystrcpy(psle->sVarName,cVarName,size_elem);

    switch(sVar.type)
    {
    case SL_INT :
        psle->Var.value.slInt     = sVar.value.slInt;
        break;
    case SL_LONG :
        psle->Var.value.slLong    = sVar.value.slLong;
        break;
    case SL_SIZE_T :
        psle->Var.value.slSize_t    = sVar.value.slSize_t;
        break;
    case SL_DOUBLE:
        psle->Var.value.slDouble  = sVar.value.slDouble;
        break;
    case SL_FLOAT:
        psle->Var.value.slFloat   = sVar.value.slFloat;
        break;
    case SL_CHAR:
        psle->Var.value.slChar    = sVar.value.slChar;
        break;
    case SL_STRING:
        size_elem = (mystrlen(sVar.value.slString)+1)*sizeof(*psle->Var.value.slString);
        psle->Var.value.slString=malloc(size_elem);
        mystrcpy(psle->Var.value.slString,sVar.value.slString,size_elem);
        break;
    case SL_UNDEF :
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }

    if (sl_er == SL_OK)
    {
        SL_writeSLE(pssl,psle);
        SLE_free(psle);
    }
    return sl_er;
}




/**
*\fn SL_CODE SL_readVar(psSL const pssl,char const * const cVarName,const SL_ID slVarT, void *pvVal)
*\brief lit dans un fichier le contenu associe au nom d'une variable de la stringlist
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param slVarT type de donnees associees a la variable
*\param pvVal  valeur pour la variable
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND, SL_ERROR_UNKNOWN_DATA ou SL_ERROR_FILE_NOT_OPENED
*/
SL_CODE SL_readVar(psSL const pssl,char const * const cVarName,const SL_ID slVarT, void *pvVal)
{
    SL_CODE sl_er = SL_OK;
    size_t j=0;
    size_t iVarLen=mystrlen(cVarName);
    char buffer[BUFFER_SIZE]={0};
    char c[2]={0};
    SL_Var slVar;

    slVar.type=slVarT;

    if (pssl->file==NULL)
    {
        sl_er = SL_ERROR_FILE_NOT_OPENED;
        return sl_er;
    }

    fseek(pssl->file,0L,SEEK_SET);    


    switch(slVar.type)
    {
    case SL_INT    : mysnprintf(buffer,BUFFER_SIZE,"int\t%s = ",cVarName);
        break;
    case SL_LONG   : mysnprintf(buffer,BUFFER_SIZE,"long\t%s = ",cVarName);
        break;
    case SL_DOUBLE : mysnprintf(buffer,BUFFER_SIZE,"double\t%s = ",cVarName);
        break;
    case SL_FLOAT  : mysnprintf(buffer,BUFFER_SIZE,"float\t%s = ",cVarName);
        break;
    case SL_CHAR   : mysnprintf(buffer,BUFFER_SIZE,"char\t%s = ",cVarName);
        break;
    case SL_SIZE_T   : mysnprintf(buffer,BUFFER_SIZE,"size_t\t%s = ",cVarName);
        break;        
    case SL_STRING : mysnprintf(buffer,BUFFER_SIZE,"string\t%s = ",cVarName);
        break;
    case SL_UNDEF :
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }
    iVarLen=mystrlen(buffer);


    while ((!feof(pssl->file))&&(j<iVarLen))
    {
        fgets(c,2,pssl->file);
        if (c[0]==buffer[j])
            j++;
        else
            j=0;
    }

    if (j!=iVarLen)
        sl_er =  SL_ERROR_VAR_NOT_FOUND;
    else
    {
        switch(slVar.type)
        {
        case SL_INT : fscanf(pssl->file,"%d;\n",&slVar.value.slInt);
            memcpy(pvVal,&slVar.value.slInt,sizeof(slVar.value.slInt));
            break;
        case SL_LONG : fscanf(pssl->file,"%ld;\n",&slVar.value.slLong);
            memcpy(pvVal,&slVar.value.slLong,sizeof(slVar.value.slLong));
            break;
        case SL_DOUBLE:fscanf(pssl->file,"%lf;\n",&slVar.value.slDouble);
            memcpy(pvVal,&slVar.value.slDouble,sizeof(slVar.value.slDouble));
            break;
        case SL_FLOAT:fscanf(pssl->file,"%f;\n",&slVar.value.slFloat);
            memcpy(pvVal,&slVar.value.slFloat,sizeof(slVar.value.slFloat));
            break;
        case SL_SIZE_T : fscanf(pssl->file,"%zu;\n",&slVar.value.slSize_t);
            memcpy(pvVal,&slVar.value.slSize_t,sizeof(slVar.value.slSize_t));
            break;            
        case SL_CHAR:fscanf(pssl->file," %c;\n", &slVar.value.slChar);
            memcpy(pvVal,&slVar.value.slChar,sizeof(slVar.value.slChar));
            break;
        case SL_STRING:
            {
                int i=0;
                size_t size_elem;
                c[0]='\0';
                c[1]='\0';
                while (c[0]!=';')
                {
                    fgets(c,2,pssl->file);
                    buffer[i]=c[0];
                    buffer[i+1]='\0';
                    i++;
                }
                buffer[i-1]='\0';
                size_elem = (mystrlen(buffer)+1)*sizeof(*slVar.value.slString);
                slVar.value.slString=malloc(size_elem);
                mystrcpy(slVar.value.slString,buffer,size_elem);
                strcpy(pvVal,buffer);
            }
            break;
        case SL_UNDEF :
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;        
        }
    }

    return sl_er;
}


/**
*\fn static psSLelement SL_findVar(psSL const pssl,char const *const cVarName)
*\brief recherche d'une variable de la liste par son nom
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\return : pointeur vers l'element de la liste ou NULL en cas d'erreur
*/
static psSLelement SL_findVar(psSL const pssl,char const *const cVarName)
{
    psSLelement pslCurrent=pssl->pSLfirst;
    size_t const lenparam=mystrlen(cVarName);
    size_t lenmin=lenparam;
    size_t lencur=lenparam;


    /*fprintf(stdout,"\n valeur a tester [%s]\n",cVarName);*/

    while ((pslCurrent!=NULL))
    {
        /*myfprintf(stdout,"var name [%s][%s]\n",pslCurrent->sVarName,cVarName);*/
        lencur=mystrlen(pslCurrent->sVarName);
        lenmin=(lencur>lenparam?lenparam:lencur);
        if (strncmp(pslCurrent->sVarName,cVarName,lenmin)!=0)
        {
            pslCurrent=pslCurrent->NextElm;
        }
        else
        {
            if (lencur==lenparam)
                break;
            else
                pslCurrent=pslCurrent->NextElm;
        }




        if (pslCurrent==NULL)
            break;

        /*fprintf(stdout," [%s] ",pslCurrent->sVarName);*/
    }

    return pslCurrent;
}






/**
*\fn static SL_CODE SL_changeValue(psSL const pssl,char const * const cVarName, SL_Var const slVariable)
*\brief modifie le contenu d'une variable dans la stringlist
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param slVariable iVarType type de donnees associees a la variable et nouvelle valeur 
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND, SL_ERROR_TYPE_MISMATCH ou SL_ERROR_UNKNOWN_DATA
*/
static SL_CODE SL_changeValue(psSL const pssl,char const * const cVarName, SL_Var const slVariable)
{
    SL_CODE sl_er = SL_OK;
    psSLelement pslCur=SL_findVar(pssl,cVarName);
    size_t size_elem=0;

    if (pslCur==NULL)
    {
        sl_er = SL_ERROR_VAR_NOT_FOUND;
    }
    else if (pslCur->Var.type!=slVariable.type)
        sl_er =  SL_ERROR_TYPE_MISMATCH;
    else
    {
        switch(pslCur->Var.type)
        {
        case SL_INT    :  pslCur->Var.value.slInt     = slVariable.value.slInt;
            break;
        case SL_LONG   :  pslCur->Var.value.slLong    = slVariable.value.slLong;
            break;
        case SL_DOUBLE :  pslCur->Var.value.slDouble  = slVariable.value.slDouble;
            break;
        case SL_FLOAT  :  pslCur->Var.value.slFloat   = slVariable.value.slFloat;
            break;
        case SL_CHAR   :  pslCur->Var.value.slChar    = slVariable.value.slChar;
            break;
        case SL_SIZE_T   :  pslCur->Var.value.slSize_t    = slVariable.value.slSize_t;
            break;            
        case SL_STRING:
            size_elem = (mystrlen(slVariable.value.slString)+1)*sizeof(*pslCur->Var.value.slString);
            free(pslCur->Var.value.slString);
            pslCur->Var.value.slString=malloc(size_elem);
            mystrcpy(pslCur->Var.value.slString,slVariable.value.slString,size_elem);
            break;
        case SL_UNDEF:
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;
        }
    }

    return sl_er;
}

/**
*\fn static SL_CODE SL_scanValue(psSL pssl,char const * const cVarName,SL_ID iVarType)
*\brief demande de saisie au clavier de la valeur d'une variable de la liste
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param iVarType type de donnees associees a la variable
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND, SL_ERROR_TYPE_MISMATCH ou SL_ERROR_UNKNOWN_DATA
*/
static SL_CODE SL_scanValue(psSL pssl,char const * const cVarName,SL_ID iVarType)
{
    SL_CODE sl_er=SL_OK;
    SLelement *sle=SLE_malloc(1);
    char buffer[BUFFER_SIZE]={0};


    if (sle==NULL)
    {
        sl_er = SL_ERROR_MEMORY;
        return sl_er;
    }

    sle->Var.type=iVarType;
    sle->sVarName=malloc((mystrlen(cVarName)+1)*sizeof(*sle->sVarName));
    strcpy(sle->sVarName,cVarName);

    myprintf("%s = ",sle->sVarName);
    switch(sle->Var.type)
    {
    case SL_INT : 
        sle->Var.value.slInt=getInt();
        break;
    case SL_LONG : 
        sle->Var.value.slLong=getLong();
        break;
    case SL_DOUBLE:
        sle->Var.value.slDouble=getDouble();
        break;
    case SL_FLOAT:
        sle->Var.value.slFloat=getFloat();
        /*scanf("%f",(float *)sle.pvVal);*/
        break;
    case SL_SIZE_T : 
        sle->Var.value.slSize_t=(size_t)getLong();
        break;        
    case SL_CHAR:
        scanf("%c",&sle->Var.value.slChar);
        while(sle->Var.value.slChar=='\n')
            scanf("%c",&sle->Var.value.slChar);
        break;
    case SL_STRING:
        mfgets(buffer,sizeof(buffer));
        if (buffer[0]=='\0')
            mfgets(buffer,sizeof(buffer));
        sle->Var.value.slString=malloc((mystrlen(buffer)+1)*sizeof(*sle->Var.value.slString));
        strcpy(sle->Var.value.slString,buffer);
        break;
    case SL_UNDEF:
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }

    if (sl_er ==SL_OK)
    {
        psSLelement const pslCurrent=SL_findVar(pssl,cVarName);
        if (pslCurrent==NULL)
        {
            SL_addVar(pssl,sle);
        }
        else
        {
            SL_changeValue(pssl,cVarName,sle->Var);
        }
    }
    SLE_free(sle),sle=NULL;
    return sl_er;
}


/**
*\fn static SL_CODE SL_delVar(psSL pssl,char const * const cVarName)
*\brief efface une variable dans la stringlist a partir de son nom
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\return : SL_OK
*/

static SL_CODE SL_delVar(psSL pssl,char const * const cVarName)
{
    SL_CODE sl_er=SL_OK;
    psSLelement pslCur;

    pslCur=SL_findVar(pssl,cVarName);

    if (pslCur==NULL)
    {
        sl_er =  SL_ERROR_VAR_NOT_FOUND;
    }
    else
    {
        if (pslCur->PrevElm!=NULL)
            pslCur->PrevElm->NextElm=pslCur->NextElm;
        if (pslCur->NextElm!=NULL)
            pslCur->NextElm->PrevElm=pslCur->PrevElm;

        if (pslCur->Var.type==SL_STRING)
            free(pslCur->Var.value.slString),pslCur->Var.value.slString=NULL;

        free(pslCur->sVarName);
        free(pslCur);
    }
    return sl_er;
}


/**
*\fn SL_CODE SL_getVar(psSL pssl,char const * const cVarName,void *pvValue)
*\brief copie le contenu associe a un nom dans une variable
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param pvValue adresse de la variable qui va contenir les donnees
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND ou SL_ERROR_UNKNOWN_DATA
*/

SL_CODE SL_getVar(psSL pssl,char const * const cVarName,void *const pvValue)
{
    SL_CODE sl_er=SL_OK;
    psSLelement pslCur=SL_findVar(pssl,cVarName);




    if (pslCur==NULL)
    {
        sl_er = SL_ERROR_VAR_NOT_FOUND;
    }
    else
    {
        switch(pslCur->Var.type)
        {
        case SL_INT    :  memcpy(pvValue,&(pslCur->Var.value.slInt),sizeof(pslCur->Var.value.slInt));
            /**pvValue=pslCur->Var.value.slInt;*/
            break;
        case SL_LONG   :  memcpy(pvValue,&(pslCur->Var.value.slLong),sizeof(pslCur->Var.value.slLong));
            break;
        case SL_SIZE_T :  memcpy(pvValue,&(pslCur->Var.value.slSize_t),sizeof(pslCur->Var.value.slSize_t));
            break;
        case SL_DOUBLE :  memcpy(pvValue,&(pslCur->Var.value.slDouble),sizeof(pslCur->Var.value.slDouble));
            break;
        case SL_FLOAT  :  memcpy(pvValue,&pslCur->Var.value.slFloat,sizeof(pslCur->Var.value.slFloat));
            break;
        case SL_CHAR   :  memcpy(pvValue,&pslCur->Var.value.slChar,sizeof(pslCur->Var.value.slChar));
            break;
        case SL_STRING :  strcpy(pvValue,pslCur->Var.value.slString);
            break;
        case SL_UNDEF :
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;
        }
    }
    return sl_er;
}



/**
*\fn void SL_free(psSL pssl)
*\brief efface une instance de stringlist
*\param pssl pointeur sur stringlist
*/

void SL_free(psSL pssl)
{
    if ((pssl) != NULL)
    {
        psSLelement pslCur=NULL;

        if (pssl->file!=NULL)
        {
            fclose(pssl->file);
            pssl->file=NULL;
        }

        if ((pssl)->pSLlast!=NULL)
        {
            pslCur=pssl->pSLlast->PrevElm;
            (pssl)->pSLlast=NULL;

            while (pslCur!=NULL)
            {
                /*fprintf(stdout,"effacement de %s\t%p\t%p\t%p\n",pslCur->NextElm->sVarName,pslCur,pssl->pSLfirst,pslCur->NextElm);*/
                SLE_free(pslCur->NextElm),pslCur->NextElm=NULL;

                pslCur=pslCur->PrevElm;
            }

            if (pssl->pSLfirst!=NULL)
            {
                SLE_free(pssl->pSLfirst);
            }
            (pssl)->pSLfirst  = NULL;

            free(pssl);
        }
    }


}

/**
*\fn SL_CODE SL_fclose(psSL const pssl)
*\brief ferme le fichier associe a une stringlist
*\param pssl pointeur vers la stringlist
*\return : SL_OK, SL_ERR0R_SL_NOT_INITIALISED, SL_ERROR
*/
SL_CODE SL_fclose(sSL *const pssl)
{
    SL_CODE sl_er=SL_OK;

    if (pssl==NULL)
        sl_er = SL_ERR0R_SL_NOT_INITIALISED;
    else
    {
        if (pssl->file==NULL)
            return SL_OK;
        if (fclose(pssl->file)==0)
            pssl->file=NULL;
        else
            sl_er = SL_ERROR;
    }

    return sl_er;
}


/**
*\fn SL_CODE SL_fopen(psSL const pssl,char const * const filename,char const * const mode)
*\brief ouvre un fichier associe a la stringlist
*\param pssl pointeur vers la stringlist
*\param filename nom du fichier
*\param mode mode d'ouverture
*\return : SL_OK, SSL_ERROR_OPEN_FILE
*/
SL_CODE SL_fopen(sSL *const pssl,char const * const filename,char const * const mode)
{
    SL_CODE sl_er=SL_OK;

    pssl->file=myfopen(filename,mode);

    if (pssl->file==NULL)
        sl_er = SL_ERROR_OPEN_FILE;

    return sl_er;
}


/**
*\fn SL_CODE SL_writeFile(psSL const pssl)
*\brief ecrit tous les elements de la liste dans un fichier
*\param pssl pointeur vers la stringlist
*\return : SL_OK
*/
SL_CODE SL_writeFile(psSL const pssl)
{
    SL_CODE sl_er=SL_OK;
    psSLelement pslCur=pssl->pSLfirst;

    rewind(pssl->file);    

    while (pslCur!=NULL)
    {
        SL_writeSLE(pssl,pslCur);
        pslCur=pslCur->NextElm;
    }

    return sl_er;
}

/**
*\fn SL_CODE SL_readFile(psSL const pssl)
*\brief lit un fichier et place les elements dans la stringlist
*\param pssl pointeur vers la stringlist
*\return : SL_OK
*/
SL_CODE SL_readFile(psSL const pssl)
{
    SL_CODE sl_er=SL_OK;
    char buffer[BUFFER_SIZE]={0};






    if (pssl->file==NULL)
    {
        sl_er = SL_ERROR_FILE_NOT_OPENED;
        return sl_er;
    }
    rewind(pssl->file);  


    while(fgets(buffer,sizeof buffer,pssl->file)!=NULL)
    {
        SLelement sle={NULL,NULL,NULL,{0.0,SL_UNDEF}};
        char buffer_extract[BUFFER_SIZE]={0};
        /*        size_t offset=0;*/
        char buffer_type[BUFFER_SIZE],buffer_name[BUFFER_SIZE];

        /*fprintf(stdout,"%s\n",buffer);*/

        if (sscanf(buffer,"%[a-z_] %[a-zA-Z0-9_] = ",buffer_type,buffer_name)==0)
        {
            break;
        }
        /*offset = mystrlen(buffer_type);*/

        if (strncmp(buffer_type,SL_CHAR_INT,sizeof SL_CHAR_INT ) ==0) 
        {sle.Var.type=SL_INT;}
        else if (strncmp(buffer_type, SL_CHAR_DOUBLE ,sizeof SL_CHAR_DOUBLE ) ==0)  
        {sle.Var.type=SL_DOUBLE;}
        else if (strncmp(buffer_type, SL_CHAR_FLOAT  ,sizeof SL_CHAR_FLOAT )  ==0)  
        {sle.Var.type=SL_FLOAT;}
        else if (strncmp(buffer_type, SL_CHAR_LONG   ,sizeof SL_CHAR_LONG )   ==0)  
        {sle.Var.type=SL_LONG;}
        else if (strncmp(buffer_type, SL_CHAR_CHAR   ,sizeof SL_CHAR_CHAR )   ==0)  
        {sle.Var.type=SL_CHAR;}
        else if (strncmp(buffer_type, SL_CHAR_STRING ,sizeof SL_CHAR_STRING ) ==0)  
        {sle.Var.type=SL_STRING;}
        else if (strncmp(buffer_type, SL_CHAR_SIZE_T ,sizeof SL_CHAR_SIZE_T ) ==0)  
        {sle.Var.type=SL_SIZE_T;}        
        else
        {
            sl_er = SL_ERROR_UNKNOWN_DATA;
            continue;
        }


        /*        sscanf(buffer + offset,"%s = ",buffer_extract);*/
        /*        sle.sVarName=malloc((mystrlen(buffer_extract)+1)*sizeof(*sle.sVarName));*/
        /*        memset(sle.sVarName,0,(mystrlen(buffer_extract)+1)*sizeof(*sle.sVarName));*/
        /*        strncpy(sle.sVarName,buffer_extract,mystrlen(buffer_extract));*/
        /*sle.sVarName[strlen(buffer_extract)]='\0';*/
        sle.sVarName=malloc((mystrlen(buffer_name)+1)*sizeof(*sle.sVarName));
        strncpy(sle.sVarName,buffer_name,mystrlen(buffer_name)+1);

        /*        offset = offset + mystrlen(sle.sVarName);*/

        /*while(*(buffer+offset-1)!='=')*/
        /*{*/
        /*offset++;*/
        /*}*/

        switch(sle.Var.type)
        {
        case SL_INT : 
            /*sscanf(buffer + offset," %d;\n",&sle.Var.value.slInt);*/

            sscanf(buffer,"%*[^=]= %d;\n",&sle.Var.value.slInt);
            break;
        case SL_LONG : 
            /*sscanf(buffer + offset," %ld;\n",&sle.Var.value.slLong);*/
            sscanf(buffer,"%*[^=]=  %ld;\n",&sle.Var.value.slLong);
            break;
        case SL_DOUBLE:
            /*sscanf(buffer + offset," %lf;\n",&sle.Var.value.slDouble);*/
            sscanf(buffer,"%*[^=]= %lf;\n",&sle.Var.value.slDouble);
            break;
        case SL_FLOAT:
            /*sscanf(buffer + offset," %f;\n",&sle.Var.value.slFloat);*/
            sscanf(buffer,"%*[^=]= %f;\n",&sle.Var.value.slFloat);
            break;
        case SL_CHAR:
            /*sscanf(buffer + offset," %c;\n",&sle.Var.value.slChar);*/
            sscanf(buffer,"%*[^=]= %c;\n",&sle.Var.value.slChar);
            break;
        case SL_SIZE_T : 
            /*sscanf(buffer + offset," %zu;\n",&sle.Var.value.slSize_t);*/
            sscanf(buffer,"%*[^=]= %zu;\n",&sle.Var.value.slSize_t);
            break;            
        case SL_STRING: 
            /*sscanf(buffer + offset," %s;\n",buffer_extract);*/
            memset(buffer_extract,'\0',BUFFER_SIZE);
            sscanf(buffer,"%*[^=]= %[a-zA-Z0-9.s/_#];\n",buffer_extract);
            sle.Var.value.slString = malloc((mystrlen(buffer_extract)+1)*sizeof(*(sle.Var.value.slString)));
            memset(sle.Var.value.slString,0,(mystrlen(buffer_extract)+1)*sizeof(*(sle.Var.value.slString)));
            strncpy(sle.Var.value.slString,buffer_extract,mystrlen(buffer_extract)+1);
            /*sle.Var.value.slString[mystrlen(sle.Var.value.slString)]='\0';*/
            /*if (sle.Var.value.slString[mystrlen(sle.Var.value.slString)-1]==';')*/
            /*sle.Var.value.slString[mystrlen(sle.Var.value.slString)-1]='\0';*/

            break;
        case SL_UNDEF :
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;
            continue;
        }
        if (SL_findVar(pssl,sle.sVarName)== NULL)
            SL_addVar(pssl,&sle);
        else
            SL_changeValue(pssl,sle.sVarName,sle.Var);

        if (sle.Var.type==SL_STRING)
            free(sle.Var.value.slString),sle.Var.value.slString=NULL;

        free(sle.sVarName)/*,sle.sVarName=NULL*/;            
    }


    return sl_er;
}


/**
*\fn SL_CODE SL_scanVar(psSL pssl,char const *const cVarName,SL_ID iVarType, void *pvVal)
*\brief demande de saisie au clavier de la valeur d'une variable de la liste et copie dans pvVal
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\param iVarType type de donnees associees a la variable
*\param pvVal adresse de la variable qui va contenir la valeur saisie
*\return : SL_OK, SL_ERROR_VAR_NOT_FOUND, SL_ERROR_TYPE_MISMATCH ou SL_ERROR_UNKNOWN_DATA
*/
SL_CODE SL_scanVar(psSL pssl,char const *const cVarName,SL_ID iVarType, void *pvVal)
{
    SL_CODE sl_er = SL_OK;
    psSLelement pslCurrent=NULL;
    SL_Var slVar;
    char buffer[BUFFER_SIZE]={0};

    slVar.type=iVarType;
    switch(iVarType)
    {
    case SL_INT : 
        myprintf("[int] %s = ",cVarName);
        slVar.value.slInt=getInt();
        memcpy(pvVal,&slVar.value.slInt,sizeof(slVar.value.slInt));
        break;
    case SL_LONG : 
        myprintf("[long] %s = ",cVarName);
        slVar.value.slLong=getLong();
        memcpy(pvVal,&slVar.value.slLong,sizeof(slVar.value.slLong));
        break;
    case SL_SIZE_T : 
        myprintf("[size_t] %s = ",cVarName);
        slVar.value.slSize_t=(size_t)getLong();
        memcpy(pvVal,&slVar.value.slLong,sizeof(slVar.value.slSize_t));
        break;
    case SL_DOUBLE:
        myprintf("[double] %s = ",cVarName);
        slVar.value.slDouble=getDouble();
        memcpy(pvVal,&slVar.value.slDouble,sizeof(slVar.value.slDouble));
        break;
    case SL_FLOAT:
        myprintf("[float] %s = ",cVarName);
        slVar.value.slFloat=getFloat();
        memcpy(pvVal,&slVar.value.slFloat,sizeof(slVar.value.slFloat));
        break;
    case SL_CHAR:
        myprintf("[char] %s = ",cVarName);
        scanf("%c",&slVar.value.slChar);
        while(slVar.value.slChar=='\n')
            scanf(" %c",&slVar.value.slChar);
        memcpy(pvVal,&slVar.value.slChar,sizeof(slVar.value.slChar));
        break;
    case SL_STRING:
        myprintf("[string] %s = ",cVarName);
        mfgets(buffer,sizeof(buffer));
        if (buffer[0]=='\0')
            mfgets(buffer,sizeof(buffer));
        slVar.value.slString=malloc((mystrlen(buffer)+1)*sizeof(*slVar.value.slString));
        strcpy(slVar.value.slString,buffer);
        strcpy(pvVal,slVar.value.slString);
        break;
    case SL_UNDEF:
    default :
        sl_er = SL_ERROR_UNKNOWN_DATA;
    }

    pslCurrent=SL_findVar(pssl,cVarName);
    if (pslCurrent==NULL)
    {
        SL_putVar(pssl,cVarName,slVar);
    }
    else
    {
        SL_changeValue(pssl,cVarName,slVar);
    }

    return SL_OK;
}

/**
*\fn SL_CODE SL_printVar(psSL pssl,char const * const cVarName)
*\brief envoie sur la sortie standard la valeur d'une variable de la liste
*\param pssl pointeur vers la stringlist
*\param cVarName nom de la variable
*\return : SL_OK
*/
SL_CODE SL_printVar(psSL pssl,char const * const cVarName)
{
    SL_CODE sl_er=SL_OK;
    psSLelement pslCurrent=SL_findVar(pssl,cVarName);

    if (pslCurrent==NULL)
        sl_er = SL_ERROR_VAR_NOT_FOUND;
    else
    {
        switch(pslCurrent->Var.type)
        {
        case SL_INT     : myprintf("%s = %d\n",cVarName,pslCurrent->Var.value.slInt);
            break;
        case SL_SIZE_T  : myprintf("%s = %zu\n",cVarName,pslCurrent->Var.value.slSize_t);
            break;            
        case SL_LONG    : myprintf("%s = %ld\n",cVarName, pslCurrent->Var.value.slLong);
            break;
        case SL_DOUBLE  : myprintf("%s = %f\n",cVarName, pslCurrent->Var.value.slDouble);
            break;
        case SL_FLOAT   : myprintf("%s = %f\n",cVarName, pslCurrent->Var.value.slFloat);
            break;
        case SL_CHAR    : myprintf("%s = %c\n",cVarName, pslCurrent->Var.value.slChar);
            break;
        case SL_STRING  : myprintf("%s = %s\n",cVarName, pslCurrent->Var.value.slString);
            break;
        case SL_UNDEF:
        default :
            sl_er = SL_ERROR_UNKNOWN_DATA;
        }
    }

    return sl_er;
}

/**
*\fn SL_CODE SL_printAllVar(psSL pssl)
*\brief affiche tous les elements sur la sortie standard
*\param pssl pointeur vers la stringlist
*\return : SL_OK
*/
SL_CODE SL_printAllVar(psSL pssl)
{
    SL_CODE sl_er = SL_OK;
    int iNum=0;
    psSLelement pslCurrent=pssl->pSLfirst;
    while (pslCurrent != NULL)
    {
        myfprintf(stdout,"%d/\t",iNum);
        SL_printVar(pssl,pslCurrent->sVarName);
        pslCurrent=pslCurrent->NextElm;
        iNum++;
    }

    return sl_er;
}

/**
*\fn SL_CODE SL_menuVar(psSL const pssl)
*\brief affichage sur la sortie standard de tous les elements de la stringlist en laissant la possibilite d'en modifier les valeurs
*\param pssl pointeur vers la stringlist
*\return : SL_OK
*/
SL_CODE SL_menuVar(psSL const pssl)
{
    SL_CODE sl_er = SL_OK;
    int choix=-1;

    int iNum=0;

    while (choix!=iNum)
    {
        int i=0;

        psSLelement pslCurrent=pssl->pSLfirst;
        iNum=0;

        while (pslCurrent!=NULL)
        {
            myprintf("%d\t",iNum);
            SL_printVar(pssl,pslCurrent->sVarName);
            pslCurrent=pslCurrent->NextElm;
            iNum++;
        }
        myprintf("%d\tEnd\n",iNum);
        myprintf("choice?\n");
        choix=getInt();
        if (choix==iNum)
            break;


        pslCurrent=pssl->pSLfirst;
        while (i<choix)
        {
            pslCurrent=pslCurrent->NextElm;
            i++;
        }
        SL_scanValue(pssl,pslCurrent->sVarName,pslCurrent->Var.type);
    }

    return sl_er;
}

/**
*\fn psSL SL_malloc(size_t count)
*\brief allocation de la memoire pour une stringlist
*\param count : nombre d'elements
*\return : adresse du pointeur ou NULL en cas d'erreur
*/
sSL *SL_malloc(size_t const count)
{
    psSL vec = malloc((count) * sizeof(*vec));
    memset(vec,0,(count) *sizeof(*vec));
    if (vec!=NULL)
    {
        vec->file = NULL;
        vec->pSLcurrent = NULL;
        vec->pSLfirst = NULL;
        vec->pSLlast = NULL;
    }

    return vec;
}
