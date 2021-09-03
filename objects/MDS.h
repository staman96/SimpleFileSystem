#ifndef MDS_h
#define MDS_h

#include <time.h>

class CFS;

class MetaData{
    public:
        unsigned long int nodeid;
        unsigned int filenamelength;/*name should have a limit*/
        unsigned int size;  /*size in bytes*/
        /*1. type = 0 is a folder. In that case:
        * the blockIDs point to internal files/folders
        * MetaData
        * 
        * 2. type = 1 is file. In that case:
        * the blockIDs point to the file data 
        * 
        * 3. type = 2 is blockIDs extension, where the
        * 1st blockID points to the MDS block
        */
        unsigned int type;
        unsigned int linkCounter;
        unsigned long int parentnodeid;
        unsigned long int parentBlockID;
        unsigned long int linksArrayBlockID;
        /*points to block with BlockIDs of for large files,
        * or folder with a lot of internal entities
        */
        unsigned long int extentBlockID;
        time_t creationTime;
        time_t accessTime;
        time_t modificationTime;
        /*rest of block size are blockIDs of data or Metadata
        */
        unsigned long int numberOfBlocks;
        
        /*initializes metadata to 0*/
        static void initMetaData(MetaData & mds);
        /*prints metadata(debug)*/
        static void print(MetaData & mds);
};

class MDS{
    protected:
        MetaData mds;
        char * name;/*entity name*/
        unsigned long int * entitiesBlockIDs, /*blockIDs of entities/data*/
            mdsBlockID; /*current blockID*/
        CFS * core;
        int CFSfd;/*file descriptor of cfs*/
        
    public:
        MDS(CFS * cfs, char * entityname, MetaData header);/*creating new file or folder*/
        MDS(CFS * cfs, unsigned long int blockID);/*for opening existing files and folders*/
        MDS(MDS & MDs); /*copy constructor*/
        ~MDS();

        /*getters*/
        unsigned long int ** getBlockIDsArray();
        unsigned long int getBlockID();
        MetaData * getMetaData();
        /*extract name from blockID*/
        char * getname(unsigned long int blockID);

        void print();/*prints data(debug)*/
        virtual void writeToFile();/*update state of entity in cfsfile*/
        virtual void readFromFile();/*get entity data from cfsfile*/

};

class Folder : public MDS{
    private:
        MetaData * entities; 

    public:
        Folder(CFS * cfs, char * foldername, MetaData header);/*creating new folder*/
        Folder(CFS * cfs, unsigned long int blockID);
        Folder(Folder & folder);
        void mutate (Folder & folder);/*used to change directory*/
        ~Folder();

        void writeToFile() override;
        void readFromFile() override;

        void initEntities();/*initializes entities array from cfsfile*/

        /*adds new entity to aray(mkdir,touch)*/
        void addEntity(unsigned long int blockID, MetaData header);
        /*removes entity from array(remove)*/
        void removeEntity(int index);

        /*functionality*/
        int mkdir(char * dirName);
        int touch(char * filename, bool optionA, bool optionB);
        int ls(char option, char * path);
        int cd(char * relatDir);
        int remove(char * entityName, bool optionR);
        int removeFolder(bool optionR);
        int removeFile(unsigned long int blockID);

};

class File : public MDS{
    private:
        
    public:
        File(CFS * cfs, char * filename, MetaData header);
        File(CFS * cfs, unsigned long int blockID, bool optionA, bool optionB);
        ~File();

        void writeToFile() override;
        void readFromFile() override;

        int remove();/*mark header and data blocks of file dead */
};

#endif