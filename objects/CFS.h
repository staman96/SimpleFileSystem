#ifndef CFS_h
#define CFS_h

// #include <fstream>

class MetaData;
class Folder;
class MDS;
using std::ofstream;

/*aka SUPER Block*/
class CFSCoreData{
    public:
    unsigned int blockSize, 
        maxMDSblocks,/*the number of blockIDs that an MDS can fit
                    * (depends on blockSize)*/
        namelength;/*max name length*/
    unsigned long int activeBlocks;/*how many blocks are used currently*/
    unsigned long int nodeIDgen;/*++ when an entity is created*/
    
};

class CFS{
    private:
        CFSCoreData * coredata;
        char * cfsfile, 
            * log, /*cfsfilename_log.txt*/
            * currDir;
        int CFSfd, LOGfd;
        ofstream output;
        Folder * working_directory;/*initializes from root*/
    public:
        CFS(unsigned int bsize, unsigned int namelen, char * filename);/*constructor for new cfs*/
        CFS(char * file);/*constructor for existing cfs*/
        ~CFS();

        
        void print();/*prints data*/
        void updateSuperBlock();/*update values of super block in file*/

        /*fills a block with header data*/
        char * blockifyMetaData(MetaData & mds, char * filename, unsigned long int * blockIDarray);
        /*converts file data to block array(not used)*/
        void ** blockifyFileData(void * data);
        bool active(void * data);
        /*data = blocksize vector with binary data*/
        int unblockifyMDS(void * data, MetaData * mds, char ** filename, unsigned long int ** blockIDArray);

        /*generates new Node ID when called*/
        unsigned long int genNodeID();

        /*if there are less empty blocks than needed then
        * continues to write at the end of file*/
        unsigned long int getEmptyBlockID();

        /*getters*/
        char * getCFSfile();
        char * getcurrDir();
        CFSCoreData * getCoreData();

        /*sets 1 byte of given to blockID to dead*/
        void removeBlock(unsigned long int blockID);
        

        /*shell commands*/

        int mkdir(char * name);
        int touch(char * name, bool optionA, bool optionB);
        int ls(char option, char * path);
        int cd(char * relatDir);
        int pwd();
        int remove(char * entityName, bool option);


};

#endif