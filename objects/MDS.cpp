#include <bits/stdc++.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "CFS.h"
#include "MDS.h"

using std::cout;
using std::endl;

void MetaData::initMetaData(MetaData & mds){
    mds.accessTime = 0;
    mds.creationTime = 0;
    mds.extentBlockID = 0;
    mds.filenamelength = 0;
    mds.linkCounter = 0;
    mds.linksArrayBlockID = 0;
    mds.modificationTime = 0;
    mds.nodeid = 0;
    mds.numberOfBlocks = 0;
    mds.parentBlockID = 0;
    mds.parentnodeid = 0;
    mds.size = 0;
    mds.type = 0;
};

void MetaData::print(MetaData & mds){
    cout 
        << "NodeID: " << mds.nodeid << endl 
        << "filenamelength: " << mds.filenamelength << endl
        << "size: "  << mds.size << endl
        << "type: "  << mds.type << endl
        << "linkCounter: "  << mds.linkCounter << endl
        << "parentnodeid: "  << mds.parentnodeid << endl
        << "parentBlockID: "  << mds.parentBlockID << endl
        << "linksArrayBlockID: "  << mds.linksArrayBlockID << endl
        << "extentBlockID: "  << mds.extentBlockID << endl
        << "creationTime: " << mds.creationTime << endl
        << "accessTime: " << mds.accessTime << endl
        << "modificationTime: " << mds.modificationTime << endl
        << "numberOfBlocks: " << mds.numberOfBlocks << endl
        ;
};

/*************************************  MDS  *************************************/

MDS::MDS(CFS * cfs, char * entityname, MetaData header)
:core(cfs),mds(header){
    /*update core data*/
    CFSCoreData * coredata = cfs->getCoreData();
    this->mdsBlockID = this->core->getEmptyBlockID();

    this->mds.filenamelength = strlen(entityname) + 1;
    time(&this->mds.creationTime);
    this->mds.accessTime = 0;
    this->mds.modificationTime = 0;
    this->mds.nodeid = this->core->genNodeID();
    this->mds.linkCounter = 0;
    this->mds.linksArrayBlockID = 0;

    this->entitiesBlockIDs = NULL;

    this->name = new char[this->mds.filenamelength];
    strcpy(this->name, entityname);

    this->CFSfd = open(this->core->getCFSfile(), O_RDWR);
    if(this->CFSfd < 0) cout << "OPEN ERROR" << endl;

   

};

MDS::MDS(CFS * cfs, unsigned long int blockID)
:mdsBlockID(blockID),core(cfs){
    CFSCoreData * coredata = cfs->getCoreData();
    this->name = NULL;


    this->CFSfd = open(this->core->getCFSfile(), O_RDWR);
    if(this->CFSfd < 0) cout << "OPEN ERROR" << endl;

};

MDS::MDS(MDS & MDs){
    /*initlize data*/

    /*header data*/
    this->mds = MDs.mds;
    /*entity name*/
    this->name = new char[strlen(MDs.name) + 1];
    strcpy(this->name,MDs.name);
    /*blockIDs*/
    this->entitiesBlockIDs = NULL;
    if(this->mds.numberOfBlocks > 0){
        if(this->mds.numberOfBlocks > 1) this->entitiesBlockIDs = new unsigned long int[this->mds.numberOfBlocks];
        else this->entitiesBlockIDs = new unsigned long int(); 
        memcpy(this->entitiesBlockIDs, MDs.entitiesBlockIDs, this->mds.numberOfBlocks * sizeof(unsigned long int));
    }
    
    /*blockID*/
    this->mdsBlockID = MDs.mdsBlockID;
    /*CFS core*/
    this->core = MDs.core;
    /*opens fd*/
    this->CFSfd = open(this->core->getCFSfile(), O_RDWR);
    if(this->CFSfd < 0) cout << "OPEN ERROR" << endl;

};

MDS::~MDS(){
    close(this->CFSfd);
    delete[] this->name;
    if (this->mds.numberOfBlocks == 1 && this->entitiesBlockIDs != NULL) delete this->entitiesBlockIDs;
    else if (this->entitiesBlockIDs != NULL) delete[] this->entitiesBlockIDs;
    this->core = NULL;
};

unsigned long int ** MDS::getBlockIDsArray(){
    return &this->entitiesBlockIDs;
};

unsigned long  int MDS::getBlockID(){
    return this->mdsBlockID;
};

