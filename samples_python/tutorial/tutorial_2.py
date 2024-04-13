"""
application + window
"""
from uisdk_loader import uisdk

def on_paint(event):
  print("on_paint")

def main():
  app = uisdk.Application()

  win = uisdk.Window(app)
  win.connect("destroy", app.quit)
  win.connect("paint", on_paint)

  win.show()
  app.run()

main()
