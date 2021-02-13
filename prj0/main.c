#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include "bitmap.h"
#include "debug.h"
#include "hash.h"
#include "hex_dump.h"
#include "limits.h"
#include "list.h"
#include "round.h"

struct list list_arr[10];
struct hash hash_arr[10];
struct bitmap *bitmap_arr[10];

int get_num(char* idx){
	int num;
	num = idx[strlen(idx)-1] - '0';
	return num;
}

bool list_less_function(const struct list_elem* a, const struct list_elem* b, void *aux){
	if(list_entry(a, struct list_item, elem)->data < list_entry(b, struct list_item, elem)->data)
		return true;
	else
		return false;
}

unsigned hash_hash_function(const struct hash_elem* a, void *aux){
	struct hash_item* temp = hash_entry(a, struct hash_item, elem);
	return hash_int(temp->data);
}

bool hash_less_function(const struct hash_elem* a, const struct hash_elem* b, void* aux){
	if(hash_entry(a, struct hash_item, elem)->data < hash_entry(b, struct hash_item, elem)->data)
		return true;
	else
		return false;
}
void hash_action_destroy(struct hash_elem *a, void *aux){
	struct hash_item* temp_item;
	temp_item = hash_entry(a, struct hash_item, elem);
	free(temp_item);
}

void hash_action_square(struct hash_elem* a, void *aux){
	struct hash_item* temp_item;
	temp_item = hash_entry(a, struct hash_item, elem);
	temp_item -> data *= temp_item->data;
}

void hash_action_triple(struct hash_elem* a, void *aux){
	struct hash_item* temp_item;
	temp_item = hash_entry(a, struct hash_item, elem);
	temp_item -> data *= temp_item->data * temp_item->data;
}

