
extern int printf(const char *format, ...);

int main(int argc, char **argv) {
	printf("Hello, World!\n");

	// TODO: all strings are the first string. we need to patch R_386_32 better.
	printf("%d args:", argc);
	for (int i = 0; i < argc; i++)
		printf(" %s", argv[i]);

	printf("\n");
	return 0;
}
