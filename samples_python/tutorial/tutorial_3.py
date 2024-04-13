"""
application + window
"""
from uisdk_loader import uisdk


class MyWindow(uisdk.Window):
  def __init__(self, app):
    super().__init__(app, title="Hello World")
    self.connect("paint", self.on_paint)

  def on_paint(self, event):
    print("on_paint")



def main(a, **kwargs):
  print(a)
  print(kwargs)
  app = uisdk.Application()

  win = MyWindow(app)
  win.connect("destroy", app.quit)
  win.show()
  app.run()
  pass

# main(0, {"b":2, "c":3})
main(0)
