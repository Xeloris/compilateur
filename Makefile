# Variables pour les fichiers
ASM_SRC		=	hello.asm
OBJ_DIR		=	obj
OBJ			=	$(OBJ_DIR)/hello.o
EXEC		=	hello

# Commandes pour nasm et ld
NASM		=	nasm
LD			=	ld

# Flags pour nasm et ld en 64 bits
NASM_FLAGS	=	-f elf64
LD_FLAGS	=

# Cible par défaut
all: $(EXEC)

# Règle pour lier l'exécutable
$(EXEC): $(OBJ)
	$(LD) $(LD_FLAGS) -o $(EXEC) $(OBJ)

# Règle pour assembler le fichier objet
$(OBJ): $(ASM_SRC) | $(OBJ_DIR)
	$(NASM) $(NASM_FLAGS) -o $(OBJ) $(ASM_SRC)

# Crée le répertoire pour les fichiers objets si nécessaire
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

# Cible pour nettoyer les fichiers objets
clean:
	rm -rf $(OBJ_DIR)

# Cible pour nettoyer les fichiers objets et l'exécutable
fclean: clean
	rm -f $(EXEC)
