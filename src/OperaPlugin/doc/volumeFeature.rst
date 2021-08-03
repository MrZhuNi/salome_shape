.. |Volume_button.icon|    image:: images/Volume_button.png

Volume
-------------

Volume feature creates a box solid.

To create a Volume in the active part:

#. select in the Main Menu *Opera - > Volume* item  or
#. click |Volume_button.icon| **Volume** button in the toolbar:

Volume is created by a Medium value and a list of solids

.. figure:: images/Volume_create.png
   :align: center

Input fields:

- **MEDIUM**, **VOLUME_LIST** define medium and shapes of the volume.

**TUI Command**:

.. py:function:: model.addVolume(Part_doc, MediumString, model.selection["SOLID", "...", ...])

    :param part: The current part object.
    :param string: Medium name.
    :param list: List of solids.
    :return: Result object.

Result
""""""

One solid for each selected solid in the volume list with according medium

.. figure:: images/Volume1.png
   :align: center

**See Also** a sample TUI Script of :ref:`tui_create_volume` operation.
