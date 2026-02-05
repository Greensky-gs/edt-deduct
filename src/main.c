#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INPUT_BUFFER_SIZE 2048
#define SILLONS_SAVE_PATH "sillons.txt"
#define MATIERES_SAVE_PATH "matieres.txt"

#define MAX(x,y) (((x)>(y))?(x):(y))
#define MIN(x,y) (((x)<(y))?(x):(y))

struct sSillon {
	int days[3];
	int starts[3];
	int ends[3];
	int id;
};
typedef struct sSillon * tSillon;

struct sChained_sillons_list {
	tSillon sillon;
	struct sChained_sillons_list * next;
};
typedef struct sChained_sillons_list * tCsl;

int csl_size(tCsl list) {
	int i = 0;
	tCsl current = list;
	while (current != NULL) {
		i++;
		current = current->next;
	};
	return i;
}
tCsl csl_create(tSillon value) {
	tCsl cell;
	if ((cell = malloc(sizeof(struct sChained_sillons_list))) == NULL) {
		perror("CSL_create : Allocation error");
		return NULL;
	}
	cell->next = NULL;
	cell->sillon = value;
	return cell;
}
tCsl csl_append(tCsl * list, tSillon sillon) {
	tCsl cell = csl_create(sillon);
	if (cell == NULL) {
		perror("CSL_append : cell creation error");
		return *list;
	}
	if (*list == NULL) {
		*list = cell;
		return cell;
	}

	tCsl current = *list;
	tCsl last = NULL;
	while (current != NULL) {
		if (current->next == NULL) {
			last = current;
		};
		current = current->next;
	}

	if (last == NULL) {
		*list = cell;
		return cell;
	}
	
	(*last).next = cell;
	return *list;
}
int csl_exists(tCsl list, int id) {
	tCsl current = list;
	while (current != NULL) {
		if (current->sillon->id == id) {
			return 1;
		}
		current = current->next;
	};
	return false;
}
tSillon csl_get_nth(tCsl list, int n) {
	tCsl current = list;
	while (current != NULL && n > 0) {
		n--;
		current = current->next;
	}
	if (current != NULL) return current->sillon;
	return NULL;
}
tSillon csl_find(tCsl list, int id) {
	tCsl current = list;
	while (current != NULL) {
		if (current->sillon->id == id) {
			return current->sillon;
		}
		current = current->next;
	}
	return NULL;
}
void csl_iter(tCsl list, void callback(tSillon)) {
	tCsl current = list;
	while (current != NULL) {
		callback(current->sillon);
		current = current->next;
	}	
}
void csl_destroy(tCsl * pList) {
	tCsl current = *pList;
	while (current != NULL) {
		tCsl next = current->next;
		free(current);
		current = next;
	}
	*pList = NULL;
}

struct sMatiere {
	char * name;
	int group;
	int id;
	tCsl sillons_list;
};
struct sEDT {
	int start;
	int end;
	int days;
	int matieres;
	struct sMatiere ** matieres_list;
};

typedef struct sMatiere * tMatiere;
typedef struct sEDT * tEDT;

struct sCml {
	tMatiere matiere;
	struct sCml * next;
};
typedef struct sCml * tCml;

