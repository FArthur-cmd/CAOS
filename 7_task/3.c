#include <string.h>
#include <stdbool.h>

typedef enum {
    START_OF_TWO_DOTS = 2,
    START_FROM_DOT_AND_SLASH = 1,
    ONLY_TWO_DOTS_AND_SLASH = 3,
    REMOVE_TWO_DOTS_AND_LAST_SLASH = 4
} constants;

static inline bool
double_slash(char *path)
{
    return (*path == '/' && *(path + 1) == '/'); //check position and next
}

static inline void
ignore_one_symbol(size_t *index)
{
    ++(*index);
}

static inline bool
point_and_slash(char *path)
{
   return (*path == '.' && *(path + 1) == '/'); //check position and next
}

static inline void
remove_dot(size_t *current_position)
{
    --(*current_position);
}

static inline void
remove_two_dots(size_t *current_position)
{
    (*current_position) -= 2;
}

static inline bool
not_started_with_two_dots(size_t *current_position)
{
    return *current_position > START_OF_TWO_DOTS;
}

static inline bool
not_part_of_file(char *last_read)
{
    return *(last_read - 1) == '/';
}

static inline bool
two_dots_and_slash(char *last_read, char *read_now)
{
    return *last_read == '.' && *(last_read - 1) == '.' && *read_now == '/';
}

static inline void
find_begining_of_previous_part(char *path, size_t *current_position)
{
    while (*current_position > 0 && path[*current_position] != '/') {
        --(*current_position);
    }
}

static inline void
ignore_next_slash_if_not_from_root(char *path, size_t *copy_from, size_t *length_of_path)
{   	//if got to start ignore all / because we go not from root
     while (path[*copy_from] == '/' && *copy_from < *length_of_path) {
	 ++(*copy_from);
    }		    
}

static inline bool
check_for_some_slash(char *path, size_t *copy_from)
{
    if (double_slash(path + *copy_from - 1)) {              // if // ignore one /
	ignore_one_symbol(copy_from);
	return true;
    }
    return false;
}

static inline bool
check_for_point_and_slash(char *path, size_t *copy_from, size_t *current_position)
{
    if (point_and_slash(path + *copy_from - 1)) {           // if ./ we alredy put . 
        if (*copy_from == START_FROM_DOT_AND_SLASH ||
	    not_part_of_file(path + *current_position - 1)) {
	       ignore_one_symbol(copy_from);                  // go back and ignore /
	       remove_dot(current_position);
	       return true;
        }
    }
    return false;    
}

static inline bool
check_for_two_dots_and_slash(char *path, size_t *copy_from, 
		             size_t *current_position, size_t *length_of_path)
{
     if (not_started_with_two_dots(current_position) &&
             two_dots_and_slash(path + *current_position - 1, path + *copy_from) &&
             not_part_of_file(path + *current_position - 2)) {    
		// -2 because last was symbol before two dots
	if (*current_position == ONLY_TWO_DOTS_AND_SLASH){                          
		 //if from / just remove two dots
	     ignore_one_symbol(copy_from);
             remove_two_dots(current_position);
	     return true;
	} else {
	    ignore_one_symbol(copy_from);
	    *current_position -= REMOVE_TWO_DOTS_AND_LAST_SLASH; 
		
		//if in the middle of word, find previous / and start to put from it
                
	    find_begining_of_previous_part(path, current_position);
            if (*current_position == 0 && path[*current_position] != '/') {
		ignore_next_slash_if_not_from_root(path, copy_from, length_of_path);
            } else {
		++(*current_position);
            }
        }
    }
    return false;
}

void
normalize_path(char *path) {
    size_t current_position = 0;
    size_t copy_from = 0;
    size_t length_of_path = strlen(path);
    while(copy_from < length_of_path) {
        if (check_for_some_slash(path, &copy_from)) {
	    continue;
	}
	if (check_for_point_and_slash(path, &copy_from, &current_position)) {
	    continue;
	}
	if (check_for_two_dots_and_slash(path, &copy_from, 
		                         &current_position, &length_of_path)) {
	    continue;
	}
        path[current_position] = path[copy_from];
	++current_position;
	++copy_from;	
    }
    path[current_position] = '\0';
}
