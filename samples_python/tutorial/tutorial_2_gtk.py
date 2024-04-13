"""
https://python-gtk-3-tutorial.readthedocs.io/en/latest/introduction.html
"""

import gi

gi.require_version("Gtk", "3.0")
from gi.repository import Gtk

win = Gtk.Window()
win.connect("destroy", Gtk.main_quit)
win.show_all()
Gtk.main()