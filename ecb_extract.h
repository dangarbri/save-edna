#pragma once
#include <stdio.h>

typedef struct Recipe {
  /** Recipe/page number i.e. 00001 or 00001A, recipes ending in a letter are page 2, 3, 4, etc */
  char* id;
  char* category;
  char* subcategory;
  /** Where the recipe came from */
  char* source;
  char* title;
  /** Number of ingredients in the recipe */
  size_t n_ingredients;
  char** ingredients;
  /** Number of instructions in the recipe */
  size_t n_instructions;
  char** instructions;
} Recipe;

/**
 * A function that does something with a recipe,
 * like printing it to stdout.
 */
typedef void (*RecipeHandler)(Recipe);

/**
 * Print the recipe to stdout
 */
void print_recipe(Recipe recipe);

/**
 * Extracts all recipes from the given file pointer.
 * This creates a bunch of text files with the extract
 * recipes.
 */
int extract_recipes(FILE* fp, RecipeHandler recipe_fn);
