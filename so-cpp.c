#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "hashmap.h"

#define MAX_LENGTH (256)
#define TABLE_SIZE 10
#define MIN_LENGTH 10

void multiline_define(FILE *inF, char *tok, struct hashMap **ht)
{
	int len = MAX_LENGTH + 1;
	int k = 0;
	char *symb = (char *) malloc(len * sizeof(char));
	char *mapp = (char *) malloc(len * sizeof(char));
	char *buf = (char *) malloc(len * sizeof(char));

	while (tok != NULL) {
		tok = strtok(NULL, " ");
		if (k == 0) {
			strcpy(symb, tok);
			k = 1;
		} else if (k == 1) {
			strcpy(mapp, tok);
			k = 2;
		}
	}
	while (fgets(buf, sizeof(buf), inF) != NULL) {
		char *tok;

		if (strcmp(buf, "\\\n") == 0)
			continue;
		if (strcmp(buf, "\n") == 0) {
			mapp[strlen(mapp) - 1] = '\0';
			break;
		}
		tok = strtok(buf, "\t ");
		if (tok != NULL) {
			strcat(mapp, " ");
			strcat(mapp, tok);
			strcat(mapp, " ");
			tok = strtok(NULL, " \\");
			if (tok != NULL)
				strcat(mapp, tok);
		}
	}
	/* add to hashmap */
	if (size(*ht) && isKey(*ht, mapp) != 0)
		insertMap(*ht, symb, inMap(*ht, mapp));
	else
		insertMap(*ht, symb, mapp);
	free(buf);
	free(symb);
	free(mapp);
}

void normal_define(FILE *inF, char *tok, struct hashMap **ht)
{
	int len = MAX_LENGTH + 1;
	char *symb = (char *) malloc(len*sizeof(char));
	char *mapp = (char *) malloc(len*sizeof(char));
	int k = 0;

	tok = strtok(NULL, " ");
	strcpy(mapp, "");
	while (tok != NULL) {
		if (k == 0) {
			if (tok[strlen(tok) - 1] == '\n') {
				strcpy(symb, tok);
				symb[strlen(symb)-1] = '\0';
				tok = NULL;
			} else {
				strcpy(symb, tok);
				k = 1;
			}
		} else if (k == 1) {
			if (size(*ht) != 0 && isKey(*ht, tok) != 0)
				strcat(mapp, inMap(*ht, tok));
			else
				strcat(mapp, tok);
			strcat(mapp, " ");
			if (mapp[strlen(mapp)-2] == '\n') {
				mapp[strlen(mapp)-2] = '\0';
				k = 2;
			}
		}
		tok = strtok(NULL, " ");
	}
	/* add to hashmap */
	if (size(*ht) && isKey(*ht, mapp) != 0)
		insertMap(*ht, symb, inMap(*ht, mapp));
	else
		insertMap(*ht, symb, mapp);
	free(symb);
	free(mapp);
}

void special_lines(char *tok, char *cpy, struct hashMap **ht,
	int *t, int *s, int *v, int *p)
{
	if (strcmp(tok, "{\n") == 0)
		(*t)++;
	else if (strlen(cpy) > 2 && cpy[strlen(cpy) - 2] == '{')
		(*t)++;
	if (strcmp(tok, "}") == 0)
		(*t)--;
	if (strcmp(tok, "#if") == 0) {
		tok = strtok(NULL, " ");
		if (tok != NULL) {
			int l = strlen(tok) + 1;
			char *ck;

			ck = (char *)malloc(l*sizeof(char));
			strcpy(ck, tok);
			ck[strlen(ck) - 1] = '\0';
			if (size(*ht) != 0
			&& isKey(*ht, ck) != 0) {
				if (atoi(inMap(*ht, ck)) != 0) {
					*v = 1;
					*p = 1;
				} else {
					*p = 0;
					*v = 1;
				}
			} else {
				if (atoi(ck) != 0) {
					*v = 1;
					*p = 1;
				} else {
					*p = 0;
					*v = 1;
				}
			}
			free(ck);
		}
	} else if (strcmp(tok, "#elif") == 0) {
		tok = strtok(NULL, " ");
		if (tok != NULL) {
			int len = strlen(tok) + 1;
			char *ck;

			ck = (char *)malloc(len*sizeof(char));
			strcpy(ck, tok);
			ck[strlen(ck) - 1] = '\0';
			if (size(*ht) != 0
			&& isKey(*ht, ck) != 0) {
				if (atoi(inMap(*ht, ck)) != 0) {
					*v = 1;
					*p = 1;
				} else {
					*s = 0;
				}
			} else {
				if (atoi(ck) != 0) {
					*v = 1;
					*p = 1;
				} else {
					*s = 0;
				}
			}
			free(ck);
		}
	}
}

