#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ecb_extract.h"
#include "sql.h"

static const char* VERSION = "1.0.0";
static void print_version(void) {
  printf("Save Edna's Cook Book - Recovery Program - v%s\n", VERSION);
}

/**
 * Arguments parsed by the program
 */
typedef struct Args {
  /** The recipe.dat file, by any name */
  const char* recipe_data_file;
  
  /** Function which determines what to do with the recipes in the file */
  RecipeHandler recipe_processor;
} Args;

void usage(const char* program_name);
Args parse_cli_args(int argc, char** argv);

int main(int argc, char** argv) {
  Args args = parse_cli_args(argc, argv);

  // Open the recipe data file
  FILE* fp = fopen(args.recipe_data_file, "rb");

  // If the file can't be opened
  if (!fp) {
    fprintf(stderr, "Failed to open %s: ", args.recipe_data_file);
    perror(NULL);
    exit(EXIT_FAILURE);
  }

  int status = extract_recipes(fp, args.recipe_processor);

  fclose(fp);
  return status;
}

/**
 * prints program usage info
 */
void usage(const char* program_name) {
  printf("Usage:\n"
         "  %s [-v|--sql] <recipe file>\n\n"
         "         -v        Print version string\n"
         "      --sql        If this option is given, then SQL will be written to stdout\n"
         "recipe file        This is the RECIPE.DAT file from your ECB program folder\n"
         "                   for inserting these recipes into a database\n", program_name);
}

/**
 * Processes command line arguments and returns a struct with
 * program arguments.
 */
Args parse_cli_args(int argc, char** argv) {
  // Input must have the recipe.dat file as an argument
  if (argc < 2) {
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  const char* data_file = NULL;
  RecipeHandler recipe_handler = print_recipe;
  for (int i = 1; i < argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      print_version();
      exit(EXIT_SUCCESS);
    }

    if (strcmp(argv[i], "--sql") == 0) {
      recipe_handler = recipe_to_sql;
    } else {
      data_file = argv[i];
    }
  }

  if (data_file == NULL) {
    printf("Missing data file. See usage information below\n\n");
    usage(argv[0]);
    exit(EXIT_FAILURE);
  }

  // return parsed arguments
  return (Args) {
    .recipe_data_file = data_file,
    .recipe_processor = recipe_handler
  };
}