MetaData * MDS::getMetaData(){
    return &this->mds;
};

void MDS::print(){
    cout << "__METADATA__" << endl;
    cout 
        << "Entity Name: " << this->name << endl 
        << "NodeID: " << this->mds.nodeid << endl 
        << "filenamelength: " << this->mds.filenamelength << endl
        << "size: "  << this->mds.size << endl
        << "type: "  << this->mds.type << endl
        << "linkCounter: "  << this->mds.linkCounter << endl
        << "parentnodeid: "  << this->mds.parentnodeid << endl
        << "parentBlockID: "  << this->mds.parentBlockID << endl
        << "linksArrayBlockID: "  << this->mds.linksArrayBlockID << endl
        << "extentBlockID: "  << this->mds.extentBlockID << endl
        << "creationTime: " << this->mds.creationTime << endl
        << "accessTime: " << this->mds.accessTime << endl
        << "modificationTime: " << this->mds.modificationTime << endl
        << "numberOfBlocks: " << this->mds.numberOfBlocks << endl
        << "Internal Entities's BlockIDs: " << endl
        ;
        for (int i = 0; i < this->mds.numberOfBlocks; i++){
            cout << this->entitiesBlockIDs[i] << '\t';   
        }
        cout << endl;
};

void MDS::writeToFile(){

    //check if numberOfBlocks > maxMDSblocks, handle extension block (not used)

    /*seek file to correct block*/
    lseek(this->CFSfd, this->mdsBlockID, SEEK_SET);

    char * blockdata = this->core->blockifyMetaData(this->mds, this->name, this->entitiesBlockIDs);

    int blockSize = this->core->getCoreData()->blockSize;

    if( write( this->CFSfd, blockdata, blockSize ) != blockSize ){
        cout << "ERROR WRITING HEADER DATA" << endl;
    }

    free(blockdata);
};


void MDS:: readFromFile(){
    /*seek file to correct block*/
    lseek(this->CFSfd, this->mdsBlockID, SEEK_SET);

    /*read data to fill header*/
    int blockSize = this->core->getCoreData()->blockSize;
    char * blockdata = (char*)malloc(blockSize);
    memset(blockdata, 0, blockSize);

    if( read( this->CFSfd, blockdata, blockSize ) != blockSize ){
        cout << "ERROR READING HEADER DATA" << endl;
    }

    this->core->unblockifyMDS(blockdata, &this->mds, &this->name, &this->entitiesBlockIDs);

    free(blockdata);

};

char * MDS::getname(unsigned long int blockID){
    /*seek file to given block*/
    lseek(this->CFSfd, blockID, SEEK_SET);

    /*read data to fill header*/
    int blockSize = this->core->getCoreData()->blockSize;
    char * blockdata = (char*)malloc(blockSize);
    memset(blockdata, 0, blockSize);

    if( read( this->CFSfd, blockdata, blockSize ) != blockSize ){
        cout << "ERROR READING HEADER DATA" << endl;
    }

    MetaData mdata;/*not used*/
    char * ret = NULL;
    unsigned long int * arrayIDs = NULL;/*not used*/

    this->core->unblockifyMDS(blockdata, &mdata, &ret, &arrayIDs);

    if(mdata.numberOfBlocks == 1 && arrayIDs != NULL ) delete arrayIDs;
    else if( arrayIDs != NULL )delete[] arrayIDs;
    free(blockdata);

    return ret;

};


/**************************************  FOLDER  *************************************/

Folder::Folder(CFS * cfs, char * foldername, MetaData header)
:MDS(cfs, foldername, header){
    this->entities = NULL;

    this->entitiesBlockIDs = NULL;

    this->mds.size = 0;
    this->mds.numberOfBlocks = 0;

    // this->MDS::print();

    /*after init write itself to cfs file*/
    this->writeToFile();

}

Folder::Folder(CFS * cfs, unsigned long int blockID)
:MDS(cfs, blockID){
    this->entities = NULL;
    this->entitiesBlockIDs = NULL;

    this->readFromFile();

    this->initEntities();

    time(&this->mds.accessTime);
    // this->MDS::print();
    this->writeToFile();

};

Folder::Folder(Folder & folder)
:MDS(folder){
    this->entities = NULL;

    /*mutate current class's data*/
    if(this->mds.numberOfBlocks > 0){
        this->entities = (MetaData*)malloc(this->mds.numberOfBlocks * sizeof(MetaData));
        memcpy(this->entities, folder.entities, this->mds.numberOfBlocks * sizeof(MetaData));
    }

    // this->print();

};


