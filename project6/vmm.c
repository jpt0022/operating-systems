#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include "address.h"

#define TLB_SIZE 16
#define NUM_PAGES 256
#define NUM_FRAMES 256
#define PAGE_SIZE 256	
#define FRAME_SIZE 256
#define OFFSET_BITS 8
#define OFFSET_MASK 0xFF
#define MAX_LIST_SIZE 256
/*
 *
 * Jessica Tomas jpt0022
 * Virtual Memory Manager - Project6
 *
 *
 */
const char backingstore[] = "BACKING_STORE";
typedef unsigned char byte;
typedef unsigned int u_int;
typedef unsigned int page_t;
typedef unsigned int frame_t;
typedef unsigned int age_t;
typedef unsigned int value_t;
typedef unsigned int offset_t;
typedef unsigned int laddress;
typedef unsigned int paddress;
typedef frame_t page_table_t;
typedef unsigned char physical_memory_t[NUM_FRAMES * PAGE_SIZE];
typedef laddress logic_address_list_t;

typedef struct	{
	page_t page_num;
	frame_t frame_num;
	age_t age;
	bool valid;
}tlb_entry_t;

typedef struct	{
	tlb_entry_t tlb_entry[TLB_SIZE];
	u_int next_tlb_ptr;
}tlb_t;
/*
typedef struct{
	bool valid;
}page_table_t;
*/
typedef struct	{
	laddress logic_addr;
	paddress physical_addr;
	value_t value;
	bool valid;
}address_value_list_t;

int replacementStrat;
int count = 0;
physical_memory_t physical_memory[NUM_FRAMES*PAGE_SIZE];
//structs: physical memory, address value list


/*prototypes*/
void welcomeMessage();
laddress get_logic_address(laddress l_address);
page_t get_page_num(laddress l_address, page_t page_num);
offset_t get_offset(laddress l_address, offset_t offset);
void init_page_table(page_table_t *page_table);
void init_address_value_list(address_value_list_t *address_value_list);
int TLB_init(tlb_t *tlb);
int TLB_replacement_LRU(page_t page_num, frame_t frame_num,
			tlb_t *tlb);
int TLB_replacement_FIFO(page_t page_num, frame_t frame_num,
			tlb_t* tlb);
int TLB_display(tlb_t tlb);
int logic_address_loader(FILE *fp, logic_address_list_t *logic_address_list);
void extract_logic_address(laddress logic_address, 
				page_t *page_num, offset_t *offset);
void search_tlb(page_t page_num, tlb_t *tlb, 
		bool *is_tlb_hit, frame_t *frame_num);
void search_page_table(page_t page_num, page_table_t *page_table, 
			bool *is_page_fault, frame_t *frame_num);
//function 5&6 look at notes
int page_fault_handler(page_t page_num, frame_t frame_num, 
			physical_memory_t *physical_mem,
			page_table_t *p_table, tlb_t *tlb);
void page_table_update(page_t page_num, frame_t *frame_numi, page_table_t *page_table);
void create_physical_address(frame_t frame_num, offset_t off,
			paddress *physical_addr);
int read_physical_memory(paddress p_address, physical_memory_t physical_mem,
				value_t *value);
//need output type
//address_value_list-t
void update_address_value_list(laddress l_address, paddress p_address,
			value_t value, address_value_list_t *addr_value_list);

void output_address_value_list(const char *output_file_name, 
				address_value_list_t *addr_value_list);


/*
 *
 *
 *
 */
