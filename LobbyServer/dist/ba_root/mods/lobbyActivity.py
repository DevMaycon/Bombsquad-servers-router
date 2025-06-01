import bascenev1 as bs
import _bascenev1 as _bs
from lobbyActors import *

class Lobby(bs.GameActivity):

    # Default Configs
    name: str = "\ue035 | Server Lobby \ue043"
    description: str = ""

    def __init__(self, settings: dict):
        settings['map'] = "Football Stadium"
        super().__init__(settings)

    def on_begin(self):
        # Show lobby name in the wall
        self.lobby_name = _bs.newnode('text', attrs={
            'in_world': True,
            'scale': 0.03,
            'text': self.getname(),
            'position': (0, 0.2, -6),
            'h_align': 'center',
            'flatness': 0.8,
            'color': (1,1,1)
        })

        # Basic Map Decoration
        self.map.floor_border = _bs.newnode('locator', attrs={
            'shape': 'box',
            'size': (27.5, 0.001, 11.5),
            'position': (0.1, 0.005, 0),
            'color': (1, 1, 1)
        })
        self.map.high_border = _bs.newnode('locator', attrs={
            'shape': 'box',
            'size': (28, 0.001, 13),
            'position': (0.1, 1.7, 0.3),
            'color': (0.7, 0.5, 1)
        })

        Redirector(name='Futbol', position=(-5, -4), scale = (2, 2))
        Redirector(name='Duel', position=(-9, -4), scale = (2, 2))

        self.map.node.color = (0.35,0.1,0.5)
        self.map.node.reflection = 'char'
        self.map.node.color_texture = bs.gettexture('white')
        self.globalsnode.slow_motion = True


    def spawn_player(self, player):
        super().spawn_player(player)

        # We don't want players die in lobby...
        player.actor.impact_scale = 0
        player.actor.equip_boxing_gloves()
        player.actor.connect_controls_to_player(enable_pickup=False, enable_bomb = False)
