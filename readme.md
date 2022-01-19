Manolas Stamatios DIT Operating Systems 2019 Project 4

Logic of CFS (Container File System)

Everything consists of blocks with size that is specified at the creation of a CFS. The only block that is different is the Super Block, which contains all the core data of the CFS. So, each file and folder have a header block with all the Metadata (dates, sizes, etc). After the metadata, if it's a folder, the remaining bytes of the header block point to other header blocks, or, if it's a file, they point to data blocks. The 1st byte of each block has it's state (dead or alive), so when a block is dead (file or folder removal), if there's a need for a new block, it takes its place.
	

MDS Block visualization

|||  *******************************************   
|||	* 1 byte, state of block (alive/dead)        
|||	* unsigned long int nodeid;               
|||	* unsigned int filenamelength;	           
|||	* unsigned int size; 			        
|||	* unsigned int type;                      
|||	* unsigned int linkCounter;				
|||	* unsigned long int parentnodeid;            
|||	* unsigned long int parentBlockID;           
 B 	* unsigned long int linksArrayBlockID;       
 L 	* unsigned long int parentnodeid;            
 O 	* time_t creationTime;                       
 C 	* time_t accessTime;                         
 K 	* time_t modificationTime;                   
|||	* unsigned int numberOfBlocks;               
|||	* ---------                   
 S 	* max filename bytes				      
 I 	* ---------      
 Z 	* size of filename  
 E 	* blockID (pointing to Data/MDS)          
|||	* blockID (pointing to Data/MDS)          
|||	* blockID (pointing to Data/MDS)          
|||	* blockID (pointing to Data/MDS)          
|||	* ...more blockIDs              		         
|||	* blockID that points to next set of blockIDs					      
|||  *****************************************  

Possible commands:

* cfs_workwith <FILE>
* cfs_mkdir <DIRECTORIES>
* cfs_touch <OPTIONS> <FILES>
* cfs_pwd
* cfs_cd <PATH>
	
	Works with <PATH>, <..>, <.>, and in folders like linux file system.
* cfs_ls <OPTIONS>
	
	-l: Print folder metadata
	
	-d: Print only catalogs
* cfs_rm <OPTIONS> <DESTINATIONS>
	
	Works only in local folder, not with path.
* cfs_create <OPTIONS> <FILE>. Create cfs in file <FILE>
	
	-bs <BLOCK SIZE>: Set block size in Bytes
	
	-fns <FILENAME SIZE>: Set max filename size in Bytes
* cfs_finish
* exit
	
Also, a log file is created with all the user commands that were used.
	
	
GENERAL INFO:
	
* The execution of the container file system in linux terminal starts with the command:

			./CFSshell
* A makefile is used, so, 'make' is used for compilation and 'make clean' to remove the .o files.
* I also used Valgrind to check for memory leaks with the command:
	
			make clean && make && valgrind -v --leak-check=yes --show-leak-kinds=all --track-fds=yes --track-origins=yes ./CFSshell

and there are no memory leaks.

	