tCml cml_create(tMatiere matiere) {
	tCml cell;
	if ((cell = malloc(sizeof(struct sCml))) == NULL) {
		perror("CSL_create : Allocation error");
		return NULL;
	}

	cell->next = NULL;
	cell->matiere = matiere;
	return cell;
}
tCml cml_append(tCml * list, tMatiere matiere) {
	tCml cell; 
	if ((cell = cml_create(matiere)) == NULL) {
		perror("CML_append : Cell creation error");
		return *list;
	}
	
	tCml current = *list;
	tCml last = *list;

	while (current != NULL) {
		if (current->next == NULL) {
			last = current;
		}
		current = current->next;
	}

	if (last == NULL) {
		*list = cell;
		return *list;
	}
	last->next = cell;
	return *list;
}
int cml_exists(tCml list, int id) {
	tCml current = list;
	while (current != NULL) {
		if (current->matiere->id == id) return 1;
		current = current->next;
	}
	return 0;
}
tMatiere cml_find(tCml  list, int id) {
	tCml current = list;
	while (current != NULL) {
		if (current->matiere->id == id) return current->matiere;
		current = current->next;
	}
	return NULL;
}
int cml_size(tCml list) {
	int count = 0;
	tCml current = list;
	while (current != NULL) {
		count++;
		current = current->next;
	}
	return count;
}
tMatiere * cml_to_array(tCml list, int * size) {
	*size = cml_size(list);
	if (size == 0) return NULL;

	tMatiere * arr;
	if ((arr = malloc(sizeof(tMatiere) * *size)) == NULL) {
		perror("CML_to_array : Allocation error");
		return NULL;
	}
	int i = 0;
	tCml current = list;
	while (current != NULL) {
		arr[i] = current->matiere;
		current = current->next;
		i++;
	}

	return arr;
}
void cml_iter(tCml list, void callback(tMatiere matiere)) {
	tCml current = list;
	while (current != NULL) {
		callback(current->matiere);
		current = current->next;
	}
}
void cml_destroy(tCml * list) {
	tCml current = *list;
	while (current != NULL) {
		tCml next = current->next;
		free(current);
		current = next;
	}
}


int global_id_count = 0;
int fetch_first_only = 1;

int strsize(char * input) {
	int i = 0;
	while (input[++i] != '\0') {}
	return i;
}

void sort_sillon(tSillon sillon) {
	int i = 0;
	while (i < 2) {
		int m = i;
		int j = i;
		while (j < 3) {
			if (sillon->days[j] < sillon->days[m]) {
				m = j;
			}
			j++;
		}
		int temp = sillon->days[m];
		sillon->days[m] = sillon->days[i];
		sillon->days[i] = temp;
		temp = sillon->starts[m];
		sillon->starts[m] = sillon->starts[i];
		sillon->starts[i] = temp;
		temp = sillon->ends[m];
		sillon->ends[m] = sillon->ends[i];
		sillon->ends[i] = temp;
		i++;
	}
}

tSillon create_sillon(int starts[3], int ends[3], int days[3]) {
	tSillon sillon;
	if ((sillon = calloc(1, sizeof(struct sSillon))) == NULL) {
		perror("create_sillon : Allocation error");
		return NULL;
	}

	int i = 0;
	while (i++ < 3) {
		sillon->starts[i - 1] = starts[i - 1];
		sillon->ends[i - 1] = ends[i - 1];
		sillon->days[i - 1] = days[i - 1];
	}

	sillon->id = global_id_count++;

	sort_sillon(sillon);

	return sillon;
}

void display_sillon(tSillon sillon) {
	if (sillon == NULL) {
		printf("\x1b[35mNULL\x1b[0m\n");
		return;
	}
	char * days[5] = { "lundi", "mardi", "mercredi", "jeudi", "vendredi" };

	printf("Sillon \x1b[91m%d\x1b[0m :\n", sillon->id);
	int i = 0;
	while (i++ < 3) {
		printf("    \x1b[93m%s\x1b[0m %d:00 - %d:00\n", days[sillon->days[i - 1]], sillon->starts[i - 1], sillon->ends[i - 1]);
	}
}

void destroy_sillon(tSillon * pSillon) {
	free(*pSillon);
	*pSillon = NULL;
}

void copy_str(char * source, char * target) {
	int i = 0;
	while (source[i] != '\0') {
		target[i] = source[i];
		i++;
	};
	target[i] = '\0';
}

