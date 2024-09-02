#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "sql.h"

/**
 * Counts the number of times that sep appears in str
 */
static size_t count_occurrences(const char* str, const char* sep) {
  size_t count = 0;
  const char* substr = str;
  while ((substr = strstr(substr, sep)) != NULL) {
    count += 1;
    substr += 1;
  }
  return count;
}

/**
 * Escape quotes in the string and return a new string
 */
static char* clean(const char* str) {
  // count the number of single quotes
  size_t count = count_occurrences(str, "'");
  // each quote means we need to add an extra escape character
  size_t orig_len = strlen(str);
  size_t len = orig_len + count + 1;
  char* out = malloc(len);
  if (!out) { perror(NULL); exit(EXIT_FAILURE); }

  char* ptr = out;
  for (const char* c = str; *c != '\0'; c++, ptr++) {
    if (*c == '\'') {
      *ptr = '\\';
      ptr++;
    }
    *ptr = *c;
  }
  *ptr = '\0';
  return out;
}

static char** clean_array(const char** array, size_t len) {
  char** out = malloc(sizeof(char*) * len);
  if (!out) { perror(NULL); exit(EXIT_FAILURE); }

  for (size_t i = 0; i < len; i++) {
    out[i] = clean(array[i]);
  }
  return out;
}

/**
 * Strip strings in recipes of quotes so they're safe for SQL
 */
static Recipe recipe_escape(Recipe recipe) {
  Recipe cleaned;
  cleaned.id = clean(recipe.id);
  cleaned.category = clean(recipe.category);
  cleaned.subcategory = clean(recipe.subcategory);
  cleaned.source = clean(recipe.source);
  cleaned.title = clean(recipe.title);
  cleaned.n_ingredients = recipe.n_ingredients;
  cleaned.ingredients = clean_array((const char**) recipe.ingredients, recipe.n_ingredients);
  cleaned.n_instructions = recipe.n_instructions;
  cleaned.instructions = clean_array((const char**) recipe.instructions, recipe.n_instructions);
  return cleaned;
}

void recipe_to_sql(Recipe recipe) {
  static int id = 1;
  Recipe cleaned = recipe_escape(recipe);
  printf("-- Recipe %s | %s | %s | %s\n", recipe.title, recipe.category, recipe.subcategory, recipe.source);
  printf("INSERT INTO recipes(ecbid, name, category, subcategory, source)\n"
         "VALUES(%d, '%s', '%s', '%s', '%s');\n\n", id, cleaned.title, cleaned.category, cleaned.subcategory, cleaned.source);
  puts("INSERT INTO recipe_ingredients(ecbid, ingredient)\n"
       "VALUES");
  for (size_t i = 0; i < cleaned.n_ingredients; i++) {
    printf("  (%d, '%s')", id, cleaned.ingredients[i]);
    if (i != (cleaned.n_ingredients - 1)) puts(",");
  }
  puts(";");

  puts("INSERT INTO recipe_instructions(ecbid, instruction)\n"
       "VALUES");
  for (size_t i = 0; i < cleaned.n_instructions; i++) {
    printf("  (%d, '%s')", id, cleaned.instructions[i]);
    if (i != (cleaned.n_instructions - 1)) puts(",");
  }
  puts(";");

  // increment static id. recipe id can't be trusted.
  // the application may have duplicate ids with different
  // recipes.
  id++;
  free_recipe(cleaned);
}
