.. |AddNode_button.icon|    image:: images/AddNode_button.png

AddNode
------

AddNode feature perform an AddNode operation used in bulding the ROOT geometrical hierarchy.

To create a AddNode in the active part:

#. select in the Main Menu *Opera - > AddNode* item  or
#. click |AddNode_button.icon| **AddNode** button in the toolbar:

AddNode is created by a solid as main object and a list of solids as tools

.. figure:: images/AddNode.png
   :align: center

Input fields:

- **MainOject** defines the solid main object;
- **Tools** defines the list of solid tools

**TUI Command**:

.. py:function::
    model.addAddNode(Part_doc, model.selection("SOLID", main_object_name), model.selection[("SOLID", ...), ...])

    :param part: The current part object.
    :param object: A main object solid.
    :param list: A list of tools solids.
    :return: Result compound.

Result
""""""

One compound with the boolean operation result and the tools objects below it, depending on geometry cases

.. figure:: images/AddNode1.png
   :align: center

   AddNode created

**See Also** a sample TUI Script of :ref:`tui_create_addnode` operation.
