//
// Created by sterling on 8/12/24.
//

#include "../src/util/general_helpers.h"
int main()
{
    int x = 1 - 1;
    int y = 0 - 1;
    int q, r;
    hex_grid_offset_to_axial(x, y, 1, &q, &r);
    printf("q: %d r: %d\n", q, r);
    hex_grid_rotation_get_next(false, 5, &q, &r);
    int new_x, new_y;
    hex_grid_axial_to_offset(q, r, 1, &new_x, &new_y);
    printf("new x: %d new y: %d\n", new_x + 1, new_y + 1);
    return 0;
}