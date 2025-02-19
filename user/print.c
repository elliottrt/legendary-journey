
extern int printf(const char *format, ...);

int main(int argc, char **argv) {
	printf("Hello, World!\n");

	printf("%d args:", argc);
	for (int i = 0; i < argc; i++)
		printf(" %s", argv[i]);

	printf("\n");
	return 0;
}
