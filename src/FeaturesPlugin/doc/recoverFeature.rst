.. |recover.icon|    image:: images/recover.png

Recover
=======

**Recover** feature restores some objects used in previous operations.

To recover in the active part:

#. select in the Main Menu *Features - > Recover* item or
#. click |recover.icon| **Recover** button in the toolbar or
#. click |recover.icon| **Recover** command in the context menu if one feature is selected in the Object Browser

The feature has the following options:

.. figure:: images/recover_default_32x32.png
   :align: left
   :height: 24px

restore only the objects used in the operation

.. figure:: images/recover_compound_32x32.png
   :align: left
   :height: 24px

restore full compound/compsolid if the operation used one or more of its sub-shapes

--------------------------------------------------------------------------------

Recover objects
---------------

.. figure:: images/recover_mode_default.png
   :align: center

   Recover objects used in operation

- **Feature** field contains one of the previous operations;
- **List of recovered** objects contains all the shapes used in the operation. Need to enable checkbox to restore a shape.

**TUI Command**:

.. py:function:: model.addRecover(Part_doc, feature, [recovering_results])

    :param part: The current part object.
    :param string: The name of the feature.
    :param list: A list of results used in the feature.
    :return: Created object.

Result
""""""

Result of restoring the shape used in Split operation in compsolid is a single solid.

.. figure:: images/recover_res_default.png
   :align: center

   Recover a solid

**See also** a sample TUI script of ref:`tui_recover_default` operation.


Recover top-level objects
-------------------------

.. figure:: images/recover_mode_compound.png
   :align: center

   Recover top-level compounds/compsolids of objects used in operation

- **Feature** field contains one of the previous operations;
- **List of recovered** objects contains all top-level shapes (compounds/compsolids) of the shapes used in the operation. Need to enable checkbox to restore a shape.

**TUI Command**:

.. py:function:: model.addRecover(Part_doc, feature, [recovering_results], True)

    :param part: The current part object.
    :param string: The name of the feature.
    :param list: A list of results used in the feature.
    :param boolean: *True* to specify restoring compounds.
    :return: Created object.

Result
""""""

Result of restoring the shape used in Split operation in compsolid is a full compsolid.

.. figure:: images/recover_res_compound.png
   :align: center

   Recover a compsolid

**See also** a sample TUI script of ref:`tui_recover_compound` operation.
