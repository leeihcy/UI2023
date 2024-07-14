"""
application + window
"""
from uisdk_loader import uisdk


class MyWindow(uisdk.Window):
  def __init__(self, app):
    super().__init__(app, title="Hello World")
    self.connect("paint", self.on_paint)

  def on_paint(self, event):
    print("on_paint", dir(event))


def main():
  app = uisdk.Application()

  win = MyWindow(app)
  win.connect("destroy", app.quit)
  win.show()
  app.run()


main()