int
main(int argc, char *argv[])	
{
/*declare local variables: */

page_t page_num;
frame_t frame_num;
offset_t offset;

logic_address_list_t logic_address_list[MAX_LIST_SIZE];
laddress l_address;
paddress p_address;

u_int laddress_list_size;

tlb_t sys_tlb[TLB_SIZE];
page_table_t page_table[NUM_PAGES];

//physical_memory_t physical_memory ;

value_t value;
address_value_list_t address_value_list;

bool is_tlb_hit;
bool is_page_fault;


//void welcomeMessage();

char *input_file;
char output_file[] = "vm_sim_output.txt";

if (argc != 2)	{
	printf("Please add an input file");
}
else	{
	
	input_file = argv[1];
	FILE *fp;
	if ( !(fp = fopen(input_file, "r")))	{
		printf("File %s cannot be opened. Please retry...\n",
			input_file);
		return EXIT_FAILURE;
	}
	laddress_list_size = logic_address_loader(fp, logic_address_list);
	fclose(fp);
}
/*
u_int k;
for (k = 0; k < laddress_list_size; k++)	{
	printf("logic address list[%d]: %d\n", k, logic_address_list[k]);
}

TLB_init(&sys_tlb);
//printf("logic_address_list at 5 after tlb init %d\n", logic_address_list[5]);
*/
init_page_table(&page_table);
//printf("logic_address_list at 5 after pt init%d\n", logic_address_list[5]);

init_address_value_list(&address_value_list);
welcomeMessage();
printf("Choose TLB Replacement Strategy [1: FIFO, 2: LRU] ");
scanf("%d", &replacementStrat);
//printf("logic_address_list at 1 %d\n", logic_address_list[1]);


//laddress_list_size = logic_address_loader(char logic_address_file_name[], laddress logic_address_list);

//printf("logic_address_list at 1 %d\n", logic_address_list[1]);
printf("\n");
int i;
for (i =0; i < laddress_list_size; i++)	{
	/*get logic address, page num, and offset for each addr in list*/
	l_address = logic_address_list[i];
	page_num = get_page_num(l_address, page_num);
	offset = get_offset(l_address, offset);
	frame_num = page_num;
//	printf("\n");
//	printf("l address is %u logic_address_value_list[i] is %d\n\n", l_address,
//			logic_address_list[i]);
//	printf("\ti = %u\n logic addr = %u\n page num = %u\n offset = %u\n",
//		i, l_address, page_num, offset);
	/*search tlb*/
	search_tlb(page_num, &sys_tlb, &is_tlb_hit, &frame_num);
	
	/*TLB HIT*/
	if (is_tlb_hit == true)	{
		create_physical_address(frame_num, offset, &p_address);
	}
	/*TLB MISS*/
	else	{
		search_page_table(page_num, &page_table,
				&is_page_fault, &frame_num);
		/*found in page table*/
		if (is_page_fault == false)	{
			create_physical_address(frame_num, offset, &p_address);
			if (replacementStrat == 1)	{
				TLB_replacement_FIFO(page_num, frame_num, &sys_tlb);
			}
			else	{
				TLB_replacement_LRU(page_num, frame_num, &sys_tlb);
			}
		}
	
		/*not found in page table - have a page fault*/
		else	{
	//		printf("entering page_fault handler");
			page_fault_handler(page_num, frame_num, &physical_memory,
						&page_table, &sys_tlb);	
			create_physical_address(frame_num, offset, &p_address);
		}
	}/*End of TLB MISS*/
	read_physical_memory(p_address, physical_memory, &value);
	update_address_value_list(l_address, p_address, value, &address_value_list);	
	
}
	output_address_value_list(output_file, &address_value_list);

}

void
welcomeMessage()
{
	printf("Welcome to Jessica's VM Simulator \n");
	printf("Page size: %d bytes\n", PAGE_SIZE);
	printf("Page table size: %d\n", NUM_PAGES);
	printf("TLB size: %d entries\n", TLB_SIZE);
	printf("Number of physical frames: %d \n", NUM_FRAMES);
	printf("Physical memory size: %d bytes\n", NUM_FRAMES*PAGE_SIZE);
}
/*
laddress
get_logic_address(laddress l_address)
{
	return itob16(logical_address);	
}
*/
page_t
get_page_num(laddress l_address, page_t page_num)	
{
	page_num = l_address >> OFFSET_BITS;
	return page_num;	
	
}

offset_t
get_offset(laddress l_address, offset_t offset)	
{   
	offset = l_address & OFFSET_MASK;
	return offset;
}