void print_defines(char *tok, FILE *outF, struct hashMap **ht)
{
	int i, ok = 0, j;

	while (tok != NULL) {
		int l = strlen(tok) + 1;
		char *ck;

		ck = (char *)malloc(l*sizeof(char));
		strcpy(ck, "");
		l = strlen(tok);
		for (j = 0; j < l; j++) {
			strcat(ck, &tok[j]);
			ck[j + 1] = '\0';
			if (size(*ht) != 0 && isKey(*ht, ck) != 0) {
				fprintf(outF, " %s", inMap(*ht, ck));
				for (i = j+1; i < l; i++)
					fprintf(outF, "%c", tok[i]);
				ok = 1;
				break;
			}
		}
		free(ck);

		if (ok == 0)
			fprintf(outF, " %s", tok);
		tok = strtok(NULL, " ");
		ok = 0;
	}
}

void check_define(char buff[], char cpy[], FILE *outF,
	struct hashMap **ht, FILE *inF, int *t, int *s, int *v, int *p)
{
	char *tok = strtok(buff, " ");
	int j;

	if (tok != NULL) {
		if (strcmp(tok, "#define") == 0) {
			if (cpy[strlen(cpy) - 2] == '\\') {
				/* multiline define */
				multiline_define(inF, tok, ht);
			} else { /* normal define */
				normal_define(inF, tok, ht);
			}
		} else if (strcmp(tok, "#undef") == 0) {
			/* remove from hashmap */
			int l = strlen(tok) + 1;
			char *ck = (char *)malloc(l*sizeof(char));

			tok = strtok(NULL, " ");
			strcpy(ck, tok);
			ck[strlen(ck) - 1] = '\0';
			if (tok != NULL)
				removeKey(*ht, ck);
			free(ck);
		} else if (strcmp(tok, "#include") != 0) {
			/* change define in code */
			if (strcmp(tok, "{\n") == 0 || strcmp(tok, "}") == 0
			|| strcmp(tok, "\n") == 0 || strcmp(tok, "#if") == 0
			|| strcmp(tok, "#else\n") == 0
			|| strcmp(tok, "#endif\n") == 0
			|| strcmp(tok, "#elif") == 0
			|| cpy[strlen(cpy) - 2] == '{') {
			/* daca nu e vreo linie care sa modifice ceva */
				special_lines(tok, cpy, ht, t, s, v, p);
				if (strcmp(tok, "{\n") == 0
				|| strcmp(tok, "}") == 0
				|| strcmp(tok, "\n") == 0
				|| cpy[strlen(cpy) - 2] == '{')
					fprintf(outF, "%s", cpy);
			} else {
				/* printeaza normal sau */
				/* verifica sa printezi define-urile */
				for (j = 0; j < *t; j++)
					fprintf(outF, "\t");
				fprintf(outF, "%s", tok);
				tok = strtok(NULL, " ");
				print_defines(tok, outF, ht);
			}
		}
	}

}

void preprocess(char *infile, FILE *oF, char **cd,
	int d, struct hashMap *ht);

