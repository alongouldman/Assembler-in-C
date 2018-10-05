#include "assembler.h"

/* this function adds a lable into the lable table, while doing validation chacks */
void add2lable_table(lablep *head, Token *lable, int status)
{
	lablep node = (lablep)malloc(sizeof(lable_t));
	lablep curr;

	if (!node)
	{
		print_error(DONT_PRINT_NUMBER, "Problem allocating memory");
		return;
	}
	if ((curr = find_lable(*head, lable)) == NULL) /* this is a new lable to add */
	{
		/* creating the node */
		strcpy(node->name, lable->data.lable_name);
		node->declaration = NOT_DECLARED;
		node->data_or_instruction = NOT_DECLARED;
		node->entry_extern = NOT_DECLARED;
		node->occurrence = NULL; /* this is a linked list inside a linked list */

		switch (status) /* which type of node to creat */
		{
			case CODE_LABLE_DECLARATION:
				node->declaration = IC;
				break;
			case CODE_LABLE:
				if (!add_lable_occurrence(&node->occurrence, IC)) return; /* adds the line to the list of occorances of this lable */
				break;
			case DATA_LABLE:
				node->declaration = DC;
				node->data_or_instruction = DATA_LABLE;
				break;
			case ENTRY_LABLE:
			case EXTERN_LABLE:
				node->entry_extern = status;
				break;
		}
		/* linking to the head of the list */
		if (*head == NULL) {
			node->next = NULL;
		}
		else {
			node->next = *head;
		}
		*head = node;
		return;
	}
	else /* this lable exists */
	{
		free(node); /* no need for a new lable */
		switch (status) {
			case CODE_LABLE_DECLARATION:
				if ((curr->declaration != NOT_DECLARED) || (curr->entry_extern == EXTERN_LABLE))
				{
					print_error(PRINT_NUMBER, "lable declared twice");
					return;
				}
				else /* add declaration */
				{
					curr->declaration = IC;
					curr->data_or_instruction = CODE_LABLE;
				}
				break;
			case CODE_LABLE:
				if (!add_lable_occurrence(&curr->occurrence, IC)) return; /* adds the line to the list of occorances of this lable */
				break;
			case DATA_LABLE:
				if ((curr->declaration != NOT_DECLARED) || (curr->entry_extern == EXTERN_LABLE))
				{
					print_error(PRINT_NUMBER, "lable declared twice");
					return;
				}
				else
				{
					curr->declaration = DC;
					curr->data_or_instruction = DATA_LABLE;
				}
				break;
			case ENTRY_LABLE:
				if (curr->entry_extern != NOT_DECLARED)
				{
					print_error(PRINT_NUMBER, "lable declared twice");
					return;
				}
				else
				{
					curr->entry_extern = status; /* ENTRY_LABLE */
				}
				break;
			case EXTERN_LABLE:
				if ((curr->declaration != NOT_DECLARED) || (curr->entry_extern != NOT_DECLARED))
				{
					print_error(PRINT_NUMBER, "lable declared twice");
					return;
				}
				else
				{
					curr->entry_extern = status; /* EXTERN_LABLE */
				}
				break;
		}
	}
}

/* this function adds a new lable occurrence to a lable node */
int add_lable_occurrence(occp *head, int line)
{
	occp node = (occp)malloc(sizeof(lable_occur));
	if (node == NULL)
	{
        print_error(DONT_PRINT_NUMBER,"Problem allocating memory");
		return 0;
	}
	node->line = line;
	if (*head == NULL) { /* no nodes in the list */
		node->next = NULL;
	}
	else
	{
		node->next = *head;
	}
	*head = node;
	return 1;
}

/* this function finds if a lable is already in the list, or is it a new lable.
if a lable is found, a pointer to the lable returns.
if no lable was found, NULL will return  */
lablep find_lable(lablep head, Token *lable)
{
	lablep curr = head;
	while (curr) /* going through the linked list */
	{
		if (strcmp(curr->name, lable->data.lable_name) == 0)
		{
			return curr;
		}
		curr = curr->next;
	}
	return NULL;
}


/* this function frees the memory allocated to a lable list */
void freelist(lablep *head)
{
	lablep curr;
	if (*head != NULL)
	{
		curr = (*head)->next;
		for (; curr; curr = curr->next)
		{
			free_occur_list(&(curr->occurrence));
			free(*head);
			*head = curr;
		}
		free(*head);
	}
}

/* frees the memory of the occurance list in each node */
void free_occur_list(occp *head)
{
	occp curr;
	if (*head != NULL)
	{
		curr = (*head)->next;
		for (; curr; curr = curr->next) {
			free(*head);
			*head = curr;
		}
		free(*head);
	}
}

/* this function goes over the lable list. if the lable is leagal, it will be added to the memory arrays,
in the proper position */
void parse_lables()
{
	lablep curr;
	curr = lable_list;
	while (curr) /* not reaching the end of the list */
	{
		/* validation checks */
		if ((curr->entry_extern == ENTRY_LABLE) && (curr->declaration == NOT_DECLARED))
		{
			print_error(DONT_PRINT_NUMBER, "lable %s not declared", curr->name);
			curr = curr->next;
			continue;
		}
		else if (curr->declaration == NOT_DECLARED)
        {
            if((curr->entry_extern != EXTERN_LABLE) && (curr->occurrence != NULL))
            {
                print_error(DONT_PRINT_NUMBER, "lable %s used without declaration", curr->name);
				curr = curr->next;
				continue;
            }
		}

		/*adding to the instruction array's */
		if (curr->entry_extern == EXTERN_LABLE) /* external lable, will add 'E' in the ERA part of the memory word */
		{
			lable2array(EXTERNAL, curr->occurrence);
		}
		else if (curr->declaration != NOT_DECLARED) /* all other lables */
		{
			if (curr->data_or_instruction == DATA_LABLE)
			{
				lable2array(((FIRST_MEMORY_ADDRESS + (curr->declaration) + IC) << DEST_ADDRESS) | RELOCATABLE , curr->occurrence);
			}
			else /* instruction lable, in the code part */
			{
				lable2array(((FIRST_MEMORY_ADDRESS + (curr->declaration)) << DEST_ADDRESS) | RELOCATABLE, curr->occurrence);
			}
		}
		curr = curr->next;
	}
}

/* this function adds number into the array, in all of the lable occurrences */
void lable2array(int number, occp occurrence_list)
{
	occp curr = occurrence_list;
	while (curr)
	{
		instruction_array[curr->line] |= number;
		curr = curr->next;
	}
}

/* checks if a lable has a valid syntax */
int is_valid_lable(char *tok)
{
	int i;
	/* lable validation: */
	for (i = 0; i< strlen(tok); i++) /* checking every character in the lable */
	{
		if (!isalnum(tok[i])) /* a lable name must be all letters or numbers */
		{
			return 0; /* error */
		}
	}
	if (!isalpha(tok[0])) /* lable name must begin with a letter */
	{
		return 0; /* error */
	}
	return 1;
}