int main(){
	char input[120] = {0, }, cmd1[20], cmd2[20], cmd3[20], cmd4[20], cmd5[20], cmd6[20];
	char temp_cmd[20];
	int listnum, hashnum, bmnum;
	int i;
	int count = 0;

	while(true){
		fgets(input, sizeof(input), stdin);
		count = sscanf(input, "%s%s%s%s%s%s", cmd1, cmd2, cmd3, cmd4, cmd5, cmd6);
		
		if(!strcmp(cmd1, "quit")){
			break;
		}
		else if(!strcmp(cmd1, "create")){
			if(!strcmp(cmd2, "list")){
				listnum = get_num(cmd3);
				list_init(&list_arr[listnum]);
			}
			else if(!strcmp(cmd2, "hashtable")){
				hashnum = get_num(cmd3);
				hash_init(&hash_arr[hashnum], hash_hash_function, hash_less_function, NULL);
			}
			else if(!strcmp(cmd2, "bitmap")){
				bmnum = get_num(cmd3);
				size_t size = (size_t)atoi(cmd4);
				bitmap_arr[bmnum] = bitmap_create(size);
			}
		}
		else if(!strcmp(cmd1, "delete")){
			strncpy(temp_cmd, cmd2, strlen(cmd2) - 1);
			if(!strcmp(temp_cmd, "list")){
				listnum = get_num(cmd2);
				for(i = 0; i < list_size(&list_arr[listnum]); i++){
					list_pop_front(&list_arr[listnum]);
				}
			}
			else if(!strcmp(temp_cmd, "hash")){
				hashnum = get_num(cmd2);
				hash_destroy(&hash_arr[hashnum], hash_action_destroy);
			}
			else if(!strcmp(temp_cmd, "bm")){
				bmnum = get_num(cmd2);
				bitmap_destroy(bitmap_arr[bmnum]);
			}
		}
		else if(!strcmp(cmd1, "dumpdata")){
			if(!strncmp(cmd2, "list", 4)){
				struct list_elem* temp_elem;
				struct list_item* temp_item;
				listnum = get_num(cmd2) ;
				
				for(temp_elem = list_begin(&list_arr[listnum]); temp_elem != list_end(&list_arr[listnum]); temp_elem = list_next(temp_elem)){
				temp_item = list_entry(temp_elem, struct list_item, elem);
				printf("%d ", temp_item->data);
				}
				if(!list_empty(&list_arr[listnum]))
					printf("\n");
			}
			else if(!strncmp(cmd2, "hash", 4)){
				struct hash_iterator it;
				struct hash_item* temp_item;
				int hashnum = get_num(cmd2);
				
				hash_first(&it, &hash_arr[hashnum]);
				while(hash_next(&it)){
					temp_item = hash_entry(hash_cur(&it), struct hash_item, elem);
					printf("%d ", temp_item->data);
				}
				if(!hash_empty(&hash_arr[hashnum]))
					printf("\n");
			}
			else if(!strncmp(cmd2, "bm", 2)){
				int bmnum = get_num(cmd2);
				size_t size = bitmap_size(bitmap_arr[bmnum]);
				for(int i = 0; i < size; i++){
					printf("%d", bitmap_test(bitmap_arr[bmnum], i));
				}
				printf("\n");
			}
		}
/*-------------------------------------list instructions-------------------------*/		
		else if(!strcmp(cmd1, "list_push_back")){
			int listnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct list_elem* new_elem=(struct list_elem*)malloc(sizeof(struct list_elem));
			struct list_item* temp_item = list_entry(new_elem, struct list_item, elem);
			temp_item->data = data;
			list_push_back(&list_arr[listnum], new_elem);
		}
		else if(!strcmp(cmd1, "list_push_front")){
			int listnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct list_elem* new_elem=(struct list_elem*)malloc(sizeof(struct list_elem));
			struct list_item* temp_item = list_entry(new_elem, struct list_item, elem);
			temp_item->data = data;
			list_push_front(&list_arr[listnum], new_elem);
		}
		else if(!strcmp(cmd1, "list_back")){
			int listnum = get_num(cmd2);
			struct list_elem* temp_elem = list_back(&list_arr[listnum]);
			struct list_item* temp_item = list_entry(temp_elem, struct list_item, elem);
			printf("%d\n", temp_item->data);
		}
		else if(!strcmp(cmd1, "list_front")){
			int listnum = get_num(cmd2);
			struct list_elem* temp_elem = list_front(&list_arr[listnum]);
			struct list_item* temp_item = list_entry(temp_elem, struct list_item, elem);
			printf("%d\n", temp_item->data);
		}
		else if(!strcmp(cmd1, "list_pop_back")){
			int listnum = get_num(cmd2);
			list_pop_back(&list_arr[listnum]);
		}
		else if(!strcmp(cmd1, "list_pop_front")){
			int listnum = get_num(cmd2);
			list_pop_front(&list_arr[listnum]);
		}
		else if(!strcmp(cmd1, "list_insert")){
			int listnum = get_num(cmd2);
			int pos = atoi(cmd3);
			int data = atoi(cmd4);

			struct list_elem* new_elem = (struct list_elem*)malloc(sizeof(struct list_elem));
			struct list_item* temp_item = list_entry(new_elem, struct list_item, elem);
			temp_item->data = data;

			struct list_elem* temp_elem = list_begin(&list_arr[listnum]);
			for(int i = 0; i < pos; i++)
				temp_elem = list_next(temp_elem);
			list_insert(temp_elem, new_elem);
		}
		else if(!strcmp(cmd1, "list_insert_ordered")){
			int listnum = get_num(cmd2);
			int data = atoi(cmd3);
			
			struct list_elem* new_elem = (struct list_elem*)malloc(sizeof(struct list_elem));
			struct list_item* temp_item = list_entry(new_elem, struct list_item, elem);
			temp_item->data = data;
			
			list_insert_ordered(&list_arr[listnum], new_elem, &list_less_function, NULL);
		}
		else if(!strcmp(cmd1, "list_empty")){
			int listnum = get_num(cmd2);
			
			if(list_empty(&list_arr[listnum]))
				printf("true\n");
			else
				printf("false\n");
		}
		else if(!strcmp(cmd1, "list_size")){
			int listnum = get_num(cmd2);
			int size = list_size(&list_arr[listnum]);

			printf("%d\n", size);
		} 
		else if(!strcmp(cmd1, "list_max")){
			int listnum = get_num(cmd2);
			struct list_elem* temp_elem = list_max(&list_arr[listnum], &list_less_function, NULL);
			struct list_item* temp_item = list_entry(temp_elem, struct list_item, elem);
			printf("%d\n", temp_item->data);
		}
		else if(!strcmp(cmd1, "list_min")){
			int listnum = get_num(cmd2);
			struct list_elem* temp_elem = list_min(&list_arr[listnum], &list_less_function, NULL);
			struct list_item* temp_item = list_entry(temp_elem, struct list_item, elem);
			printf("%d\n", temp_item->data);
		}
		else if(!strcmp(cmd1, "list_remove")){
			int listnum = get_num(cmd2);
			int pos = atoi(cmd3);
			struct list_elem* temp_elem = list_begin(&list_arr[listnum]);
			for(int i=0; i<pos; i++){
				temp_elem = list_next(temp_elem);
			}
			list_remove(temp_elem);
		}
		else if(!strcmp(cmd1, "list_reverse")){
			int listnum = get_num(cmd2);
			list_reverse(&list_arr[listnum]);
		}
		else if(!strcmp(cmd1, "list_shuffle")){
			int listnum = get_num(cmd2);
			list_shuffle(&list_arr[listnum]);
		}
		else if(!strcmp(cmd1, "list_sort")){
			int listnum = get_num(cmd2);
			list_sort(&list_arr[listnum], &list_less_function, NULL);
		}
		else if(!strcmp(cmd1, "list_splice")){
			int listnum1 = get_num(cmd2);
			int pos1 = atoi(cmd3);
			int listnum2 = get_num(cmd4);
			int pos2 = atoi(cmd5);
			int pos3 = atoi(cmd6);
			
			struct list_elem *elem1 = list_begin(&list_arr[listnum1]);
			for(int i = 0; i < pos1; i++)
				elem1 = list_next(elem1);
			struct list_elem *elem2 = list_begin(&list_arr[listnum2]);
			for(int i = 0; i < pos2; i++)
				elem2 = list_next(elem2);
			struct list_elem *elem3 = list_begin(&list_arr[listnum2]);
			for(int i = 0; i < pos3; i++)
				elem3 = list_next(elem3);
			list_splice(elem1, elem2, elem3);
		}
		else if(!strcmp(cmd1, "list_swap")){
			int listnum = get_num(cmd2);
			int pos1 = atoi(cmd3);
			int pos2 = atoi(cmd4);

			struct list_elem *elem1 = list_begin(&list_arr[listnum]);
			for(int i = 0; i < pos1; i++)
				elem1 = list_next(elem1);
			struct list_elem *elem2 = list_begin(&list_arr[listnum]);
			for(int i = 0; i < pos2; i++)
				elem2 = list_next(elem2);

			list_swap(elem1, elem2);
		}
		else if(!strcmp(cmd1, "list_unique")){
			int listnum1 = get_num(cmd2);

			if(count == 2)
				list_unique(&list_arr[listnum1], NULL, list_less_function, NULL);
			else if(count == 3){
				int listnum2 = get_num(cmd3);
				list_unique(&list_arr[listnum1], &list_arr[listnum2], list_less_function, NULL);
			}
		}
/*-------------------------------------hash instructions-------------------------*/		
		else if(!strcmp(cmd1, "hash_insert")){
			int hashnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct hash_elem* temp_elem = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item* temp_item = hash_entry(temp_elem, struct hash_item, elem);
			
			temp_item->data = data;
			hash_insert(&hash_arr[hashnum], temp_elem);
		}
		else if(!strcmp(cmd1, "hash_apply")){
			int hashnum = get_num(cmd2);
			if(!strcmp(cmd3, "square")){
				hash_apply(&hash_arr[hashnum], &hash_action_square);
			}
			else if(!strcmp(cmd3, "triple")){
				hash_apply(&hash_arr[hashnum], &hash_action_triple);
			}
		}
		else if(!strcmp(cmd1, "hash_delete")){
			int hashnum = get_num(cmd2);
			int data = atoi(cmd3);
			
			struct hash_elem* temp_elem = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item* temp_item = hash_entry(temp_elem, struct hash_item, elem);
			temp_item -> data = data;

			hash_delete(&hash_arr[hashnum], temp_elem);
		}
		else if(!strcmp(cmd1, "hash_empty")){
			int hashnum = get_num(cmd2);
			
			if(hash_empty(&hash_arr[hashnum]))
				printf("true\n");
			else
				printf("false\n");
		}
		else if(!strcmp(cmd1, "hash_size")){
			int hashnum = get_num(cmd2);
			printf("%zu\n", hash_size(&hash_arr[hashnum]));
		}
		else if(!strcmp(cmd1, "hash_clear")){
			int hashnum = get_num(cmd2);
			hash_clear(&hash_arr[hashnum], &hash_action_destroy);
		}
		else if(!strcmp(cmd1, "hash_find")){
			int hashnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct hash_elem* temp_elem = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item* temp_item = hash_entry(temp_elem, struct hash_item, elem);
			temp_item->data = data;

			if(hash_find(&hash_arr[hashnum], temp_elem) != NULL)
				printf("%d\n", data);
		}
		else if(!strcmp(cmd1, "hash_insert")){
			int hashnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct hash_elem* temp_elem = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item* temp_item = hash_entry(temp_elem, struct hash_item, elem);
			temp_item->data = data;
			
			hash_insert(&hash_arr[hashnum], temp_elem);
		}
		else if(!strcmp(cmd1, "hash_replace")){
			int hashnum = get_num(cmd2);
			int data = atoi(cmd3);

			struct hash_elem* temp_elem = (struct hash_elem *)malloc(sizeof(struct hash_elem));
			struct hash_item* temp_item = hash_entry(temp_elem, struct hash_item, elem);
			temp_item->data = data;
		
			hash_replace(&hash_arr[hashnum], temp_elem);
		}
/*-------------------------------------bitmap instructions-------------------------*/			
		else if(!strcmp(cmd1, "bitmap_mark")){
			int bmnum = get_num(cmd2);
			size_t idx = (size_t)atoi(cmd3);
			bitmap_mark(bitmap_arr[bmnum], idx);
		}
		else if(!strcmp(cmd1, "bitmap_all")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			
			if(bitmap_all(bitmap_arr[bmnum], start, cnt))
				printf("true\n");
			else
				printf("false\n");
		}
		else if(!strcmp(cmd1, "bitmap_any")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);

			if(bitmap_any(bitmap_arr[bmnum], start, cnt))
				printf("true\n");
			else
				printf("false\n");
		}
		else if(!strcmp(cmd1, "bitmap_contains")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			bool value = false;

			if(!strcmp(cmd5, "true"))
				value = true;

			if(bitmap_contains(bitmap_arr[bmnum], start, cnt, value))
				printf("true\n");
			else 
				printf("false\n");
		}
		else if(!strcmp(cmd1, "bitmap_count")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			bool value = false;

			if(!strcmp(cmd5, "true"))
				value = true;
			printf("%zu\n", bitmap_count(bitmap_arr[bmnum], start, cnt, value));
		}
		else if(!strcmp(cmd1, "bitmap_dump")){
			int bmnum = get_num(cmd2);
			bitmap_dump(bitmap_arr[bmnum]);
		}
		else if(!strcmp(cmd1, "bitmap_expand")){
			int bmnum = get_num(cmd2);
			int size = atoi(cmd3);

			bitmap_arr[bmnum] = bitmap_expand(bitmap_arr[bmnum], size);
		}
		else if(!strcmp(cmd1, "bitmap_set_all")){
			int bmnum = get_num(cmd2);
			bool value = false;

			if(!strcmp(cmd3, "true"))
				value = true;
			
			bitmap_set_all(bitmap_arr[bmnum], value);
		}
		else if(!strcmp(cmd1, "bitmap_flip")){
			int bmnum = get_num(cmd2);
			size_t idx = (size_t)atoi(cmd3);
			
			bitmap_flip(bitmap_arr[bmnum], idx);
		}
		else if(!strcmp(cmd1, "bitmap_none")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);

			if(bitmap_none(bitmap_arr[bmnum], start, cnt))
				printf("true\n");
			else
				printf("false\n");
		}
		else if(!strcmp(cmd1, "bitmap_reset")){
			int bmnum = get_num(cmd2);
			size_t idx = (size_t)atoi(cmd3);
			bitmap_reset(bitmap_arr[bmnum], idx);
		}
		else if(!strcmp(cmd1, "bitmap_scan_and_flip")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			bool value = false;

			if(!strcmp(cmd5, "true"))
				value = true;
			printf("%zu\n", bitmap_scan_and_flip(bitmap_arr[bmnum], start, cnt, value));
		}
		else if(!strcmp(cmd1, "bitmap_scan")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			bool value = false;

			if(!strcmp(cmd5, "true"))
				value = true;
			printf("%zu\n", bitmap_scan(bitmap_arr[bmnum], start, cnt, value));
		}
		else if(!strcmp(cmd1, "bitmap_set")){
			int bmnum = get_num(cmd2);
			size_t idx = (size_t)atoi(cmd3);
			bool value = false;

			if(!strcmp(cmd4, "true"))
				value = true;
			bitmap_set(bitmap_arr[bmnum], idx, value);				
		}
		else if(!strcmp(cmd1, "bitmap_set_multiple")){
			int bmnum = get_num(cmd2);
			size_t start = (size_t)atoi(cmd3);
			size_t cnt = (size_t)atoi(cmd4);
			bool value = false;

			if(!strcmp(cmd5, "true"))
				value = true;
			bitmap_set_multiple(bitmap_arr[bmnum], start, cnt, value);
		}
		else if(!strcmp(cmd1, "bitmap_size")){
			int bmnum = get_num(cmd2);
			printf("%zu\n", bitmap_size(bitmap_arr[bmnum]));
		}
		else if(!strcmp(cmd1, "bitmap_test")){
			int bmnum = get_num(cmd2);
			size_t idx = (size_t)atoi(cmd3);
			if(bitmap_test(bitmap_arr[bmnum], idx))
				printf("true\n");
			else
				printf("false\n");
		}
	}
		

	return 0;
}
