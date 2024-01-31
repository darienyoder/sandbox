# Sandbox

This is a [falling sand simulation](https://en.wikipedia.org/wiki/Falling-sand_game) made with SDL.
The user can select an element from the right sidebar and click on the canvas to create particles of that element.

![sand](https://github.com/darienyoder/sandbox/assets/116597751/20c37825-f73b-4929-bd1a-9a0e200e388c)

## How it works

Each particle on the grid has a **material** (sand, water, stone, etc.) and a **state** (solid, liquid, gas).

1. **Solids** first look at the space below them and fill that space if it is empty.
   If not, they attempt to move into the tiles down and to the side of them.
   This results in pyramid-shaped piles.
2. **Liquids** behave the same as solids, but can also move horizontally, resulting in flatter surfaces.
3. **Gases** flutter around randomly while trending slightly upward.

Particles will exchange heat with surrounding particles, and can change state depending on their temperature and material.

| Material | Solid<br>Name(Color) | Melting Point | Liquid<br>Name(Color) | Boiling Point | Gas<br>Name(Color) |
|----------|----------------------|--------------:|-----------------------|--------------:|--------------------|
| Water    | Ice (Light Blue)     |             0 | Water (Dark Blue)     |           100 | Steam (Light Gray) |
| Sand     | Sand (Yellow)        |          1000 | Lava (Orange)         |          2500 | Smoke (Dark Gray)  |
| Stone    | Stone (Dark Gray)    |           100 | Lava (Orange)         |          2500 | Smoke (Dark Gray)  |
| Wood     | Wood (Brown)         |           200 | *                     |           200 | Smoke (Dark Gray)  |

\* The melting point of wood is the same as its boiling point to give the appearance of smoke coming off the wood as it burns.
  Because of this, it has no liquid state.
