MultilayerINI
=========
The library defines a multi-layer nested configuration file format (called MINI, similar to XML), and provides the use of this format for file read and write mechanism.

The MINI format string consists of two parts, each of which is `layer` and `variable`. `variable` are stored in a `layer`. `layer` can be nested. In particular, the variable in the MINI format string after a layer name is called `parameter`, and its read and write variables use different functions with `variable`.

class MINI
-----------
This class is the core of the library. Its consists of MINI format string and its corresponding parse tree two parts of data.
### Constructor
You can use the parse tree or MINI format string one of the two formats to construct, will not automatically generate another after the construction.
* `MINI(String content,bool ispath=true)` Create a MINI class instance by reading the MINI file(`ispath=true`) or reading the MINI format string directly(`ispath=false`).
* `MINI(Tree *tree)` Create a MINI class instance from an existing MINI parse tree.
### Basic operation
* `void setCode(String code)` Resets the MINI format string corresponding to the instance(will not parse directly).
* `Tree* toTree()` Parse the current MINI format string to generate the parse tree.
* `Tree* getTree()` Returns the current parse tree.
* `String toCode(String path=NULL_String)` Generates a MINI format string from the current parse tree. If the parameter is not `NULL_String`, the resulting MINI format string is written to its corresponding file.
* `String getCode(String path=NULL_String)` Returns the current MINI format string. If the parameter is not `NULL_String`, the current MINI format string is written to its corresponding file.
* `MINIsta getState()` Returns the currently constructed data type. `TREE` only constructs the parse tree. `CODE` only constructs the MINI format string. `ALL` shows that both have been constructed.
* `void format()` Reconstructs the parse tree according to the current MINI format string, and formats the string.
### Read
The following function reads the content from the MINI format string, which is slower:
* `String readVar(vector<String>layer,String var)` Reads the variable value at the specified position. `vector<String>layer` represents the nested layer, vector¡¯s first element represents the outermost layer to which the variable belongs, the second element represents the secondary outer layer, and the last element represents the layer to which the variable belongs directly. `String var` is the name of the variable to be read.
* `String getBlockCode(vector<String>layer)` Get the MINI format string that is inside a layer.
* `String readPar(vector<String>layer,String par)` Reads the parameter value at the specified position.
The following function reads content directly from the parse tree and is faster:
* `String readVarInTree(vector<String>layer,String var)` Reads the variable value at the specified position.
* `String readParInTree(vector<String>layer,String par)`  Reads the parameter value at the specified position.
### Write
The following function reads the content from the MINI format string. If you want to operate on the tree, use the `class Tree` method.
* `String writeVar(vector<String>layer, String var, String val, String path=NULL_String)` Writes a variable to a specified location(If it does not exist, will be added).
* `String writePar(vector<String>layer, String var, String val, String path=NULL_String)` Writes a parameter to a specified location(If it does not exist, will be added).

class Tree
----------
Parse of MINI format string generated parse tree, you can use its MINI for more complex operations. Each Tree instance corresponds to a layer(the Tree pointer returned by the MINI instance corresponds to the top layer of the MINI format string), and its child layer is linked by `subtree`. If you want to find a deep sub-layer, use static function `searchSubTree`.
* `void addField(variable var)` Add a variable to the layer corresponding to the current instance.
* `void deleteField(String var)` Delete a variable to the layer corresponding to the current instance.
* `void addPar(variable var)` Add a parameter to the layer corresponding to the current instance.
* `void deletePar(String var)` Delete a parameter to the layer corresponding to the current instance.
* `void addChildNode(Tree *tree)` Add a sub-layer to the layer corresponding to the current instance.
* `void deleteTree(Tree *tree)` Delete a sub-layer to the layer corresponding to the current instance.
* `Tree* getFather()` Get the parent-layer of the layer corresponding to the current instance.
* `static Tree* searchSubTree(Tree *nowTree, vector<String> &layer)
` Get an instance of a deep sub-layer of the current instance(If not found, return `nullptr`).
* `String readVar(String var)` Get the value of a variable at the layer corresponding to the current instance.
* `String readPar(String var)`  Get the value of a parameter at the layer corresponding to the current instance.
* `String getCode(String tab=NULL_String)` Generates the MINI format string that the layer corresponding to the current instance and its sub-layer. The parameter is the stack of tabs that should be present in this layer. If the top layer, do not need to specify the parameters.
