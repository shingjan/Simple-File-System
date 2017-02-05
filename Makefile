SFSmake: mkfs_t.c tshell.c
	gcc -o mkfs_t mkfs_t.c
	gcc -o tshell tshell.c -I.
clean:
	rm -rf mkfs_t tshell