void display_matiere(tMatiere matiere) {
	if (matiere == NULL) {
		printf("\x1b[35mNULL\x1b[0m\n");
		return;
	}
	printf("%s (\x1b[90m%d\x1b[0m) (\x1b[92m%d\x1b[0m) : \n", matiere->name, matiere->group, matiere->id);

	if (matiere->sillons_list == NULL) {
		printf("\x1b[35mNULL\x1b[0m\n");
		return;
	}
	csl_iter(matiere->sillons_list, display_sillon);
}
tMatiere create_matiere(char * name, int group) {
	char * copy;
	if ((copy = calloc(strsize(name) + 1, sizeof(char))) == NULL) {
		perror("create_matiere : Copy allocation error");
		return NULL;
	}
	copy_str(name, copy);
	tMatiere matiere;
	if ((matiere = malloc(sizeof(struct sMatiere))) == NULL) {
		perror("create_matiere : Allocation error");
		free(copy);
		return NULL;
	}

	matiere->name = copy;
	matiere->group = group;
	matiere->sillons_list = NULL;
	matiere->id = global_id_count++;

	return matiere;
}
void append_sillon_to_matiere(tMatiere matiere, tSillon sillon) {
	csl_append(&(matiere->sillons_list), sillon);
}
void destroy_matiere(tMatiere * pMatiere) {
	csl_destroy(&(*pMatiere)->sillons_list);
	free((*pMatiere)->name);
	free(*pMatiere);
	*pMatiere = NULL;
}

char ** split_string(char * input, char delimiter, size_t * num_tokens) {
	*num_tokens = 0;
	char * copy = strdup(input);
	char * token = strtok(copy, &delimiter);
	char ** result = NULL;

	while (token != NULL) {
		result = realloc(result, (*num_tokens + 1) * sizeof(char *));

		if (result == NULL) {
			perror("split_string realloc error");
			free(copy);
			return NULL;
		}
		result[*num_tokens] = strdup(token);
		(*num_tokens)++;
		token = strtok(NULL, &delimiter);
	}

	free(copy);
	return result;
}

int maximum(int * list, int size) {
	if (size == 0) return -1;
	int i = 0;
	int max = list[0];
	while (i++ < size) {
		if (list[i - 1] > max) max = list[i - 1];
	}

	return max;
}
int minimum(int * list, int size) {
	if (size == 0) return -1;
	int i = 0;
	int min = list[0];
	while (i++ < size) {
		if (list[i - 1] < min) min = list[i - 1];
	}
	return min;
}

char * join_name(char ** input, int argc, int offset) {
	int i = 0, total = 0;

	while (i++ + offset < argc) {
		total += strsize(input[i - 1 + offset]);
	}

	total += argc - offset;

	char * res;
	if ((res = malloc(total + 1))  == NULL) {
		perror("JOIN_NAME : Allocation error");
		return NULL;
	}

	char * ptr = res;
	i = 0;
	while (i++ + offset < argc) {
		strcpy(ptr, input[i - 1 + offset]);
		ptr += strsize(input[i - 1 + offset]);

		strcpy(ptr, " ");
		ptr += 1;
	}

	*ptr = '\0';
	return res;
}

void commands_help() {
	printf("This is the \x1b[94medt-deduct\x1b[0m : here are the commands you can use :\n    sillon create \x1b[90m[day 1] [day2] [day3] [start 1] [start 2] [start 3] [end 1] [end 2] [end3]\x1b[0m\n    silview\n    matview\n    matiere create \x1b[90m[group] [name]\x1b[0m\n    matiere addsill \x1b[90m[ID matiere] [ID sillon]\x1b[0m\n    compute\n    fetchfirst \x1b[94mActive/désactive le premier résultat seulement\x1b[0m\n    export\n    import\n    clear\n    exit\n");
}

int check_edt_valid(tSillon * array, int size) {
	int encountered[size];
	int i = 0;
	while (i < size) {
		encountered[i] = array[i]->id;
		int j = 0;
		while (j < i) {
			if (encountered[j] == encountered[i]) {
				return 0;
			}
			j++;
		}
		i++;
	}
	return 1;
}

void increase(int * tab, int * mods, int size) {
	int i = size - 1;
	while (i-- >= 0) {
		tab[i + 1] = (tab[i + 1] + 1) % mods[i + 1];
		if (tab[i + 1] != 0) {
			break;
		}
	}
}

int int_size(int input) {
	if (input == 0) return 1;

    int size = 0;
    while (input > 0) {
        size++;
        input /= 10;
    }
    return size;
}

