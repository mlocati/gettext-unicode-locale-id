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
 * Map from Gettext modifier identifiers to Unicode script identifiers.
 */
const char* GettextModifierToUnicodeScriptDictionary[][2] = {
	{"ahom", "Ahom"}, // Ahom, Tai Ahom
	{"anatolian_hieroglyphs", "Hluw"}, // Anatolian Hieroglyphs (Luwian Hieroglyphs, Hittite Hieroglyphs)
	{"arabic", "Arab"}, // Arabic
	{"armenian", "Armn"}, // Armenian
	{"avestan", "Avst"}, // Avestan
	{"balinese", "Bali"}, // Balinese
	{"bamum", "Bamu"}, // Bamum
	{"bassa_vah", "Bass"}, // Bassa Vah
	{"batak", "Batk"}, // Batak
	{"bengali", "Beng"}, // Bengali
	{"bopomofo", "Bopo"}, // Bopomofo
	{"brahmi", "Brah"}, // Brahmi
	{"braille", "Brai"}, // Braille
	{"buginese", "Bugi"}, // Buginese
	{"buhid", "Buhd"}, // Buhid
	{"canadian_aboriginal", "Cans"}, // Unified Canadian Aboriginal Syllabics
	{"carian", "Cari"}, // Carian
	{"caucasian_albanian", "Aghb"}, // Caucasian Albanian
	{"chakma", "Cakm"}, // Chakma
	{"cham", "Cham"}, // Cham
	{"cherokee", "Cher"}, // Cherokee
	{"common", "Zyyy"}, // Code for undetermined script
	{"coptic", "Copt"}, // Coptic
	{"cuneiform", "Xsux"}, // Cuneiform, Sumero-Akkadian
	{"cypriot", "Cprt"}, // Cypriot
	{"cyrillic", "Cyrl"}, // Cyrillic
	{"deseret", "Dsrt"}, // Deseret (Mormon)
	{"devanagari", "Deva"}, // Devanagari (Nagari)
	{"duployan", "Dupl"}, // Duployan shorthand, Duployan stenography
	{"egyptian_hieroglyphs", "Egyp"}, // Egyptian hieroglyphs
	{"elbasan", "Elba"}, // Elbasan
	{"ethiopic", "Ethi"}, // Ethiopic (Ge'ez)
	{"georgian", "Geok"}, // Khutsuri (Asomtavruli and Nuskhuri)
	{"georgian", "Geor"}, // Georgian (Mkhedruli)
	{"glagolitic", "Glag"}, // Glagolitic
	{"gothic", "Goth"}, // Gothic
	{"grantha", "Gran"}, // Grantha
	{"greek", "Grek"}, // Greek
	{"gujarati", "Gujr"}, // Gujarati
	{"gurmukhi", "Guru"}, // Gurmukhi
	{"han", "Hani"}, // Han (Hanzi, Kanji, Hanja)
	{"hangul", "Hang"}, // Hangul (Hangeul)
	{"hanunoo", "Hano"}, // Hanunoo
	{"hatran", "Hatr"}, // Hatran
	{"hebrew", "Hebr"}, // Hebrew
	{"hiragana", "Hira"}, // Hiragana
	{"imperial_aramaic", "Armi"}, // Imperial Aramaic
	{"inherited", "Zinh"}, // Code for inherited script
	{"inscriptional_pahlavi", "Phli"}, // Inscriptional Pahlavi
	{"inscriptional_parthian", "Prti"}, // Inscriptional Parthian
	{"javanese", "Java"}, // Javanese
	{"kaithi", "Kthi"}, // Kaithi
	{"kannada", "Knda"}, // Kannada
	{"katakana", "Kana"}, // Katakana
	{"katakana_or_hiragana", "Hrkt"}, // Japanese syllabaries (alias for Hiragana + Katakana)
	{"kayah_li", "Kali"}, // Kayah Li
	{"kharoshthi", "Khar"}, // Kharoshthi
	{"khmer", "Khmr"}, // Khmer
	{"khojki", "Khoj"}, // Khojki
	{"khudawadi", "Sind"}, // Khudawadi, Sindhi
	{"lao", "Laoo"}, // Lao
	{"latin", "Latn"}, // Latin
	{"lepcha", "Lepc"}, // Lepcha (Rong)
	{"limbu", "Limb"}, // Limbu
	{"linear_a", "Lina"}, // Linear A
	{"linear_b", "Linb"}, // Linear B
	{"lisu", "Lisu"}, // Lisu (Fraser)
	{"lycian", "Lyci"}, // Lycian
	{"lydian", "Lydi"}, // Lydian
	{"mahajani", "Mahj"}, // Mahajani
	{"malayalam", "Mlym"}, // Malayalam
	{"mandaic", "Mand"}, // Mandaic, Mandaean
	{"manichaean", "Mani"}, // Manichaean
	{"meetei_mayek", "Mtei"}, // Meitei Mayek (Meithei, Meetei)
	{"mende_kikakui", "Mend"}, // Mende Kikakui
	{"meroitic_cursive", "Merc"}, // Meroitic Cursive
	{"meroitic_hieroglyphs", "Mero"}, // Meroitic Hieroglyphs
	{"miao", "Plrd"}, // Miao (Pollard)
	{"modi", "Modi"}, // Modi
	{"mongolian", "Mong"}, // Mongolian
	{"mro", "Mroo"}, // Mro, Mru
	{"multani", "Mult"}, // Multani
	{"myanmar", "Mymr"}, // Myanmar (Burmese)
	{"nabataean", "Nbat"}, // Nabataean
	{"new_tai_lue", "Talu"}, // New Tai Lue
	{"nko", "Nkoo"}, // N'Ko
	{"ogham", "Ogam"}, // Ogham
	{"ol_chiki", "Olck"}, // Ol Chiki (Ol Cemet', Ol, Santali)
	{"old_hungarian", "Hung"}, // Old Hungarian (Hungarian Runic)
	{"old_italic", "Ital"}, // Old Italic (Etruscan, Oscan, etc.)
	{"old_north_arabian", "Narb"}, // Old North Arabian (Ancient North Arabian)
	{"old_permic", "Perm"}, // Old Permic
	{"old_persian", "Xpeo"}, // Old Persian
	{"old_south_arabian", "Sarb"}, // Old South Arabian
	{"old_turkic", "Orkh"}, // Old Turkic, Orkhon Runic
	{"oriya", "Orya"}, // Oriya
	{"osmanya", "Osma"}, // Osmanya
	{"pahawh_hmong", "Hmng"}, // Pahawh Hmong
	{"palmyrene", "Palm"}, // Palmyrene
	{"pau_cin_hau", "Pauc"}, // Pau Cin Hau
	{"phags_pa", "Phag"}, // Phags-pa
	{"phoenician", "Phnx"}, // Phoenician
	{"psalter_pahlavi", "Phlp"}, // Psalter Pahlavi
	{"rejang", "Rjng"}, // Rejang (Redjang, Kaganga)
	{"runic", "Runr"}, // Runic
	{"samaritan", "Samr"}, // Samaritan
	{"saurashtra", "Saur"}, // Saurashtra
	{"sharada", "Shrd"}, // Sharada, Sarada
	{"shavian", "Shaw"}, // Shavian (Shaw)
	{"siddham", "Sidd"}, // Siddham, Siddham, Siddhamatrka
	{"signwriting", "Sgnw"}, // SignWriting
	{"sinhala", "Sinh"}, // Sinhala
	{"sora_sompeng", "Sora"}, // Sora Sompeng
	{"sundanese", "Sund"}, // Sundanese
	{"syloti_nagri", "Sylo"}, // Syloti Nagri
	{"syriac", "Syrc"}, // Syriac
	{"tagalog", "Tglg"}, // Tagalog (Baybayin, Alibata)
	{"tagbanwa", "Tagb"}, // Tagbanwa
	{"tai_le", "Tale"}, // Tai Le
	{"tai_tham", "Lana"}, // Tai Tham (Lanna)
	{"tai_viet", "Tavt"}, // Tai Viet
	{"takri", "Takr"}, // Takri, Tankri
	{"tamil", "Taml"}, // Tamil
	{"telugu", "Telu"}, // Telugu
	{"thaana", "Thaa"}, // Thaana
	{"thai", "Thai"}, // Thai
	{"tibetan", "Tibt"}, // Tibetan
	{"tifinagh", "Tfng"}, // Tifinagh (Berber)
	{"tirhuta", "Tirh"}, // Tirhuta
	{"ugaritic", "Ugar"}, // Ugaritic
	{"unknown", "Zzzz"}, // Code for uncoded script
	{"vai", "Vaii"}, // Vai
	{"warang_citi", "Wara"}, // Warang Citi (Varang Kshiti)
	{"yi", "Yiii"}, // Yi
	{NULL, NULL},
};

