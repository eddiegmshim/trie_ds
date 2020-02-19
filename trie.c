#include <stdio.h>
#include <stdlib.h>
#include "trie.h"
#include <string.h>

// typedef struct trie_data_t * trie_t;
// typedef struct trie_node_t * trie_pos_t;

// The structure representing the trie
struct trie_data_t 
{
	trie_pos_t first_node;
	unsigned int num_keys;
};

// A structure representing a trie node
struct trie_node_t
{
	char key;
	void * data;
	unsigned int num_children;

	trie_pos_t first_child;	
	trie_pos_t sibling;
	trie_pos_t parent;
};

// Create a new empty trie
trie_t trie_new()
{
	trie_t t = (trie_t) malloc(sizeof(*t));

	t -> first_node = (trie_pos_t) malloc(sizeof(*(t->first_node)));
	t -> first_node -> key = '\0';
	t -> first_node -> data = NULL;
	// t -> first_node -> num_children = 0;
	t -> first_node -> first_child = NULL;
	t -> first_node -> sibling = NULL;
	t -> first_node -> parent = NULL;
	t -> num_keys = 0;

	return t;
}

/// Free trie
/// If freefunc is not NULL, calls freefunc for every void * value
/// associated with a key.
void trie_destroy(trie_t trie, trie_free_t freefunc)
{
	void recursive_destroy(trie_pos_t node, trie_free_t freefunc);
	trie_pos_t current = trie -> first_node;
	trie_pos_t next = current -> first_child;	

	recursive_destroy(next, freefunc);

	// if(freefunc != NULL && current -> data != NULL) freefunc(current -> data);	
	free(current);
	free(trie);	
}

void recursive_destroy(trie_pos_t node, trie_free_t freefunc)
{
	if(node == NULL) return;

	trie_pos_t first_child = node -> first_child;
	trie_pos_t sibling = node -> sibling;
	void * data = node -> data;

	if(first_child == NULL && sibling == NULL)
	{
		// printf("TEST0 %c\n", node -> key);
		if(freefunc != NULL && data != NULL) freefunc(data);
		free(node);
	}
	else if(first_child == NULL)
	{
		recursive_destroy(node -> sibling, freefunc);
		// printf("TEST1 %c\n", node -> key);
		if(freefunc != NULL && data != NULL) freefunc(data);
		free(node);
	}
	else if(sibling == NULL)
	{
		recursive_destroy(node -> first_child, freefunc);
		// printf("TEST2 %c\n", node -> key);
		if(freefunc != NULL && data != NULL) freefunc(data);
		free(node);
	}
	else
	{
		recursive_destroy(node -> sibling, freefunc);
		recursive_destroy(node -> first_child, freefunc);
		// printf("TEST3 %c\n", node -> key);
		if(freefunc != NULL && data != NULL) freefunc(data);
		free(node);
	}
}

/// Visit every key in the trie
///   Calls walkfunc for every key
///   - If walkfunc returns true, the tree walking continues;
///   - If walkfunc returns false, the tree walking stops immediately
/// 
/// priv is passed to the walking function; trie_walk does not use it.
///
/// Returns true if the walkfunc never returned false or if there were
/// no keys in the trie
///
/// Returns false if the walkfunc returned false.
///
bool trie_walk (trie_t trie, trie_walk_t walkfunc, void * priv)
{
	void recursive_walk(trie_pos_t node, trie_walk_t walkfunc, void * priv, trie_t trie, bool * walk_condition, char * key_full, int size);
	trie_pos_t current = trie -> first_node;
	unsigned int num_keys = trie -> num_keys;
	if(num_keys == 0) return true;
	bool * walk_condition; // condition which tells us if we hit false on walkfunc
	bool initial_condition = true;
	walk_condition = &initial_condition;
	int size = 0;

	char * key_full = malloc(size);
	
	recursive_walk(current, walkfunc, priv, trie, walk_condition, key_full, size);

	return *walk_condition;
}

