import bascenev1 as bs
import _bascenev1 as _bs
from bascenev1lib.gameutils import SharedObjects
import server_connector

class Redirector:
    def __init__(self,
          name: str,
          position: tuple[2],
          scale: tuple[2]
        ):
        self.name = name

        # Create title node
        self.title_node = _bs.newnode('text', attrs={
            'in_world': True,
            'scale': 0.02,
            'text': name,
            'position': (position[0], 2, position[1]),
            'h_align': 'center',
            'flatness': 0.8,
            'color': (1,1,1)
        })

        # Create Material node
        self.node_material = bs.Material()

        # Enable Material Collision
        shared = SharedObjects.get()
        self.node_material.add_actions(
            conditions=("they_have_material", shared.player_material),
            actions=(
                ('modify_part_collision', 'physical', False),
                ('modify_part_collision', 'collide', True)
            )
        )

        # Set Material Collision handler
        self.node_material.add_actions(
            actions=('call', 'at_connect', self.collision_handler),
        )

        # Create locator / border node
        self.node = bs.newnode('locator', attrs={
            'shape': 'box',
            'size': (scale[0], 2, scale[1]),
            'position': (position[0], 1, position[1])
        })

        # Create Region Node
        self.node_region =  bs.newnode('region', attrs={
            'scale': (scale[0], 2, scale[1]),
            'position': (position[0], 1, position[1]),
            'materials': [self.node_material]
        })


    def collision_handler(self):
        collision = bs.getcollision()
        sourcenode = collision.sourcenode
        opposingnode = collision.opposingnode
        print(self.name)