/*
 * Convert a Gettext modifier identifier to the corresponding Unicode script identifier.
 * Returns NULL if gettextModifier is empty or if no correspondance has been found.
 */
const char* GettextModifierToUnicodeScript(const char *gettextModifier) {
	const char* result;
	size_t p;
	result = NULL;
	if (gettextModifier && gettextModifier[0]) {
		for (p = 0; GettextModifierToUnicodeScriptDictionary[p][0]; p++) {
			if (!strcasecmp(GettextModifierToUnicodeScriptDictionary[p][0], gettextModifier)) {
				result = GettextModifierToUnicodeScriptDictionary[p][1];
				break;
			}
		}
	}
	return result;
}

/*
 * Convert a Unicode script identifier to the corresponding Gettext modifier identifier.
 * Returns NULL if unicodeScript is empty or if no correspondance has been found.
 */
const char* UnicodeScriptToGettextModifier(const char *unicodeScript) {
	const char* result;
	size_t p;
	result = NULL;
	if (unicodeScript && unicodeScript[0]) {
		for (p = 0; GettextModifierToUnicodeScriptDictionary[p][0]; p++) {
			if (!strcasecmp(GettextModifierToUnicodeScriptDictionary[p][1], unicodeScript)) {
				result = GettextModifierToUnicodeScriptDictionary[p][0];
				break;
			}
		}
	}
	return result;
}