void recursive_walk(trie_pos_t node, trie_walk_t walkfunc, void * priv, trie_t trie, bool *walk_condition, char * key_full, int size)
{
	if(*walk_condition == false) return;
	if(node == NULL) return;

	trie_pos_t first_child = node -> first_child;
	trie_pos_t sibling = node -> sibling;
	void * data = node -> data;
	char key;
	char * temp = malloc(size + 1);
	strcpy(temp, key_full);
	char * key_temp = malloc(size);

	for(int i = 0; i < size; i ++)
	{
		key_temp[i] = temp[i];
	}
	
	if(first_child == NULL && sibling == NULL)
	{
		if(data != NULL)
		{
			// printf("testing key_temp1: %s\n", key_temp);
			*walk_condition = walkfunc(trie, node, key_temp, priv); 
			free(temp);
			free(key_temp);
			return;
		}
		
	}
	else if(first_child == NULL)
	{
		if(data != NULL)
		{
			// printf("testing key_temp2: %s\n", key_temp);
			*walk_condition = walkfunc(trie, node, key_temp, priv);
		}
		//go back one character and replace first_child's key with sibling's key
		key = node -> sibling -> key;
		key_full[size-1] = key;

		recursive_walk(node -> sibling, walkfunc, priv, trie, walk_condition, key_full, size);
		free(temp);
		free(key_temp);
		return;
		
	}
	else if(sibling == NULL)
	{
		if(data != NULL)
		{
			// printf("testing key_temp3: %s\n", key_temp);
			*walk_condition = walkfunc(trie, node, key_temp, priv);			
		}
		key = node -> first_child -> key;
		// char * temp = malloc(size + 1);
		// strcpy(temp, key_full);
		temp[size] = key;
		size++;
		
		free(key_temp);
		char * key_temp = malloc(size);

		for(int i = 0; i < size; i ++)
		{
			key_temp[i] = temp[i];
		}

		// printf("testing key_temp30: %s\n", temp);
		recursive_walk(node -> first_child, walkfunc, priv, trie, walk_condition, temp, size);
		// free(temp);
		// free(key_temp);
		return;
		
	}
	else
	{
		if(data != NULL)
		{
			// printf("testing key_temp4: %s\n", key_temp);
			*walk_condition = walkfunc(trie, node, key_temp, priv);			
		}
		key = node -> first_child -> key;
		// char * temp = malloc(size + 1);
		// strcpy(temp, key_full);
		temp[size] = key;
		int size2 = size + 1;
		
		free(key_temp);
		char * key_temp = malloc(size2);
		for(int i = 0; i < size2; i ++)
		{
			key_temp[i] = temp[i];
		}

		//go back one character and replace first_child's key with sibling's key
		key = node -> sibling -> key;
		key_full[size-1] = key;


		recursive_walk(node -> sibling, walkfunc, priv, trie, walk_condition, key_full, size);
		recursive_walk(node -> first_child, walkfunc, priv, trie, walk_condition, temp, size2);
		free(temp);
		free(key_temp);		
		return;
		
	}
	free(temp);
	free(key_temp);	
}


/// Get value associated with a key
/// NOTE: the pos was obtained by a call to trie_insert or trie_find
void * trie_get_value (const trie_t trie, trie_pos_t pos)
{
	return pos -> data;
}

/// Set value associated with a key
/// NOTE: the pos was obtained by a call to trie_insert or trie_find.
void trie_set_value (trie_t trie, trie_pos_t pos, void * value)
{
	pos -> data = value;
}

/// Return the number of keys in the trie
unsigned int trie_size (const trie_t trie)
{
	return trie -> num_keys;
}


/// Insert a key in the trie; 
///
///  Returns true if a new key was inserted, in which case the data 
///  value associated with the new key is set to newval;
///
///  Returns false if the key already existed, in which case
///  its data value IS NOT UPDATED.
///
///  In both cases, newpos is set to the position of the new (or existing)
///  key in the trie, *provided* newpos is not NULL.
///
///  Note: 
///  A position only remains valid until the next change (insertion or removal)
///  in the trie.
/// 
bool trie_insert (trie_t trie, const char * str, void * newval, trie_pos_t * newpos)
{
	trie_pos_t current = trie -> first_node;
	trie_pos_t first_child;
	trie_pos_t temp;	
	bool sibling_check;
	// bool is_new_key = false;

	for(int i = 0; i < strlen(str); i++)
	{
		// top:
		// printf("check: %c\n", str[i]);
		first_child = current -> first_child;
		char c = str[i];

		if(first_child == NULL)
		{
			// printf("check2: %c\n", str[i]);
			//insert in new node with character as key
			temp = (trie_pos_t) malloc(sizeof(*temp));
			temp -> key = c;
			temp -> first_child = NULL;
			temp -> sibling = NULL;
			temp -> data = NULL;

			//set current, representing parent, as first_child for next iteration
			current -> first_child = temp;
			current = current -> first_child;
			// is_new_key = true;
		}
		else // first child exists
		{
			if(first_child -> key == c)
			{
				//set current, representing parent, as first_child for next iteration
				current = current -> first_child;
			}

			// look for key in siblings
			else
			{	
				current = first_child;
				sibling_check = false;
				while(current -> sibling != NULL)
				{
					if(current -> sibling -> key == c) //key found amongst siblings
					{
						current = current -> sibling;
						sibling_check = true;
					}
					//loop through siblings
					if(sibling_check) break;
					else current = current -> sibling;
				}

				if(!sibling_check) //key not found in siblings
				{
					//initialize new sibling if key not found at this height
					temp = (trie_pos_t) malloc(sizeof(*temp));
					temp -> key = c;
					temp -> sibling = NULL;
					temp -> first_child = NULL;
					temp -> data = NULL;

					current -> sibling = temp;
					current = current -> sibling;
					// is_new_key = true;
				}
			}
		}	
	}

	//set newpos
	if(newpos != NULL)
	{
		*newpos = current;
	}

	//set data and return bool value if new key was inserted
	if(current -> data == NULL) 
	{
		(trie -> num_keys)++;
		current -> data = newval;
		return true;
	}

	return false;
}

