# How to use
./server [PORT] <br />
nc localhost [PORT]<br />
# COMMANDS
SET [KEY] [VALUE] : set the value for a key.<br />
RENAME [KEY] [NEWKEY] : rename a key.<br />
APPEND [KEY] [VALUE_TOAPPEND] : append content to a value.<br />
SETNX [KEY] [VALUE] : set the value if the key doesnt exist already.<br />
GET [KEY] : get a key's values.<br />
EXISTS [KEY] : check whether or not a key exists. <br />
DEL [KEY] : delete a key. <br />
STRLEN [KEY] : return the length of a value. <br />
PING/ECHO [MESSAGE] <br />
SAVE [FILENAME] : save the databse in a json  file. <br />
LOAD [FILENAME] : load a json file into the database. <br />
RANDKEY : returns a random key from the database. <br />
KEYS : returns all keys in the database. <br />
VALUES : returns all values in the database. <br />
