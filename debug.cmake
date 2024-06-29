add_compile_options(-std=c17 -Wall -Wextra -pedantic 
        -fsanitize=address -g)

link_libraries(-fsanitize=address)

# add_compile_definitions(DEBUG)