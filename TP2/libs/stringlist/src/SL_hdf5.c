/**
*\file SL_hdf5.c
*\author G. Fuhr
*\version 2.0
*\date 2011/04/01
*
* 
*
*/

#include "../headers/SL.h"

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

/**
*\fn SL_codes  SLToHDF5Type(SL_ID_CODE SLvarType)
*\brief convertit un type de donnee utilise par la librairie stringlist par un type compatible hdf5
*\param SLvarType : type de donnée pour la "classe" stringlist
*\return : -1 en cas d'erreur ou le type hdf5 associe
*/
hid_t  SLToHDF5Type(SL_ID SLvarType)
{
    static const char FCNAME[] = "SL_getHDF5AssociatedType";
    hid_t hdf5_type=-1;


    switch(SLvarType)
    {
    case SL_INT    :
        hdf5_type = H5T_STD_I32LE;
        break;
    case SL_LONG   : 
        hdf5_type = H5T_STD_I64LE;
        break;
    case SL_DOUBLE : 
        hdf5_type = H5T_IEEE_F64LE;
        break;
    case SL_FLOAT  : 
        hdf5_type = H5T_IEEE_F32LE;
        break;
    case SL_CHAR   : 
        hdf5_type = H5T_STD_I8LE;
        break;
    case SL_SIZE_T   : 
        hdf5_type = H5T_STD_U64LE;
        break;
    case SL_STRING : 
        hdf5_type = H5T_STD_I8LE;
        break;
    }
    return hdf5_type;
}





/**
*\fn SL_codes  HDF5ToSlType(hid_t SLvarType)
*\brief convertit un type de donnee utilise par la librairie hdf5 par un type compatible stringlist
*\param SLvarType : type de donnée de la librairie hdf5
*\return : le type stringlist associe ou SL_UNDEF en cas d'erreur
*/
SL_ID  HDF5ToSlType(hid_t h5vartype)
{
    static const char FCNAME[] = "SL_getHDF5AssociatedType";
    SL_ID sl_type=SL_UNDEF;




    if (H5Tequal(h5vartype,SLToHDF5Type(SL_INT))>0)
    {
        sl_type=SL_INT;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_FLOAT))>0) 
    {
        sl_type=SL_FLOAT;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_DOUBLE))>0) 
    {
        sl_type=SL_DOUBLE;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_CHAR))>0) 
    {
        sl_type=SL_CHAR;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_STRING))>0) 
    {
        sl_type=SL_STRING;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_LONG))>0) 
    {
        sl_type=SL_LONG;
    }
    else if (H5Tequal(h5vartype,SLToHDF5Type(SL_SIZE_T))>0) 
    {
        sl_type=SL_SIZE_T;
    }

    return sl_type;
}


