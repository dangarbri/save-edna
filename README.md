# Edna's Cookbook Recovery program

This is a program to recover recipes from the DOS program
Edna's Cook Book. Edna's Cookbook writes all recipes to a file
named `RECIPE.DAT`. That's the input to this program

## Usage

To recover recipes, first compile the program, then run
```bash
save_edna RECIPE.DAT
```

This will print all recipes in the data file to stdout.

## Sample Output

Normal Output:
```
$ ./save_edna ~/PCBACKUP/ECB/RECIPE.DAT
------------
         id: 00001
   category: Vegetables
subcategory: Other
     source: Nutrition Education Sarah
      title: Oriental Stir Fry
Ingredients:
  - 1/3 Cup chicken broth
  - 1 1/2 Tbs soy sauce
  - 1 tsp cider vinegar
  - 2 tsp granulated sugar
  - 1 1/2 tsp cornstarch
  - 1 tsp Oriental sesame oil or veg.
  - 1 1/2 cusp thin sliced carrots
  - 2 stems bok choy cut 1/2 " pieces
  - 1 1/2 cups sliced mushrooms
  - 1 chopped green pepper
  - 1/3 cup chopped scallions
  - 1 large clove garlic pressed
  - 1/2 tsp grated fresh ginger
  - or 1/4 tsp ground ginger
  - 10 o.z. firm or extra firm tofu
  - cut into 1/4" cubes
  - 2 cups cooked brown or white rice
Instructions:
  In a small bowl, stir together the broth, soy sauce, vinegar, sugar
  and cornstarch; set aside.  In a 12 inch non-stick skillet, heat the
  oil over medium heat.  Add carrots; stir fry for 4 minutes.  Add bok
  choy, mushrooms and bell pepper; stir fry for 3 minutes.  Add ginger,
  stir fry for 30 seconds.  Stir the broth mixture and add to skillet;
  cook, stirring, for 1 minute or until thickened.  Ad tofu, toss
  gently and heat throughtly about 2 minutes.  Serve over rice.

  Serves 4, with 252 calories and 5 g fat per serving
....
....
```

The program can also output SQL for inserting the recipes into
a database:
```
$ ./save_edna --sql ~/PCBACKUP/ECB/RECIPE.DAT
INSERT INTO recipes(ecbid, name, category, subcategory, source)
VALUES(1, 'Oriental Stir Fry', 'Vegetables', 'Other', 'Nutrition Education Sarah');

INSERT INTO recipe_ingredients(ecbid, ingredient)
VALUES
  (1, '1/3 Cup chicken broth'),
  (1, '1 1/2 Tbs soy sauce'),
  (1, '1 tsp cider vinegar'),
  (1, '2 tsp granulated sugar'),
  (1, '1 1/2 tsp cornstarch'),
  (1, '1 tsp Oriental sesame oil or veg.'),
  (1, '1 1/2 cusp thin sliced carrots'),
  (1, '2 stems bok choy cut 1/2 " pieces'),
  (1, '1 1/2 cups sliced mushrooms'),
  (1, '1 chopped green pepper'),
  (1, '1/3 cup chopped scallions'),
  (1, '1 large clove garlic pressed'),
  (1, '1/2 tsp grated fresh ginger'),
  (1, 'or 1/4 tsp ground ginger'),
  (1, '10 o.z. firm or extra firm tofu'),
  (1, 'cut into 1/4" cubes'),
  (1, '2 cups cooked brown or white rice');
INSERT INTO recipe_instructions(ecbid, instruction)
VALUES
  (1, 'In a small bowl, stir together the broth, soy sauce, vinegar, sugar '),
  (1, 'and cornstarch; set aside.  In a 12 inch non-stick skillet, heat the '),
  (1, 'oil over medium heat.  Add carrots; stir fry for 4 minutes.  Add bok '),
  (1, 'choy, mushrooms and bell pepper; stir fry for 3 minutes.  Add ginger, '),
  (1, 'stir fry for 30 seconds.  Stir the broth mixture and add to skillet; '),
  (1, 'cook, stirring, for 1 minute or until thickened.  Ad tofu, toss '),
  (1, 'gently and heat throughtly about 2 minutes.  Serve over rice.'),
  (1, ''),
  (1, 'Serves 4, with 252 calories and 5 g fat per serving');
...
... More inserts for other recipes
...
```


## Compiling

You need to have a c compiler like gcc or clang, and have
make installed. Then just run `make`. This program doesn't
use any libraries.

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

# Background

After unearthing my family's 30 year old computer, I found it
had some recipes saved on it in the program [Edna's Cook Book](https://archive.org/details/EDNACOOK_ZIP).
There were 149 recipes saved in program! My dad typed in
each one. Edna's Cook Book didn't come with any preprogrammed
recipes.

I cook at home a lot, and I collect a lot of cook books, and I
couldn't leave all these recipes in a program that can only
be accessed with DOS, so here we are. And now this exists in
case anyone else finds their own recipes in Edna's cook book.
I only hope the search engines can point them here.

