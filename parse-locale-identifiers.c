/*
 * Parse & convert locale identifiers, both in Gettext and Unicode formats.
 *
 * Author: Michele Locati <mlocati@gmail.com>
 * MIT License
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifndef __USE_GNU
char *strndup (const char *s, size_t n)
{
	char * result;
	result = (char*) malloc((n + 1) * sizeof(char));
	if (result) {
		if (n > 0) {
			memcpy(result, s, n);
		}
		result[n] = '\0';
	}
	return result;
}
#endif

/*
 * Contain all the possible chunck of the locale identifiers.
 */
typedef struct _LocaleChunks {
	// Language ID (NULL or not empty)
	char* language;
	// Territory/Country/Region ID (NULL or not empty)
	char* territory;
	// Codeset (NULL or not empty)
	char* codeset;
	// Modifier (NULL or not empty)
	char* modifier;
	// Script (NULL or not empty)
	char* script;
	// Number of variant tags
	size_t variantCount;
	// List of null-terminated variant tags (NULL if empty)
	char** variants;
} LocaleChunks;

/*
 * Initializes a new LocaleChunks structure and returns its pointer.
 * Returns NULL in case of out-of-memory problems.
 */
LocaleChunks* ConstructLocaleChunks()
{
	return (LocaleChunks*)calloc(1, sizeof(LocaleChunks));
}

/*
 * Frees a LocaleChunks structure and all its members.
 * lc may be NULL (in that case nothing happens).
 */
void FreeLocaleChunks(LocaleChunks* lc)
{
	size_t i;
	if (lc) {
		if (lc->language) {
			free(lc->language);
		}
		if (lc->territory) {
			free(lc->territory);
		}
		if (lc->codeset) {
			free(lc->codeset);
		}
		if (lc->modifier) {
			free(lc->modifier);
		}
		if (lc->script) {
			free(lc->script);
		}
		if (lc->variants) {
			for (i = 0; i < lc->variantCount; i++) {
				if (lc->variants[i]) {
					free(lc->variants[i]);
				}
			}
			free(lc->variants);
		}
		free(lc);
	}
}

/*
 * Parse a locale identifier in Gettext format (language[_territory][.codeset][@modifier]).
 * Returns NULL if locale NULL or invalid, or in case of out-of-memory problems.
 */
LocaleChunks* GettextLocaleIDToLocaleChunks(const char* locale)
{
	LocaleChunks* result;
	char *p, *separator;
	if (!locale) {
		return NULL;
	}
	separator = NULL;
	result = NULL;
	for (p = (char*) locale; !separator || *separator != '\0'; p++) {
		switch (*p) {
			case '_':
			case '.':
			case '@':
			case '\0':
				if (!result) {
					// First chunk: language
					if (p == locale) {
						// Empty language -> error
						return NULL;
					}
					result = ConstructLocaleChunks();
					if (result == NULL) {
						return NULL;
					}
					result->language = strndup(locale, p - locale);
				} else {
					if (p == separator + 1) {
						// Empty chunk -> error
						FreeLocaleChunks(result);
						return NULL;

					}
					switch (*separator) {
						case '_':
							if (result->territory || result->codeset || result->modifier) {
								// Duplicated or misplaced territory -> error
								FreeLocaleChunks(result);
								return NULL;
							}
							result->territory = strndup(separator + 1, p - separator - 1);
							break;
						case '.':
							if (result->codeset || result->modifier) {
								// Duplicated or misplaced codeset -> error
								FreeLocaleChunks(result);
								return NULL;
							}
							result->codeset = strndup(separator + 1, p - separator - 1);
							break;
						case '@':
							if (result->modifier) {
								// Duplicated modifier -> error
								FreeLocaleChunks(result);
								return NULL;
							}
							result->modifier = strndup(separator + 1, p - separator - 1);
							break;
					}
				}
				separator = p;
				break;
			default:
				if (!isalnum(*p)) {
					// Invalid character
					FreeLocaleChunks(result);
					return NULL;
				}
				break;
		}
	}
	return result;
}

/*
 * Convert a LocaleChunks to the Gettext locale ID format (language[_territory][.codeset][@modifier]).
 * Returns NULL if LocaleChunks is NULL or invalid, or in case of out-of-memory problems.
 */
