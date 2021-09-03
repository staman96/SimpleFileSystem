#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <ctime>
#include "CFS.h"
#include "MDS.h"

using std::cout;
using std::endl;
using std::ofstream;
using std::ios;

CFS::CFS(unsigned int bsize, unsigned int namelen, char * filename){

    this->coredata = (CFSCoreData*)malloc(sizeof(CFSCoreData));
    memset(this->coredata, 0, sizeof(CFSCoreData));

    this->coredata->blockSize = bsize;
    this->coredata->namelength = namelen;
    /*block size - (namelength + \0) - sizeof(MDS) - active byte - last blockID position; */
    this->coredata->maxMDSblocks = bsize - namelen - sizeof(MDS) - 1;
    this->coredata->maxMDSblocks /= sizeof(unsigned long int);
    this->coredata->activeBlocks = 0;
    this->coredata->nodeIDgen = 0;

    this->cfsfile = new char[strlen(filename) + 1 + 4];
    strcpy(this->cfsfile, filename);
    strcat(this->cfsfile,".cfs");

    this->log = NULL;

    this->CFSfd = open(this->cfsfile, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP);
    if(this->CFSfd < 0) cout << "OPEN ERROR" << endl;
    if( write(this->CFSfd, this->coredata, sizeof(CFSCoreData)) != sizeof(CFSCoreData)){
        cout << "CoreData WRITE ERROR" << endl;
    }

    /*create root folder*/

    /*create and init metadata*/
    MetaData header;
    MetaData::initMetaData(header);
    /*parentID should be 0, and parent blockID too*/
    char root[] = "/";
    
    this->currDir = new char[2];
    strcpy(this->currDir,root);

    this->working_directory = new Folder(this, root, header);
    
};


CFS::CFS(char * file){

    this->coredata = (CFSCoreData*)malloc(sizeof(CFSCoreData));
    memset(this->coredata, 0, sizeof(CFSCoreData));

    int len = strlen(file) + 1;
    this->cfsfile = new char[len];
    strcpy(this->cfsfile, file);

    /*make logfile name*/
    char * ending = new char[sizeof("_CFSlog.txt") + 1];
    strcpy(ending, "_CFSlog.txt" );
    this->log = new char[len + sizeof("_CFSlog.txt") - 4];/*i subtract the .cfs*/
    strcpy(this->log,file);
    this->log = strtok(this->log, ".");
    strcat(this->log, ending);
    cout << "LOG FILE: " << this->log << endl;
    delete[] ending;

    this->output.open(this->log,ios::app);
    this->output << "root accessed" << endl;

    this->CFSfd = open(this->cfsfile, O_RDWR);
    if(this->CFSfd < 0) cout << "OPEN ERROR" << endl;
    if( int rd = read(this->CFSfd, this->coredata, sizeof(CFSCoreData)) != sizeof(CFSCoreData)){
        cout << "READ ERROR with " << rd << endl;
    }

    /*open root folder as working directory*/
    this->working_directory = new Folder(this, sizeof(CFSCoreData));

    char root[] = "/";
    this->currDir = new char[2];
    strcpy(this->currDir,root);

};

CFS::~CFS(){
    free(this->coredata);
    delete[] this->cfsfile;
    close(this->CFSfd);
    delete this->working_directory;
    if(this->log != NULL){
        delete[] this->log;
        this->output.close();
    } 
    delete[] this->currDir;
    
};

CFSCoreData * CFS::getCoreData(){
    return this->coredata;
};

void CFS::print(){
    cout << "________________________________________" << endl 
        << "Block Size: " << this->coredata->blockSize << " bytes" << endl
        << "Max Amount of Blocks per file/dir: " << this->coredata->maxMDSblocks << endl 
        << "Max name Length: " << this->coredata->namelength-1 << " caharacters " << endl
        << "Active Blocks: " << this->coredata->activeBlocks << endl
        << "NodeID generator value: " << this->coredata->nodeIDgen << endl
        << "________________________________________" << endl;
};

void CFS::updateSuperBlock(){
    // this->print();

    lseek(this->CFSfd, 0, SEEK_SET);
    if( write(this->CFSfd, this->coredata, sizeof(CFSCoreData)) != sizeof(CFSCoreData)){
        cout << "usb WRITE ERROR" << endl;
    }
};

char * CFS::getCFSfile(){
    return this->cfsfile;
};

char * CFS::blockifyMetaData(MetaData & mds, char * filename, unsigned long int * blockIDarray){
    
    /*allocate block fill it with 0*/
    char * block = (char*)malloc(this->coredata->blockSize);
    memset (block,'\0',this->coredata->blockSize);

    int index = 0;

    /*fill memory with actual data*/

    /*active block byte*/
    block[0] = 't';
    /*metadata bytes*/
    memcpy(&block[1],&mds, sizeof(MetaData));
    index = sizeof(MetaData) + 1;
    /*filename bytes*/
    memcpy(&block[index], filename, mds.filenamelength);
    index += this->coredata->namelength;

    /*write blockIDs*/
    int loopcounter;
    if (mds.numberOfBlocks < this->coredata->maxMDSblocks) loopcounter = mds.numberOfBlocks;
    else loopcounter = this->coredata->maxMDSblocks;
    for(int i = 0; i < loopcounter ; i++ ){
        memcpy(&block[index], &blockIDarray[i], sizeof(unsigned long int));
        index += sizeof(unsigned long int);
    }

    return block;

};