void check_include(char buff[], char cpy[], char **cd,
	int dirs, FILE *outF, struct hashMap *ht)
{
	char *tok = strtok(buff, " ");
	int i = 0, j = 0;
	char *tp;
	FILE *f;

	if (tok != NULL && strcmp(tok, "#include") == 0) {
		tok = strtok(NULL, " ");
		if (tok[0] == '"') {
			tp = (char *) malloc((strlen(tok)-2)*sizeof(char));
			while (tok[i+1] != '"') {
				tp[i] = tok[i+1];
				i++;
			}
			tp[i] = '\0';
			/* ma asigur ca am calea buna catre .h */
			f = fopen(tp, "r");
			if (f == NULL && dirs != 0) {
				/*se verifica toate directoarele*/
				for (j = 0; j < dirs; j++) {
					int len = strlen(cd[j])+strlen(tp)+2;
					char *t_n;

					t_n = (char *) malloc(len*sizeof(char));
					strcpy(t_n, cd[j]);
					strcat(t_n, "/");
					strcat(t_n, tp);
					free(tp);
					len = strlen(t_n) + 1;
					tp = (char *) malloc(len*sizeof(char));
					strcpy(tp, t_n);
					free(t_n);

					f = fopen(tp, "r");
					if (f != NULL) {
						fclose(f);
						break;
					}
				}
			} else if (f == NULL && dirs == 0) {
				/* aici ar trb sa dea eroare si sa iasa */
				exit(1);
			} else {
				fclose(f);
			}
			preprocess(tp, outF, cd, dirs, ht);
			fprintf(outF, "\n");
			free(tp);
		} else {
			fprintf(outF, "%s", cpy);
		}
	}
}


void helper(FILE *iF, FILE *oF, char **cd,
	int d, struct hashMap *ht, char *b, int *t, int *s, int *v, int *p)
{
	char *c1, *c2;
	c1 = (char *) malloc((strlen(b) + 1) * sizeof(char));
	c2 = (char *) malloc((strlen(b) + 1) * sizeof(char));
	strcpy(c1, b);
	strcpy(c2, b);

	/* check includes */
	check_include(b, c1, cd, d, oF, ht);
	/* input defines to map */
	check_define(c2, c1, oF, &ht, iF, t, s, v, p);

	free(c1);
	free(c2);
}

void preprocess(char *infile, FILE *oF, char **cd,
	int d, struct hashMap *ht)
{
	FILE *iF;
	int t = 0, v = 0, p = 0, s = 0;
	char b[MAX_LENGTH];

	if (infile != NULL) {
		iF = fopen(infile, "r");
		if (iF == NULL)
			exit(1);
	} else {
		iF = stdin;
	}

	while (fgets(b, sizeof(b), iF) != NULL) {

		if (strstr(b, "#if") != NULL)
			s = 1;
		if (s == 0) { /* normal */
			helper(iF, oF, cd, d, ht, b, &t, &s, &v, &p);
		} else if (s == 1 && v == 0 && p == 0) { /* if */
			int jmp = 0;

			do {
				if (strstr(b, "#else") != NULL
				|| strstr(b, "#elif") != NULL)
					break;
				if (strstr(b, "#endif") != NULL) {
					jmp = 1;
					s = 0;
					v = 0;
					p = 0;
					break;
				}
				if (v == 1 && p == 0) {
					jmp = 1;
					break;
				}
				helper(iF, oF, cd, d, ht, b, &t, &s, &v, &p);
			} while (fgets(b, sizeof(b), iF) != NULL);
			if (jmp == 0) {
				while (fgets(b, sizeof(b), iF) != NULL) {
					if (strstr(b, "#endif") != NULL) {
						s = 0;
						v = 0;
						p = 0;
						break;
					}
				}
			}
		} else if (s == 1 && v == 1 && p == 0) { /* elif */
			int jmp = 0;

			do {
				if (strstr(b, "#elif") != NULL
				|| strstr(b, "#endif") != NULL)
					break;
				if (strstr(b, "#else") != NULL) {
					v = 0;
					p = 1;
					jmp = 1;
					break;
				}
			} while (fgets(b, sizeof(b), iF) != NULL);
			do {
				if (strstr(b, "#else") != NULL)
					break;
				if (strstr(b, "#endif") != NULL) {
					s = 0;
					jmp = 1;
					v = 0;
					p = 0;
					break;
				}
				if (s == 0) {
					jmp = 1;
					s = 1;
					break;
				}
				helper(iF, oF, cd, d, ht, b, &t, &s, &v, &p);
			} while (fgets(b, sizeof(b), iF) != NULL);
			if (jmp == 0) {
				while (fgets(b, sizeof(b), iF) != NULL) {
					if (strstr(b, "#endif") != NULL) {
						s = 0;
						v = 0;
						p = 0;
						break;
					}
				}
			}
		} else if (s == 1 && v == 0 && p == 1) { /* else */
			do {
				if (strstr(b, "#endif") != NULL) {
					s = 0;
					v = 0;
					p = 0;
					break;
				}
				helper(iF, oF, cd, d, ht, b, &t, &s, &v, &p);
			} while (fgets(b, sizeof(b), iF) != NULL);
		}
	}

	if (iF != stdin)
		fclose(iF);
}

