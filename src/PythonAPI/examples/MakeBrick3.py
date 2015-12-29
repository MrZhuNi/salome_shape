# Creation of a box using the end-user API
# Author: Daniel Brunier-Coulin
# -----------------------------

from .. import model
from .. import extension

def main():
    # Initialisation

    model.begin()
    mypartset = model.moduleDocument()


    # Creating a new Part

    mypart = model.addPart(mypartset).document()


    # Creating the base of the box

    extension.addBox(mypart, 10, 20, 30)
    model.end()

if __name__ == '__main__':
    main()