char * LocaleChunksToGettextLocaleID(const LocaleChunks* lc)
{
	char* result;
	size_t length;
	result = NULL;
	if (lc && lc->language) {
		length = strlen(lc->language) + 1;
		if (lc->territory) {
			length += 1 + strlen(lc->territory);
		}
		if (lc->codeset) {
			length += 1 + strlen(lc->codeset);
		}
		if (lc->modifier) {
			length += 1 + strlen(lc->modifier);
		}
		result = (char*)malloc(length * sizeof(char));
		if(result) {
			strcpy(result, lc->language);
			if (lc->territory) {
				strcat(result, "_");
				strcat(result, lc->territory);
			}
			if (lc->codeset) {
				strcat(result, ".");
				strcat(result, lc->codeset);
			}
			if (lc->modifier) {
				strcat(result, "@");
				strcat(result, lc->modifier);
			}
		}
	}
	return result;
}

/*
 * Parse a locale identifier in Gettext format ( http://unicode.org/reports/tr35/#Unicode_language_identifier ).
 * Returns NULL if locale NULL or invalid, or in case of out-of-memory problems.
 */
LocaleChunks* UnicodeLocaleIDToLocaleChunks(const char* locale)
{
	LocaleChunks* result;
	size_t numChunks;
	size_t nextChunk;
	size_t initialVariantChunk;
	char** chunks;
	size_t *chunkLengths;
	size_t maxNumChunks;
	char *p, *chunkStart;
	int badData;
	if (!(locale && locale[0])) {
		return NULL;
	}
	result = NULL;
	maxNumChunks = 1 + (strlen(locale) >> 1);
	chunks = (char**)malloc(maxNumChunks * sizeof(char*));
	if (chunks) {
		chunkLengths = (size_t*)malloc(maxNumChunks * sizeof(size_t));
		if (chunkLengths) {
			numChunks = 0;
			for (badData = 0, chunkStart = p = (char*) locale; !badData; p++) {
				switch (*p) {
					case '-':
					case '_':
					case '\0':
						if (p == chunkStart) {
							// Empty chunk
							badData = 1;
						} else {
							chunks[numChunks] = chunkStart;
							chunkLengths[numChunks] = p - chunkStart;
							numChunks++;
							chunkStart = p + 1;
						}
						break;
					default:
						if (!isalnum(*p)) {
							// Invalid character
							badData = 1;
						}
						break;
				}
				if (*p == '\0') {
					break;
				}
			}
			if (!badData) {
				result = ConstructLocaleChunks();
				if (result == NULL) {
					badData = 1;
				}
			}
			if (!badData) {
				if (chunkLengths[0] == 4 && !strncmp("root", chunks[0], 4)) {
					// First chunk: "root"
					result->language = strdup("root");
					if (!result->language) {
						badData = 1;
					} else {
						nextChunk = 1;
					}
				} else {
					// First chunks: language and/or script
					if (chunkLengths[0] >= 2 && chunkLengths[0] <= 3) {
						// language - alpha{2,3}
						if (
							isalpha(chunks[0][0])
							&& isalpha(chunks[0][1])
							&& (
								chunkLengths[0] == 2
								|| isalpha(chunks[0][2])
							)
						) {
							result->language = strndup(chunks[0], chunkLengths[0]);
						}
						if (!result->language) {
							badData = 1;
						}
						nextChunk = 1;
					} else {
						nextChunk = 0;
					}
					if (
						nextChunk < numChunks
						&& chunkLengths[nextChunk] == 4
						&& isalpha(chunks[nextChunk][0])
						&& isalpha(chunks[nextChunk][1])
						&& isalpha(chunks[nextChunk][2])
						&& isalpha(chunks[nextChunk][3])
					) {
						result->script = strndup(chunks[nextChunk], chunkLengths[nextChunk]);
						if (!result->script) {
							badData = 1;
						}
						nextChunk++;
					} else if(nextChunk == 0) {
						badData = 1;
					}
				}
				if (!badData && nextChunk < numChunks) {
					// Next we may optionally have the region tag
					if (chunkLengths[nextChunk] == 2) {
						// Region - alpha{2}
						if (
							isalpha(chunks[nextChunk][0])
							&& isalpha(chunks[nextChunk][1])
						) {
							result->territory = strndup(chunks[nextChunk], chunkLengths[nextChunk]);
						}
						if (!result->territory) {
							badData = 1;
						}
						nextChunk++;
					} else if(chunkLengths[nextChunk] == 3) {
						// Region - digit{2}
						if (
							isdigit(chunks[nextChunk][0])
							&& isdigit(chunks[nextChunk][1])
							&& isdigit(chunks[nextChunk][3])
						) {
							result->territory = strndup(chunks[nextChunk], chunkLengths[nextChunk]);
						}
						if (!result->territory) {
							badData = 1;
						}
						nextChunk++;
					}
				}
				if (!badData) {
					// Finally we have a variable number of variant tags (alphanum{5,8} or digit+alphanum{3}) )
					initialVariantChunk = nextChunk;
					while (!badData && nextChunk < numChunks) {
						switch (chunkLengths[nextChunk]) {
							case 8:
								if (!isalnum(chunks[nextChunk][7])) {
									badData = 1;
								}
								/* no break */
							case 7:
								if (!isalnum(chunks[nextChunk][6])) {
									badData = 1;
								}
								/* no break */
							case 6:
								if (!isalnum(chunks[nextChunk][5])) {
									badData = 1;
								}
								/* no break */
							case 5:
								if (!isalnum(chunks[nextChunk][4])) {
									badData = 1;
								}
								if (!badData) {

								}
								break;
							case 4:
								// digit+alphanum{3}
								if (!(
									isdigit(chunks[nextChunk][0])
									&& isalnum(chunks[nextChunk][1])
									&& isalnum(chunks[nextChunk][2])
									&& isalnum(chunks[nextChunk][3])
								)) {
									badData = 1;
								}
								break;
						}
						nextChunk++;
					}
					if (!badData) {
						result->variantCount = numChunks - initialVariantChunk;
						if (result->variantCount) {
							result->variants = (char**)calloc(result->variantCount, sizeof(char*));
							if (!result->variants) {
								badData = 1;
							} else {
								for (nextChunk = initialVariantChunk; nextChunk < numChunks; nextChunk++) {
									result->variants[nextChunk - initialVariantChunk] = strndup(chunks[nextChunk], chunkLengths[nextChunk]);
									if (!result->variants[nextChunk - initialVariantChunk]) {
										badData = 1;
										break;
									}
								}
							}
						}

					}
				}
			}
			if (badData) {
				FreeLocaleChunks(result);
				result = NULL;
			}
			free(chunkLengths);
		}
		free(chunks);
	}

	return result;
}

