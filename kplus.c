#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[])
{
	if (argc < 2)
	{
		printf("Usage: %s <source file> [-64 | -32 | -bin] [--keep-asm] [-o <output_name>]\n", argv[0]);
		return 1;
	}

	const char *nasm_format = "elf64";	// Format par défaut
	const char *output_name = "output"; // Nom de sortie par défaut
	int keep_asm = 0;

	for (int i = 2; i < argc; ++i)
	{
		if (strcmp(argv[i], "-64") == 0)
		{
			nasm_format = "elf64";
		}
		else if (strcmp(argv[i], "-32") == 0)
		{
			nasm_format = "elf32";
		}
		else if (strcmp(argv[i], "-bin") == 0)
		{
			nasm_format = "bin";
		}
		else if (strcmp(argv[i], "--keep-asm") == 0)
		{
			keep_asm = 1;
		}
		else if (strcmp(argv[i], "-o") == 0)
		{
			if (i + 1 < argc)
			{
				output_name = argv[i + 1];
				i++;
			}
			else
			{
				fprintf(stderr, "Error: -o requires an output file name\n");
				return 1;
			}
		}
	}

	FILE *source_file = fopen(argv[1], "r");
	if (!source_file)
	{
		perror("Could not open source file");
		return 1;
	}

	FILE *asm_file = fopen("temp.asm", "w");
	if (!asm_file)
	{
		perror("Could not open asm file");
		return 1;
	}

	// fprintf(asm_file, "section .text\n");

	char line[256];
	char func_name[256];
	int in_func = 0;

	while (fgets(line, sizeof(line), source_file))
	{
		char *line_start = line + strspn(line, " \t");

		if (strstr(line_start, "\\bits 64") == line_start)
		{
			fprintf(asm_file, "bits 64\n");
		}
		else if (strstr(line_start, "\\bits 32") == line_start)
		{
			fprintf(asm_file, "bits 32\n");
		}
		else if (strstr(line_start, "\\section text") == line_start)
		{
			fprintf(asm_file, "section .text\n");
		}
		else if (strstr(line_start, "\\section data") == line_start)
		{
			fprintf(asm_file, "section .data\n");
		}
		else if (strstr(line_start, "\\section bss") == line_start)
		{
			fprintf(asm_file, "section .bss\n");
		}
		else if (strstr(line_start, "func ") == line_start)
		{
			sscanf(line_start, "func %[^[]", func_name);
			fprintf(asm_file, "global %s\n", func_name);
			fprintf(asm_file, "%s:\n", func_name);
			in_func = 1;
		}
		else if (in_func && strstr(line_start, "return") == line_start)
		{
			fprintf(asm_file, "    ret\n");
		}
		else if (in_func && strstr(line_start, "]") == line_start)
		{
			in_func = 0;
		}
	}

	fclose(source_file);
	fclose(asm_file);

	char nasm_command[256];
	if (strcmp(nasm_format, "bin") == 0)
	{
		sprintf(nasm_command, "nasm -f %s temp.asm -o %s", nasm_format, output_name);
	}
	else
	{
		sprintf(nasm_command, "nasm -f %s temp.asm -o temp.o", nasm_format);
	}
	system(nasm_command);

	if (strcmp(nasm_format, "bin") != 0)
	{
		char ld_command[256];
		if (strcmp(nasm_format, "elf32") == 0)
		{
			sprintf(ld_command, "ld -m elf_i386 temp.o -o %s", output_name);
		}
		else
		{
			sprintf(ld_command, "ld temp.o -o %s", output_name);
		}
		system(ld_command);
		system("rm temp.o");
	}

	if (keep_asm)
	{
		system("mv temp.asm output.asm");
	}
	else
	{
		system("rm temp.asm");
	}

	return 0;
}