/**
*\fn SL_CODE  SL_writeFileHDF5(psSL const pssl,char const*const groupname)
*\brief ecriture d'une stringlist dans un groupe d'un fichier hdf5
*\param  pssl : stringlist qui doit etre ecrite
*\param  groupname : nom du groupe
*\return : SL_OK
*/
SL_CODE  SL_writeFileHDF5(psSL const pssl,char const*const groupname)
{
    static const char FCNAME[] = "SL_writeFileHDF5";
    SL_CODE sl_er = SL_OK;
    hid_t fhandle=NULL;
    herr_t status;
    hid_t dataspace_id=-1;
    hid_t dataset_id;
    hsize_t     dims[1]={1};
    hsize_t     rdims[1];
    hid_t var_datatype;
    char buffer[256];
    void *ptrVal=NULL;
    hid_t subgroup;
    hid_t space;
    psSLelement pslCur=pssl->pSLfirst;

    /*si le groupe n'existe pas alors on le crée sinon, rien a faire*/
    if (isObjectExist(pssl,groupname)==SL_ERROR_UNKNOWN_DATA)
    {
        subgroup = H5Gcreate (pssl->file_hdf5,groupname, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        status = H5Gclose (subgroup);
    }


    while (pslCur!=NULL)
    {
        dims[0]=1;
        switch(pslCur->Var.type)
        {
        case SL_INT    :
            ptrVal = &pslCur->Var.value.slInt;
            var_datatype = SLToHDF5Type(SL_INT);
            break;
        case SL_LONG   : 
            ptrVal = &pslCur->Var.value.slLong;
            var_datatype = SLToHDF5Type(SL_LONG);
            break;
        case SL_DOUBLE : 
            ptrVal = &pslCur->Var.value.slDouble;
            var_datatype = SLToHDF5Type(SL_DOUBLE);
            break;
        case SL_FLOAT  : 
            ptrVal = &pslCur->Var.value.slFloat;
            var_datatype = SLToHDF5Type(SL_FLOAT);
            break;
        case SL_CHAR   : 
            ptrVal = &pslCur->Var.value.slChar;
            var_datatype = SLToHDF5Type(SL_CHAR);
            break;
        case SL_SIZE_T   :
            ptrVal = &pslCur->Var.value.slSize_t;
            var_datatype = SLToHDF5Type(SL_SIZE_T);
            break;
        case SL_STRING : 
            dims[0]=strlen(pslCur->Var.value.slString)+1;
            ptrVal = pslCur->Var.value.slString;
                var_datatype = SLToHDF5Type(SL_STRING);
            break;
        case SL_UNDEF :
        default :
            continue;
        }

        /* create dataset. */
        mysnprintf(buffer,256,"%s/%s",groupname,pslCur->sVarName);
        
        /*check if dataset exist*/
        if (isObjectExist(pssl,buffer)==SL_OK)
        {
            dataset_id = H5Dopen(pssl->file_hdf5,buffer,H5P_DEFAULT);
            dataspace_id=H5Dget_space(dataset_id);
            if (dataspace_id<0)
            {
                H5Dclose(dataset_id);
                continue;
            }

            H5Sget_simple_extent_dims(dataspace_id,rdims,NULL);
            /*si la taille de l'element actuel ne correspond pas a la taille de l'objet existant*/
            if (dims[0]!=rdims[0])
            {
                /*H5Dset_extent(dataset_id,dims);*/
                H5Sset_extent_simple(dataspace_id,1,dims,dims);
            }

        }
        else
        {
            dataspace_id = H5Screate_simple(1, dims, NULL);
            dataset_id = H5Dcreate(pssl->file_hdf5, buffer, var_datatype, dataspace_id, 
                H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
        }

        /* Write the dataset. */
        status = H5Dwrite(dataset_id, var_datatype, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
            ptrVal);
        status = H5Dclose (dataset_id);
        if (dataspace_id>=0)
            H5Sclose(dataspace_id);

        pslCur=pslCur->NextElm;
    }



    return sl_er;
}


/**
*\fn SL_CODE  SL_openHDF5File(psSL stringlist, char const*const filename)
*\brief  ouverture d'un fichier hdf5 pour faire des I/O associees a la stringlist
*\param  stringlist : objet stringlist
*\param  filename : nom du fichier hdf5
*\return : SL_OK
*/
SL_CODE  SL_openHDF5File(psSL stringlist, char const*const filename)
{
    static const char FCNAME[] = "SL_openHDF5File";
    SL_CODE sl_er = SL_OK;
    htri_t h5exist= H5Fis_hdf5(filename);

    /*check filename exist and if it's an hdf5 file */
    if (h5exist>0)
    {
        stringlist->file_hdf5=H5Fopen(filename,H5F_ACC_RDWR , H5P_DEFAULT);
    }
    else   /*if it's not the case, file is created */
    {
        stringlist->file_hdf5=H5Fcreate(filename,H5F_ACC_TRUNC,H5P_DEFAULT, H5P_DEFAULT);
    }


    return sl_er;
}






/**
*\fn SL_CODE  isObjectExist(psSL stringlist,char const*const objectname)
*\brief  verifie l'existence d'un objedt hdf5
*\param stringlist : objet stringlist qu'on manipule
*\param  : nom de l'objet
*\return : SL_OK si l'objet existe, SL_ERROR_UNKNOWN_DATA si il n'existe pas et SL_ERROR en cas d'erreur
*/
SL_CODE  isObjectExist(psSL stringlist,char const*const objectname)
{
    static const char FCNAME[] = "isObjectExist";
    SL_CODE sl_er = SL_OK;
    htri_t retval=H5Lexists(stringlist->file_hdf5,objectname,H5P_DEFAULT);

    if (retval<0) /*erreur */
        sl_er=SL_ERROR;
    else if (retval==FALSE) /*l'objet n'existe pas*/
        sl_er=SL_ERROR_UNKNOWN_DATA;

    return sl_er;
}



/**
*\fn RBM_codes  SL_closeHDF5File(psSL stringlist, char const*const filename)
*\brief   fermeture d'un fichier hdf5 
*\param  stringlist : objet stringlist associe au fichier
*\return : SL_OK
*/
SL_CODE  SL_closeHDF5File(psSL stringlist)
{
    static const char FCNAME[] = "SL_openHDF5File";
    SL_CODE sl_er = SL_OK;
    H5Fclose(stringlist->file_hdf5);


    return sl_er;
}




/**
*\fn SL_CODE  SL_readFileHDF5(psSL const pssl,char const*const groupname)
*\brief  lecture d'un fichier hdf5 et copie des elements dans un objet de type stringlist
*\param  pssl : objet stringlist
*\param  groupname :  groupe hdf5 que l'on veut lire
*\return : RBM_OK
*/
SL_CODE  SL_readFileHDF5(psSL const pssl,char const*const groupname)
{
    static const char FCNAME[] = "SL_writeFileHDF5";
    SL_CODE sl_er = SL_OK;
    hid_t hid_grp;
    herr_t status;

    /*ouverture du groupe */
    /*si le groupe n'existe pas => erreur et rien a lire*/
    if (isObjectExist(pssl,groupname)==SL_ERROR_UNKNOWN_DATA)
    {
        sl_er = SL_ERROR_READ;
        return sl_er;
    }

    hid_grp=H5Gopen(pssl->file_hdf5,groupname,H5P_DEFAULT);
    status = H5Literate (hid_grp, H5_INDEX_NAME, H5_ITER_NATIVE, NULL, op_stringlist,
        (void *)pssl);


    H5Gclose(hid_grp);

    return sl_er;
}



/**
*\fn herr_t op_stringlist( hid_t loc_id, const char *name, const H5L_info_t *info, void *op_data) 
*\brief fonction "callback" utilise par H5Literate pour parcourir le groupe et mettre en place la stringlist
*\param  : 
*\param  : 
*\param  : 
*\return : RBM_OK
*/
herr_t op_stringlist( hid_t loc_id, const char *name, const H5L_info_t *info, void *op_data) 
{
    herr_t          status, return_val = 0;
    H5O_info_t      infobuf;
    sSL   *sl = (sSL *) op_data;
    hid_t dset;
    hid_t dtype;
    hid_t space;
    hsize_t     rdims[1]={1};                                /* Type conversion */
    void *pval=NULL;

    /*
    * Get type of the object and display its name and type.
    * The name of the object is passed to this function by
    * the Library.
    */
    status = H5Oget_info_by_name (loc_id, name, &infobuf, H5P_DEFAULT);
    switch (infobuf.type) {
    case H5O_TYPE_GROUP:
        printf ("Group: %s \n", name);


        break;
    case H5O_TYPE_DATASET: /*cas d'une nouvelle variable*/
        {
            SL_Var sl_var;
            rdims[0]=1;
            printf ("Dataset: %s\n", name);

            dset=H5Dopen(loc_id,name,H5P_DEFAULT);
            dtype=H5Dget_type(dset);
            space=H5Dget_space(dset);
            if (space<0)
            {
                H5Dclose(dset);
                return 1;

            }

            H5Sget_simple_extent_dims(space,rdims,NULL);

            if (H5Tequal(dtype,SLToHDF5Type(SL_INT))>0)
            {
                sl_var.type=SL_INT;
                pval=&sl_var.value.slInt;

            }
            else if (H5Tequal(dtype,SLToHDF5Type(SL_FLOAT))>0) 
            {
                sl_var.type=SL_FLOAT;
                pval=&sl_var.value.slFloat;
            }
            else if (H5Tequal(dtype,SLToHDF5Type(SL_DOUBLE))>0) 
            {
                sl_var.type=SL_DOUBLE;
                pval=&sl_var.value.slDouble;
            }
            else if ((H5Tequal(dtype,SLToHDF5Type(SL_CHAR))>0) && (rdims[0]==1))
            {
                sl_var.type=SL_CHAR;
                pval=&sl_var.value.slChar;
            }
            else if (H5Tequal(dtype,SLToHDF5Type(SL_STRING))>0) 
            {
                sl_var.type=SL_STRING;
                sl_var.value.slString=malloc((rdims[0]+1)*sizeof(*sl_var.value.slString));
                pval=sl_var.value.slString;
            }
            else if (H5Tequal(dtype,SLToHDF5Type(SL_LONG))>0) 
            {
                sl_var.type=SL_LONG;
                pval=&sl_var.value.slLong;
            }
            else if (H5Tequal(dtype,SLToHDF5Type(SL_SIZE_T))>0) 
            {
                sl_var.type=SL_SIZE_T;
                pval=&sl_var.value.slSize_t;
            }


            status = H5Dread (dset, dtype, H5S_ALL, space, H5P_DEFAULT, pval);
            SL_putVar(sl,name,sl_var);
            H5Dclose(dset);

        }
        break;
    case H5O_TYPE_NAMED_DATATYPE:
        printf ("Datatype: %s\n", name);
        break;
    default:
        printf ( "Unknown: %s\n", name);

    }
    return return_val;
}