/*
 * Convert a LocaleChunks to the Unicode locale ID format ( http://unicode.org/reports/tr35/#Unicode_language_identifier ).
 * Returns NULL if LocaleChunks is NULL or invalid, or in case of out-of-memory problems.
 */
char* LocaleChunksToUnicodeLocaleID(const LocaleChunks* lc)
{
	char* result;
	size_t length, i;
	result = NULL;
	if (lc && (lc->language || lc->script)) {
		length = 1;
		if (lc->language && lc->script) {
			length += strlen(lc->language) + 1 + strlen(lc->script);
		} else {
			length += strlen(lc->language ? lc->language : lc->script);
		}
		if (lc->territory) {
			length += 1 + strlen(lc->territory);
		}
		for (i = 0; i < lc->variantCount; i++) {
			length += 1 + strlen(lc->variants[i]);
		}
		result = (char*)malloc(length * sizeof(char));
		if(result) {
			if (lc->language) {
				strcpy(result, lc->language);
				if (lc->script) {
					strcat(result, "_");
					strcat(result, lc->script);
				}
			} else {
				strcpy(result, lc->script);
			}
			if (lc->territory) {
				strcat(result, "_");
				strcat(result, lc->territory);
			}
			for (i = 0; i < lc->variantCount; i++) {
				strcat(result, "_");
				strcat(result, lc->variants[i]);
			}
		}
	}
	return result;
}


