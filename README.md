# Edna's Cook Book - Recovery

This is a program to recover recipes from the DOS program
Edna's Cook Book. Edna's Cookbook writes all recipes to a file
named `RECIPE.DAT`. That's the input to this program

To recover recipes run
```bash
ecb_extract RECIPE.DAT
```

This will print all recipes in the data file to stdout.

## Compiling

You need to have a c compiler like gcc or clang, and have
make installed. Then just run `make`.
