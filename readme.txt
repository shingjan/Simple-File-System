Usage of Simple File System

1. gcc -o mkfs_t mkfs_t.c
2. gcc -o tshell shell.c 

Expecting output would be something like this:

In file included from tshell.c:10:
./open_t.h:144:9: warning: passing 'const char *' to parameter of type 'char *' discards qualifiers
      [-Wincompatible-pointer-types-discards-qualifiers]
        strcpy(pathname, path);
               ^~~~~~~~
/usr/include/secure/_string.h:83:27: note: expanded from macro 'strcpy'
  __builtin___strcpy_chk (dest, src, __darwin_obsz (dest))
                          ^~~~
In file included from tshell.c:10:
./open_t.h:239:1: warning: control may reach end of non-void function [-Wreturn-type]
}
^
2 warnings generated.

which means no error during the compilation of our system

3. Then put a HD binary file with a size of 110MB(required and tested) under the same directory of output executable file “mkfs_t” and “shell”

4. ./mkfs_t HD

The expecting output would be like this:
File system offset reset!.

5. ./tshell
The expecting output would be like this:
//-----------Simple file system--------------//
tshell### 
>ls_t
inode no	file type	size	name		date

and you can test all commands required in the documentation