void display_valid(tMatiere * mats, tSillon * sillons, int size) {
	int i = 0;
	while (i++ < size) {
		printf("\x1b[91m%s\x1b[0m (\x1b[90mgroupe %d\x1b[0m) (\x1b[92m%d\x1b[0m)\n  ", mats[i - 1]->name, mats[i - 1]->group, mats[i - 1]->id);
		display_sillon(sillons[i - 1]);
	}
}

void commands_export(tCml * matieres, tCsl * sillons) {
    FILE * stream;
    if ((stream = fopen(SILLONS_SAVE_PATH, "wb")) == NULL) {
        perror("Coomands_export : Open sillons error");
        return;
    }

    tCsl current = *sillons;
    while (current != NULL) {
        struct sSillon buffer = *(current->sillon);

        int transform[10] = { buffer.id, buffer.starts[0], buffer.starts[1], buffer.starts[2], buffer.ends[0], buffer.ends[1], buffer.ends[2], buffer.days[0], buffer.days[1], buffer.days[2] };

        if (fwrite(&transform, sizeof(int), 10, stream) == 0) {
            perror("Commands_export : Write error");
            return;
        }
        current = current->next;
    }

    fclose(stream);
    if ((stream = fopen(MATIERES_SAVE_PATH, "wt")) == NULL) {
        perror("commands_export : Open matiere error");
        return;
    }

    tCml currentm = *matieres;
    while (currentm != NULL) {
        struct sMatiere buffer = *(currentm->matiere);

        int tsize = strsize(buffer.name);
        int ssize = csl_size(buffer.sillons_list);

        fprintf(stream, "%d %d %d %d\n", buffer.id, buffer.group, tsize, ssize);

        tCsl currentl = buffer.sillons_list;
        while (currentl != NULL) {
            fprintf(stream, "%d ", currentl->sillon->id);
            currentl = currentl->next;
        }
        fprintf(stream, "%s\n", buffer.name);
        
        currentm = currentm->next;
    }

    fclose(stream);
}
void commands_import(tCml * matieres, tCsl * sillons) {
	int appended_matieres = 0, appended_sillons = 0;
    FILE * stream;
    if ((stream = fopen(SILLONS_SAVE_PATH, "rb")) == NULL) {
        printf("\x1b[91mNo save file found\x1b[0m\n");
        return;
    }

    int input[10];
    while (fread(&input, sizeof(int), 10, stream) > 0) {
        tSillon sillon;
        if ((sillon = malloc(sizeof(struct sSillon))) == NULL) {
            perror("commands_import : Sillon Allocation error");
            fclose(stream);
            return;
        }
        int i = 0;
        while (i < 9) {
            int * arr[3] = { sillon->starts, sillon->ends, sillon->days };
            int * target = arr[i / 3];

            target[i % 3] = input[i + 1];
            i++;
        }
        sillon->id = input[0];

        if (csl_exists(*sillons, sillon->id) == 1) {
            printf("Sillon \x1b[33m%d\x1b[0m already registered, skipping\n");
            continue;
        }

        csl_append(sillons, sillon);
		appended_sillons++;
    }
    fclose(stream);
    if ((stream = fopen(MATIERES_SAVE_PATH, "rt")) == NULL) {
        printf("\x1b[91mCannot open matieres save file");
        return;
    }

    char * buffer;
    if ((buffer = calloc(256, sizeof(char))) == NULL) {
        perror("Commands_import : Buffer allocation error");
        fclose(stream);
        return;
    }
    while (fgets(buffer, 256, stream) != NULL) {
        int id, group, tsize, ssize;
        if (sscanf(buffer, "%d %d %d %d", &id, &group, &tsize, &ssize) != 4) {
            printf("Save malformed... Aborting\n");
            free(buffer);
            fclose(stream);
            return;
		}

		int total_size = 1 + tsize + ssize * 5;
		char * line;
		if ((line = calloc(total_size, sizeof(char))) == NULL) {
			perror("Comands_import : line allocation error");
			free(buffer);
			fclose(stream);
			return;
		}

		if (fgets(line, total_size, stream) == NULL) {
			perror("Commands_import : Cannot read line");
			free(buffer);
			free(line);
			fclose(stream);
			return;
		}

		if (cml_exists(*matieres, id)) {
			printf("Matiere ID \x1b[33m%d\x1b[0m already registered... Skipping...\n", id);
			free(line);
			continue;
		}

		tMatiere matiere;
		if ((matiere = malloc(sizeof(struct sMatiere))) == NULL) {
			perror("Commands_import : Matiere allocation error");
			free(buffer);
			free(line);
			fclose(stream);
			return;
		}

		matiere->name = NULL;
		matiere->group = group;
		matiere->id = id;
		matiere->sillons_list = NULL;

		int offset = 0;
		int i = 0;
		int skip = 0;
		while (i < ssize) {
			int read;
			if (sscanf(line + offset, "%d", &read) == 0) {
				printf("Cannot read sillon \x1b[33m%d\x1b[0m of matiere. Skipping\n", i);
				skip = 1;
				break;
			}
			i++;
			offset += 1 + int_size(read);

			tSillon matching;
			if ((matching = csl_find(*sillons, read)) == NULL) {
				printf("Cannot find sillon id \x1b[33m%d\x1b[0m. SKipping.\n", read);
				skip = 1;
				break;
			}
			if (csl_exists(matiere->sillons_list, read)) {
				printf("Sillon already in matiere... Skipping...\n");
				skip = 1;
				break;
			}

			csl_append(&matiere->sillons_list, matching);
		}

		if (skip) {
			free(line);
			continue;
		}

		char * name;
		if ((name = calloc(tsize + 1, sizeof(char))) == NULL) {
			perror("Commands_import : Name allocation error");
			free(line);
			free(buffer);
			fclose(stream);
			return;
		}

		int j = 0;
		while (j < tsize && (line + offset)[j] != '\n' && (line + offset)[j] != '\0') {
			name[j] = (line + offset)[j];
			j++;
		}

		matiere->name = name;
		if (cml_exists(*matieres, id)) {
			printf("Matiere \x1b[33m%d\x1b[0m already registered, skipping this one\n", id);
			free(name);
			free(line);
			continue;
		}

		cml_append(matieres, matiere);
		appended_matieres++;
		free(line);
    }

    free(buffer);

    fclose(stream);

	printf("Loaded \x1b[33m%d\x1b[0m matieres and \x1b[33m%d\x1b[0m sillons\n", appended_matieres, appended_sillons);
}

