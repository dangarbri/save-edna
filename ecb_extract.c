#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ecb_extract.h"

// This is how much space is consumed by each page in recipe.dat
#define RECIPE_SIZE 1385

/**
 * A Page is a special subset of a recipe with only 12 ingredients
 * and 12 instructions. The recipe struct itself is meant to hold
 * the combination of all ingredients and instructions from every
 * page. The difference here is only semantic.
 */
typedef Recipe Page;

/**
 * Attempt to execute cmd, and if it doesn't return the given success value, then
 * print the error and exits the program.
 */
#define try(cmd, success) do { if (cmd != (success)) exit(EXIT_FAILURE); } while (0)

/**
 * Read the next string from the file.
 * The format seems to be 1 byte of length, followed by that many
 * ascii characters. Chunk size is how many bytes are reserved
 * for what you're reading. Use 33 for reading an ingredient and
 * 70 for reading an instruction
 */
static char* read_string(FILE* fp, int chunk_size);

/**
 * Reads a page of the recipe file.
 * This will return a page with id == NULL when the end
 * of the data file is reached.
 */
static Page read_page(FILE* fp);

/**
 * This reads a recipe from the data file.
 * A recipe can span multiple pages, each page is
 * processed to return a single recipe
 */
static Recipe read_recipe(FILE* fp);

#define free_page(x) free_recipe(x)

/**
 * Ingredients consume 34 bytes.
 * 1 byte size, 33 bytes of space.
 * This will read the ingredient, and then move the pointer
 * forward to the next ingredient
 */
#define read_ingredient(fp) read_string(fp, 33)
#define read_instruction(fp) read_string(fp, 70)

/**
 * The data file is full of chunks of just NULL bytes, this scans forward on the file
 * until it finds some real data
 */
static void find_data(FILE* fp);

/**
 * Merges two char** lists. The triple pointer is a lot, but it is
 * equivalent to a pointer to an array of strings
 *
 * This copies the strings from b into a. A is resized to fit
 * the new array length
 */
static void merge(char*** a, size_t* a_len, const char** b, size_t b_len);

/**
 * Clone the given string
 */
static char* str_clone(const char* str) {
  char* out = malloc(strlen(str) + 1);
  if (!out) { perror(NULL); exit(EXIT_FAILURE); }

  strcpy(out, str);
  return out;
}


/**
 * If an operation fails, this is called to print the error
 * then the program should be exited
 */
static inline int fail() {
  perror(NULL);
  return EXIT_FAILURE;
}


int extract_recipes(FILE* fp, RecipeHandler recipe_fn) {
  assert(fp);
  assert(recipe_fn);
  // Don't know what the first 8 bytes are, so just move past them.
  try(fseek(fp, 8, SEEK_CUR), 0);

  // Next, the file just has a bunch of NULL bytes. Read through them until
  // we get to the recipe data
  find_data(fp);

  try(fseek(fp, 4, SEEK_CUR), 0);

  // Just go until the reads fail.
  while (1) {
    // Now there's bytes of unknow stuff, skip past it
    Recipe recipe = read_recipe(fp);
    // We successfully read the entire file.
    if (recipe.id == NULL) return 0;
    recipe_fn(recipe);
    free_recipe(recipe);
  }
  return EXIT_SUCCESS;
}

static char* read_string(FILE* fp, int chunk_size) {
  uint8_t len;
  if (fread(&len, 1, 1, fp) != 1) {
    // This happens after reading the final recipe
    return NULL;
  }

  // Even if length is 0, read it anyway (as an empty string).
  char* str = malloc(len + 1);
  if (!str) { perror(NULL); exit(EXIT_FAILURE); }

  if (len > 0) {
    // Read the string
    try(fread(str, 1, len, fp), len);
  }

  // Add the null character
  str[len] = 0;

  // Move the pointer forward to the next ingredient
  try(fseek(fp, chunk_size - len, SEEK_CUR), 0);
  return str;
}

static void find_data(FILE* fp) {
  uint32_t data = 0;
  while (data == 0) try(fread(&data, 1, 1, fp), 1);
  // There's values of 01 lying around, I don't know what they mean,
  // but its not the data we're interested in. So keep scanning forward
  if (data == 1) return find_data(fp);

  // This data is the number of header strings followed by the number of
  // recipe strings. Go back a couple bytes so we can read the whole header.
  try(fseek(fp, -1, SEEK_CUR), 0);
}