void Folder::mutate(Folder & folder){

    /*chain call destructors*/
    this->~Folder();
    new(this) Folder(folder);

    // this->print();

};

Folder::~Folder(){
    if (this->entities != NULL) free(this->entities);

};

void Folder::writeToFile(){

    this->MDS::writeToFile();
    //then write any folder specific data, ex. the extension block
};

void Folder::readFromFile(){

    this->MDS::readFromFile();
    //then read any folder specific data
};

void Folder::initEntities(){

    /*allocate required space*/
    if (this->mds.numberOfBlocks > 0) this->entities = (MetaData*)malloc(this->mds.numberOfBlocks * sizeof(MetaData));

    for (int i = 0; i < this->mds.numberOfBlocks; i++){
        MetaData::initMetaData(this->entities[i]);
        /*seek file to correct block*/
        lseek(this->CFSfd, this->entitiesBlockIDs[i], SEEK_SET);

        /*read data to fill header*/
        int blockSize = this->core->getCoreData()->blockSize;
        char * blockdata = (char*)malloc(blockSize);
        memset(blockdata, 0, blockSize);

        if( read( this->CFSfd, blockdata, blockSize ) != blockSize ){
            cout << "ERROR READING HEADER DATA" << endl;
        }

        char * name = NULL; /*not used*/
        unsigned long int * arrayIDs = NULL;/*not used*/

        this->core->unblockifyMDS(blockdata, &this->entities[i], &name, &arrayIDs);
        // MetaData::print(this->entities[i]);

        delete[] name;
        if (this->entities[i].numberOfBlocks == 1 && arrayIDs != NULL) delete arrayIDs;
        else if (arrayIDs != NULL) delete[] arrayIDs;
        free(blockdata);
    }
};

int Folder::mkdir(char * dirName){

    /*iniliatize header*/
    MetaData header;
    MetaData::initMetaData(header);
    header.parentnodeid = this->mds.nodeid;
    header.parentBlockID = this->mdsBlockID;

    /*create directory*/
    Folder * newDir = new Folder( this->core, dirName, header);

    /*update parent info*/
    this->addEntity(newDir->mdsBlockID, newDir->mds);
    time(&this->mds.modificationTime);
    // this->MDS::print();
    this->writeToFile();

    delete newDir;

    return 0;
};

void Folder::addEntity(unsigned long int blockID, MetaData header){

    this->mds.numberOfBlocks++;
    if(this->mds.numberOfBlocks == 1){
        this->entities = (MetaData*)malloc( sizeof(MetaData) );
        this->entitiesBlockIDs = new unsigned long int();

        
    }
    else{

        // this->print();
        MetaData * MDtemp = (MetaData*)malloc(this->mds.numberOfBlocks * sizeof(MetaData));
        
        unsigned long int * bIDtemp = new unsigned long int[this->mds.numberOfBlocks];

        for (int i = 0; i < this->mds.numberOfBlocks-1; i++){
            MetaData::initMetaData(MDtemp[i]);
            MDtemp[i] = this->entities[i];
            bIDtemp[i] = this->entitiesBlockIDs[i];
        }

        free(this->entities);

        if(this->mds.numberOfBlocks == 2){
            delete this->entitiesBlockIDs;
        }
        else{
            delete[] this->entitiesBlockIDs;
        }

        this->entities = MDtemp;
        this->entitiesBlockIDs = bIDtemp;
    }
    /*init new addition*/
    MetaData::initMetaData(this->entities[this->mds.numberOfBlocks-1]);

    this->entities[this->mds.numberOfBlocks-1] = header;
    this->entitiesBlockIDs[this->mds.numberOfBlocks-1] = blockID;
    
};