/*
 * Contain all the possible chunck of the locale identifiers.
 */
typedef struct _LocaleChunks {
	// Is this the "root" for Unicode?
	int isRoot;
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
	const char* modifier;
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
		modifier = lc->modifier ? lc->modifier : UnicodeScriptToGettextModifier(lc->script);
		if (modifier) {
			length += 1 + strlen(modifier);
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
			if (modifier) {
				strcat(result, "@");
				strcat(result, modifier);
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
					result->isRoot = 1;
					nextChunk = 1;
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
	const char* script;
	size_t length, i;
	result = NULL;
	script = lc ? (lc->script ? lc->script : GettextModifierToUnicodeScript(lc->modifier)) : NULL;
	if (lc && (lc->isRoot || lc->language || script)) {
		length = 1;
		if (lc->isRoot) {
			length += 4; //strlen("root");
		} else if (lc->language && script) {
			length += strlen(lc->language) + 1 + strlen(script);
		} else {
			length += strlen(lc->language ? lc->language : script);
		}
		if (lc->territory) {
			length += 1 + strlen(lc->territory);
		}
		for (i = 0; i < lc->variantCount; i++) {
			length += 1 + strlen(lc->variants[i]);
		}
		result = (char*)malloc(length * sizeof(char));
		if(result) {
			if (lc->isRoot) {
				strcpy(result, "root");
			} else if (lc->language) {
				strcpy(result, lc->language);
				if (script) {
					strcat(result, "_");
					strcat(result, script);
				}
			} else {
				strcpy(result, script);
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
void DumpLocaleChunksID(LocaleChunks* lc, const char* which, const char* expected, char* calculated)
{
	int die;
	die = 0;
	if (expected && calculated && strcasecmp(expected, calculated)) {
		printf("\t\tERROR: expected %s ID: %s, calculated: %s\n", which, expected, calculated);
		die = 1;
	} else if ((!expected) != (!calculated)) {
		printf("\t\tERROR: expected %s ID: %s, calculated: %s\n", which, expected ? expected : "<NULL>", calculated ? calculated : "<NULL>");
		die = 1;
	} else {
		printf("\t\t%s ID: %s (as expected)\n", which, expected ? expected : "<NULL>");
	}
	if (die) {
		if (calculated) {
			free(calculated);
		}
		FreeLocaleChunks(lc);
		exit(1);
	}
}
void DumpLocaleChunks(LocaleChunks* lc, const char* expectedGettextID, const char* expectedUnicodeID)
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
				printf("\t\tvariant %lu: %s\n", (long unsigned int) i, lc->variants[i]);
			}
		}
		DumpLocaleChunksID(lc, "Gettext", expectedGettextID, LocaleChunksToGettextLocaleID(lc));
		DumpLocaleChunksID(lc, "Unicode", expectedUnicodeID, LocaleChunksToUnicodeLocaleID(lc));
		id = LocaleChunksToGettextLocaleID(lc);
		if (id && expectedGettextID && strcasecmp(id, expectedGettextID)) {
			printf("\t\tERROR: expected gettext ID: %s, calculated gettext ID: %s\n", expectedGettextID, id);
			free(id);
			FreeLocaleChunks(lc);
			exit(1);
		}
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
void Test(const char* id, int okForGettext, const char* expectedGettextID, int okForUnicode, const char* expectedUnicodeID)
{
	LocaleChunks* lc;
	if (id) {
		printf("\"%s\"\n", id);
	} else {
		printf("<NULL>\n");
	}
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
		printf("\tValid Gettext identifier\n");
		DumpLocaleChunks(lc, expectedGettextID, expectedUnicodeID);
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
		printf("\tValid Unicode identifier\n");
		DumpLocaleChunks(lc, expectedGettextID, expectedUnicodeID);
		FreeLocaleChunks(lc);
	}
}
int main(void) {
	Test("it_IT.utf8@euro", 1, "it_IT.utf8@euro", 0, "it_IT");
	Test("it_IT.utf8", 1, "it_IT.utf8", 0, "it_IT");
	Test("it_IT@euro", 1, "it_IT@euro", 0, "it_IT");
	Test("it_IT.utf8", 1, "it_IT.utf8", 0, "it_IT");
	Test("it@euro", 1, "it@euro", 0, "it");
	Test("it.utf8", 1, "it.utf8", 0, "it");
	Test("it_IT", 1, "it_IT", 1, "it_IT");
	Test("it", 1, "it", 1, "it");
	Test("it@latin", 1, "it@latin", 0, "it_Latn");

	Test("it-Latn-IT-POSIX-NYNORSK", 0, "it_IT@latin", 1, "it_Latn_IT_POSIX_NYNORSK");
	Test("it-Latn-IT-POSIX", 0, "it_IT@latin", 1, "it_Latn_IT_POSIX");
	Test("it-Latn-IT-NYNORSK", 0, "it_IT@latin", 1, "it_Latn_IT_NYNORSK");
	Test("it-Latn-IT", 0, "it_IT@latin", 1, "it_Latn_IT");
	Test("it-Latn-POSIX-NYNORSK", 0, "it@latin", 1, "it_Latn_POSIX_NYNORSK");
	Test("it-Latn-POSIX", 0, "it@latin", 1, "it_Latn_POSIX");
	Test("it-Latn-NYNORSK", 0, "it@latin", 1, "it_Latn_NYNORSK");
	Test("it-Latn", 0, "it@latin", 1, "it_Latn");
	Test("it-IT-POSIX-NYNORSK", 0, "it_IT", 1, "it_IT_POSIX_NYNORSK");
	Test("it-IT-POSIX", 0, "it_IT", 1, "it_IT_POSIX");
	Test("it-IT-NYNORSK", 0, "it_IT", 1, "it_IT_NYNORSK");
	Test("it-IT", 0, "it_IT", 1, "it_IT");
	Test("it_IT", 1, "it_IT", 1, "it_IT");
	Test("it-POSIX-NYNORSK", 0, "it", 1, "it_POSIX_NYNORSK");
	Test("it-POSIX", 0, "it", 1, "it_POSIX");
	Test("it-NYNORSK", 0, "it", 1, "it_NYNORSK");
	Test("it", 1, "it", 1, "it");

	Test("Latn-IT-POSIX-NYNORSK", 0, NULL, 1, "Latn_IT_POSIX_NYNORSK");
	Test("Latn-IT-POSIX", 0, NULL, 1, "Latn_IT_POSIX");
	Test("Latn-IT-NYNORSK", 0, NULL, 1, "Latn_IT_NYNORSK");
	Test("Latn-IT", 0, NULL, 1, "Latn_IT");
	Test("Latn-POSIX-NYNORSK", 0, NULL, 1, "Latn_POSIX_NYNORSK");
	Test("Latn-POSIX", 0, NULL, 1, "Latn_POSIX");
	Test("Latn-NYNORSK", 0, NULL, 1, "Latn_NYNORSK");

	Test("root-Latn", 0, NULL, 0, NULL);
	Test("root-IT", 0, NULL, 1, "root_IT");

	Test(NULL, 0, NULL, 0, NULL);
	Test("", 0, NULL, 0, NULL);
	Test(" ", 0, NULL, 0, NULL);
	Test("  ", 0, NULL, 0, NULL);
	Test("foo@bar@baz", 0, NULL, 0, NULL);

	printf("\n\nAll ok.\n");
	return 0;
}