void commands_compute(tCml matslist) {
	int size;
	tMatiere * mats;
	if ((mats = cml_to_array(matslist, &size)) == NULL) {
		printf("\x1b[31mSomething went wrong\x1b[0m\n");
		return;
	}

	tSillon * copy;
	if ((copy = malloc(sizeof(struct sSillon) * size)) == NULL) {
		perror("commands_compute : Allocation error");
		return;
	}

	int max = 1;
	int i = 0;
	int modulos[size];
	int indexes[size];
	while (i < size) {
		int len = csl_size(mats[i]->sillons_list);
		if (len == 0) {
			printf("\x1b[31mMatiere \x1b[33m%d\x1b[31m has no sillon", i);
			return;
		}
		modulos[i] = len;
		max *= len;
		indexes[i] = 0;
		i++;
	}

	int successes = 0;
	i = 0;
	while (i++ < max) {
		int j = 0;
		while (j < size) {
			copy[j] = csl_get_nth(mats[j]->sillons_list, indexes[j]);
			j++;
		}

		increase(indexes, modulos, size);
		int valid = check_edt_valid(copy, size);

		if (valid) {
			printf("Found a valid model\n");
			display_valid(mats, copy, size);
			successes++;

			if (fetch_first_only) {
				printf("Fetching only the first result, escaping.\n   \x1b[33mHint :\x1b[0m use the \x1b[90mfetchfirst\x1b[0m command to disable this behavior\n");
				break;
			}
		}
	}

	printf("Found \x1b[33m%d\x1b[0m valid models\n", successes);
}

