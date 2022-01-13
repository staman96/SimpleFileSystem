Manolas Stamatios DIT Operating Systems 2019 Project 4

Logic of CFS (Container File System)

Everything consists of blocks with size that is specified at the creation of a CFS. The only block that is different is the Super Block, which contains all the core data of the CFS. So, each file and folder have a header block. The 1st byte of each block has it's state (dead or alive). 

MDS Block visualization

*******************************************  ---
* 1 byte, state of block (alive/dead)     *   |
* unsigned long int nodeid;               *   |
* unsigned int filenamelength;	          *   |
* unsigned int size; 			          *   |
* unsigned int type;                      *   |
* unsigned int linkCounter;				  *   |
* unsigned long int parentnodeid;         *   |
* unsigned long int parentBlockID;        *   |
* unsigned long int linksArrayBlockID;    *   |
* unsigned long int parentnodeid;         *   |
* time_t creationTime;                    *   |
* time_t accessTime;                      *   |
* time_t modificationTime;                *   |
* unsigned int numberOfBlocks;            *   |
* -----------------------------           *   |
* max filename bytes				      *
* ------------------------------          *   Block Size
* size of filename                        *
* blockID (pointing to Data/MDS)          *   |
* blockID (pointing to Data/MDS)          *   |
* blockID (pointing to Data/MDS)          *   |
* blockID (pointing to Data/MDS)          *   |
				. 				          	  |
				.                         	  |
				.				          	  |
* blockID (pointing to Data/MDS)          *   |
* blockID points to next set of           *   |
*	blockIDs					          *   |

*******************************************  ---