unsigned long int CFS::genNodeID(){
    this->coredata->activeBlocks++;
    this->coredata->nodeIDgen++;
    unsigned long int ret = this->coredata->nodeIDgen;
    this->updateSuperBlock();
    return ret;

};

int CFS::unblockifyMDS(void * data, MetaData * mds, char ** filename, unsigned long int ** blockIDArray){

    /*extract header data from block*/

    int index = 1;
    /*getting metadata*/
    memcpy(mds, &((char*)data)[index], sizeof(MetaData));
    index += sizeof(MetaData);
    /*getting filename*/
    *filename = new char[mds->filenamelength];

    strcpy(*filename, &((char*)data)[index]);
    index += this->coredata->namelength;

    /*getting blockIDarray*/
    if(mds->numberOfBlocks > 0){

        if(mds->numberOfBlocks == 1) *blockIDArray = new unsigned long int();
        else *blockIDArray = new unsigned long int[mds->numberOfBlocks];

        int loopcounter;
        if (mds->numberOfBlocks < this->coredata->maxMDSblocks) loopcounter = mds->numberOfBlocks;
        /*when using extension block to store more blockIDs*/
        else loopcounter = this->coredata->maxMDSblocks;

        for(int i = 0; i < loopcounter ; i++ ){
            
            memcpy( &((*blockIDArray)[i]), &((char*)data)[index], sizeof(unsigned long int));
            index += sizeof(unsigned long int);
        }
    }
    else{
        *blockIDArray = NULL;
    }

    return 0;
};

bool active(void * data){
    return ((char*)data)[0] == 't' ? true : false ;
};

unsigned long int CFS::getEmptyBlockID(){

    unsigned long int seeker = sizeof(CFSCoreData);

    /*seek file to correct block*/
    lseek(this->CFSfd, seeker, SEEK_SET);

    char * blockdata = (char*)malloc(this->coredata->blockSize);
    memset(blockdata, 0, this->coredata->blockSize);

    /*search for an empty block*/
    while( read(this->CFSfd, blockdata, this->coredata->blockSize) == this->coredata->blockSize ){
        if(blockdata[0] == 'f'){
            break;
        }

        seeker += this->coredata->blockSize;
    }

    free(blockdata);
    return seeker;
};

void CFS::removeBlock(unsigned long int blockID){
    /*seek file to correct block*/
    lseek(this->CFSfd, blockID, SEEK_SET);

    char f = 'f';
    if( write( this->CFSfd, &f, 1 ) != 1){
        cout << "ERROR REMOVING BLOCK" << endl;
    }

    this->coredata->activeBlocks--;
    this->updateSuperBlock();
};

int CFS::mkdir(char * name){

    /*call mkdir on working directory*/
    int ret = this->working_directory->mkdir(name);
    char * succ[] = {"successful", "unsuccessful"};
    this->output << "mkdir " << name << " was " << succ[ret] << endl;
};

int CFS::touch(char * name, bool optionA, bool optionB){
    /*call touch on working directory*/
    int ret = this->working_directory->touch(name, optionA, optionB);
    char * succ[] = {"successful", "unsuccessful"};
    this->output << "touch " << name << " was " << succ[ret] << endl;
};

int CFS::ls(char option, char * path){
    /*call ls on working directory*/
    int ret = this->working_directory->ls(option, path);
    char * succ[] = {"successful", "unsuccessful"};
    this->output << "ls was " << succ[ret] << endl;
};

int CFS::cd(char * relatDir){
    /*call cd on working directory, and modify working directory*/
    char root[2] = "/";
    int ret;
    if( ret = this->working_directory->cd(relatDir) == 0){
        if(strcmp("..",relatDir) == 0){
            /*locating previous directory name(or root)*/
            int len = strlen(this->currDir);
            int pos = len-1;
            for(pos; pos > 0; pos--){
                if(this->currDir[pos] == '/'){
                    break;
                }
            }
            if(pos > 0){
                char * temp = new char[pos+1];
                memcpy(temp, this->currDir, pos);
                temp[pos] = '\0';
                delete[] this->currDir;
                this->currDir = temp;
            }
            else{
                delete[] this->currDir;
                this->currDir = new char[2];
                strcpy(this->currDir,root);
            }
            
        }
        else if(strcmp(".",relatDir) == 0){
            delete[] this->currDir;
            this->currDir = new char[2];
            strcpy(this->currDir,root);
        }
        else{
            /*length of currDir + / + relatDir + \0*/
            char * temp = new char[strlen(this->currDir) + 1 + strlen(relatDir) + 1];
            /*make current directory string*/
            strcpy(temp, this->currDir);
            if (strcmp(this->currDir, root) != 0) strcat(temp,"/");
            strcat(temp,relatDir);
            /*delete old current and replace with new*/
            delete[] this->currDir;
            this->currDir = temp;
        }
    }
    char * succ[] = {"successful", "unsuccessful"};
    this->output << "cd to " << relatDir << " was " << succ[ret] << endl;
};

int CFS::pwd(){
    cout << this->currDir << endl;
    this->output << "PWD: " << this->currDir << endl;
};

char * CFS::getcurrDir(){
    return this->currDir;
}

int CFS::remove(char * entityName, bool option){
    int ret;
    if( ret = this->working_directory->remove(entityName, option) == 0){
        cout << "Entity " << entityName << " was removed!" << endl;
    }
    else{
        cout << "Entity " << entityName << " was not found!" << endl;
    }
    char * succ[] = {"successful", "unsuccessful"};
    this->output << "removing " << entityName << (char*)(option? " with -r" : "") << " was " << succ[ret] << endl;
};