tMatiere commands_matiere(char ** args, size_t argc, tCml * matslist, tCsl * sillslist) {
	if (argc < 2) {
		printf("\x1b[31mInsufficient arguments\x1b[0m\n");
		return NULL;
	}
	if (strcmp(args[1], "create") == 0) {
		if (argc < 4) {
			printf("\x1b[31mInsufficient arguments\x1b[0m\n");
			return NULL;
		}

		int group;
		if (sscanf(args[2], "%d", &group) == 0 || group < 0) {
			printf("\x1b[31mThe group must be a strictly positive number\x1b[0m\n");
			return NULL;
		}
		char * name = join_name(args, argc, 3);

		tMatiere matiere = create_matiere(name, group);
		display_matiere(matiere);

		cml_append(matslist, matiere);
		return matiere;
	} else if (strcmp(args[1], "addsill") == 0) {
		if (argc < 4) {
			printf("\x1b[91mInsufficient arguments\x1b[0m\n");
			return NULL;
		}
		int ids, idm;
		if (sscanf(args[2], "%d", &idm) == 0 || sscanf(args[3], "%d", &ids) == 0) {
			printf("\x1b[91mSpecified IDS are invalid numbers\x1b[0m\n");
			return NULL;
		}

		tMatiere matiere;
		tSillon sillon;

		if ((matiere = cml_find(*matslist, idm)) == NULL) {
			printf("\x1b[91mInvalid matiere ID\x1b[0m\n");
			return NULL;
		}
		if ((sillon = csl_find(*sillslist, ids)) == NULL) {
			printf("\x1b[91mInvalid sillon ID\x1b[0m\n");
			return NULL;
		}

		if (csl_exists(matiere->sillons_list, ids) == 1) {
			printf("\x1b[31mSillon already registered\x1b[0m\n");
			return NULL;
		}

		append_sillon_to_matiere(matiere, sillon);
		display_matiere(matiere);

		return matiere;
	} else {
		printf("\x1b[31mUnknown command\x1b[0m\n");
		return NULL;
	}
}

tSillon commands_sillon(char ** args, size_t argc, tCsl * sillslist) {
	if (argc < 2) {
		printf("\x1b[31mInsufficient arguments\x1b[0m\n");
		return NULL;
	}
	if (strcmp(args[1], "create") == 0) {
		int starts[3], ends[3], days[3];

		if (argc < 11) {
			printf("\x1b[31mInsufficient arguments\x1b[0m\n");
			return NULL;
		}
		int i = 0;
		while (i < 3) {
			int * matches[3] = { days, starts, ends };
			int * array = matches[i];

			int j = 0;
			while (j < 3) {
				int res = sscanf(args[2 + i * 3 + j], "%d", &array[j]);

				if (res == 0) {
		printf("\x1b[31mInvalid number at position \x1b[33m%d\x1b[0m\n", i * 3 + j + 1);
					return NULL;
				}
				j++;
			}

			i++;
		}

		int s[3], e[3];

		i = 0;
		while (i < 3) {
			if (starts[i] < 8 || starts[i] > 18) {
				printf("\x1b[91mStarts must be between 8 and 18\x1b[0m\n");
				return NULL;
			}
			if (ends[i] < 10 || ends[i] > 20) {
				printf("\x1b[91mEnds must be between 10 and 20\x1b[0m\n");
				return NULL;
			}
			if (days[i] < 0 || days[i] > 4) {
				printf("\x1b[91m days must be between 0 \x1b[90m(monday)\x1b[91m and 4 \x1b[90m(friday)\x1b[0m\n");
				return NULL;
			}
			if (starts[i] > ends[i]) {
				printf("\x1b[91mStarts must be greater than ends\x1b[0m\n");
				return NULL;
			}
			i++;
		}

		int j = 0;
		while (j < 5) {
			int indexes[3] = { -1, -1, -1 };
			int k = 0;
			int last = 0;
			while (k++ < 3) {
				if (days[k - 1] == j) indexes[last++] = k - 1;
			}

			printf("j = %d, [%d %d %d]\n", j, indexes[0], indexes[1], indexes[2]);
			printf("last = %d\n", last);

			if (last >= 2) {
				int l = 0;
				while (l < last) {
					int m = 0;
					while (m < last) {
						if (m == l) {
							m++;
							continue;
						}

						printf("ends[l] = %d, ends[m] = %d, starts[l] = %d, starts[m] = %d\n", ends[l], ends[m], starts[l], starts[m]);

						int overlap = MAX(0, MIN(ends[l], ends[m]) - MAX(starts[l], starts[m]));
						if (overlap > 0) {
							printf("\x1b[91mOverlap detected\x1b[0m\n");
							return NULL;
						}

						m++;
					}
					l++;
				}
			}
			j++;
		}

		tSillon sillon;
		if ((sillon = create_sillon(starts, ends, days)) == NULL) {
			perror("commands_sillon : Create creation error");
			return NULL;
		}

		tCsl current_sil = *sillslist;
		while (current_sil != NULL) {
			int l = 0;
			while (l < 5) {
				int j = 0;
				int ia = -1, ib = -1;
				while (j < 3) {
					if (sillon->days[j] == l) ia = j;
					if (current_sil->sillon->days[j] == l) ib = j;
					j++;
				}

				if (ia != -1 && ib != -1) {
					int overlap = MAX(0, MIN(sillon->ends[ia], current_sil->sillon->ends[ib]) - MAX(sillon->starts[ia], current_sil->sillon->starts[ib]));
					if (overlap > 0) {
						printf("\x1b[31mOverlap detect with sillon \x1b[33m%d\x1b[31m at \x1b[33m%d\x1b[0m\n", current_sil->sillon->id, ia);
						return NULL;
					}
				}

				l++;
			}
			current_sil = current_sil->next;
		}

		display_sillon(sillon);
		csl_append(sillslist, sillon);
		return sillon;
	} else {
		printf("\x1b[31mUnknown commands\x1b[0m\n");
	}

	return NULL;

}