void
init_page_table(page_table_t *page_table)
{
	
	u_int i;
	for (i = 0; i < NUM_PAGES; i++)	{
		page_table[i] = -1;
						
	}
}
int 
TLB_init(tlb_t *tlb)
{
	u_int i;
	for (i =0; i < TLB_SIZE; i++)	{
		tlb->tlb_entry[i].valid = false;
	}	
	return 0;	
}

void
init_address_value_list(address_value_list_t *address_value_list)
{
	u_int i =0;
	for (i=0; i < 256; i++)
	{
		address_value_list[i].valid = false;
	}
}

int
TLB_replacement_LRU(page_t page_num, frame_t frame_num, tlb_t *tlb)
{
	u_int i;
	/*scan through tlb for empty slot*/
	for(i = 0; i < TLB_SIZE; i++)	{
		/*found empty space in tlb*/
		if (tlb->tlb_entry[i].valid == false)	{
			tlb->tlb_entry[i].page_num = page_num;
			tlb->tlb_entry[i].frame_num = frame_num;
			tlb->tlb_entry[i].age = 1;
			tlb->tlb_entry[i].valid = true;
			return 0;
		}
		
	}
	u_int oldest = 1;
	u_int replace;
	/*no empty space look for oldest entry*/
	for (i = 0; i < TLB_SIZE; i ++)	{
		/*older entry found*/
		if (oldest < tlb->tlb_entry[i].age)	{
			oldest = tlb->tlb_entry[i].age;
			replace = i;
		}
	}
	tlb->tlb_entry[replace].page_num = page_num;
	tlb->tlb_entry[replace].frame_num = frame_num;
	tlb->tlb_entry[replace].age = 1;
	tlb->tlb_entry[replace].valid = true;
	return 0;
	
}

int
TLB_replacement_FIFO(page_t page_num, frame_t frame_num, tlb_t *tlb)
{

	u_int i;
	for (i = 0; i < TLB_SIZE; i++)	{
		/*empty space in tlb*/
		if (tlb->tlb_entry[i].valid == false)	{
			tlb->tlb_entry[i].page_num = page_num;
                        tlb->tlb_entry[i].frame_num = frame_num;
                        tlb->tlb_entry[i].age = 1;
                        tlb->tlb_entry[i].valid = true;
		}
	}
	if (count > 15)
	{
		count = 0;
	}
	tlb->tlb_entry[count].page_num = page_num;
	tlb->tlb_entry[count].frame_num = frame_num;
	tlb->tlb_entry[count].age = 1;
	tlb->tlb_entry[count].valid = true;
	count++;
}
int
tlb_display(tlb_t *tlb)	
{
	u_int i;
	for (i=0; i < TLB_SIZE; i++)	{
		printf("TLBB entry %d, page num: %d, frame num: %d"
			, tlb->tlb_entry[i].page_num, 
			tlb->tlb_entry[i].frame_num);
		if (tlb->tlb_entry[i].valid == false)	{
			printf("Invalid\n");
		}
		else printf("Valid\n");
	}
	return 0;
}

int
logic_address_loader(FILE *fp,
		logic_address_list_t *logic_address_list)
{
	int i = 0;
	while (fscanf(fp,"%u", &logic_address_list[i]) != EOF)	{
		//logic_address_list[i]
//		printf("logic address list[%d]: %d\n",i, logic_address_list[i]);
		i++;
	}
	return i;	
}
void 
search_tlb(page_t page_num, tlb_t *tlb, 
	bool *is_tlb_hit, frame_t *frame_num)
{
	*is_tlb_hit = false;
	u_int i;
	/*scan through tlb*/
	for (i = 0; i < TLB_SIZE; i++)	{
		/*look for page_num*/
		if (tlb->tlb_entry[i].valid == true && tlb->tlb_entry[i].page_num == page_num)	{
			/*found it*/
			*is_tlb_hit = true;
			*frame_num = tlb->tlb_entry[i].frame_num;
			tlb->tlb_entry[i].age = 1;
		}
	}
	
}