/************************/
/* Simple testing stuff */
/************************/
void DumpLocaleChunks(LocaleChunks* lc)
{
	size_t i;
	char* id;
	if (!lc) {
		printf("\t\t<NULL>\n");
	} else {
		printf("\t\tlanguage: %s\n", lc->language ? lc->language : "<NULL>");
		printf("\t\tterritory: %s\n", lc->territory ? lc->territory : "<NULL>");
		printf("\t\tcodeset: %s\n", lc->codeset ? lc->codeset : "<NULL>");
		printf("\t\tmodifier: %s\n", lc->modifier ? lc->modifier : "<NULL>");
		printf("\t\tscript: %s\n", lc->script ? lc->script : "<NULL>");
		if (!lc->variantCount) {
			printf("\t\tno variants\n");
		} else {
			for (i = 0; i < lc->variantCount; i++) {
				printf("\t\tvariant %lu: %s\n", i, lc->variants[i]);
			}
		}
		id = LocaleChunksToGettextLocaleID(lc);
		printf("\t\tGettext ID: %s\n", id ? id : "<NULL>");
		if (id) {
			free(id);
		}
		id = LocaleChunksToUnicodeLocaleID(lc);
		printf("\t\tUnicode ID: %s\n", id ? id : "<NULL>");
		if (id) {
			free(id);
		}
	}
}
void Test(const char* id, int okForGettext, int okForUnicode)
{
	LocaleChunks* lc;
	printf("%s\n", id ? id : "<NULL>");
	lc = GettextLocaleIDToLocaleChunks(id);
	if (lc == NULL) {
		if (okForGettext) {
			printf("\tERROR: it should be valid for Gettext\n");
			exit(1);
		}
		printf("\tNot a Gettext identifier (as expected)\n");
	} else if (!okForGettext) {
		FreeLocaleChunks(lc);
		printf("\tERROR: it has been detected as valid for Gettext, but it should't\n");
		exit(1);
	} else {
		printf("\tGettext identifier ok:\n");
		DumpLocaleChunks(lc);
		FreeLocaleChunks(lc);
	}
	lc = UnicodeLocaleIDToLocaleChunks(id);
	if (lc == NULL) {
		if (okForUnicode) {
			printf("\tERROR: it should be valid for Unicode\n");
			exit(1);
		}
		printf("\tNot an Unicode identifier (as expected)\n");
	} else if (!okForUnicode) {
		FreeLocaleChunks(lc);
		printf("\tERROR: it has been detected as valid for Unicode, but it should't\n");
		exit(1);
	} else {
		printf("\tUnicode identifier ok:\n");
		DumpLocaleChunks(lc);
		FreeLocaleChunks(lc);
	}
}
int main(void) {
	Test("it_IT.utf8@euro", 1, 0);
	Test("it_IT.utf8", 1, 0);
	Test("it_IT@euro", 1, 0);
	Test("it_IT.utf8", 1, 0);
	Test("it@euro", 1, 0);
	Test("it.utf8", 1, 0);
	Test("it_IT", 1, 1);
	Test("it", 1, 1);

	Test("it-Latn-IT-POSIX-NYNORSK", 0, 1);
	Test("it-Latn-IT-POSIX", 0, 1);
	Test("it-Latn-IT-NYNORSK", 0, 1);
	Test("it-Latn-IT", 0, 1);
	Test("it-Latn-POSIX-NYNORSK", 0, 1);
	Test("it-Latn-POSIX", 0, 1);
	Test("it-Latn-NYNORSK", 0, 1);
	Test("it-Latn", 0, 1);
	Test("it-IT-POSIX-NYNORSK", 0, 1);
	Test("it-IT-POSIX", 0, 1);
	Test("it-IT-NYNORSK", 0, 1);
	Test("it-IT", 0, 1);
	Test("it_IT", 1, 1);
	Test("it-POSIX-NYNORSK", 0, 1);
	Test("it-POSIX", 0, 1);
	Test("it-NYNORSK", 0, 1);
	Test("it", 1, 1);

	Test("Latn-IT-POSIX-NYNORSK", 0, 1);
	Test("Latn-IT-POSIX", 0, 1);
	Test("Latn-IT-NYNORSK", 0, 1);
	Test("Latn-IT", 0, 1);
	Test("Latn-POSIX-NYNORSK", 0, 1);
	Test("Latn-POSIX", 0, 1);
	Test("Latn-NYNORSK", 0, 1);
	Test("Latn", 1, 1);

	Test(NULL, 0, 0);
	Test("", 0, 0);
	Test(" ", 0, 0);
	Test("  ", 0, 0);
	Test("foo@bar@baz", 0, 0);

	printf("\n\nAll ok.\n");
	return 0;
}
