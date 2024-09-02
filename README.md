# Edna's Cook Book - Recovery

This is a program to recover recipes from the DOS program
Ednas Cook Book. The program writes all recipes to a file
named `RECIPE.DAT`.

To recover recipes run
```bash
ecb_extract RECIPE.DAT
```

This will create a folder named `recipes` and inside
will be a bunch of text files containing the recipes
extracted from recipe.dat.

## Compiling

You need to have a c compiler like gcc or clang, and have
make installed. Then just run `make`.