void Folder::removeEntity(int index){
    this->mds.numberOfBlocks--;
    /*if it was the last entity*/
    if(this->mds.numberOfBlocks == 0){

        free(this->entities);
        delete this->entitiesBlockIDs;
        this->entities = NULL;
        this->entitiesBlockIDs = NULL;
    }
    else{
        /*allocate new space */
        MetaData * MDtemp = (MetaData*)malloc(this->mds.numberOfBlocks * sizeof(MetaData));
        unsigned long int * bIDtemp = new unsigned long int[this->mds.numberOfBlocks];

        for(int i = 0; i < index; i++){
            MetaData::initMetaData(MDtemp[i]);
            MDtemp[i] = this->entities[i];
            bIDtemp[i] = this->entitiesBlockIDs[i];
        }

        for (int i = index; i < this->mds.numberOfBlocks; i++){
            MetaData::initMetaData(MDtemp[i]);
            MDtemp[i] = this->entities[i+1];
            bIDtemp[i] = this->entitiesBlockIDs[i+1];
        }

        /*free old space*/
        free(this->entities);
        delete[] this->entitiesBlockIDs;
       
        /*replace with new*/
        this->entities = MDtemp;
        this->entitiesBlockIDs = bIDtemp;
    }
};

int Folder::touch(char * filename, bool optionA, bool optionB){

    File * newFile;

    /*change stats if an option is set*/
    if(optionA || optionB){
        unsigned long int fileBlockID;
        char * nameTemp = NULL;
        /*locate file*/
        for(int i = 0; i < this->mds.numberOfBlocks; i++){
            nameTemp = this->getname(this->entitiesBlockIDs[i]);
            if(this->entities[i].type == 1 && strcmp(nameTemp, filename) == 0){
                fileBlockID = this->entitiesBlockIDs[i];
                delete[] nameTemp;
                break;
            }
            delete[] nameTemp;
        }

        /*update stats on file*/
        newFile = new File( this->core, fileBlockID, optionA, optionB);
    }
    /*create file*/
    else{

        /*iniliatize file header*/
        MetaData header;
        MetaData::initMetaData(header);
        header.parentnodeid = this->mds.nodeid;
        header.parentBlockID = this->mdsBlockID;

        /*create file*/
        newFile = new File( this->core, filename, header);
        

        this->addEntity(newFile->getBlockID(), *newFile->getMetaData());

    }

    /*update parent info*/
    time(&this->mds.modificationTime);
    // this->MDS::print();
    this->writeToFile();

    delete newFile;

    return 0;
};

int Folder::ls(char option, char * path){
    if(option == '\0'){
        for(int i = 0; i < this->mds.numberOfBlocks; i++){
            char * nameTemp = this->getname(this->entitiesBlockIDs[i]);
            cout << nameTemp << (char)(this->entities[i].type == 0 ? '/' : 32) << '\t';
            delete[] nameTemp;
        }
        cout << endl;
    }
    else if (option == 'l'){
        for(int i = 0; i < this->mds.numberOfBlocks; i++){
            char * nameTemp = this->getname(this->entitiesBlockIDs[i]);
            cout << nameTemp << (char)(this->entities[i].type == 0 ? '/' : 32) << '\t'
                << "crt: " << strtok(ctime(&this->entities[i].creationTime), "\n") << '\t'
                << "acc: " << strtok(ctime(&this->entities[i].accessTime), "\n") << '\t'
                << "mod: " << strtok(ctime(&this->entities[i].modificationTime), "\n") << '\t'
                << "size: " << (this->entities[i].type == 0 ? this->entities[i].numberOfBlocks : this->entities[i].size) << '\t'
                << "id: " << this->entities[i].nodeid << endl;
            delete[] nameTemp;
        }
        cout << endl;
    }
    else if (option == 'd'){
        for(int i = 0; i < this->mds.numberOfBlocks; i++){
            if(this->mds.type == 0){
                char * nameTemp = this->getname(this->entitiesBlockIDs[i]);
                cout << nameTemp << (char)(this->entities[i].type == 0 ? '/' : 32) << '\t'
                    << "crt: " << strtok(ctime(&this->entities[i].creationTime), "\n") << '\t'
                    << "acc: " << strtok(ctime(&this->entities[i].accessTime), "\n") << '\t'
                    << "mod: " << strtok(ctime(&this->entities[i].modificationTime), "\n") << '\t'
                    << "size: " << this->entities[i].numberOfBlocks << '\t'
                    << "id: " << this->entities[i].nodeid << endl;
                delete[] nameTemp;
            }
        }
        cout << endl;
    }
    else{
        return 1;
    }
    return 0;
};

