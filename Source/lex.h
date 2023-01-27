# ifndef LEX_H
# define LEX_H

#include <string>

using std::string;

#define BUFFER_SIZE 4096
#define find_max(x,y) (((x) >= (y)) ? (x) : (y))
#define find_min(x,y) (((x) <= (y)) ? (x) : (y))

void lex_file(string file_path);


#endif