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

## SQL

This program can output the data as a sql query by passing the `--sql` option.

The table names are not customizable at this time, but the following
tables are expected. You may use a different primary key on the recipes
table, but the type of ecbid must be an int.

```
CREATE TABLE recipes (
  ecbid       INT         NOT NULL AUTO_INCREMENT PRIMARY KEY,
  name        VARCHAR(40) NOT NULL,
  category    VARCHAR(16) NOT NULL,
  subcategory VARCHAR(16) NOT NULL,
  source      VARCHAR(33) NOT NULL
);

CREATE TABLE recipe_ingredients (
  id         INT         NOT NULL AUTO_INCREMENT PRIMARY KEY,
  ecbid      INT         NOT NULL,
  ingredient VARCHAR(33) NOT NULL,
  FOREIGN KEY (ecbid) REFERENCES recipes(ecbid)
);

CREATE TABLE recipe_instructions (
  id          INT         NOT NULL AUTO_INCREMENT PRIMARY KEY,
  ecbid       INT         NOT NULL,
  instruction VARCHAR(70) NOT NULL,
  FOREIGN KEY (ecbid) REFERENCES recipes(ecbid)
);
```
