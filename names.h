#ifndef names_h
#define names_h

#include <string>
#include <vector>

using namespace std;

typedef int name;
typedef string namestring;

const name blankname = -1;   /* special name                 */
const name dev_id = -50;
const name con_id = -51;
const name mon_id = -52;

class names{


private:
    vector<namestring> name_table;
    vector<namestring> keyword_table;

public:
  name lookup (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not already in the name table, it is          */
    /* automatically inserted.                                             */

  name cvtname (namestring str);
    /* Returns the internal representation of the name given in character  */
    /* form.  If the name is not in the name table then 'blankname' is     */
    /* returned.                                                           */

  void writename (name id);
    /* Prints out the given name on the console                            */

  int namelength (name id);
    /* Returns length ie number of characters in given name                */

  bool is_keyword (namestring str);

  names (void);
    /* names initialises the name table.  This procedure is called at      */
    /* system initialisation before any of the above procedures/functions  */
    /* are used.                                                           */
};

#endif /* names_h */
