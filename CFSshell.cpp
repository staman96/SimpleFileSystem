#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string.h>
#include <bits/stdc++.h>
#include "objects/CFS.h"
#include "objects/MDS.h"

using std::cin;
using std::cout;
using std::endl;
using std::ofstream;
using std::streamsize;

void create(char * arguments);

int main(int argc,char* argv[]){

    /*variables and objects*/
    char * pch;         /*used for strtok*/
    size_t length = 512;
    char line[length];  /*its probably too much*/
    CFS * cfs = NULL;


    /*user input*/
    streamsize inputLength = 512;
    char * input = new char[512];
    char * shell = "CFS_Shell:";

    cout << endl << "___    Welcome to Container File System    ___" << endl << endl;

    // cout << "Size of CoreData is " << sizeof(CFSCoreData) << endl;

    // cout << "Size of MetaData is " << sizeof(MetaData) << endl;

    // cout << "Size of CFS is " << sizeof(CFS) << endl;

    // cout << "Size of MDS is " << sizeof(MDS) << endl;

    // cout << "Size of Folder is " << sizeof(Folder) << endl;


    do{/*check for double \n*/
        cout << endl << shell << (char*)(cfs != NULL ? cfs->getcurrDir() : "") << " ";
        cin.getline(input,inputLength);
    }while(strcmp(input,"\0") == 0);
    pch = strtok(input," ");
    
    /*read stdin while input != "exit"*/
    while(strcmp(pch,"exit") != 0){

        /*
        ******************** instuctions from stdin ********************
        *
        *
        */

        /*start working with a CFS
        **************************/
        if(strcmp(pch,"cfs_workwith") == 0 && cfs == NULL){

            pch = strtok (NULL, " ");
            cfs = new CFS(pch);

        }
        /*make new directory or directories
        ***********************************/
        else if(strcmp(pch,"cfs_mkdir") == 0 && cfs != NULL ){

            pch = strtok (NULL, " ");
            while(pch != NULL){
                int ret = cfs->mkdir(pch);
                pch = strtok (NULL, " ");
            }
        }
        /*make new file or change times of a single file
        ************************************************/
        else if(strcmp(pch,"cfs_touch") == 0 && cfs != NULL ){
            
            pch = strtok (NULL, " ");
            bool optionA = false, optionB = false;
            /*if it is an option*/
            if (pch[0] == '-'){
                if (pch[1] == 'a') optionA = true;
                else optionB = true;
                pch = strtok (NULL, " ");
                if (pch[0] == '-'){
                    if (pch[1] == 'a') optionA = true;
                    else optionB = true;
                    pch = strtok (NULL, " ");
                }
            }
            while(pch != NULL){
                int ret = cfs->touch(pch, optionA, optionB);
                pch = strtok (NULL, " ");
            }
        

        }
        /*prints working directory
        **************************/
        else if(strcmp(pch,"cfs_pwd") == 0 && cfs != NULL ){

           cfs->pwd();

        }
        /*change directory to RELATIVE path only
        (supports only reverse path with .. or .)
        ***********************************************/
        else if (strcmp(pch,"cfs_cd") == 0 && cfs != NULL ) {

            pch = strtok (NULL, " /");
            while (pch != NULL){
                cfs->cd(pch);
                // cout << pch << 1 << endl;
                pch = strtok (NULL, "/ ");
            }
            

        }
        /*list files in current directory with or without a single option
        * (supported options: -l, -d)
        *****************************************************************/
        else if(strcmp(pch,"cfs_ls") == 0 && cfs != NULL ){
            pch = strtok (NULL, " ");
            char option;
            if(pch != NULL){
                if (pch[0] == '-'){
                    option = pch[1];
                }
                pch = strtok (NULL, " ");
                cfs->ls(option, pch);
            }
            else{
                cfs->ls('\0', NULL);
            }
        }
        /*
        ****************/
        else if (strcmp(pch,"cfs_cp") == 0 && cfs != NULL ) {

            
        }
        /*vote keys voters from file
        ****************************/
        else if (strcmp(pch,"cfs_cat") == 0 && cfs != NULL ) {
            
            
        }
        /*print all voters and total votes
        **********************************/
        else if (strcmp(pch,"cfs_ln") == 0 && cfs != NULL ) {
            
        }
        /*
        ************************************************/
        else if (strcmp(pch,"cfs_mv") == 0 && cfs != NULL ) {
            

        }
        /*removes a file or a folder(with all subfolders and files)
        * that EXISTS IN working directory
        ***********************************************************/
        else if (strcmp(pch,"cfs_rm") == 0 && cfs != NULL ) {

            bool perm = false; 
            bool r = false;
            pch = strtok (NULL, " ");

            /*check for options*/
            if (pch[0] == '-'){
                if (pch[1] == 'i') perm = true;
                else r = true;
                pch = strtok (NULL, " ");
                if (pch[0] == '-'){
                    if (pch[1] == 'a') perm = true;
                    else r = true;
                    pch = strtok (NULL, " ");
                }
            }

            while (pch != NULL){
                /*asks permission to delete*/
                if (perm){
                    cout << "Do you want to remove " << pch << "? (y/n)" << endl;
                    char reply = getchar();
                    if(reply == 'y' || 'Y') cfs->remove(pch,r);
                }
                else{
                    cfs->remove(pch,r);
                }
                /*step*/
                pch = strtok (NULL, " ");
            }
        } 
        /*
        ************************************************/
        else if (strcmp(pch,"cfs_import") == 0 && cfs != NULL ) {
            
        }
        /*
        ************************************************/
        else if (strcmp(pch,"cfs_export") == 0 && cfs != NULL ) {
            
        }
        /*
        ************************************************/
        else if (strcmp(pch,"cfs_import") == 0 && cfs != NULL ) {
            
        }
        /*create a new CFS (<filename>, <options>)*/
        else if (strcmp(pch,"cfs_create") == 0) {
            /*de 9a xehsimopoihsw th metablhth cfs edw*/
            create(input);
        }
        else if(strcmp(pch,"cfs_finish") == 0 && cfs != NULL ){
            delete cfs;
            cfs = NULL;
        }
        else{
            cout << "Unknown Instruction: " << pch << endl;
        }

        /*step*/
        do{/*check for double \n*/
            cout << endl << shell << (char*)(cfs != NULL ? cfs->getcurrDir() : "") << " ";
            cin.getline(input,inputLength);
        }while(strcmp(input,"\0") == 0);
        pch = strtok(input," ");
    }


    delete[] input;
    if(cfs != NULL) delete cfs;
    return 0;

}


/* creates new CFS file
***********************/
void create(char * arguments){
    char * pch, * filename;
    unsigned int blockSize = 512, nameLength = 128;
    CFS * cfs;

    pch = strtok(NULL," ");
    filename = new char[strlen(pch) + 1];
    strcpy(filename, pch);

    /*argument checking*/
    pch = strtok(NULL," ");
    while(pch != NULL){
        if(strcmp(pch,"-bs") == 0){
            //blocksize
            pch = strtok(NULL," ");
            blockSize = (unsigned int)atoi(pch);
            
        }
        else if(strcmp(pch,"-fns") == 0){
            //max file name size
            pch = strtok(NULL," ");
            nameLength = (unsigned int)atoi(pch);
            
        }
        else{
            cout << "Invalid parameter" << endl;
            return;
        }

        /*step*/
        pch = strtok(NULL," ");
    }

    cout << "Filename: " << filename << ".cfs" << endl;
    cout << "Block Size is : " << blockSize << " bytes." << endl;
    cout << "Name Length is : " << nameLength - 1 << " characters." << endl;
    cfs = new CFS(blockSize, nameLength, filename);
    delete cfs;
    delete[] filename;

}