int main() {
	tCsl sills = NULL;
	tCml matieres = NULL;

	char * buffer = NULL;
	size_t len = 0;
	ssize_t lineSize = 0;

	while (true) {
		printf("> ");
		fflush(stdout);

		lineSize = getline(&buffer, &len, stdin);
		if (lineSize == -1) {
			printf("An error occured while reading your input\n");
			continue;
		}
		if (lineSize > 0 && buffer[lineSize - 1] == '\n') {
			buffer[lineSize - 1] = '\0';
		}

		char ** args;
		size_t argc;
		if ((args = split_string(buffer, ' ', &argc)) == NULL) {
			perror("Main : error while reading args");
			continue;
		}

		if (!argc) {
			free(args);
			continue;
		}

		if (strcmp(args[0], "help") == 0) {
			commands_help();
		} else if (strcmp(args[0], "sillon") == 0) {
			commands_sillon(args, argc, &sills);
		} else if (strcmp(args[0], "matiere") == 0) {
			commands_matiere(args, argc, &matieres, &sills);
		} else if (strcmp(args[0], "compute") == 0) {
			commands_compute(matieres);
		} else if (strcmp(args[0], "fetchfirst") == 0) {
			fetch_first_only = !fetch_first_only;
			printf("New value: \x1b[90m%d\x1b[0m\n", fetch_first_only);
		} else if (strcmp(args[0], "import") == 0) {
            commands_import(&matieres, &sills);
        } else if (strcmp(args[0], "export") == 0) {
            commands_export(&matieres, &sills);
        } else if (strcmp(args[0], "exit") == 0) {
            printf("Exiting...\n");
            return 0;
        } else if (strcmp(args[0], "matview") == 0) {
			if (cml_size(matieres) == 0) {
				printf("0 matière\n");
			} else {
				cml_iter(matieres, display_matiere);
			}
		} else if (strcmp(args[0], "silview") == 0) {
			if (csl_size(sills) == 0) {
				printf("0 sillons\n");
			} else {
				csl_iter(sills, display_sillon);
			}
		} else if (strcmp(args[0], "clear") == 0) {
			printf("\e[1;1H\e[2J");
		}

		free(args);
	}

	if (buffer != NULL) free(buffer);

	csl_destroy(&sills);
	cml_destroy(&matieres);
}