/// Find a key in a trie
/// Returns the position or TRIE_INVALID_POS if the key could not be found.
trie_pos_t trie_find (const trie_t trie, const char * key)
{
	trie_pos_t current = trie -> first_node;
	trie_pos_t first_child;
	bool sibling_check = false;

	if(trie == NULL || current == NULL) return TRIE_INVALID_POS;

	for(int i = 0; i < strlen(key); i++)
	{		
		first_child = current -> first_child;
		char c = key[i];

		if(first_child == NULL) 
		{
			return TRIE_INVALID_POS;
		}		
		else // first child exists
		{
			if(first_child -> key == c)
			{
				current = first_child;
				// printf("\nFound first child %c,%c\n", c, first_child -> key);
			}

			// look for key in siblings
			else
			{	
				current = first_child;
				sibling_check = false;
				while(current -> sibling != NULL)
				{
					if((current -> sibling -> key) == c) //key found amonst siblings
					{
						current = current -> sibling;
						sibling_check = true;
					}
					//loop through siblings
					if(sibling_check) break;
					else current = current -> sibling;
				}
				
				if(!sibling_check) 
				{
					// printf("NOT FOUND IN SIBLINGS\n");
					return TRIE_INVALID_POS; //key not found in siblings
				}
			}
		}	
	}
	return current;
}

/// Remove a key from a trie
/// Returns false if the key could not be found
///
/// Returns true if the key was removed, and -- if data is not NULL -- 
/// sets *data to the data value
/// associated with the key (so it can be properly disposed of by the user,
/// if needed).
///
bool trie_remove (trie_t trie, const char * key, void ** data)
{	
	// printf("beggining of remove: %s\n", key);
	void trie_remove_helper(trie_pos_t current, const char * key, trie_t trie, void ** data);
	trie_pos_t target = trie_find(trie, key);


	if(target == TRIE_INVALID_POS || target-> data == NULL)
	{
		// printf("data null case\n");
		return false;
	}	
	if(data != NULL) *data = target -> data;

	//if node is a leaf, check to make sure we clean parent nodes not needee
	if(target -> first_child == NULL && target -> sibling == NULL)
	{
		// printf("data AVAILABLE CASE\n");
		trie_remove_helper(target, key, trie, data);		
	}

	//if node is not a leaf, just set its data to null
	else if(target -> first_child != NULL || target -> sibling != NULL)
	{
		target -> data = NULL;
	}

	(trie -> num_keys)--;
	return true;
}

void trie_remove_helper(trie_pos_t target, const char * key, trie_t trie, void ** data)
{
	char * key_prior = malloc(strlen(key));
	strcpy(key_prior, key);
	key_prior[strlen(key_prior)-1] = 0;
	trie_pos_t target_parent = trie_find(trie, key_prior);

	// printf("\nkey:%s\n", keyqq	// printf("key_prior:%s\n", key_prior);
	// printf("key_single_prior:%c\n", target_parent -> key);

	if(target_parent == TRIE_INVALID_POS ||
		target_parent -> key == '\0' ||
		target_parent -> data != NULL ||
		target_parent -> sibling != NULL ||
		target_parent -> first_child != NULL
		)
	{
		// printf("condition hit\n");
		if(target_parent -> first_child == target)
		{
			// printf("condition hit2\n");
			target_parent -> first_child = NULL;
			target -> data = NULL;
			target -> first_child = NULL;
			target -> sibling = NULL;			
			free(target);
		}
		else if(target_parent -> sibling == target)	
		{
			// printf("condition hit3\n");
			target_parent -> sibling = NULL;
			target -> data = NULL;
			target -> first_child = NULL;
			target -> sibling = NULL;
			free(target);
		}
		free(key_prior);
		return;
	}
	// else
	// {	
	// printf("condition hit4, %c\n", target->key);
		trie_remove_helper(target_parent, key_prior, trie, data);
		target -> data = NULL;
		target -> first_child = NULL;
		target -> sibling = NULL;
		free(target);
		free(key_prior);
		return;
	// }
	
	
}