void
search_page_table(page_t page_num, page_table_t *page_table,
			bool *is_page_fault, frame_t *frame_num)
{
	*is_page_fault = true;
	u_int i;
	for (i = 0; i < NUM_PAGES; i++)	{
		if (page_table[i] == page_num)	{
			*frame_num = page_num;
			*is_page_fault = false;
		}
	}
//	return 0;
}

int
page_fault_handler(page_t page_num, frame_t frame_num, 
			physical_memory_t *physical_memory,
			page_table_t *page_table, tlb_t *tlb)

{
//	printf("inside");
	FILE *file;
	byte one_byte;
	fpos_t pos;
	u_int i;
	u_int frame;
	file = fopen(backingstore, "r");
	if (file == 0)	{
	printf("Count not open file %s.\n", backingstore);
	}	
	else	{
		fseek(file, page_num*FRAME_SIZE, SEEK_SET);
	/*	fgetpos(file, &pos);
		for (frame = 0; frame < 256; frame++)	{
			if (physical_memory[frame] != NULL)	{
				break;
			}
		}*/
		for (i = 0; i < 256; i++)	{
			fread(&one_byte, 1, 1, file);
		//	printf("0x%x, %d\n", one_byte, one_byte);
			physical_memory[frame_num][i] = one_byte;	
		}
		fclose(file);
		frame_num = &frame;
//		printf("got here");
		
	}

	page_table_update(page_num, &frame_num, page_table);
	
	if (replacementStrat == 1)	{
		TLB_replacement_FIFO(page_num, frame_num, tlb);
	}
	else {
		TLB_replacement_LRU(page_num, frame_num, tlb);
	}
}

void
page_table_update(page_t page_num, frame_t *frame_num, page_table_t *page_table)
{
	u_int i;
	for (i = 0; i < NUM_PAGES; i++)	{
		/*empty spot in page table*/
		if (page_table[i] < 0)	{
			page_table[i] = page_num;
			*frame_num = page_num;
			printf("page_table_update page num %d frame num %dd\n", page_num, frame_num);
			//printf("page table[%d] is//create_physical_address(frame_t frame_num, offset_t offset,
		}
	}
}
void
create_physical_address(frame_t frame_num, offset_t offset, paddress *physical_address)
{
	 *physical_address = frame_num << OFFSET_BITS | offset;
	//printf("physical address is %u\n", physical_address);	
}

int 
read_physical_memory(paddress p_address, physical_memory_t physical_mem,
                                value_t *value)
{
	
}

void
update_address_value_list(laddress l_address, paddress p_address,
                        value_t value, address_value_list_t *address_value_list)
{
	u_int i;
	for (i = 0; i < 256; i++)	{
		if (address_value_list[i].valid == false)	{
			address_value_list[i].logic_addr = l_address;
			address_value_list[i].physical_addr = p_address;
			address_value_list[i].value = value;
			address_value_list[i].valid = true;
			break;
		}
	}
/*
	i = 0;
	while(address_value_list[i].valid == true)	{
		printf("Virtual address: %d; Physical address: %d; Value: %d\n",
                                address_value_list[i].logic_addr,
                                address_value_list[i].physical_addr,
                                address_value_list[i].value);
                
		i++;
	}		*/
}
void 
output_address_value_list(const char *output_file_name,
                                address_value_list_t *address_value_list)
{
	FILE *fp;
	u_int i;
	 for (i = 0; i < 256; i++)       {
                if (address_value_list[i].valid == true)        {
                        printf("Virtual address: %d;\tPhysical address: %d;\tValue: %d\n",
                                address_value_list[i].logic_addr,
                                address_value_list[i].physical_addr,
                                address_value_list[i].value);
                }
        }	

	
	fp = fopen(output_file_name, "w");
	
	for (i = 0; i < 256; i++)	{
		if (address_value_list[i].valid == true)	{
			fprintf(fp, "Virtual address: %d;\tPhysical address: %d;\tValue: %d\n",
				address_value_list[i].logic_addr, 
				address_value_list[i].physical_addr,
				address_value_list[i].value);
		}	
	}
	fclose(fp);
}	