int Folder::cd(char * relatDir){

    if(strcmp("..",relatDir) == 0){

        if(this->mds.parentnodeid == 0){
            cout << "Can't go backwards past the root directory" << endl;
            return 1;
        }

        /*initialize previous directory*/
        Folder * prevDir = new Folder(this->core, this->mds.parentBlockID);
        /*mutate this directory to its previous*/
        this->mutate(*prevDir);

        delete prevDir;

        return 0;
    }
    else if(strcmp(".",relatDir) == 0){

        while (this->mds.parentnodeid > 0){

            /*initialize previous directory*/
            Folder * prevDir = new Folder(this->core, this->mds.parentBlockID);
            /*mutate this directory to its previous*/
            this->mutate(*prevDir);

            delete prevDir;
        }
        return 0;
        
    }
    else{
        for(int i = 0; i < this->mds.numberOfBlocks; i++){
            if(this->entities[i].type == 0){
                char * nameTemp = this->getname(this->entitiesBlockIDs[i]);
                if (strcmp(relatDir, nameTemp) == 0){
                    /*initialize next directory*/
                    Folder * nextDir = new Folder(this->core, this->entitiesBlockIDs[i]);
                    /*mutate this directory to its previous*/
                    this->mutate(*nextDir);
                    delete nextDir;
                    // this->print();

                    delete[] nameTemp;
                    return 0;
                }
                delete[] nameTemp;
            }
        }
    }

    return 1;
};

int Folder::remove(char * entityName, bool optionR){
    int ret = 1;
    /*search for entity with entityName*/
    int i;
    for(i = 0; i < this->mds.numberOfBlocks; i++){
            
        char * nameTemp = this->getname(this->entitiesBlockIDs[i]);
        if (strcmp(entityName, nameTemp) == 0){
            if(this->entities[i].type == 0){
                
                /*init directory to be removed*/
                Folder * rmDir = new Folder(this->core, this->entitiesBlockIDs[i]);
                rmDir->removeFolder(optionR);
                delete rmDir;

                delete[] nameTemp;
                ret = 0;
                break;
            }
            else if(this->entities[i].type == 1){
                
                /*remove file*/
                this->removeFile(this->entitiesBlockIDs[i]);

                delete[] nameTemp;
                ret = 0;
                break;
            }
        }
        delete[] nameTemp;
    }

    /*if removal was successful*/
    if(ret == 0){
        /*update metadata of current directory*/
        this->removeEntity(i);
        time(&this->mds.modificationTime);
        // this->MDS::print();
        this->writeToFile();
    }

    return ret;
};

int Folder::removeFile(unsigned long int blockID){
    File * rmFile = new File(this->core, blockID, false, false);
    rmFile->remove();
    delete rmFile;
};

int Folder::removeFolder(bool optionR){

    /*deletes every entity in directory*/
    for(int i = 0; i < this->mds.numberOfBlocks; i++){
        if(this->entities[i].type == 0){
            if(!optionR)this->core->removeBlock(this->entitiesBlockIDs[i]);
            else{/*if -r otion was given*/
                 /*init directory to be removed*/
                Folder * rmDir = new Folder(this->core, this->entitiesBlockIDs[i]);
                rmDir->removeFolder(optionR);
                delete rmDir;
            }
            
        }
        else if (this->entities[i].type == 1){
            this->removeFile(this->entitiesBlockIDs[i]);
        }

    }

    /*deletes itself*/
    this->core->removeBlock(this->mdsBlockID);
    
};


/*************************************  FILE  *************************************/

File::File(CFS * cfs, char * filename, MetaData header)
:MDS(cfs, filename, header){
    this->mds.size = 0;
    this->mds.numberOfBlocks = 0;
    this->mds.type = 1;
    time(&this->mds.modificationTime);
    time(&this->mds.accessTime);

    // this->MDS::print();

    this->writeToFile();
};

File::File(CFS * cfs, unsigned long int blockID, bool optionA, bool optionB)
:MDS(cfs, blockID){

    this->readFromFile();

    if(optionA) time(&this->mds.accessTime);
    if(optionB) time(&this->mds.modificationTime);
         
    // this->MDS::print();

    this->writeToFile();

};

File::~File(){

};

void File::writeToFile(){
    this->MDS::writeToFile();
};

void File::readFromFile(){
    this->MDS::readFromFile();
};

int File::remove(){

    /*remove datablocks*/
    for(int b = 0; b < this->mds.numberOfBlocks; b++){
        this->core->removeBlock(this->entitiesBlockIDs[b]);
    }

    /*remove header block*/
    this->core->removeBlock(this->mdsBlockID);
}