static Page read_page(FILE* fp) {
  Page page = {0};

  // Each field is fixed-width in the data file
  // the numbers here are the number of bytes reserved.
  // for each field
  page.id = read_string(fp, 6);
  // This indicates the last page was already processed.
  if (page.id == NULL) return page;

  page.category = read_string(fp, 16);
  page.subcategory = read_string(fp, 16);
  page.source = read_string(fp, 33);
  page.title = read_string(fp, 40);

  // pages are fixed at 12 ingredients and 12 instructions.
  // not all ingredients are actually present.
  page.n_ingredients = 12;
  page.n_instructions = 12;

  page.ingredients = malloc(sizeof(char*) * 12);
  if (!page.ingredients) { perror(NULL); exit(EXIT_FAILURE); }
  for (int i = 0; i < 12; i++) {
    page.ingredients[i] = read_ingredient(fp);

    // If we've read a null ingredient, then we can set the number of
    // ingredients to how many we were able to read. Only set this
    // on the first NULL (handled by the n_ingredients == 12 check).
    // We don't want to break because we need to use read_ingredient
    // to continue to move the file pointer forward
    if ((page.ingredients[i] == NULL) && (page.n_ingredients == 12)) {
      page.n_ingredients = i;
    }
  }

  page.instructions = malloc(sizeof(char*) * 12);
  if (!page.instructions) { perror(NULL); exit(EXIT_FAILURE); }
  for (int i = 0; i < 12; i++) {
    page.instructions[i] = read_instruction(fp);
    // See comment above for why this is done.
    if ((page.instructions[i] == NULL) && (page.n_instructions == 12)) {
      page.n_instructions = i;
    }
  }

  try(fseek(fp, 9, SEEK_CUR), 0);
  return page;
}


static Recipe read_recipe(FILE* fp) {
  // Read the first page of the recipe
  Recipe recipe = read_page(fp);
  while (1) {
    // Move to the next recipe

    // Read the next ID to see if its part of this recipe
    char* id = read_string(fp, 6);
    if (id == NULL) break;

    // Move the file pointer back so the next page read works.
    try(fseek(fp, -7, SEEK_CUR), 0);

    // If the last character of the ID is a space, 
    // then its a new recipe, so don't continue.
    // But if it's a letter (not a space) then it is part of
    // the current recipe, so read that page and merge it into
    // the main recipe
    if (id[5] != ' ') {
      // Otherwise, it's part of this recipe, so we will combine them.
      Page page = read_page(fp);
      merge(&recipe.ingredients, &recipe.n_ingredients, (const char**) page.ingredients, page.n_ingredients);
      merge(&recipe.instructions, &recipe.n_instructions, (const char**) page.instructions, page.n_instructions);
      free_page(page);
      free(id);
    } else {
      free(id);
      break;
    }
  } 

  return recipe;
}

static void merge(char*** a, size_t* a_len, const char** b, size_t b_len) {
  assert(a); assert(a_len); assert(b);
  size_t total = *a_len + b_len;
  // If total is 0, there's nothing to do, so exit early.
  // Do not continue, because then realloc will behave like free.
  if (total == 0) return;

  // reallocate a to have enough space for all elements.
  *a = realloc(*a, sizeof(char*) * total);
  if (!*a) { perror(NULL); exit(EXIT_FAILURE); }

  for (size_t i = 0; i < b_len; i++) {
    (*a)[i + *a_len] = str_clone(b[i]);
  }
  *a_len = total;
}

void free_recipe(Recipe recipe) {
  free(recipe.id);
  free(recipe.category);
  free(recipe.subcategory);
  free(recipe.source);
  free(recipe.title);
  for (size_t i = 0; i < recipe.n_ingredients; i++) {
    char* str = recipe.ingredients[i];
    if (str) free(str);
  }
  free(recipe.ingredients);


  for (size_t i = 0; i < recipe.n_instructions; i++) {
    char* str = recipe.instructions[i];
    if (str) free(str);
  }
  free(recipe.instructions);
}

void print_recipe(Recipe recipe) {
    puts("------------");
  printf("         id: %s\n", recipe.id);
  printf("   category: %s\n", recipe.category);
  printf("subcategory: %s\n", recipe.subcategory);
  printf("     source: %s\n", recipe.source);
  printf("      title: %s\n", recipe.title);
  puts("Ingredients:");
  for (size_t i = 0; i < recipe.n_ingredients; i++)
    printf("  - %s\n", recipe.ingredients[i]);

  puts("Instructions:");
  for (size_t i = 0; i < recipe.n_instructions; i++)
    printf("  %s\n", recipe.instructions[i]);
}
