#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#define NASM_PATH "/usr/bin/nasm"
#define LD_PATH "/usr/bin/ld"
#define NAME_FILE_OUTPUT "aled"

// Fonction pour assembler le fichier assembleur en fichier objet
int assemble_file(const char *source_file, const char *output_obj) {
	
	pid_t pid = fork();

	if (pid == 0) {
		execlp(NASM_PATH, "nasm", "-f", "elf64", "-o", output_obj, source_file, NULL);
		perror("Erreur lors de l'assemblage du fichier");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			fprintf(stderr, "Erreur: nasm a retourné un code de sortie %d\n", WEXITSTATUS(status));
			return 1;
		}
	} else {
		perror("Erreur de fork");
		return 1;
	}

	return 0;
}

// Fonction pour lier tous les fichiers objets en un seul exécutable
int link_files(char **input_objs, int num_objs) {

	pid_t pid = fork();

	if (pid == 0) {

		char *ld_args[num_objs + 6]; // ld, -m, elf_x86_64, -o, NAME_FILE_OUTPUT, NULL
		ld_args[0] = "ld";
		ld_args[1] = "-m";
		ld_args[2] = "elf_x86_64";
		ld_args[3] = "-o";
		ld_args[4] = NAME_FILE_OUTPUT;

		for (int i = 0; i < num_objs; i++) {
			ld_args[i + 5] = input_objs[i];
		}
		ld_args[num_objs + 5] = NULL;

		execvp(LD_PATH, ld_args);
		perror("Erreur lors du linkage des fichiers objets");
		exit(EXIT_FAILURE);
	} else if (pid > 0) {
		int status;
		waitpid(pid, &status, 0);
		if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
			fprintf(stderr, "Erreur: ld a retourné un code de sortie %d\n", WEXITSTATUS(status));
			return 1;
		}
	} else {
		perror("Erreur de fork");
		return 1;
	}

	return 0;
}

int main(int argc, char *argv[]) {
	
	if (argc < 2) {
		fprintf(stderr, "Usage: %s <fichier1.asm> [<fichier2.asm> ...]\n", argv[0]);
		return 1;
	}

	const int max_files = argc - 1;
	char *input_objs[max_files];
	int num_objs = 0;

	// Assembler chaque fichier source .asm en un fichier objet .o
	for (int i = 1; i < argc; i++) {
		
		char *source_file = argv[i];

		// Trouver la position du dernier '.' dans le nom du fichier source
		const char *dot = strrchr(source_file, '.');
		if (!dot || strcmp(dot, ".asm") != 0) {
			fprintf(stderr, "Erreur: Le fichier %s n'est pas un fichier .asm valide.\n", source_file);
			continue;
		}

		// Construire le nom du fichier objet en remplaçant l'extension .asm par .o
		char object_file[strlen(source_file) - 3 + 2];
		strncpy(object_file, source_file, dot - source_file);
		object_file[dot - source_file] = '\0';
		strcat(object_file, ".o");

		// Assembler le fichier source
		if (assemble_file(source_file, object_file) != 0) {
			fprintf(stderr, "Erreur lors de l'assemblage du fichier %s\n", source_file);
			continue;
		}

		input_objs[num_objs++] = strdup(object_file);
	}

	// Lier tous les fichiers objets en un seul exécutable
	if (link_files(input_objs, num_objs) != 0) {
		fprintf(stderr, "Erreur lors du linkage des fichiers objets en un exécutable\n");
		return 1;
	}

	// Libérer la mémoire
	for (int i = 0; i < num_objs; i++) {
		free(input_objs[i]);
	}

	return 0;
}