int main(int argc, char *argv[])
{
	int i, in = 0, o = 0;
	FILE *outF;
	struct hashMap *hT;
	char *tok, *symb, *mp, *dir, *out;
	char *outfile = (char *) malloc(MIN_LENGTH * sizeof(char));
	char *infile = NULL;
	char **cd = (char **) calloc(MIN_LENGTH, sizeof(char *));
	int dId = 0;

	hT = createMap(TABLE_SIZE);

	for (i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-D") == 0) {
			i++;
			tok = strtok(argv[i], "=");
			if (tok != NULL) {
				int l = strlen(tok) + 1;

				symb = (char *) malloc(l * sizeof(char));
				strcpy(symb, tok);
				tok = strtok(NULL, "\0");
				if (tok != NULL) {
					l = strlen(tok) + 1;
					mp = (char *) malloc(l*sizeof(char));
					strcpy(mp, tok);
					/* add to map symb mp */
					insertMap(hT, symb, mp);
					free(mp);
				} else {
					/* add to map symb "" */
					insertMap(hT, symb, "");
				}
				free(symb);
			}
		} else if (strcmp(argv[i], "-I") == 0) {
			int len;

			i++;
			len = strlen(argv[i]) + 1;
			cd[dId] = (char *) calloc(len, sizeof(char));
			strcpy(cd[dId], argv[i]);
			dId++;
		} else if (strcmp(argv[i], "-o") == 0) {
			i++;
			strcpy(outfile, argv[i]);
			o = 1;
		} else {
			if (argv[i][0] == '-' && argv[i][1] == 'D') {
				tok = strtok(argv[i], "=");
				if (tok != NULL) {
					symb = &tok[2];
					tok = strtok(NULL, "\0");
					if (tok != NULL) {
						/* add to map symb map */
						insertMap(hT, symb, tok);
					} else {
						/* add to map symb "" */
						insertMap(hT, symb, "");
					}
				}
			} else if (argv[i][0] == '-' || argv[i][1] == 'I') {
				int len;

				dir = &argv[i][2];
				len = strlen(dir) + 1;
				cd[dId] = (char *) calloc(len, sizeof(char));
				strcpy(cd[dId], dir);
				dId++;

			} else if (argv[i][0] == '-' || argv[i][1] == 'o') {
				out = &argv[i][2];
				strcpy(outfile, out);
				o = 1;
			} else {
				int len = strlen(argv[i]) + 1;

				if (in == 1)
					exit(1);
				infile = (char *) malloc(len * sizeof(char));
				strcpy(infile, argv[i]);
				in = 1;
			}
		}
	}

	if (o == 1) {
		outF = fopen(outfile, "w");
		if (outF == NULL) {
			for (i = 0; i < dId; i++)
				free(cd[i]);
			free(cd);
			if (outF != stdout)
				fclose(outF);
			free(infile);
			free(outfile);
			deleteMap(hT);
			exit(1);
		}
	} else {
		outF = stdout;
	}

	preprocess(infile, outF, cd, dId, hT);

	for (i = 0; i < dId; i++)
		free(cd[i]);

	free(cd);
	if (outF != stdout)
		fclose(outF);
	free(infile);
	free(outfile);
	deleteMap(hT);

	return 0;
}
