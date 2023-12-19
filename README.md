# How to use
./server [PORT]
nc localhost [PORT]
# COMMANDS
SET [KEY] [VALUE] : set the value for a key.
RENAME [KEY] [NEWKEY] : rename a key.
APPEND [KEY] [VALUE_TOAPPEND] : append content to a value.
SETNX [KEY] [VALUE] : set the value if the key doesnt exist already.
GET [KEY] : get a key's values.
EXISTS [KEY] : check whether or not a key exists.
DEL [KEY] : delete a key.
STRLEN [KEY] : return the length of a value.
PING/ECHO [MESSAGE] 
SAVE [FILENAME] : save the databse in a json  file.
LOAD [FILENAME] : load a json file into the database.
RANDKEY : returns a random key from the database.
KEYS : returns all keys in the database.
VALUES : returns all values in the database
