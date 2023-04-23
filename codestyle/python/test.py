import math

class Square(object):
    def __init__(self, side):
        self.side = side

    def __get_area(self):
        """Calculates the 'area' property."""
        return self.side ** 2

    def __set_area(self, area):
        """Sets the 'area' property."""
        self.side = math.sqrt(area)

    area = property(__get_area, __set_area,
                    doc="""Gets or sets the area of the square.""")

    @property
    def perimeter(self):
        # return self.side * 4



