from bascenev1lib.gameutils import SharedObjects
from bascenev1lib.actor.spaz import Spaz
import bascenev1 as bs
import _bascenev1 as _bs
import server_connector

class Redirector:
    def __init__(self,
          name: str,
          position: tuple[2],
          scale: tuple[2],
          server_id: int = 0
        ):
        self.name = name
        self.server_id = server_id

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

        if opposingnode.getnodetype() == 'spaz':
            self.teleport_player(opposingnode)

    def teleport_player(self, player_node):
        player = player_node.source_player
        sessionplayer = player.sessionplayer
        roster = self.get_player_roster(sessionplayer.get_v1_account_id())

        if not roster:
            print(f'eh... failed to teleport player {sessionplayer.get_v1_account_id()}')
            return

        client_id = roster['client_id']
        server_connector.send(
            server_connector.MessageType.REDIRECT_PLAYER,
            client_id,
            self.server_id,
        )

    def get_player_roster(self, pbid: str) -> dict:
        for player in _bs.get_game_roster():
            if player['account_id'] == pbid:
                return player

        return {}
